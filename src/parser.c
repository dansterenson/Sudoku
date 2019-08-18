/*
 * parser.c
 *
 *  Created on: Aug 12, 2019
 *      Author: dan
 */

#include <stdlib.h>
#include "structures.h"
#include "main_aux.h"
#include "parser.h"
#include <stdio.h>
#include "commands.h"
#include <string.h>

bool check_param_missing(char* command, char* next_param, char* format){
	if(next_param == NULL){
		printf("Error: some parameters are missing in %s command, right format: %s.\n", command, format);
		fflush(stdout);
		return true;
	}
	return false;
}

bool check_in_right_mode(game_modes right_mode, game* current_game){
	if(current_game->mode != right_mode){
		return false;
	}
	return true;
}

bool too_many_param(char* command, char*next_param, char* format){
	if(next_param != NULL){
		printf("Error: there are too many parameters, right format: %s %s.\n", command, format);
		fflush(stdout);
		return true;
	}
	return false;
}

int command_parser(char* command, game* current_game){
	char delimitor[8] = " t\r\n";
	char* command_name;
	char* path;
	char* parameters[3];
	char* too_many;
	board* current_board = (board*)current_game->undo_redo_list->head->data;
	game_modes previous_mode = current_game->mode;

	command_name = strtok(command, delimitor);

	if(command_name == NULL){ /*empty command*/
		return 0;
	}


	/*-------------------------------solve command------------------------------*/
	else if(strcmp(command_name, "solve") == 0){

		path = strtok(NULL, delimitor);
		if(check_param_missing("solve", path, "solve <file_path>") == true){
			return 0;
		}

		too_many = strtok(NULL, delimitor);
		if(too_many_param("solve", too_many, "solve <file_path>") == true){
			return 0;
		}

		current_game->mode = solve;
		if(handle_solve_command(current_game, path) < 0){
			current_game->mode = previous_mode;
		}
		else{
			print_board(current_board, current_game);
		}
	}


	/*-------------------------------edit command------------------------------*/
	else if(strcmp(command_name, "edit") == 0){
		path = strtok(NULL, delimitor);
		current_game->mode = edit;
		too_many = strtok(NULL, delimitor);
		if(too_many_param("edit", too_many, "edit [optional] <file_path>") == true){
			return 0;
		}
		if(handle_edit_command(current_game, path) < 0){
			current_game->mode = previous_mode;
		}
		else{
			print_board(current_board, current_game);
		}
	}


	/*-------------------------------mark errors command------------------------------*/
	else if(strcmp(command_name, "mark_errors") == 0){
		if(check_in_right_mode(solve, current_game) == false){/*not in the right mode*/
			print_flush("Error: this command is only available in solve mode\n");
			return 0;
		}

		parameters[0] = strtok(NULL, delimitor);
		if (check_param_missing("mark_errors", parameters[0], "mark_errors X[0 or 1]") == true){
			return 0;
		}

		too_many = strtok(NULL, delimitor);
		if (too_many_param("mark_errors", too_many, "mark_errors X[0 or 1]") == true){
			return 0;
		}

		if(strcmp(parameters[0], "0") == 0){
			current_game->is_mark_errors = 0;
		}

		else if(strcmp(parameters[0], "1") != 0){
			current_game->is_mark_errors = 1;
		}
		else{
			print_flush("Error: parameter is invalid, mark_errors X[0 or 1]\n");
		}
	}


	/*-------------------------------print board command------------------------------*/
	else if(strcmp(command_name, "print_board")){
		if(check_in_right_mode(solve, current_game) == false &&
				check_in_right_mode(edit ,current_game) == false){
			print_flush("Error: this command is only available in solve mode or edit mode\n");
		}

		too_many = strtok(NULL, delimitor);

		if (too_many_param("print_board", too_many, "print_board") == true){
			return 0;
		}
		print_board(current_board, current_game);
	}


	/*-------------------------------set command------------------------------*/
	else if(strcmp(command_name, "set")){
		/*check mode - not available in init mode*/
		if(check_in_right_mode(solve, current_game) == false &&
				check_in_right_mode(edit ,current_game) == false){
			print_flush("Error: this command is only available in solve mode or edit mode\n");
		}

		for(int i = 0; i < 3; i++){
			parameters[i] = strtok(NULL, delimitor);
			if(check_param_missing("set", parameters[i], "set X Y Z") == true){
				return 0;
			}
		}

		too_many = strtok(NULL, delimitor);

		if (too_many_param("set", too_many, "set X Y Z") == true){
			return 0;
		}

		if (handle_set_command(current_game, parameters) < 0){
			return 0;
		}
	}


	/*-------------------------------edit command------------------------------*/
	else if(strcmp(command_name, "validate")){
		if(check_in_right_mode(solve, current_game) == false &&
				check_in_right_mode(edit ,current_game) == false){
			print_flush("Error: this command is only available in solve mode or edit mode\n");
		}
		too_many = strtok(NULL, delimitor);

		if (too_many_param("validate", too_many, "validate") == true){
			return 0;
		}

		if(board_is_erroneous(current_board) == true){
			print_flush("Error: the board is erroneous\n");
		}

		if(handle_validate_command(current_game) < 0){
			print_flush("The board is unsolvable!\n");
			return 0;
		}
		else{
			print_flush("The board is solvable!\n");
			return 0;
		}
	}


	/*-------------------------------guess command------------------------------*/
	else if(strcmp(command_name, "guess")){
		if(check_in_right_mode(solve, current_game) == false){/*not in the right mode*/
			print_flush("Error: this command is only available in solve mode\n");
			return 0;
		}

		parameters[0] = strtok(NULL, delimitor);
		if (check_param_missing("guess", parameters[0], "guess X") == true){
			return 0;
		}


		too_many = strtok(NULL, delimitor);
		if (too_many_param("guess", too_many, "guess X") == true){
			return 0;
		}


		if(handle_guess_command(current_game, parameters[0]) < 0){

		}
	}


	/*-------------------------------generate command------------------------------*/
	else if(strcmp(command_name, "generate")){
		if(check_in_right_mode(edit, current_game) == false){/*not in the right mode*/
			print_flush("Error: this command is only available in edit mode\n");
			return 0;
		}

		for(int i = 0; i < 2; i++){
			parameters[i] = strtok(NULL, delimitor);

			if (check_param_missing("generate", parameters[i], "generate X Y") == true){
				return 0;
			}
		}

		too_many = strtok(NULL, delimitor);
		if (too_many_param("generate", too_many, "generate X Y") == true){
			return 0;
		}

		if(num_empty_cells(current_board) != parameters[0]){
			printf("Error: the board does not contain %s empty cells.\n", parameters[0]);
			fflush(stdout);
			return 0;
		}

	}


	/*-------------------------------undo command------------------------------*/
	else if(strcmp(command_name, "undo")){
		if(check_in_right_mode(solve, current_game) == false &&
				check_in_right_mode(edit ,current_game) == false){
			print_flush("Error: this command is only available in solve mode or edit mode\n");
			return 0;
		}

		too_many = strtok(NULL, delimitor);

		if (too_many_param("undo", too_many, "undo") == true){
			return 0;
		}

		if(handle_undo_command(current_game) < 0){
			return 0;
		}
		return 0;
	}


	/*-------------------------------redo command------------------------------*/
	else if(strcmp(command_name, "redo")){
		if(check_in_right_mode(solve, current_game) == false &&
				check_in_right_mode(edit ,current_game) == false){
			print_flush("Error: this command is only available in solve mode or edit mode\n");
			return 0;
		}

		too_many = strtok(NULL, delimitor);

		if (too_many_param("undo", too_many, "undo") == true){
			return 0;
		}

		if(handle_redo_command(current_game) < 0){
			return 0;
		}
		return 0;
	}


	/*-------------------------------save command------------------------------*/
	else if(strcmp(command_name, "save")){
		if(check_in_right_mode(solve, current_game) == false &&
				check_in_right_mode(edit ,current_game) == false){
			print_flush("Error: this command is only available in solve mode or edit mode\n");
			return 0;
		}

		parameters[0] = strtok(NULL, delimitor);
		if (check_param_missing("save", parameters[0], "save X") == true){
			return 0;
		}
		too_many = strtok(NULL, delimitor);
		if (too_many_param("save", too_many, "save X") == true){
			return 0;
		}

		if(handle_save_command(current_game, parameters[0]) < 0){
			return 0;
		}
		return 0;
	}


	/*-------------------------------hint command------------------------------*/
	else if(strcmp(command_name, "hint")){
		if(check_in_right_mode(solve, current_game) == false){/*not in the right mode*/
			print_flush("Error: this command is only available in solve mode\n");
			return 0;
		}

		for(int i = 0; i < 2; i++){
			parameters[i] = strtok(NULL, delimitor);
			if (check_param_missing("hint", parameters[i], "hint X Y") == true){
				return 0;
			}
		}

		too_many = strtok(NULL, delimitor);
		if (too_many_param("hint", too_many, "hint X Y") == true){
			return 0;
		}
		if(handle_hint_command(current_game, parameters[3]) < 0){
			return 0;
		}
		return 0;
	}


	/*-------------------------------guess_hint command------------------------------*/
	else if(strcmp(command_name, "guess_hint")){
		if(check_in_right_mode(solve, current_game) == false){/*not in the right mode*/
			print_flush("Error: this command is only available in solve mode\n");
			return 0;
		}

		for(int i = 0; i < 2; i++){
			parameters[i] = strtok(NULL, delimitor);
			if (check_param_missing("guess_hint", parameters[i], "guess_hint X Y") == true){
				return 0;
			}
		}

		too_many = strtok(NULL, delimitor);
		if (too_many_param("guess_hint", too_many, "guess_hint X Y") == true){
			return 0;
		}

		if(handle_guess_hint_command(current_game, parameters[3]) < 0){
			return 0;
		}
		return 0;
	}


	/*-------------------------------num_solutions command------------------------------*/
	else if(strcmp(command_name, "num_solutions")){
		if(check_in_right_mode(solve, current_game) == false &&
				check_in_right_mode(edit ,current_game) == false){
			print_flush("Error: this command is only available in solve mode or edit mode\n");
			return 0;
		}

		too_many = strtok(NULL, delimitor);
		if (too_many_param("num_solutions", too_many, "num_solutions") == true){
			return 0;
		}

		if(handle_num_solution_command(current_game) < 0){
			return 0;
		}
		return 0;
	}


	/*-------------------------------autofill command------------------------------*/
	else if(strcmp(command_name, "autofill")){
		if(check_in_right_mode(solve, current_game) == false){/*not in the right mode*/
			print_flush("Error: this command is only available in solve mode\n");
			return 0;
		}

		too_many = strtok(NULL, delimitor);
		if (too_many_param("autofill", too_many, "autofill") == true){
			return 0;
		}

		if(handle_autofill_command(current_game) < 0){
			return 0;
		}

		return 0;
	}


	/*-------------------------------reset command------------------------------*/
	else if(strcmp(command_name, "reset")){
		if(check_in_right_mode(solve, current_game) == false &&
				check_in_right_mode(edit ,current_game) == false){
			print_flush("Error: this command is only available in solve mode or edit mode\n");
			return 0;
		}

		too_many = strtok(NULL, delimitor);
		if (too_many_param("reset", too_many, "reset") == true){
			return 0;
		}

		handle_reset_command(current_game);
		return 0;
	}


	/*-------------------------------exit command------------------------------*/
	else if(strcmp(command_name, "exit")){
		too_many = strtok(NULL, delimitor);
		if (too_many_param("reset", too_many, "reset") == true){
			return 0;
		}

		free_game_mem(current_game);
		return -1;
	}


	/*-------------------------------another command------------------------------*/
	else{
		print_flush("This command does not exists\n");
	}
	return 0;
}

