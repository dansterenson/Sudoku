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
	print_flush("Error, file is not in the right format\n");
	if(fclose(fp) != 0){
		print_flush("Error, was not able to close file");
	}
	return -1;
}

int load_game_from_file(game* current_game, char* path){
	FILE *fp;
	char c;
	int cell_value;
	int new_m, new_n;
	int num_read = 0;
	board* new_board;

	fp = fopen(path, "r");

	if(fp == NULL){
		print_flush("Error, was not able to open file\n");
		return(-1);
	}

	num_read = fscanf(fp, "%d", &new_m);

	if(num_read != 1 || new_m == 0){
		return(file_not_right_format(fp));
	}

	num_read = fscanf(fp, "%d", &new_n);

	if(num_read != 1 || new_n == 0){
		return(file_not_right_format(fp));
	}

	new_board = create_board(new_n, new_m);

	for(int i = 0; i < new_n*new_m; i++){
		for(int j = 0; j < new_n*new_m; j++){
			num_read = fscanf(fp, "%d", &cell_value);
			if(!cell_in_right_format(new_n, new_m, cell_value)){
				return(file_not_right_format(fp));
			}
			new_board->board[i][j].value = cell_value;

			num_read = fscanf(fp, "%c", &c);
			if(c == '.'){
				new_board->board[i][j].is_fixed = 1;
			}
			else if(c != ' ' || c != '\n' || c != '\r' || c != 't'){
				new_board->board[i][j].is_fixed = 0;
			}
			else{
				return(file_not_right_format(fp));
			}
		}
	}

	list_push(current_game->undo_redo_list, new_board);

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
		print_flush("Error, was not able to open file\n");
		return -1;
	}

	fprintf(fp, "%d %d\n", m, n);

	for(int i = 0; i < n*m; i++){
		for(int j = 0; j < n*m; j++){
			fprintf(fp, "%d",current_board->board[i][j].value);
			/*in edit mode every non empty cell is marked as fixed so needs a "."*/
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
		print_flush("Error, was not able to close file\n");
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
					printf("|\n");
				}
			}
		}

		if((i + 1) % (N/n) == 0){
			print_separating_line(n,m);
		}
	}
}

board* copy_board(board* board_to_copy){
	int n = board_to_copy->n;
	int m = board_to_copy->m;
	board* new_board = create_board(n, m);

	for (int i = 0; i < n*m; i++){
		for(int j = 0; j < n*m; j++){
			new_board->board[i][j].is_error = board_to_copy->board[i][j].is_error;
			new_board->board[i][j].is_fixed = board_to_copy->board[i][j].is_fixed;
			new_board->board[i][j].value = board_to_copy->board[i][j].value;
		}
	}
	return new_board;
}

bool board_is_full(board* board_to_check){
	int N = board_to_check->n*board_to_check->m;
	for (int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){
			if(board_to_check->board[i][j].value == 0){
				return false;
			}
		}
	}
	return true;
}

bool board_is_erroneous(board* board_to_check){
	int N = board_to_check->n*board_to_check->m;
	for (int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){
			if(board_to_check->board[i][j].is_error == 1){
				return true;
			}
		}
	}
	return false;
}

int num_empty_cells(board* board_to_check){
	int N = board_to_check->n*board_to_check->m;
	int count = 0;
	for (int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){
			if(board_to_check->board[i][j].value == 0){
				count++;
			}
		}
	}
	return count;
}

bool check_atoi_error(char* param_name, int after_convert, char* before_convert, int N){
	if(after_convert == 0 && strcmp(before_convert, "0") != 0){
		printf("Error: parameter %s is invalid, should be a number in range 1 - %d\n", param_name, N);
		fflush(stdout);
		return true;
	}
	return false;
}

bool check_range(char* param, int num_to_check, int lower_bound, int upper_bound){
	if(num_to_check < lower_bound || num_to_check > upper_bound){
		printf("Error: parameter %s is not in range, should be in range %d - %d\n",param, lower_bound, upper_bound);
		fflush(stdout);
		return false;
	}
	return true;
}

bool check_board(board* current_board){
	int n = current_board->n;
	int m = current_board->m;

	for(int i = 0; i < m*n; i++){
		for(int j = 0; j < m*n; j++){
			if((is_row_legal(current_board->board, i, j, current_board->board[i][j], m*n) == 0)
			|| (is_col_legal(current_board->board, i, j, current_board->board[i][j], m*n) == 0)
			|| (is_block_legal(current_board->board, i, j, current_board->board[i][j], n, m) == 0)){
				return false;
			}
		}
	}
	return true;
}




