/*
 * main_aux.c
 *
 *  Created on: Aug 11, 2019
 *      Author: dan
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "structures.h"
#include "linked_list.h"
#include "main_aux.h"
#include "game_utils.h"


void memory_alloc_problem() {
	printf("Error in allocating memory.\n");
	exit(EXIT_FAILURE);
}

int get_command_from_user(char command[COMMAND_SIZE + 1]){ /*from hw3*/
	printf("enter command please: ");

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
	int i,j;

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

	for (i = 0; i < N; i++){
		new_board->board[i] = (board_cell*)calloc(N, sizeof(board_cell));
		if(new_board->board[i] == NULL){
			memory_alloc_problem();
		}
		for(j = 0; j < N; j++){
			new_board->board[i][j].is_error = false;
			new_board->board[i][j].is_fixed = false;
			new_board->board[i][j].value = 0;
		}
	}
	return new_board;
}

game* create_game(int n, int m, game_modes mode, bool is_mark_errors){
	board* new_board;
	list* undo_redo_list;
	game* game_new;

	game_new = (game*) calloc(1, sizeof(game));
	if(game_new == NULL){
		memory_alloc_problem();
	}

	undo_redo_list = (list*) calloc(1, sizeof(list));
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

int cell_in_right_format(int n, int m, int cell){
	if(cell < 0 || cell > n*m){
		return false;
	}
	else{
		return true;
	}
}

void file_not_right_format(FILE *fp){
	printf("Error, file is not in the right format\n");
	if(fclose(fp) != 0){
		printf("Error, was not able to close file");
	}
}

int load_game_from_file(game* current_game, char* path){
	FILE *fp;
	char c;
	int cell_value;
	int i,j;
	int new_m, new_n;
	int num_read = 0;
	board* new_board;

	fp = fopen(path, "r");

	if(fp == NULL){
		printf("Error, was not able to open file\n");
		return false;
	}

	num_read = fscanf(fp, "%d", &new_m);

	if(num_read != 1 || new_m == 0){
		file_not_right_format(fp);
		return false;
	}

	num_read = fscanf(fp, "%d", &new_n);

	if(num_read != 1 || new_n == 0){
		file_not_right_format(fp);
		return false;
	}

	new_board = create_board(new_n, new_m);

	for(i = 0; i < new_n*new_m; i++){
		for(j = 0; j < new_n*new_m; j++){
			num_read = fscanf(fp, "%d", &cell_value);
			if(!cell_in_right_format(new_n, new_m, cell_value)){
				file_not_right_format(fp);
				return false;
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
				file_not_right_format(fp);
				return false;
			}
		}
	}

	list_push(current_game->undo_redo_list, new_board);

	return true;
}

int save_game_to_file(game* current_game, char* path){
	FILE *fp;
	board* current_board;
	int i,j,n,m;

	list* temp_list = current_game->undo_redo_list;
	current_board = (board*)(temp_list->head->data);
	n = current_board->n;
	m = current_board->m;

	fp = fopen(path, "w+");

	if(fp == NULL){
		printf("Error, was not able to open file\n");
		return false;
	}

	fprintf(fp, "%d %d\n", m, n);

	for(i = 0; i < n*m; i++){
		for(j = 0; j < n*m; j++){
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
		printf("Error, was not able to close file\n");
		return false;
	}

	printf("Board saved to file\n");
	return true;
}


void free_board_mem(void* board_to_free){
	int i;
	board* temp_board = (board*)board_to_free;
	int N = temp_board->n * temp_board->m;

	for (i = 0; i < N; i++){
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
	int i,j,n,m;
	board* new_board;

	n = board_to_copy->n;
	m = board_to_copy->m;
	new_board = create_board(n, m);

	for (i = 0; i < n*m; i++){
		for(j = 0; j < n*m; j++){
			new_board->board[i][j].is_error = board_to_copy->board[i][j].is_error;
			new_board->board[i][j].is_fixed = board_to_copy->board[i][j].is_fixed;
			new_board->board[i][j].value = board_to_copy->board[i][j].value;
		}
	}
	return new_board;
}

int board_is_full(board* board_to_check){
	int i,j;

	int N = board_to_check->n*board_to_check->m;

	for (i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			if(board_to_check->board[i][j].value == 0){
				return false;
			}
		}
	}
	return true;
}

int board_is_erroneous(board* board_to_check){
	int i,j;

	int N = board_to_check->n*board_to_check->m;

	for (i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			if(board_to_check->board[i][j].is_error == 1){
				return true;
			}
		}
	}
	return false;
}

int num_empty_cells(board* board_to_check){
	int i,j;

	int N = board_to_check->n*board_to_check->m;
	int count = 0;

	for (i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			if(board_to_check->board[i][j].value == 0){
				count++;
			}
		}
	}
	return count;
}

int check_atoi_error(char* param_name, int after_convert, char* before_convert){
	if(after_convert == 0 && strcmp(before_convert, "0") != 0){
		printf("Error: parameter %s is invalid, should be a positive number\n", param_name);
		return true;
	}
	return false;
}

int check_range(char* param, int num_to_check, int lower_bound, int upper_bound){
	if(num_to_check < lower_bound || num_to_check > upper_bound){
		printf("Error: parameter %s is not in range, should be in range %d - %d\n",param, lower_bound, upper_bound);
		return false;
	}
	return true;
}

int check_board(board* current_board){
	int i,j;
	int n = current_board->n;
	int m = current_board->m;

	for(i = 0; i < m*n; i++){
		for(j = 0; j < m*n; j++){
			if(is_legal_cell(current_board,i,j,current_board->board[i][j].value) == 0) {
				return false;
			}
		}
	}
	return true;
}

void keep_y_cells(board* board_to_clear, int y){
	int N = board_to_clear->m * board_to_clear->n;
	int i = 0;
	int j = 0;
	int num_to_clear = N*N - y;

	while(num_to_clear > 0){
		i = rand()%N;
		j = rand()%N;

		if (board_to_clear->board[i][j].value != 0){
			board_to_clear->board[i][j].value = 0;
			num_to_clear--;
		}
	}
	return;
}

void print_changes_boards(board* first_board, board* second_board){
	int n = first_board->n;
	int m = first_board->m;
	int N = n*m;

	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			if (first_board->board[i][j].value != second_board->board[i][j].value){
				printf("Cell (%d, %d) was changed from value %d to value %d.\n", i + 1, j + 1, first_board->board[i][j].value , second_board->board[i][j].value);
			}
		}
	}
	return;
}

void set_array_zero(int* arr, int size){
	int i;
	for(i = 0; i < size; i++){
		arr[i] = 0;
	}
}

int board_is_completed(board* current_board){
	if(check_board(current_board) == true){
		printf("Puzzle was solved successfully! Congratulations!\n");
		return true;
	}
	else{
		printf("The solution you provided is erroneous, you can use the undo command\n");
		return false;
	}
}

void cell_legal_values(board* current_board, int* legal_values, int N, int row, int col){
	for(int i = 0; i < N; i++){
		if (is_legal_cell(current_board, row, col, i + 1)){
			legal_values[i] = 1;
		}
	}
}

void find_empty_cell(board* current_board, int* empty_cell_row, int* empty_cell_col, int r, int N){
	int cnt = 0;
	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){
			if(current_board->board[i][j].value == 0){
				cnt++;
				if(cnt == r){
					empty_cell_row = &i;
					empty_cell_col = &j;
					return;
				}
			}
		}
	}
}
