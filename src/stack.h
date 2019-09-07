/*
 * stack.h
 *
 *  Created on: Aug 7, 2019
 *      Author: dan
 */

#ifndef STACK_H_
#define STACK_H_

#include <stdbool.h>
#include "linked_list.h"


/**
 * A stack based on a double linked list.
 */


typedef list Stack;

/*
 * Return an initialized stack.
 */
Stack* stack_init();

/*
 * Checks if the stack is empty.
 */
bool stack_is_empty(const Stack* stack);

/*
 * Push a new item to the stack.
 */
void stack_push(Stack* stack, void* new_data);

/*
 * remove the top element from the stack.
 */
void* stack_pop(Stack* stack);

/*
 * Get the number of elements in the stack.
 */
int get_stack_size(Stack* stack);

/*
 * gives the top element of the stack (does not pop it)
 * returns NULL if stack is empty
 */
void* get_stack_top_element(const Stack* stack);

/* Free the stack's memory - by freeing all of it's nodes then the stack itself */
void free_stack_mem(Stack* stack);

#endif /* STACK_H_ */
