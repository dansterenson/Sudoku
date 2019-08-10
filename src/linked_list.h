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
} List;

/*
 * initialize the list
 */
void init_list(List* list);

/*
 * checks if list is empty
 */
bool list_is_empty(List* list);

/*
 * insert item to list
 */
void list_push(List* list, void* data);

/*
 * removes node from list
 */
void list_pop(List* list);

/*
 * removes node from list.
 */
void get_list_size(List* list);

/*
 * get the node's data.
 */
void* get_node_data(const Node* node);


#endif /* LINKED_LIST_H_ */
