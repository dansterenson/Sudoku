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



 /*
 * element of the list, contains two pointers,
 * one to the next node and one to the previous one.
 * contains a pointer to the data of the node.
 */
typedef struct Node {
    void* data;
    struct Node* next;
    struct Node* prev;
}Node;

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
 * creates a node, initialize his fields
 * returns the new node
 * returns NULL if memory allocation problem occur;
 */
Node* create_node(void* data);

/*
 * initialize the list
 */
void init_list(list* list);

/*
 * checks if list is empty
 * returns true if the list is empty
 * returns false otherwise
 */
bool list_is_empty(const list* list);

/*
 * insert the provided item to list
 * returns true in success.
 */
void list_push(list* list, void* data);

/*
 * removes the node from the head of the list
 * returns the removed node or NULL if list is empty.
 */
void* list_pop(list* list);

/*
 * returns the list's size
 */
int get_list_size(list* list);

/*
 * returns the node's data
 */
void* get_node_data(const Node* node);

/*
 * returns the node at the head of the list.
 * return NULL if list is empty.
 */
Node* get_head_of_list(const list* list);

/*
 * returns the node at the tail of the list.
 * return NULL if list is empty.
 */
Node* get_tail_of_list(const list* list);

/*
 * frees the list's memory.
 */
void free_list_mem(list* list_to_free, void (*func_to_free_data)(void*));

#endif /* LINKED_LIST_H_ */
