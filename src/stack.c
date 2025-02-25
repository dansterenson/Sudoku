#include "stack.h"
#include "structures.h"
#include <stdlib.h>
#include <stdio.h>


Stack* stack_init(){
	Stack* stack = (Stack*)calloc(1, sizeof(Stack));
	if(stack == NULL){
		printf("Error in allocating memory.\n");
		exit(EXIT_FAILURE);
	}

	init_list(stack);

	return stack;
}

bool stack_is_empty(const Stack* stack){
	return list_is_empty(stack);
}

void stack_push(Stack* stack, void* new_data){
	list_push(stack, new_data);
}

void* stack_pop(Stack* stack){
	return list_pop(stack);
}

int get_stack_size(Stack* stack){
	return get_list_size(stack);
}

void* get_stack_top_element(const Stack* stack){

	if (stack_is_empty(stack)){
		return NULL;
	}

	return get_node_data(get_head_of_list(stack));
}


void free_stack_mem(Stack* stack) {
	free_list_mem(stack, free);
}
