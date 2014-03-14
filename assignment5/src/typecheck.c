#include "typecheck.h"
extern int outputStage;

data_type_t td(node_t* root);
data_type_t te(node_t* root);
data_type_t tv(node_t* root);

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

int equal_types(data_type_t a, data_type_t b)
{	
	if(a.base_type == CLASS_TYPE)
		return !strcmp(a.class_name, b.class_name);
	else	
		return a.base_type == b.base_type;
		
}

data_type_t typecheck_default(node_t* root)
{
	return td(root);
}

data_type_t typecheck_expression(node_t* root)
{
	data_type_t toReturn;

	if(outputStage == 10)
		fprintf( stderr, "Type checking expression %s\n", root->expression_type.text);

	toReturn = te(root);
		
	//Insert additional checking here
	switch(root->expression_type.index)
	{
		case FUNC_CALL_E:
			/* check number of arguments */
			fprintf(stderr, "");	/*hack to avoid some wierd error */
			function_symbol_t *func = malloc(sizeof(function_symbol_t));
			func = function_get(root->children[0]->label);
			if(func->nArguments != root->children[1]->n_children)
				type_error(root);
			else
				for(int i = 0; i < func->nArguments; i++)
					if(!equal_types(func->argument_types[i], root->children[1]->children[i]->data_type))
						type_error(root);
			return func->return_type;
		break;
		
		case CLASS_FIELD_E: 	/* add code to compute the 
					 * type of class_field_access
					 * expressions
					 */
			return root->children[1]->data_type;
		break;
	}
}

data_type_t typecheck_variable(node_t* root){
	return tv(root);
}


data_type_t typecheck_assignment(node_t* root)
{
	data_type_t type1;
	data_type_t type2;
	type1 = root->children[0]->typecheck(root->children[0]);
	type2 = root->children[1]->typecheck(root->children[1]);
	if(type1.base_type == NO_TYPE)
		type1 = root->children[0]->entry->type;
	if(type2.base_type == NO_TYPE)
		type2 = root->children[1]->entry->type;
	
	else if(type1.base_type == CLASS_TYPE) /* class type is a special case where the class names needs to be equal */
		type2 = root->children[1]->data_type;
	
	if(!equal_types(type1, type2))
		type_error(root);
	
	return type1;
}












