/*
 * main_aux.c
 *
 *  Created on: Aug 11, 2019
 *      Author: dan
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"
#include "linked_list.h"
#include "main_aux.h"
#include <ctype.h>

void print_flush(char* str){
	printf("%s", str);
	fflush(stdout);
}

void memory_alloc_problem() {
	print_flush("Error in allocating memory.\n");
	exit(EXIT_FAILURE);
}


int get_command_from_user(char command[COMMAND_SIZE + 1]){ /*from hw3*/
	print_flush("enter command please: ");

	if(fgets(command, (COMMAND_SIZE + 1), stdin) == NULL){
		return -1;
	}

	while(command[0] == '\n' || command[0] == '\r'){
		if(fgets(command, (COMMAND_SIZE + 1), stdin) == NULL){
			return -1;
		}
		if(command[0] == '\0'){
			return 0;
		}
	}
	return 0;
}

board* create_board(int n, int m){
	int N = n*m;
	board* new_board = (board*)calloc(1, sizeof(board));
	if(new_board == NULL){
		memory_alloc_problem();
	}

	new_board->n = n;
	new_board->m = m;

	new_board->board = (board_cell**)calloc(N, sizeof(board_cell*));

	if(new_board->board == NULL){
		memory_alloc_problem();
	}

	for (int i = 0; i < N; i++){
		new_board->board[i] = (board_cell*)calloc(N, sizeof(board_cell));
		if(new_board->board[i] == NULL){
			memory_alloc_problem();
		}
		for(int j = 0; j < N; j++){
			new_board->board[i][j].is_error = false;
			new_board->board[i][j].is_fixed = false;
			new_board->board[i][j].value = 0;
		}
	}
	return new_board;
}

game* create_game(int n, int m, game_modes mode, bool is_mark_errors){
	board* new_board;

	game* game_new = (game*) calloc(1, sizeof(game));
	if(game_new == NULL){
		memory_alloc_problem();
	}

	list* undo_redo_list = (list*) calloc(1, sizeof(list));
	if(undo_redo_list == NULL){
		memory_alloc_problem();
	}

	init_list(undo_redo_list);
	new_board = create_board(n, m);
	list_push(undo_redo_list, new_board);

	game_new->is_mark_errors = is_mark_errors;
	game_new->mode = mode;
	game_new->undo_redo_list = undo_redo_list;

	return game_new;
}

bool cell_in_right_format(int n, int m, int cell){
	if(cell < 0 || cell > n*m){
		return false;
	}
	else{
		return true;
	}
}

int file_not_right_format(FILE *fp){
	print_flush("Error, file is not in the right format");
	fclose(fp);
	return -1;
}

int load_game_from_file(game* current_game,char* path){
	FILE *fp;
	Node* new_node;
	char c;
	int new_m, new_n;
	board* new_board;

	fp = fopen(path, "r");

	if(fp == NULL){
		print_flush("Error, was not able to open file");
		return(-1);
	}

	c = fgetc(fp);

	if(!isdigit(c)){
		return(file_not_right_format(fp));
	}

	new_m = c - '0'; /*converts char to int*/

	c = fgetc(fp);
	if(!isdigit(c)){
		return(file_not_right_format(fp));
	}

	new_n = c - '0'; /*converts char to int*/
	new_board = create_board(new_n, new_m);

	for(int i = 0; i < new_n*new_m; i++){
		for(int j = 0; j < new_n*new_m; j++){
			c = fgetc(fp);
			while (c != ' ' || c != '\n' || c != '\r' || c != 't'){
				if(!isdigit(c) || cell_in_right_format(new_n, new_m, c - '0')){
					return(file_not_right_format(fp));
				}
				if(c == '.'){
					new_board->board[i][j].is_fixed = 1;
				}
				new_board->board[i][j].value = c - '0';
			}
		}
	}
	new_node = create_node(new_board);

	if(new_node == NULL){
		memory_alloc_problem();
	}

	if(list_push(current_game->undo_redo_list, new_node) != true){
		printf("Error: problem while pushing to list\n");
		return -1;
	}
	return 0;
}

int save_game_to_file(game* current_game, char* path){
	FILE *fp;
	board* current_board;
	list* temp_list = current_game->undo_redo_list;
	current_board = (board*)(temp_list->head->data);
	int n = current_board->n;
	int m = current_board->m;

	fp = fopen(path, "w+");

	if(fp == NULL){
		print_flush("Error, was not able to open file");
		return -1;
	}

	fprintf(fp, "%d %d\n", m, n);

	for(int i = 0; i < n*m; i++){
		for(int j = 0; j < n*m; j++){
			fprintf(fp, "%d",current_board->board[i][j].value);
			/*in edit mode' every non empty cell is marked as fixed so needs a "."*/
			if(current_board->board[i][j].is_fixed == true
					|| (current_game->mode == edit && current_board->board[i][j].value != 0)){
				fprintf(fp, ".");
			}

			if(j == n*m - 1){
				fprintf(fp, "\n");
			}
			else fprintf(fp, " ");
		}
	}

	if(fclose(fp) != 0){
		print_flush("Error, was not able to close file");
		return -1;
	}

	printf("Board saved to file\n");
	return 0;
}


void free_board_mem(void* board_to_free){
	board* temp_board = (board*)board_to_free;
	int N = temp_board->n * temp_board->m;
	for (int i = 0; i < N; i++){
		free(temp_board->board[i]);
	}
	free(temp_board->board);
	free(temp_board);
}

void free_undo_redo_list(list* list_to_free){
	free_list_mem(list_to_free, free_board_mem);
}

void free_game_mem(game* game){
	free_undo_redo_list(game->undo_redo_list);
	free(game);
}


void print_separating_line(int n, int m){
	int N = n*m;
	int num_of_dots;
	int i;

	num_of_dots = 4*N + m + 1;

	for(i = 0; i < num_of_dots; i++){
		printf("-");
	}
	printf("\n");
}

void print_board(board* board_to_print, game* game){
	int m = board_to_print->m;
	int n = board_to_print->n;
	int N = n*m;
	int i, j;

	print_separating_line(n, m);
	for (i = 0; i < N; i++){
		printf("|");
		for (j = 0; j < N; j++){
			if(board_to_print->board[i][j].value == 0){
				printf("    ");
			}
			else{
				if(board_to_print->board[i][j].is_fixed == true){
					printf(" %2d.", board_to_print->board[i][j].value);
				}
				else if(board_to_print->board[i][j].is_error == true
						&& (game->is_mark_errors == true || game->mode == edit)){
					printf(" %2d*", board_to_print->board[i][j].value);
				}
				else{
					printf(" %2d ", board_to_print->board[i][j].value);
				}

			}
			if((j + 1) % (N/m) == 0){
				if((j + 1) != N){
					printf("|");
				}
				else{
					printf(" |\n");
				}
			}
		}

		if((i + 1) % (N/n) == 0){
			print_separating_line(n,m);
		}
	}
}


