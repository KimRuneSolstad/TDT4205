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
								node_finalize(A); /*destroy the old*/				\
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
//	if(root->nodetype.index == CONSTANT)
//		fprintf(stderr, "hit const%s\n", root->data_type.base_type); //class_name	
	return NULL;
}

/**
 * The input in this function will be the parent of a type node. The type node does not have any children. The Type node will be set to NULL and the parent will be modified to contain the type. 
 */
Node_t *simplify_types ( Node_t *root, int depth )
{
	if(outputStage == 4)
		fprintf ( stderr, "%*cSimplify %s \n", depth, ' ', root->nodetype.text );
	simplify_all_children(root, depth);

	if(root->children[0] != NULL){
		//root->data_type.class_name = root->children[0]->label;
		copy_label(root->data_type.class_name, root->children[0]->label);
		root->n_children = 0;
		node_finalize(root->children[0]);
	}
	
	return NULL;
}


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


Node_t *simplify_class( Node_t *root, int depth )
{
	if(outputStage == 4)
		fprintf ( stderr, "%*cSimplify %s \n", depth, ' ', root->nodetype.text );
	simplify_all_children(root, depth);
	
	copy_label(root->label, root->children[0]->label);
	node_finalize(root->children[0]);
	
	root->children[0] = root->children[1]; //moving declaration_list to first spot.
	root->children[1] = root->children[2]; //moving function_list to secound spot.
	root->n_children = 2;
	return NULL;
}



Node_t *simplify_declaration_statement ( Node_t *root, int depth )
{
	if(outputStage == 4)
		fprintf ( stderr, "%*cSimplify %s \n", depth, ' ', root->nodetype.text );
	simplify_all_children(root, depth);
	
	root->data_type = root->children[0]->data_type;
	copy_label(root->label, root->children[1]->label);
	
	node_finalize(root->children[0]);
	node_finalize(root->children[1]);
	root->n_children = 0;
	return NULL;
}


Node_t *simplify_single_child ( Node_t *root, int depth )
{
	if(outputStage == 4)
		fprintf ( stderr, "%*cSimplify %s \n", depth, ' ', root->nodetype.text );
	simplify_all_children(root, depth);//should only contain one child
	*root = *root->children[0];
	return NULL;
}

Node_t *simplify_list_with_null ( Node_t *root, int depth )
{
	if(outputStage == 4)
		fprintf ( stderr, "%*cSimplify %s \n", depth, ' ', root->nodetype.text );
	simplify_all_children(root, depth);
	
	if(root->children[0] != NULL) {
	//	remove_list_surplus(root);
	/*Save pointer to the first child*/					
	Node_t *tmp = root->children[1];
	/*collect the grandchildren*/					
	Node_t **g_children = root->children[0]->children;		
	/*make them siblings*/								
	g_children[root->children[0]->n_children] = tmp;	
	root->children[0]->n_children++;						
	//node_finalize(A); /*destroy the old*/				
	/*let this new and better generation take over*/	
	*root = *root->children[0];
	
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
 * Spør om det finnes mulighet for å gjøre denne mer smooth.
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
	Node_t *tmp = root->children[1];						
	/*collect the grandchildren*/					
	Node_t **g_children = root->children[0]->children;		
	/*make them siblings*/								
	g_children[root->children[0]->n_children] = tmp;	
	root->children[0]->n_children++;						
	//node_finalize(A); /*destroy the old*/				
	/*let this new and better generation take over*/	
	*root = *root->children[0];/*Save pointer to the first child*/					
//		Node_t *tmp = A->children[1];						
		/*collect the grandchildren*/						
//		Node_t **g_children = A->children[0]->children;		
		/*make them siblings*/								
//		g_children[root->children[0]->n_children] = tmp;	
//		A->children[0]->n_children++;						
//		node_finalize(A); /*destroy the old*/				
		/*let this new and better generati
//remove_list_surplus(root);
		*Save pointer to the first child*/					
		//Node_t *tmp = root->children[1];						
		/*collect the grandchildren*/						
		//Node_t **g_children = root->children[0]->children;		
		/*make them siblings*/								
		//g_children[root->children[0]->n_children] = tmp;
		//root->children[0]->n_children++;

		//Node_t ptr = *root->children[0];						
		//root->n_children = 0;
		//node_finalize(root);	
		//*root = ptr;
			

//		tmp_root->n_children = 0;
//		tmp_root->children = NULL;
//		node_finalize(tmp_root); /*destroy the old*/
//		tmp_root = NULL;
		/*let this new and better generation take over*/
		//*root = *root->children[0];
//		root->label = "kim";
		
	//*root = *root->children[0];
	}	
	return NULL;
}


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
		//struct Node_t *itmp = malloc(sizeof(*root->children[0]));
		Node_t * tmp = root->children[0];
		//root->children = NULL;
		//root->n_children = 0;
		//node_finalize(root);
		
		*root = *tmp;
		//*tmp = root->children[0];
		//node_finalize(root);
		
		//*root = *tmp;// *root->children[0];
	}	
	
	return NULL;
}





























