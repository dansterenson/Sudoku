/*
 * linked_list.c
 *
 *  Created on: Aug 6, 2019
 *      Author: dan
 */

#include <stdlib.h>
#include <stdio.h>
#include "linked_list.h"


Node* create_node(void* data){
	Node* node;
	node = (Node*) calloc(1, sizeof(Node));
	if(node == NULL){ /*problem allocating memory*/
		printf("Error in allocating memory.\n");
		fflush(stdout);
		exit(EXIT_FAILURE);
	}
	else{
		node->data = data;
		node->next = NULL;
		node->prev = NULL;
	}
	return node;
}

void init_list(list* list){
	if(list != NULL){
		list->head = NULL;
		list->tail = NULL;
		list->size = 0;
	}
}

bool list_is_empty(const list* list){
	if (list->size == 0){
		return true;
	}
	return false;
}

void list_push(list* list, void* data){
    Node* new_node = create_node(data);

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
}

void* list_pop(list* list){
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

int get_list_size(list* list){
	return list->size;
}

void* get_node_data(const Node* node){
    if (node == NULL) {
        return NULL;
    }
    return node->data;
}

void free_list_mem(list* list_to_free, void (*func_to_free_data)(void*)){
	Node* temp;
	while(list_to_free->head != NULL){
		temp = list_to_free->head;
		list_to_free->head = list_to_free->head->next;
		func_to_free_data(list_to_free->head->data);
		free(temp);
	}
	free(list_to_free);
}

Node* get_head_of_list(const list* list) {
    if (list == NULL || list_is_empty(list)) {
        return NULL;
    }
    return list->head;
}

Node* get_tail_of_list(const list* list) {
    if (list == NULL || list_is_empty(list)) {
        return NULL;
    }

    return list->tail;
}
