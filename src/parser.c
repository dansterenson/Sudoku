/*
 * parser.c
 *
 *  Created on: Aug 12, 2019
 *      Author: dan
 */

#include <stdlib.h>
#include "structures.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>

void param_missing_err(char* command,char* format){
	print_flush("Error: some parameters are missing, %s %s.\n", command, format);
}

void too_many_param_err(char* command,char* format){
	print_flush("Error: there are too many parameters, %s %s.\n", command, format);
}

int command_parser(char* command, game* current_game){
	char delimitor[8] = " t\r\n";
	char* command_name;
	char* path;
	char* too_many;
	game_modes previous_mode = current_game->mode;

	command_name = strtok(command, delimitor);

	if(command_name == NULL){ /*empty command*/
		return 0;
	}

	if(strcmp(command_name, "solve") == 0){

		path = strtok(command, delimitor);
		if(path == NULL){
			param_missing_err("solve", "<file_path>");
			return 0;
		}

		too_many = strtok(command, delimitor);
		if(too_many != NULL){
			too_many_param_err("solve", "<file_path>");
			return 0;
		}

		current_game->mode = solve;
		if(handle_solve_command(current_game, path) < 0){
			current_game->mode = previous_mode;
		}
	}

	if(strcmp(command_name, "edit") == 0){
		path = strtok(command, delimitor);
		current_game->mode = edit;
		too_many = strtok(command, delimitor);
		if(too_many != NULL){
			too_many_param_err("edit", "[optional] <file_path>");
			return 0;
		}
		if(handle_edit_command(current_game, path) < 0){
			current_game->mode = previous_mode;
		}
	}
	return 0;
}
