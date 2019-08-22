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


int handle_solve_command(game* current_game, char* parameters[MAX_PARAMETERS]){
	current_game->mode = solve;
	return load_game_from_file(current_game, parameters[0]);
}

int handle_edit_command(game* current_game, char* parameters[MAX_PARAMETERS]){
	board* new_board;
	Node* new_node;
	current_game->mode = edit;

	if(parameters[0] != NULL){ /*there is a path (optional parameter)*/
		return load_game_from_file(current_game, parameters[0]);
	}
	else{
		new_board = create_board(3, 3);
		new_node = create_node(new_board);
		list_push(current_game->undo_redo_list, new_node);
	}
	return 0;
}

int handle_mark_errors_command(game* current_game, char* parameters[MAX_PARAMETERS]){
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

int handle_print_board_command(game* current_game, char* parameters[MAX_PARAMETERS]){
	print_board((board*)current_game->undo_redo_list->head->data, current_game);
	return 0;
}

int handle_set_command(game* current_game, char* parameters[MAX_PARAMETERS]){
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
		if(board_is_full(current_board)){

			/*if(check_board(current_board) == true){ //guy needs to write//
				print_flush("Puzzle was solved successfully! Congratulations!\n");
				current_game->mode = init;
			}
			else{
				print_flush("The solution you provided is erroneous, you can use the undo command\n");
			}*/
		}
	}
	return 0;
}

int handle_validate_command(game* current_game, char* parameters[MAX_PARAMETERS]){
	if(board_is_erroneous((board*)current_game->undo_redo_list->head->data) == true){
		print_flush("Error: the board is erroneous\n");
		return -2;
	}
	/*
	 * if(!return ILP_func()){
			print_flush("The board is unsolvable!\n");
			return 0;
		else{
		print_flush("The board is solvable!\n");
		return 0;
		} //guy needs to write//
	 */
	return 0;
}

//int handle_guess_command(game* current_game, parameters[MAX_PARAMETERS]){
//	board* current_board = current_game->undo_redo_list->head->data;
//	board* copy_of_board;
//	char* thresh = parameters[0];
//
//	if(num_empty_cells((board*)current_game->undo_redo_list->head->data) != atoi(parameters[0])){
//		printf("Error: the board does not contain %s empty cells.\n", parameters[0]);
//		fflush(stdout);
//		return 0;
//	}
//	return 0;
//
//	float threshold = atof(thresh);
//	if(threshold == 0.0 && !strcmp(thresh, "0")){
//		print_flush("Error: threshold X is invalid, should be a number. format: guess X\n");
//		return -1;
//	}
//
//	if(threshold > 1.0 || threshold < 0.0){
//		print_flush("Error: threshold X is invalid, should be a number between 0 and 1. format: guess\n");
//		return -1;
//	}
//
//	if(board_is_erroneous(current_board)){
//		print_flush("board is erroneous");
//		return -1;
//	}
//
//	copy_of_board = copy_board(current_board);
//
//	/*
//	 * if (LP(copy_of_board, threshold) == 1){ // guy need to write
//	 *
//	 *
//	 */
//	return 0;
//}


int handle_generate_command(game* current_game, char* parameters[MAX_PARAMETERS]){
	return 0;
}


int handle_undo_command(game* current_game, char* parameters[MAX_PARAMETERS]){
	list* undo_redo_list = current_game->undo_redo_list;
	board* board_before_undo = (board*)undo_redo_list->head->data;
	board* board_after_undo = (board*)undo_redo_list->head->prev->data;
	board* current_board = (board*)undo_redo_list->head->data;
	int N = current_board->m*current_board->n;

	/*
	 * check if there is a undo move
	 */

	if(undo_redo_list->head->prev == NULL){
		print_flush("Error, there are no moves to undo\n");
		return -1;
	}

	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			if (board_before_undo->board[i][j].value != board_after_undo->board[i][j].value){
				printf("Cell (%d, %d) was changed from value %d to value %d.\n", i + 1, j + 1, board_before_undo->board[i][j].value , board_after_undo->board[i][j].value);
			}
		}
	}
	undo_redo_list->head = undo_redo_list->head->prev;
	print_board((board*)undo_redo_list->head->data, current_game);
	return 0;
}

int handle_redo_command(game* current_game, char* parameters[MAX_PARAMETERS]){
	list* undo_redo_list = current_game->undo_redo_list;
	board* board_before_redo = (board*)undo_redo_list->head->data;
	board* board_after_redo = (board*)undo_redo_list->head->prev->data;
	board* current_board = (board*)undo_redo_list->head->data;
	int N = current_board->m*current_board->n;

	/*
	 * check if there is a undo move
	 */

	if(undo_redo_list->head->next == NULL){
		print_flush("Error, there are no moves to redo\n");
		return -1;
	}

	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			if (board_before_redo->board[i][j].value != board_after_redo->board[i][j].value){
				printf("Cell (%d, %d) was changed from value %d to value %d.\n", i + 1, j + 1, board_before_redo->board[i][j].value , board_after_redo->board[i][j].value);
			}
		}
	}
	undo_redo_list->head = undo_redo_list->head->next;
	print_board((board*)undo_redo_list->head->data, current_game);
	return 0;
}

int handle_save_command(game* current_game, char* parameters[MAX_PARAMETERS]){
	board* current_board = (board*)current_game->undo_redo_list->head->data;
	char* path = parameters[0];

	if(current_game->mode == edit){
		if(board_is_erroneous(current_board)){
			print_flush("Erroneous boards are not saved in edit mode\n");
			return -1;
		}
		/*
		 * in edit mode boards without a solution may not be saved.
		 * if(validate(current_board) == false){
		 * print_flush("unsolvable boards are not saved in edit mode\n");
		 * return -1;
		 */
	}
	return save_game_to_file(current_game, path);
}

//int handle_hint_command(game* current_game, char* cells[MAX_PARAMETERS]){
//	int x, y;
//	board* current_board = current_game->undo_redo_list->head->data;
//	board* copy_of_board;
//	int N = current_board->m*current_board->n;
//
//	x = atoi(cells[0]);
//	y = atoi(cells[1]);
//
//	if(check_atoi_error("X", x, cells[0], N) || check_atoi_error("Y", y, cells[1], N)){
//		return -1;
//	}
//
//	if(!check_range("X", x, 1, N) || !check_range("Y", y, 1, N)){
//		return -1;
//	}
//
//	if(board_is_erroneous(current_board) == true){
//		print_flush("Error: board is erroneous\n");
//		return -1;
//	}
//
//	if(current_board->board[y-1][x-1].is_fixed == 1){
//		print_flush("Error: the provided cell is fixed\n");
//		return -1;
//	}
//
//	if(current_board->board[y-1][x-1].value != 0){
//		print_flush("Error: the provided cell contains a value already\n");
//		return -1;
//	}
//
//	copy_of_board = copy_board(current_board);
//
//	/*
//	 * if(ILP(copy_board(current_board))){
//	 * 	printf("hint: the value of cell (%d,%d) is %d\n", x, y, copy_board->board[y-1][x-1].value)
//	 * 	fflush(stdout):
//	 * 	free_board_mem(copy_board);
//	 * 	return 0;
//	 *
//	 * else{
//	 * 	print_flush("The board is unsolvable\n");
//	 *
//	 */
//
//	free_board_mem(copy_board);
//	return 0;
//}

//int handle_guess_hint_command(game* current_game, char* cells[MAX_PARAMETERS]){
//	int x, y;
//	board* current_board = current_game->undo_redo_list->head->data;
//	board* copy_of_board;
//	int N = current_board->m*current_board->n;
//
//	x = atoi(cells[0]);
//	y = atoi(cells[1]);
//
//	if(check_atoi_error("X", x, cells[0], N) || check_atoi_error("Y", y, cells[1], N)){
//		return -1;
//	}
//
//	if(!check_range("X", x, 1, N) || !check_range("Y", y, 1, N)){
//		return -1;
//	}
//
//	if(board_is_erroneous(current_board) == true){
//		print_flush("Error: board is erroneous\n");
//		return -1;
//	}
//
//	if(current_board->board[y-1][x-1].is_fixed == 1){
//		print_flush("Error: the provided cell is fixed\n");
//		return -1;
//	}
//
//	if(current_board->board[y-1][x-1].value != 0){
//		print_flush("Error: the provided cell contains a value already\n");
//		return -1;
//	}
//
//	copy_of_board = copy_board(current_board);
//
//	/*
//	 * if(LP(copy_board(current_board))){
//	 * 	printf("hint: the value of cell (%d,%d) is %d\n", x, y, copy_board->board[y-1][x-1].value)
//	 * 	fflush(stdout):
//	 * 	free_board_mem(copy_board);
//	 * 	return 0;
//	 *
//	 * else{
//	 * 	print_flush("The board is unsolvable\n");
//	 *
//	 */
//
//	free_board_mem(copy_board);
//	return 0;
//}

//int handle_num_solution_command(game* current_game, char* parameters[MAX_PARAMETERS]){
//	board* current_board = current_game->undo_redo_list->head->data;
//	int counts;
//
//	if(board_is_erroneous(current_board) == true){
//		print_flush("Error: board is erroneous\n");
//		return -1;
//	}
//	/*
//	 * counts = exhaustive_backtracking(current_board);
//	 * printf("The number of found solutions is %d\n, count);
//	 * fflush(stdout);
//	 */
//	return 0;
//}

//int handle_autofill_command(game* current_game, char* parameters[MAX_PARAMETERS]){
//	board* current_board = current_game->undo_redo_list->head->data;
//	int N = current_board->m*current_board->n;
//
//	if(board_is_erroneous(current_board) == true){
//		print_flush("Error: board is erroneous\n");
//		return -1;
//	}
//
//	/*
//	 * ...
//	 */
//	return 0;
//}

int handle_reset_command(game* current_game, char* parameters[MAX_PARAMETERS]){
	list* undo_redo_list = current_game->undo_redo_list;

	while(undo_redo_list->head->prev != NULL){
		undo_redo_list->head = undo_redo_list->head->prev;
	}
	return 0;
}

int handle_exit_command(game* current_game, char* parameters[MAX_PARAMETERS]){
	free_game_mem(current_game);
	return -1;
}






