#include "typecheck.h"
extern int outputStage;

void type_error(node_t* root){
	fprintf(stderr, "Type error at:\n");
	if(root != NULL){
		fprintf(stderr,"%s", root->nodetype.text);
		if(root->nodetype.index == EXPRESSION){
			fprintf(stderr," (%s)", root->expression_type.text);
		}
		fprintf(stderr,"\n");
	}
	fprintf(stderr,"Exiting\n");
	exit(-1);
}



data_type_t typecheck_default(node_t* root)
{
	for(int i = 0; i < root->n_children; i++)
		if(root->children[i] != NULL)
			root->children[i]->typecheck(root->children[i]);
}

data_type_t typecheck_expression(node_t* root)
{

	if(outputStage == 10)
		fprintf( stderr, "Type checking expression %s\n", root->expression_type.text);
	if(root->expression_type.index == CLASS_FIELD_E)
	{
		return root->children[0]->typecheck(root->children[0]);
		
 	}
		 
	else if(root->expression_type.index == THIS_E)
	{
		return root->entry->type;
	}

	else if(root->expression_type.index == ADD_E
	|| 	root->expression_type.index == SUB_E
	||	root->expression_type.index == MUL_E
	||	root->expression_type.index == DIV_E)
	{
		if(root->children[0]->data_type.base_type != NO_TYPE)
			root->children[0]->data_type = root->children[0]->typecheck(root->children[0]);
		if(root->children[1]->data_type.base_type == NO_TYPE)
			root->children[1]->data_type = root->children[1]->typecheck(root->children[1]);	

		if(root->children[0]->data_type.base_type == root->children[1]->data_type.base_type
		&&(root->children[0]->data_type.base_type == INT_TYPE
		|| root->children[0]->data_type.base_type == FLOAT_TYPE))
			return root->children[0]->data_type;	
		else
			type_error(root);
	}
	else if(root->expression_type.index == LEQUAL_E
	||	root->expression_type.index == GEQUAL_E
	||	root->expression_type.index == GREATER_E
	||	root->expression_type.index == LESS_E)
	{
		if(root->children[0]->data_type.base_type == NO_TYPE)
			root->children[0]->data_type = root->children[0]->typecheck(root->children[0]);
		if(root->children[1]->data_type.base_type == NO_TYPE)
			root->children[1]->data_type = root->children[1]->typecheck(root->children[1]);	
		
		if(root->children[0]->data_type.base_type == root->children[1]->data_type.base_type
		&&(root->children[0]->data_type.base_type == INT_TYPE
		|| root->children[0]->data_type.base_type == FLOAT_TYPE))
			return root->children[0]->data_type;
		else
			type_error(root);			
	}
	else if(root->expression_type.index == EQUAL_E
	||	root->expression_type.index == NEQUAL_E)
	{
		if(root->children[0]->data_type.base_type == NO_TYPE)
			root->children[0]->data_type = root->children[0]->typecheck(root->children[0]);
		if(root->children[1]->data_type.base_type == NO_TYPE)
			root->children[1]->data_type = root->children[1]->typecheck(root->children[1]);	
		
		if(root->children[0]->data_type.base_type == root->children[1]->data_type.base_type
		&&(root->children[0]->data_type.base_type == INT_TYPE
		|| root->children[0]->data_type.base_type == FLOAT_TYPE
		|| root->children[0]->data_type.base_type == BOOL_TYPE))
			return root->children[0]->data_type;
		else
			type_error(root);		
		
	}
	else if(root->expression_type.index == NOT_E)
	{
		if(root->children[0]->data_type.base_type == NO_TYPE)
			root->children[0]->data_type = root->children[0]->typecheck(root->children[0]);
		if(root->children[0]->data_type.base_type == BOOL_TYPE)
			return root->children[0]->data_type;
		else
			type_error(root);
	}
	else if(root->expression_type.index == AND_E
	||	root->expression_type.index == OR_E)
	{
		if(root->children[1]->data_type.base_type == NO_TYPE)
			root->children[1]->data_type = root->children[1]->typecheck(root->children[1]);
		if(root->children[0]->data_type.base_type == NO_TYPE)
			root->children[0]->data_type = root->children[0]->typecheck(root->children[0]);
		if(root->children[0]->data_type.base_type == BOOL_TYPE 
		&& root->children[1]->data_type.base_type == BOOL_TYPE)
			return root->children[0]->data_type;
		else
			type_error(root);
	}
	
	else if(root->expression_type.index == UMINUS_E)
	{	
		if(root->children[0]->data_type.base_type == NO_TYPE)
			root->children[0]->data_type = root->children[0]->typecheck(root->children[0]);
		
		if(root->children[0]->data_type.base_type == INT_TYPE 
		|| root->children[0]->data_type.base_type == BOOL_TYPE)
			return root->children[0]->data_type;
		else
			type_error(root);
	}
}

data_type_t typecheck_variable(node_t* root)
{
	return root->entry->type;	
}
