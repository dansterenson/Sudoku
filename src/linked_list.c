/*
 * linked_list.c
 *
 *  Created on: Aug 6, 2019
 *      Author: dan
 */

#include <stdlib.h>
#include "linked_list.h"

/*
 * element of the list, contains two pointers,
 * one to the next node and one to the previous one.
 * contains a pointer to the data of the node.
 */
struct Node {
    void* data;
    struct Node* next;
    struct Node* prev;
};

Node* create_node(void* data){
	Node* node;
	node = (Node*) calloc(1, sizeof(Node));
	if(node != NULL){
		node->data = data;
		node->next = NULL;
		node->prev = NULL;
	}
	return node;
}

void init_list(List* list){
	if(list != NULL){
		list->head = NULL;
		list->tail = NULL;
		list->size = 0;
	}
}

bool list_is_empty(List* list){
	if (list->size == 0){
		return true;
	}
	return false;
}

bool list_push(List* list, void* data){
    Node* new_node = create_node(data);

    if (new_node == NULL) {
    	return false;
    }

    if (!list_is_empty(list)) {
        list->head->prev = new_node;
        new_node->next = list->head;
        list->head = new_node;
    }
    else {
        list->head = new_node;
        list->tail = list->head;
    }
    list->size++;
    return true;
}

void* list_pop(List* list){
	void* data;
    Node* popped = NULL;

    if (list_is_empty(list)) {
        return NULL;
    }

    popped = list->head;
    data = popped->data;

    if (list->size == 1) {
        list->head = NULL;
        list->tail = NULL;
    }

    else {
        list->head = popped->next;
        list->head->prev = NULL;
        popped->next = NULL;
    }

    free(popped);
    list->size--;
    return data;
}

int get_list_size(List* list){
	return list->size;
}

void* get_node_data(const Node* node){
    if (node == NULL) {
        return NULL;
    }
    return node->data;
}

const Node* get_head_of_list(const List* list) {
    if (list == NULL || list_is_empty(list)) {
        return NULL;
    }

    return list->head;
}

const Node* get_tail_of_list(const List* list) {
    if (list == NULL || list_is_empty(list)) {
        return NULL;
    }

    return list->tail;
}
