/*
 * command.c
 *
 *  Created on: Aug 12, 2019
 *      Author: dan
 */

#include "commands.h"
#include "main_aux.h"
#include <stdlib.h>
#include <string.h>

bool board_is_completed(board* current_board){
	if(check_board(current_board) == true){
		print_flush("Puzzle was solved successfully! Congratulations!\n");
		return true;
	}
	else{
		print_flush("The solution you provided is erroneous, you can use the undo command\n");
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
			if(current_board->board[i][j] == 0){
				cnt++;
				if(cnt == r){
					empty_cell_row = i;
					empty_cell_col = j;
					return;
				}
			}
		}
	}
}

int fill_empty_cell_in_index(board* current_board, int r, int N){
	int cnt = 0, random_legal;
	int cnt2 = 0;
	int num_of_legal_val = 0;

	int* legal_values = (int*)calloc(N*sizeof(int));
	if(legal_values == NULL){
		memory_alloc_problem();
	}

	for(int i = 0; i < N; i++){
		legal_values[i] = 0;
	}

	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){
			if(current_board->board[i][j].value == 0){
				cnt++;
				if(cnt == r){
					legal_values = cell_legal_values(current_board, legal_values, N, i, j);
					for(int k = 0; k < N; k++){
						if(legal_values[k] != 0){
							num_of_legal_val ++;
						}
					}
					if(num_of_legal_val == 0){
						free(legal_values);
						return -1; /*needs another iteration*/
					}
					else{
						random_legal = rand() % num_of_legal_val;
					}
					for (int k = 0; k < N; k++){
						if(legal_values[k] != 0){
							cnt2++;
						}
						if(cnt2 == random_legal){
							current_board->board->value[i][j] = k + 1;
						}
					}
				}
			}
		}
	}
	return 0;
}

int handle_solve_command(game* current_game, char* parameters[MAX_PARAMETERS], char* command_name){
	current_game->mode = solve;
	return load_game_from_file(current_game, parameters[0]);
}

int handle_edit_command(game* current_game, char* parameters[MAX_PARAMETERS], char* command_name){
	board* new_board;
	Node* new_node;
	current_game->mode = edit;

	if(parameters[0] != NULL){ /*there is a path (optional parameter)*/
		return load_game_from_file(current_game, parameters[0]);
	}
	else{
		new_board = create_board(3, 3);
		list_push(current_game->undo_redo_list, new_board);
	}
	return 0;
}

int handle_mark_errors_command(game* current_game, char* parameters[MAX_PARAMETERS], char* command_name){
	if(strcmp(parameters[0], "0") == 0){
		current_game->is_mark_errors = 0;
	}

	else if(strcmp(parameters[0], "1") == 0){
		current_game->is_mark_errors = 1;
	}
	else{
		print_flush("Error: parameter is invalid, mark_errors X[0 or 1]\n");
	}
	return 0;
}

int handle_print_board_command(game* current_game, char* parameters[MAX_PARAMETERS], char* command_name){
	print_board((board*)current_game->undo_redo_list->head->data, current_game);
	return 0;
}

int handle_set_command(game* current_game, char* parameters[MAX_PARAMETERS], char* command_name){
	board* current_board = current_game->undo_redo_list->head->data;
	board* copy_of_board;
	int N = current_board->m*current_board->n;

	int x = atoi(parameters[0]);
	int y = atoi(parameters[1]);
	int z = atoi(parameters[2]);

	if(check_atoi_error("X", x, parameters[0], N) || check_atoi_error("Y", y, parameters[1], N)
			|| check_atoi_error("Z", z, parameters[2], N)){
		return -1;
	}

	if(!check_range("X", x, 1, N) || !check_range("Y", y, 1, N) || !check_range("Z", z, 0, N)){
		return -1;
	}

	/*in solve mode user can't change fixed cells*/
	if((current_game->mode == solve) && (current_board->board[y-1][x-1].is_fixed == true)){
		print_flush("Error: this cell is fixed and cannot be changed.\n");
		return -1;
	}

	copy_of_board = copy_board(current_board);

	copy_of_board->board[y-1][x-1].value = z;
	list_push(current_game->undo_redo_list, copy_of_board);
	current_board = (board*)current_game->undo_redo_list->head->data;
	print_board(current_board, current_game);

	if(current_game->mode == solve){
		if(board_is_completed(current_board) == true){
			current_game->mode = init;
		}
	}
	return 0;
}

int handle_validate_command(game* current_game, char* parameters[MAX_PARAMETERS], char* command_name){
	board* current_board = (board*)current_game->undo_redo_list->head->data;
	if(board_is_erroneous(current_board) == true){
		print_flush("Error: the board is erroneous\n");
		return -1;
	}
	return(gurobi_main_ILP(current_board, 0) == 1);/*1 if optimal solution found*/
}

int handle_guess_command(game* current_game, parameters[MAX_PARAMETERS], char* command_name){
	board* current_board = current_game->undo_redo_list->head->data;
	board* copy_of_board;
	char* thresh = parameters[0];


	float threshold = atof(thresh);
	if(threshold == 0.0 && !strcmp(thresh, "0")){
		print_flush("Error: threshold X is invalid, should be a number. format: guess X\n");
		return -1;
	}

	if(threshold > 1.0 || threshold < 0.0){
		print_flush("Error: threshold X is invalid, should be a number between 0 and 1. format: guess X\n");
		return -1;
	}

	if(board_is_erroneous(current_board)){
		print_flush("board is erroneous\n");
		return -1;
	}

	copy_of_board = copy_board(current_board);

	if (gurobi_main_LP(copy_of_board, threshold) == 1){ /*optimal solution found*/
		list_push(current_game->undo_redo_list, copy_of_board);
		if(board_is_completed(current_board) == true){
			current_game->mode = init;
			return 1;
		}
	}
	else{
		free_board_mem(copy_of_board);
	}
	return 0;
}


int handle_generate_command(game* current_game, char* parameters[MAX_PARAMETERS], char* command_name){
	board* current_board = current_game->undo_redo_list->head.data;
	int n = current_board->n;
	int m = current_board->m;
	int N = n*m;
	int num_of_iteration = 0;
	int need_new_iteration = 0;
	int cnt = 0, cnt2;
	int empty_cells_count;
	int r;
	board* copy_of_board;
	int empty_cell_row = -1;
	int empty_cell_col = -1;
	int* legal_values;
	int num_of_legal_val = 0;
	int random_legal;
	int cnt2;

	int x = atoi(parameters[0]);
	int y = atoi(parameters[1]);

	if(check_atoi_error("X", x, parameters[0], N*N) || check_atoi_error("Y", y, parameters[1], N*N)){
		return -1;
	}

	if(!check_range("X", x, 0, N*N) == false || !check_range("Y", y, 0, N*n)){
		return -1;
	}

	empty_cells_count = num_empty_cells(current_board);
	if(empty_cells_count < x){
		printf("Error: the board does not contain %d empty cells.\n", x);
		fflush(stdout);
		return 0;
	}

	legal_values = (int*)calloc(N*sizeof(int));
	if(legal_values == NULL){
		memory_alloc_problem();
	}
	for(int i = 0; i < N; i++){
			legal_values[i] = 0;
	}


	while(num_of_iteration < GENERATE_MAX_ITERATIONS){
		cnt = 0;
		for(int i = 0; i < N; i++){
			legal_values[i] = 0;
		}

		copy_of_board = copy_board(current_board);

		while(cnt < x){
			cnt2 = 0;
			empty_cells_count = num_empty_cells(current_board);
			r = rand() % empty_cells_count;
			find_empty_cell(copy_of_board, &empty_cell_row, &empty_cell_col, r, N);
			cell_legal_values(copy_of_board, legal_values, N, empty_cell_row, empty_cell_col);

			for(int i = 0; i < N; i++){
				if(legal_values[i] != 0){
					num_of_legal_val++;
				}
			}

			if(num_of_legal_val == 0){
				free(legal_values);
				need_new_iteration = 1;
				break;
			}
			else{
				random_legal = rand() % num_of_legal_val;
			}
			for (int k = 0; k < N; k++){
				if(legal_values[k] != 0){
					cnt2++;
				}
				if(cnt2 == random_legal){
					copy_of_board->board->value[empty_cell_row][empty_cell_col] = k + 1;
				}
			}
			cnt++;
		}

		if(need_new_iteration == 1){
			free_board(copy_of_board);
			num_of_iteration++;
		}
		else{
			if(gurobi_main_ILP(copy_of_board, 1) != 1){
				free_board(copy_of_board);
				num_of_iteration++;
			}
			else{
				keep_y_cells(copy_of_board, y);
				list_push(current_game->undo_redo_list, copy_of_board);
				free(legal_values);
				return 0;
			}
		}
	}

	print_flush("Error: generator problem, could not generate a solvable board\n");
	free_board(copy_of_board);
	free(legal_values);
	return -1;
}


int handle_undo_redo_command(game* current_game, char* parameters[MAX_PARAMETERS], char* command_name){
	list* undo_redo_list = current_game->undo_redo_list;
	board* board_before_undo = (board*)undo_redo_list->head->data;
	board* board_after_undo;
	board* current_board = (board*)undo_redo_list->head->data;
	int N = current_board->m*current_board->n;

	if(strcmp(parameters[0], "u") == 0){
		board_after_undo = (board*)undo_redo_list->head->prev->data;
		if(undo_redo_list->head->prev == NULL){ /*check if there is a undo move*/
			print_flush("Error, there are no moves to undo\n");
			return -1;
		}
	}
	else if(strcmp(parameters[0], "r") == 0){
		board_after_undo = (board*)undo_redo_list->head->next->data;
		if(undo_redo_list->head->prev == NULL){ /*check if there is a redo move*/
			print_flush("Error, there are no moves to redo\n");
			return -1;
		}
	}

	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			if (board_before_undo->board[i][j].value != board_after_undo->board[i][j].value){
				printf("Cell (%d, %d) was changed from value %d to value %d.\n", i + 1, j + 1, board_before_undo->board[i][j].value , board_after_undo->board[i][j].value);
			}
		}
	}

	if(strcmp(command_name, "undo") == 0){
		undo_redo_list->head = undo_redo_list->head->prev;
	}
	else if(strcmp(command_name, "redo") == 0){
		undo_redo_list->head = undo_redo_list->head->next;
	}
	print_board((board*)undo_redo_list->head->data, current_game);
	return 0;
}

int handle_save_command(game* current_game, char* parameters[MAX_PARAMETERS], char* command_name){
	board* current_board = (board*)current_game->undo_redo_list->head->data;
	char* path = parameters[0];

	if(current_game->mode == edit){
		if(board_is_erroneous(current_board)){
			print_flush("Erroneous boards are not saved in edit mode\n");
			return -1;
		}
		if(gurobi_main_ILP(current_board, 0) != 1){/* in edit mode boards without a solution may not be saved.*/
			print_flush("This board is unsolvable and thus is not saved in edit mode\n");
			return -1;
		}
	}
	return save_game_to_file(current_game, path);
}

int handle_hint_and_ghint_command(game* current_game, char* cells[MAX_PARAMETERS], char* command_name){
	int x, y, return_val;
	board* current_board = current_game->undo_redo_list->head->data;
	board* copy_of_board;
	int N = current_board->m*current_board->n;

	x = atoi(cells[0]);
	y = atoi(cells[1]);

	if(check_atoi_error("X", x, cells[0], N) || check_atoi_error("Y", y, cells[1], N)){
		return -1;
	}

	if(!check_range("X", x, 1, N) || !check_range("Y", y, 1, N)){
		return -1;
	}

	if(board_is_erroneous(current_board) == true){
		print_flush("Error: board is erroneous\n");
		return -1;
	}

	if(current_board->board[y-1][x-1].is_fixed == 1){
		print_flush("Error: the provided cell is fixed\n");
		return -1;
	}

	if(current_board->board[y-1][x-1].value != 0){
		print_flush("Error: the provided cell contains a value already\n");
		return -1;
	}

	copy_of_board = copy_board(current_board);
	if(strcmp(command_name, "hint") == 0){ /*hint command*/
		return_val = gurobi_main_ILP(copy_of_board, 1);
	}
	else if(strcmp(command_name, "guess_hint") == 0){/*guess_hint_commant*/
		return_val = gurobi_main_LP(copy_of_board, 0.0);
	}

	if(return_val != 1){
		print_flush("Error: this board is unsolvable\n");
		free_board(copy_of_board);
		return -1;
	}
	else{
		printf("Hint: the value of cell (%d,%d) is %d\n", x, y, copy_of_board->board[x-1][y-1].value);
		fflush(stdout);
		free_board(copy_of_board);
		return 0;
	}
}

int handle_num_solution_command(game* current_game, char* parameters[MAX_PARAMETERS], char* command_name){
	board* current_board = current_game->undo_redo_list->head->data;
	board* copy_of_board;
	int num_sol = 0;

	if(board_is_erroneous(current_board) == true){
		print_flush("Error: board is erroneous\n");
		return -1;
	}

	copy_of_board = copy_board(current_board);
	num_sol = exhaustive_backtracking_solution_count(copy_of_board);
	if(num_sol == 0){
		print_flush("No solution found to the current board\n");
	}
	else if(num_sol > 0){
		printf("The number of found solutions is %d\n", num_sol);
		fflush(stdout);
	}
	else{
		print_flush("Error: backtracking failed\n");
	}
	free_board(copy_of_board);
	return 0;
}

int handle_autofill_command(game* current_game, char* parameters[MAX_PARAMETERS], char* command_name){
	board* current_board = current_game->undo_redo_list->head->data;
	int N = current_board->m*current_board->n;
	int* legal_values;
	int num_legal_val = 0;
	board* copy_of_board;
	int val_to_fill;

	if(board_is_erroneous(current_board) == true){
		print_flush("Error: board is erroneous\n");
		return -1;
	}

	legal_values = (int*)calloc(N*sizeof(int));
	if(legal_values == NULL){
		memory_alloc_problem();
	}


	copy_of_board = board_copy(current_board);

	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){

			for(int i = 0; i < N; i++){/*all cells are invalid at first*/
				legal_values[i] = 0;
			}

			if(current_board->board[i][j].value == 0){
				cell_legal_values(current_board, legal_values, N, i, j);
				for(int k; k < N; k++){
					if(legal_values[k] == 1){
						num_legal_val++;
						val_to_fill = k;
					}
				}

				if(num_legal_val == 1){
					copy_of_board[i][j]->board->value = val_to_fill + 1;
				}
			}
		}
	}

	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			if (current_board->board[i][j].value != copy_of_board->board[i][j].value){
				printf("Cell (%d, %d) was filled with value %d.\n", i + 1, j + 1, copy_of_board->board[i][j].value);
			}
		}
	}

	list_push(current_game->undo_redo_list, copy_of_board);

	if(board_is_completed((board*)current_game->undo_redo_list->head->data) == true){
		current_game->mode = init;
	}
	free(legal_values);
	return 0;
}

int handle_reset_command(game* current_game, char* parameters[MAX_PARAMETERS], char* command_name){
	list* undo_redo_list = current_game->undo_redo_list;

	while(undo_redo_list->head->prev != NULL){
		undo_redo_list->head = undo_redo_list->head->prev;
	}
	return 0;
}

int handle_exit_command(game* current_game, char* parameters[MAX_PARAMETERS], char* command_name){
	free_game_mem(current_game);
	return -1;
}






