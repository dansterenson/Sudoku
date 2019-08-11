/*
 * main_aux.c
 *
 *  Created on: Aug 11, 2019
 *      Author: dan
 */

#include <stdio.h>
#include <stdlib.h>
#include "structures.h"
#include "linked_list.h"
#include "main_aux.h"


void memory_alloc_problem() {
	printf("Error in allocating memory.\n");
	fflush(stdout);
	exit(EXIT_FAILURE);
}

int get_command_from_user(char command[COMMAND_SIZE + 1]){
	printf("enter command please: ");
	fflush(stdout);

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

void free_board_mem(board* board_to_free){
	int N = board_to_free->n * board_to_free->m;
	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			free(board_to_free->board[i]);
		}
		free(board_to_free->board);
	}
	free(board_to_free);
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


