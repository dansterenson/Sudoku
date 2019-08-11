/*
 * linked_list.h
 *
 *  Created on: Aug 6, 2019
 *      Author: dan
 */

#ifndef LINKED_LIST_H_
#define LINKED_LIST_H_

#include <stdbool.h>

/*
 * this module contains an implementation of a double linked list.
 */

typedef struct Node Node;

/*
 * The double linked list contains a pointer to the node
 * at the head of the list and to the node at the tail.
 * also the number of elements in the list.
 */

typedef struct list
{
    Node* head;
    Node* tail;
    int size;
} list;

/*
 * initialize the list
 */
void init_list(list* list);

/*
 * checks if list is empty
 */
bool list_is_empty(const list* list);
/*
 * insert item to list
 */
bool list_push(list* list, void* data);
/*
 * removes node from list
 */
void* list_pop(list* list);

/*
 * removes node from list.
 */
int get_list_size(list* list);
/*
 * get the node's data.
 */
void* get_node_data(const Node* node);

const Node* get_head_of_list(const list* list);

const Node* get_tail_of_list(const list* list);

void free_list_mem(list* list_to_free, void (*func_to_free_data)(void*));




#endif /* LINKED_LIST_H_ */
