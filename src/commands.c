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


bool check_range(char* param, int num_to_check, int lower_bound, int upper_bound){
	if(num_to_check < lower_bound || num_to_check > upper_bound){
		printf("Error: parameter %s is not in range, should be in range %d - %d\n",param, lower_bound, upper_bound);
		return false;
	}
	return true;
}

int handle_solve_command(game* current_game, char* path){
	return load_game_from_file(current_game, path);
}

int handle_edit_command(game* current_game, char* path){
	board* new_board;
	Node* new_node;

	if(path != NULL){ /*there is a path (optional parameter)*/
		return load_game_from_file(current_game, path);
	}
	else{
		new_board = create_board(3, 3);
		new_node = create_node(new_board);
		list_push(current_game->undo_redo_list, new_node);
	}
	return 0;
}

int handle_set_command(game* current_game, char* parameters[3]){
	board* current_board = current_game->undo_redo_list->head->data;
	board* copy_of_board;
	int N = current_board->m*current_board->n;

	int x = atoi(parameters[0]);
	int y = atoi(parameters[1]);
	int z = atoi(parameters[2]);

	if(x == 0 && strcmp(parameters[0], "0") != 0){ /*convert problem, not a number*/
		printf("Error: parameter X is invalid, should be a number in range 1 - %d\n",N);
		fflush(stdout);
		return -1;
	}

	if(y == 0 && strcmp(parameters[1], "0") != 0){ /*convert problem, not a number*/
		printf("Error: parameter Y is invalid, should be a number in range 1 - %d\n",N);
		fflush(stdout);
		return -1;
	}

	if(z == 0 && strcmp(parameters[2], "0") != 0){ /*convert problem, not a number*/
		printf("Error: parameter Z is invalid, should be a number in range 1 - %d\n",N);
		fflush(stdout);
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

int handle_validate_command(game* current_game){
	board* board = current_game.undo_redo_list.head.data
	/*
	 * return ILP_func(); //guy needs to write//
	 */
	return 0;
}

int handle_guess_command(game* current_game, char* thresh){
	float threshold = atof(thresh);
	if(threshold == 0.0 && !strcmp(thresh, "0")){
		print_flush("Error: threshold X is invalid, should be a number\n");
		return -1;
	}

}
