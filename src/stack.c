/*
 * stack.c
 *
 *  Created on: Aug 7, 2019
 *      Author: dan
 */

#include "stack.h"
#include <stdlib.h>


void stack_init(Stack* stack){
	init_list(stack);
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
