#include "commands.h"
#include "main_aux.h"
#include <stdlib.h>
#include <string.h>
#include "game_utils.h"
#include "gurobi_solver_ILP.h"
#include "gurobi_solver_LP.h"


void handle_solve_command(game* current_game, char* path){
	board* loaded_board;
	list* new_list;

	if(load_game_from_file(path, &loaded_board, E_SOLVE_CMD) == true){
		free_list_mem(current_game->undo_redo_list, free_board_mem);
		new_list = create_empty_list();
		list_push(new_list, loaded_board);
		current_game->undo_redo_list = new_list;
		current_game->mode = solve;
		print_board((board*)current_game->undo_redo_list->head->data, current_game);
	}
	return;
}

void handle_edit_command(game* current_game, char* path){
	board* loaded_board;
	board* new_board;
	list* new_list;

	if(path != NULL){ /*there is a path (optional parameter)*/
		if(load_game_from_file(path, &loaded_board, E_EDIT_CMD) == true){
			free_list_mem(current_game->undo_redo_list, free_board_mem);
			new_list = create_empty_list();
			list_push(new_list, loaded_board);
			current_game->undo_redo_list = new_list;

			print_board((board*)current_game->undo_redo_list->head->data, current_game);
		}
		else{
			return;
		}
	}
	else{
		new_board = create_board(3, 3);
		free_list_mem(current_game->undo_redo_list, free_board_mem);
		new_list = create_empty_list();
		list_push(new_list, new_board);
		current_game->undo_redo_list = new_list;
		print_board((board*)current_game->undo_redo_list->head->data, current_game);
	}
	current_game->mode = edit;
}

void handle_mark_errors_command(game* current_game, int setting){
	if(setting == 0){
		current_game->is_mark_errors = 0;
	}

	else if(setting == 1){
		current_game->is_mark_errors = 1;
	}
}

void handle_print_board_command(game* current_game){
	print_board((board*)current_game->undo_redo_list->head->data, current_game);
}

void handle_set_command(game* current_game,int row, int col, int new_value){
	board* current_board = current_game->undo_redo_list->head->data;
	board* copy_of_board;

	copy_of_board = copy_board(current_board);

	copy_of_board->board[row][col].value = new_value;
	update_error_cells(copy_of_board);

	list_push(current_game->undo_redo_list, copy_of_board);
	current_board = (board*)current_game->undo_redo_list->head->data;
	print_board(current_board, current_game);

	if(current_game->mode == solve){
		if(board_is_full(current_board)){
			if(board_is_completed(current_board) == true){
				current_game->mode = init;
				free_list_mem(current_game->undo_redo_list, free_board_mem);
				current_game->undo_redo_list = create_empty_list();
			}
		}
	}
	return;
}

void handle_validate_command(game* current_game){
	board* current_board = (board*)current_game->undo_redo_list->head->data;
	int ret = gurobi_main_ILP(current_board, 0);
	
	if(ret == 1){ /*1 if optimal solution found*/
		printf("Validation completed successfully: Solution found to the board\n");
	}
	else if(ret == 0){
		printf("Validation failed: Board is unsolvable\n");
	}
	else{
		printf("Validation failed: Optimization was stopped early\n");
	}
}

void handle_guess_command(game* current_game, float threshold){
	board* current_board = current_game->undo_redo_list->head->data;
	board* copy_of_board;
	int ret;

	copy_of_board = copy_board(current_board);
	ret = gurobi_main_LP(copy_of_board, threshold, NULL);

	if (ret == 1){ /*optimal solution found*/
		list_push(current_game->undo_redo_list, copy_of_board);
		print_board((board*)current_game->undo_redo_list->head->data, current_game);
		current_board = current_game->undo_redo_list->head->data;
		if(board_is_full(current_board)){
			if(board_is_completed(current_board) == true){
				current_game->mode = init;
			}
		}
		return;
	}

	else{
		printf("Error: Board is unsolvable\n"); 
	}
		
	free_board_mem(copy_of_board);
	return;
}


void handle_generate_command(game* current_game, int x, int y){
	board* current_board = current_game->undo_redo_list->head->data;
	int n = current_board->n;
	int m = current_board->m;
	int N = n*m;
	int i,k;
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
	

	empty_cells_count = num_empty_cells(current_board);
	if(empty_cells_count < x){
		printf("Error: the board does not contain %d empty cells.\n", x);
		return;
	}
	
	if(board_is_erroneous(current_board)){
		printf("Error: board is erroneous and unsolvable, generate command cannot be performed\n");
		return;
	}

	legal_values = (int*)calloc(N, sizeof(int));
	if(legal_values == NULL){
		memory_alloc_problem();
	}

	while(num_of_iteration < GENERATE_MAX_ITERATIONS){
		need_new_iteration = 0;
		cnt = 0;

		copy_of_board = copy_board(current_board);

		while(cnt < x){
			num_of_legal_val = 0;
			cnt2 = 0;
			set_array_zero(legal_values, N);
			empty_cells_count = num_empty_cells(copy_of_board);
			r = rand() % empty_cells_count; /*choose random empty cell*/
			find_empty_cell(copy_of_board, &empty_cell_row, &empty_cell_col, r, N);/*find this cell*/
			cell_legal_values(copy_of_board, legal_values, N, empty_cell_row, empty_cell_col);

			for(i = 0; i < N; i++){ /*count legal values in cell*/
				if(legal_values[i] != 0){
					num_of_legal_val++;
				}
			}

			if(num_of_legal_val == 0){ /*no legal values*/
				need_new_iteration = 1;
				break;
			}
			else{
				random_legal = rand() % num_of_legal_val; /*choose random legal value*/
				for (k = 0; k < N; k++){ /*find him*/
					if(legal_values[k] != 0){
						cnt2++;
					}
					if(cnt2 - 1 == random_legal){
						copy_of_board->board[empty_cell_row][empty_cell_col].value = k + 1;
						break;
					}
				}
			}
			cnt++;
		}

		if(need_new_iteration == 1){
			free_board_mem(copy_of_board);
			num_of_iteration++;
		}
		else{
			if(gurobi_main_ILP(copy_of_board, 1) != 1){/*no solution found*/
				free_board_mem(copy_of_board);
				num_of_iteration++;
			}
			else{
				keep_y_cells(copy_of_board, y);
				list_push(current_game->undo_redo_list, copy_of_board);
				free(legal_values);
				print_board((board*)current_game->undo_redo_list->head->data, current_game);
				return;
			}
		}
	}

	printf("Error: generator problem, could not generate the board\n");
	free(legal_values);
	return;
}


void handle_undo_redo_command(game* current_game, int command){
	list* current_board_list = current_game->undo_redo_list;
	board* board_before_command = (board*)current_board_list->head->data;
	board* board_after_command;

	if(command == E_UNDO_CMD){ /*undo command*/
		if(current_board_list->head->prev == NULL){ /*check if there is a undo move*/
			printf("Error: there are no moves to undo\n");
			return;
		}
		board_after_command = (board*)current_board_list->head->prev->data;
		current_board_list->head = current_board_list->head->prev;
	}
	else { /*redo command*/
		if(current_board_list->head->next == NULL){ /*check if there is a redo move*/
			printf("Error: there are no moves to redo\n");
			return;
		}
		board_after_command = (board*)current_board_list->head->next->data;
		current_board_list->head = current_board_list->head->next;
	}

	print_changes_boards(board_before_command, board_after_command);

	print_board((board*)current_board_list->head->data, current_game);
	return;
}

void handle_save_command(game* current_game, char* path){
	board* current_board = (board*)current_game->undo_redo_list->head->data;

	if(gurobi_main_ILP(current_board, 0) != 1 && current_game->mode == edit){/* in edit mode boards without a solution may not be saved.*/
		printf("Error: board in unsolvable. Boards without a solution may not be saved in edit mode\n");
		return;
	}

	save_game_to_file(current_game, path);
}

void handle_hint_and_ghint_command(game* current_game, int row, int col, int command){
	int return_val;
	board* current_board = current_game->undo_redo_list->head->data;
	board* copy_of_board;
	double* cell_prob_arr = NULL;
	int i;
	int N = current_board->n * current_board->m;
	cell_probability* prob_struct = NULL;

	copy_of_board = copy_board(current_board);

	if(command == E_HINT_CMD){ /*hint command*/
		return_val = gurobi_main_ILP(copy_of_board, 1);
	}

	else {/*guess_hint_commant*/
		cell_prob_arr = (double*)calloc(N, sizeof(double));
		if(cell_prob_arr == NULL){
			memory_alloc_problem();
		}

		prob_struct = (cell_probability*)calloc(1, sizeof(struct cell_probability));
		if(prob_struct == NULL){
			memory_alloc_problem();
		}

		init_cell_prob_struct(prob_struct, cell_prob_arr, row, col);
		return_val = gurobi_main_LP(copy_of_board, 0.0, prob_struct);
	}


	if(return_val != 1){
		printf("Error: Board is unsolvable\n");
		free_board_mem(copy_of_board);

		if(command == E_GUESS_HINT_CMD){
			free(prob_struct);
			free(cell_prob_arr);
		}
		return;
	}
	else{
		if(command == E_HINT_CMD){
			printf("Hint: set cell (%d,%d) to %d\n", col + 1, row + 1, copy_of_board->board[row][col].value);
		}
		
		else{
			i = 0;
			for(i = 0; i < N; i++){
				if (prob_struct->probability[i] > 0){
					printf("Value is: %d, Score is: %f\n", i + 1, prob_struct->probability[i]);
				}
			}
			free(cell_prob_arr);
			free(prob_struct);
		}
		free_board_mem(copy_of_board);
		return;
	}
}

void handle_num_solutions_command(game* current_game){
	board* current_board = current_game->undo_redo_list->head->data;
	board* copy_of_board;
	int num_sol = 0;

	copy_of_board = copy_board(current_board);
	num_sol = exhaustive_backtracking_solution_count(copy_of_board);
	if(num_sol == 0){
		printf("No solution found to the current board\n");
	}
	else if(num_sol > 0){
		printf("The number of found solutions is %d\n", num_sol);
	}
	else{
		printf("Error: backtracking failed\n");
	}
	free_board_mem(copy_of_board);
	return;
}

void handle_autofill_command(game* current_game){
	board* current_board = current_game->undo_redo_list->head->data;
	int N = current_board->m*current_board->n;
	int i,j,k;
	int* legal_values;
	int num_legal_val;
	board* copy_of_board;
	int val_to_fill = 0;

	legal_values = (int*)calloc(N, sizeof(int));
	if(legal_values == NULL){
		memory_alloc_problem();
	}

	copy_of_board = copy_board(current_board);

	for(i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			num_legal_val = 0;
			set_array_zero(legal_values, N);/*all cells are not allowed at first*/

			if(current_board->board[i][j].value == 0){ /*if cell is empty*/
				cell_legal_values(current_board, legal_values, N, i, j);
				for(k = 0; k < N; k++){
					if(legal_values[k] == 1){
						num_legal_val++;
						val_to_fill = k;
					}
				}

				if(num_legal_val == 1){/*only one legal value*/
					copy_of_board->board[i][j].value = val_to_fill + 1;
				}
			}
		}
	}
	
	print_changes_boards(current_board, copy_of_board);
	update_error_cells(copy_of_board);
	list_push(current_game->undo_redo_list, copy_of_board);
	print_board(current_game->undo_redo_list->head->data, current_game);

	if(board_is_full(current_game->undo_redo_list->head->data)){
		if(board_is_completed((board*)current_game->undo_redo_list->head->data) == true){
			current_game->mode = init;
		}
	}
	free(legal_values);
	return;
}

void handle_reset_command(game* current_game){
	list* undo_redo_list = current_game->undo_redo_list;

	while(undo_redo_list->head->prev != NULL){
		undo_redo_list->head = undo_redo_list->head->prev;
	}
	printf("Board was reset\n");
	print_board(current_game->undo_redo_list->head->data, current_game);
	return;
}

void handle_exit_command(game* current_game){
	free_game_mem(current_game);
	return;
}
