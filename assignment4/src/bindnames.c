#include "bindnames.h"
extern int outputStage; // This variable is located in vslc.c
char* thisClass;
static int locvar_offset =0;
int b_d(node_t* root, int stack_offset);
int b_c(node_t* root, int stack_offset);

/*
 * This function fucks up the stackOffset
 */
int bind_default ( node_t *root, int stackOffset)
{
//	fprintf(stderr, "DEFAULT\n");
	int temp = stackOffset;
	b_d(root,stackOffset);
	return stackOffset;
}
int off = 0;
/*
 * The function has two children, one parameter_list and one statement_list.
 * The first one is optional, the other is not.
 */
int bind_function ( node_t *root, int stackOffset)
{
	if(outputStage == 6)
		fprintf ( stderr, "FUNCTION: Start: %s\n", root->label);
	scope_add();	
	/*
	 * Function parameters have decreasing, positive offsets, the last
	 * parameter has offset 8, each parameter has an offset 4 smaller than
	 * its predecessor.
	 */
	 if(root->children[0] != NULL)
	{
		int offs = stackOffset + root->children[0]->n_children * 4;
		for(int i = 0; i < root->children[0]->n_children; i++)
			root->children[0]->children[i]->bind_names(root->children[0]->children[i], offs -= 4);
	}
	/* 
 	 * local variables have decreasing, negative offsets, the first 
	 * local variable has offset -4, each variable has an offset 4 
	 * smaller than its predecessor. 
	 */
	for(int i = 0; i < root->children[1]->n_children; i++)
	{	
	
		if(root->children[1]->children[i]->nodetype.index == DECLARATION_STATEMENT)
			off -= 4;
		root->children[1]->children[i]->bind_names(root->children[1]->children[i], off); 
	}
	off = 0;
	locvar_offset = 0;
	scope_remove();
	if(outputStage == 6)
		fprintf ( stderr, "FUNCTION: End\n");

}

int bind_declaration_list ( node_t *root, int stackOffset)
{
	if(outputStage == 6)
		fprintf ( stderr, "DECLARATION_LIST: Start\n");


	if(outputStage == 6)
		fprintf ( stderr, "DECLARATION_LIST: End\n");

}
int bind_class ( node_t *root, int stackOffset)
{
	if(outputStage == 6)
		fprintf(stderr, "CLASS: Start: %s\n", root->children[0]->label);
	thisClass = root->label;
		
	class_symbol_t* entry = malloc(sizeof(class_symbol_t));
	entry->size = root->children[0]->n_children * 4;
	entry->symbols = ght_create(8);
	entry->functions = ght_create(8);
	class_add(STRDUP(root->label), entry);
	
	locvar_offset = 0;
	/* 
	 * The class node has two children, a declaration list
	 * and a function_list. These should be handled here.
	 */
	if(root->children[0] != NULL)/* declarations */
	{	int offset = 0;
		for(int i = 0; i < root->children[0]->n_children; i++)
		{	
			class_insert_field(	STRDUP(root->label), 						/* class_name */
						root->children[0]->children[i]->label, 				/* key */
						create_symbol(root->children[0]->children[i], 0));		/* value */
		}
	}
	if(root->children[1] != NULL)/* functions */ 
	{
		for(int i = 0; i < root->children[1]->n_children; i++)
			class_insert_method(	STRDUP(root->label), 						/* class_name */
						root->children[1]->children[i]->label, 				/* key */
						create_function_symbol(root->children[1]->children[i]));	/* value */
		/*
	 	 * It should then recursivley continue into the bodies of the methods
	 	 */
		for(int i = 0; i < root->children[1]->n_children; i++)
		{	
			root->children[1]->children[i]->bind_names(root->children[1]->children[i], 12);
		}

	}
	thisClass = NULL;	 
	locvar_offset = 0;
	if(outputStage == 6)
			fprintf(stderr, "CLASS: End\n");
	return stackOffset;
}
 
function_symbol_t* create_function_symbol(node_t* function_node)
{
	function_symbol_t* value = malloc(sizeof(function_symbol_t));
	value->label = STRDUP(function_node->label);
	value->return_type = function_node->data_type;
	if(function_node->children[0] != NULL && function_node->children[0]->n_children > 0)
	{
		value->nArguments = function_node->children[0]->n_children;
		value->argument_types = malloc(sizeof(data_type_t) * value->nArguments);
		for(int i = 0; i < value->nArguments; i++)
			value->argument_types[i] = function_node->children[0]->children[i]->data_type;
	} else
	{
		value->nArguments = 0;
		value->argument_types = NULL;
	}
	return value;	
}

int bind_function_list ( node_t *root, int stackOffset)
{
	if(outputStage == 6)
		fprintf ( stderr, "FUNCTION_LIST: Start\n");
	scope_add();
	/* 
	 * We have to look at all its children and put them into the symbol table
	 * before we continue the travesral
	 */ 
	for(int i = 0; i < root->n_children; i++)
	{
		function_symbol_t* tst = create_function_symbol(root->children[i]);
		function_add(tst->label, tst);;
	}

	for(int i = 0; i < root->n_children; i++)
		root->children[i]->bind_names(root->children[i], 8);
	scope_remove();
	if(outputStage == 6)
		fprintf ( stderr, "FUNCTION_LIST: End\n");
}

int bind_constant ( node_t *root, int stackOffset)
{
	return b_c(root, stackOffset);
}


symbol_t* create_symbol(node_t* declaration_node, int stackOffset)
{
	symbol_t* entry = malloc(sizeof(symbol_t));
	entry->stack_offset = stackOffset;
	entry->label = STRDUP(declaration_node->label);
	entry->type = declaration_node->data_type;
	return entry;
}

/*
 * Variables are declared with declaration_statement nodes, handled by the bind_declaration() function.
 * They can be either in function bodies, or in parameter lists.
 * When a variable is declared, we create a symbol table entry, and instert it. 
 */
int bind_declaration ( node_t *root, int stackOffset)
{	
	if(outputStage == 6)
		fprintf(stderr, "DECLARATION: parameter/variable : '%s', offset: %d\n", root->label, stackOffset);
	
	symbol_insert(root->label, create_symbol(root, stackOffset));
	return stackOffset -4;
}

/*
 * Variables are used in variable nodes, handled by the bind_variable() function.
 * When we see a usage, we look up the variable in teh symbol table, retrieve the entry, 
 * and assign the entry to the variable nodes antry field.
 */
int bind_variable ( node_t *root, int stackOffset)
{
//		fprintf(stderr, "HIT!!!");
	if(outputStage == 6)
		fprintf ( stderr, "VARIABLE: access: %s\n", root->label);

	root->entry = symbol_get(root->label);
}

/*
 * Functions should be retrieved in the bind_expression() function. The returned entry
 * should be assigned to the nodes function_entry field.
 */
int testCount = 0;
int bind_expression( node_t* root, int stackOffset)
{
	if(outputStage == 6)
		fprintf( stderr, "EXPRESSION: Start: %s\n", root->expression_type.text);
	
	if(root->expression_type.index == FUNC_CALL_E)
	{
		root->function_entry = function_get(root->children[0]->label);
		if(root->children[1] != NULL)
			for(int i = 0; i < root->children[1]->n_children; i++)
				root->children[1]->children[i]->bind_names(root->children[1]->children[i], stackOffset);	
//	else
//		root->children[0]->bind_names(root->children[0], stackOffset);
		/* The variable name lies in the first child */
	} else if(root->expression_type.index == NEW_E)
	{
		root->class_entry = class_get(root->children[0]->data_type.class_name);
	}else if(root->expression_type.index == CLASS_FIELD_E)
	{	
		/*
		 * Field access expressions don't follow the usual post order traversal, 
		 * we never enter the second child. The first thing we do is recursivley 
		 * handle the first child.
		 */
		root->children[0]->bind_names(root->children[0], stackOffset);
		if(root->children[0]->entry->type.class_name != NULL)
			root->children[0]->entry->label = STRDUP(root->children[0]->entry->type.class_name);	
		/* 
		 * We then look up the second child in the private symbol table of the 
		 * first child. We assign the entry to both the second child and the
		 * field acces expression.
		 */
		root->entry = root->children[1]->entry = class_get_symbol(	root->children[0]->entry->label,/* class_name */ 
										root->children[1]->label);	/* key */ 
	}else if(root->expression_type.index == METH_CALL_E)
	{
		/*
		 * Three nodes. The class object identifier, the class method name and 
		 * an optional expression_list
		 */
		root->children[0]->bind_names(root->children[0], stackOffset);
		root->entry = root->children[1]->function_entry = class_get_method(	root->children[0]->entry->type.class_name,	/* class_name */
											root->children[1]->label);			/* key */
		if(root->children[2] != NULL)
			for(int i = 0; i < root->children[2]->n_children; i++)
				root->children[2]->children[i]->bind_names(root->children[2]->children[i], stackOffset);

	}else if(root->expression_type.index == THIS_E)
	{ 	
		root->entry = malloc(sizeof(symbol_t));
		root->entry->stack_offset = 8;
		root->entry->label = STRDUP(thisClass);
		root->entry->type.base_type = CLASS_TYPE;
		root->entry->type.class_name = NULL;
		
	}else /* probably some kind of mathematical expression */
	{
		for(int i = 0; i < root->n_children; i++)
			root->children[i]->bind_names(root->children[i], stackOffset);//	bind_constant(root->children[i], stackOffset);
	}
	if(outputStage == 6)
		fprintf( stderr, "EXPRESSION: End\n");
}



