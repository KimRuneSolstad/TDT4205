#include "bindnames.h"
extern int outputStage; // This variable is located in vslc.c
char* thisClass;

int bind_default ( node_t *root, int stackOffset)
{
	for(int i = 0; i < root->n_children; i++)
		if(root->children[i] != NULL)
			root->children[i]->bind_names(root->children[i], 0);
	return 0;	
}

int bind_constant ( node_t *root, int stackOffset)
{
	if(outputStage == 6)
		fprintf( stderr, "CONSTANT\n");

	for(int i = 0; i < root->n_children; i++)
		if(root->children[i] != NULL)
			root->children[i]->bind_names(root->children[i], 0);
	
	if(root->data_type.base_type == STRING_TYPE)
	{
		char* ptr = root->string_const;
			
	//	fprintf(stderr, "bind: %s\n", root->string_const);
		strings_add(ptr);
	}
	return 0;
}
