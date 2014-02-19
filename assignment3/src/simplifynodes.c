#include <string.h>
#include "simplifynodes.h"
#define MAXSYM 500
#define simplify_all_children(A, B)	for(int i = 0; i < A->n_children; i++) 	\
										if(A->children[i] != NULL) 			\
											A->children[i]->simplify(A->children[i], B + 1)

#define remove_list_surplus(A) 	/*Save pointer to the first child*/					\
								Node_t *tmp = A->children[1];						\
								/*collect the grandchildren*/						\
								Node_t **g_children = A->children[0]->children;		\
								/*make them siblings*/								\
								g_children[root->children[0]->n_children] = tmp;	\
								A->children[0]->n_children++;						\
								/*node_finalize(A); destroy the old*/				\
								/*let this new and better generation take over*/	\
								*A = *A->children[0]

#define copy_label(A, B) 	/*node_finalize should deallocate this memory*/	\
							char *lbl = malloc(sizeof(char) * strlen(B));	\
							strcpy(lbl, B);									\
							A = lbl



extern int outputStage; // This variable is located in vslc.c

Node_t* simplify_default ( Node_t *root, int depth )
{
	simplify_all_children(root, depth);
	return NULL;
}

/*
 *	If this node has any children, it is a class node. The label of that 
 *	node will be copied to the class_name by the copy_label macro.
 */ 
Node_t *simplify_types ( Node_t *root, int depth )
{
	if(outputStage == 4)
		fprintf ( stderr, "%*cSimplify %s \n", depth, ' ', root->nodetype.text );
	simplify_all_children(root, depth);

	if(root->children[0] != NULL){
		//allocate memory and copy content
		copy_label(root->data_type.class_name, root->children[0]->label);
		root->n_children = 0;
		node_finalize(root->children[0]);
	}
	
	return NULL;
}

/*
 *	The nodes containing data_type and variables will be copied to the 
 *	root. Those nodes will then be finalized. Lastly the spots of the
 *	finalized nodes will be filled with the parameter_list and statement_list
 */
Node_t *simplify_function ( Node_t *root, int depth )
{
	if(outputStage == 4)
		fprintf ( stderr, "%*cSimplify %s \n", depth, ' ', root->nodetype.text );
	simplify_all_children(root, depth);	

	root->data_type = root->children[0]->data_type; //assign type to function
	node_finalize(root->children[0]);
	root->children[0] = NULL;	
	//assign variable to function
	copy_label(root->label, root->children[1]->label);
	node_finalize(root->children[1]);	
	
	/* The simplified function will have max two nodes: parameter_list 0, and statement_list 1.*/	
	if(root->children[2] != NULL) //parameter list optional
		root->children[0] = root->children[2];
		
	root->children[1] = root->children[3]; //statement list not optional
	root->n_children = 2;

	return NULL;
}

/*
 *	The label from the variable node is copied to the label spot
 *	of the class node. That node is then finalized. The declaration_list
 *	and function_list is then moved so that the variable-node spot is filled
 */
Node_t *simplify_class( Node_t *root, int depth )
{
	if(outputStage == 4)
		fprintf ( stderr, "%*cSimplify %s \n", depth, ' ', root->nodetype.text );
	simplify_all_children(root, depth);
	
	copy_label(root->label, root->children[0]->label);
	node_finalize(root->children[0]);
//	root->label = root->children[0]->label;
		
	root->children[0] = root->children[1]; //moving declaration_list to first spot.
	root->children[1] = root->children[2]; //moving function_list to secound spot.
	root->n_children = 2;
	return NULL;
}


/*
 *	The declaration statement should not have any children. The data_type of the
 * 	type node and the label of the variable node is copied to the root. Those 
 *	nodes are then removed from memory. Lastly the number of children is set to 0.
 */
Node_t *simplify_declaration_statement ( Node_t *root, int depth )
{
	if(outputStage == 4)
		fprintf ( stderr, "%*cSimplify %s \n", depth, ' ', root->nodetype.text );
	simplify_all_children(root, depth);
	
	root->data_type = root->children[0]->data_type;
	copy_label(root->label, root->children[1]->label);	

	node_finalize(root->children[0]); //remove the nodes from memory
	node_finalize(root->children[1]);
	root->n_children = 0;
	return NULL;
}

/*
 *	Memory de-allocation code has been commented out because it caused corruption of 
 *	the labels. I don't know how to properly de-allocate the memory here. 
 */
Node_t *simplify_single_child ( Node_t *root, int depth )
{
	if(outputStage == 4)
		fprintf ( stderr, "%*cSimplify %s \n", depth, ' ', root->nodetype.text );
	simplify_all_children(root, depth);//should only contain one child
	
	Node_t *tmp = root->children[0]; //create a pointer to the child
	*root = *tmp;
	tmp->children = NULL;//To prevent de-allocation of wanted nodes, children is set to NULL
//	tmp->children = NULL;	
//	node_finalize(tmp);
	return NULL;
}

/*
 *	Memory de-allocation code has been commented out because it caused corruption of 
 *	the labels. I don't know how to properly de-allocate the memory here. 
 */
Node_t *simplify_list_with_null ( Node_t *root, int depth )
{
	if(outputStage == 4)
		fprintf ( stderr, "%*cSimplify %s \n", depth, ' ', root->nodetype.text );
	simplify_all_children(root, depth);
	
	if(root->children[0] != NULL) {
	/*Save pointer to the first child*/					
	Node_t *single_child = root->children[1];
	/*Save pointer to the list*/
	Node_t *tmp = root->children[0];
	/*collect the grandchildren*/					
	Node_t **g_children = tmp->children;
//	g_children = realloc(g_children, (root->children[0]->n_children + 1) * sizeof(Node_t));
	/*make them siblings*/								
	g_children[root->children[0]->n_children] = single_child;	
	root->children[0]->n_children++;						
	/*let this new and better generation take over*/
	*root = *root->children[0];
	/*prevent de-allocation of wanted nodes*/
//	tmp->children = NULL;
	/*remove the unecessary node*/
//	node_finalize(tmp);
	
	}

	/*if this is a node with a single child, that child gets a promotion. */
	else if(root->children[0] == NULL){
		root->children[0] = root->children[1];
		root->children[1] = NULL;
		root->n_children = 1;
	}
	return NULL;
}

/*
 *	Memory de-allocation code has been commented out because it caused the corruption of 
 *	labels. I don't know how to properly de-allocate the memory here. 
 */
Node_t *simplify_list ( Node_t *root, int depth )
{
	if(outputStage == 4)
		fprintf ( stderr, "%*cSimplify %s \n", depth, ' ', root->nodetype.text );
	simplify_all_children(root, depth);
	
	if(root->children[0] != NULL && root->children[0]->nodetype.index == STATEMENT_LIST
								|| 	root->children[0]->nodetype.index == VARIABLE_LIST
								||	root->children[0]->nodetype.index == EXPRESSION_LIST
								||	root->children[0]->nodetype.index == CLASS_LIST) {	
		/*Save pointer to the first child*/					
		Node_t *single_child = root->children[1];
		/*Save pointer to the list*/
		Node_t *tmp = root->children[0];
		/*collect the grandchildren*/					
		Node_t **g_children = tmp->children;		
		//g_children = realloc(g_children, (root->children[0]->n_children + 1) * sizeof(Node_t));
		/*make them siblings*/								
		g_children[root->children[0]->n_children] = single_child;	
		root->children[0]->n_children++;						
		/*let this new and better generation take over*/
		*root = *tmp;
		/*prevent de-allocation of wanted nodes*/
//		tmp->children = NULL;
//		node_finalize(tmp);
	}	
	return NULL;
}

/*
 *	Memory de-allocation code has been commented out because it caused corruption of 
 *	the labels. I don't know how to properly de-allocate the memory here. 
 */
Node_t *simplify_expression ( Node_t *root, int depth )
{
	if(outputStage == 4)
		fprintf ( stderr, "%*cSimplify %s (%s) \n", depth, ' ', root->nodetype.text, root->expression_type.text );
	
	simplify_all_children(root, depth);

	if(	root->expression_type.text != new_e.text
	&&	root->expression_type.text != uminus_e.text
	&&	root->expression_type.text != not_e.text
	&&	root->n_children == 1)
	{	
		//*root = *tmp;// *root->children[0];
		Node_t *tmp = root->children[0]; //create a pointer to the child

		*root = *tmp;

//		tmp->children = NULL;
//		node_finalize(tmp);
	}	
	
	return NULL;
}
