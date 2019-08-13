/*
 * command.c
 *
 *  Created on: Aug 12, 2019
 *      Author: dan
 */

#include "commands.h"
#include <stdlib.h>

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

		if(new_node == NULL){
			memory_alloc_problem();
		}

		if(list_push(current_game->undo_redo_list, new_node) != true){
			printf("Error: problem while pushing to list\n");
			return -1;
		}
	}
	return 0;
}
