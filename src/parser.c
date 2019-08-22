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


bool check_in_right_mode(game_modes right_mode, game* current_game){
	if(current_game->mode != right_mode){
		return false;
	}
	return true;
}


int check_call_func(game* current_game, char* command_name, int number_of_params,
		char* format, int (*handle_func)(game* , char**)){

	char* parameters[MAX_PARAMETERS];
	char* too_many;
	char delimitor[8] = " \t\r\n";

	for(int i = 0; i < number_of_params; i++){
		parameters[i] = strtok(NULL, delimitor);
		if(parameters[i] == NULL){
			printf("Error: some parameters are missing in %s command, right format: %s.\n", command_name, format);
			fflush(stdout);
			return -1;
		}
	}

	too_many = strtok(NULL, delimitor);
	if(too_many != NULL){
		printf("Error: there are too many parameters, right format: %s.\n", format);
		fflush(stdout);
		return -1;
	}

	return handle_func(current_game, parameters);
}

int command_parser(char* command, game* current_game){
	char delimitor[8] = " \t\r\n";
	char* command_name;
	int return_value;

	command_name = strtok(command, delimitor); /*get the command name*/

	if(command_name == NULL){ /*empty command*/
		return 0;
	}

	/*-------------------------------solve command------------------------------*/
	else if(strcmp(command_name, "solve") == 0){
		game_modes previous_mode = current_game->mode;

		return_value = check_call_func(current_game, "solve", SOLVE_NUM_PARAMS, "solve <file_path>", handle_solve_command);

		if(return_value < 0){
			current_game->mode = previous_mode;
		}
		else{
			print_board((board*)current_game->undo_redo_list->head->data, current_game);
		}
	}

	/*-------------------------------edit command------------------------------*/
	else if(strcmp(command_name, "edit") == 0){
		game_modes previous_mode = current_game->mode;

		return_value = check_call_func(current_game, "edit", edit_NUM_PARAMS, "edit [optional] <file_path>", handle_edit_command);

		if(return_value < 0){
			current_game->mode = previous_mode;
		}
		else{
			print_board((board*)current_game->undo_redo_list->head->data, current_game);
		}
	}

	/*-------------------------------mark errors command------------------------------*/
	else if(strcmp(command_name, "mark_errors") == 0){

		if(check_in_right_mode(solve, current_game) == false){/*not in the right mode*/
			print_flush("Error: this command is only available in solve mode\n");
			return 0;
		}

		return_value = check_call_func(current_game, "mark_errors", MARK_ERRORS_NUM_PARAMS, "mark_errors X[0 or 1]", handle_mark_errors_command);

	}

	/*-------------------------------print board command------------------------------*/
	else if(strcmp(command_name, "print_board") == 0){

		if(check_in_right_mode(solve, current_game) == false &&
				check_in_right_mode(edit ,current_game) == false){
			print_flush("Error: this command is only available in solve mode or edit mode\n");
		}

		return_value = check_call_func(current_game, "print_board", PRINT_BOARD_NUM_PARAMS, "print_board", handle_print_board_command);

	}

	/*-------------------------------set command------------------------------*/
	else if(strcmp(command_name, "set") == 0){
		/*check mode - not available in init mode*/
		if(check_in_right_mode(solve, current_game) == false &&
				check_in_right_mode(edit ,current_game) == false){
			print_flush("Error: this command is only available in solve mode or edit mode\n");
		}

		return_value = check_call_func(current_game, "set", SET_NUM_PARAMS, "set X Y Z", handle_set_command);
		if(return_value < 0){
			return 0;
		}
	}


	/*-------------------------------edit command------------------------------*/
	else if(strcmp(command_name, "validate") == 0){
		if(check_in_right_mode(solve, current_game) == false &&
				check_in_right_mode(edit ,current_game) == false){
			print_flush("Error: this command is only available in solve mode or edit mode\n");
		}
		return_value = check_call_func(current_game, "validate", VALIDATE_NUM_PARAMS, "validate", handle_validate_command);

		if(return_value < 0){
			return 0;
		}
	}

	/*-------------------------------guess command------------------------------*/
//	else if(strcmp(command_name, "guess") == 0){
//		if(check_in_right_mode(solve, current_game) == false){/*not in the right mode*/
//			print_flush("Error: this command is only available in solve mode\n");
//			return 0;
//		}
//
//		return_value = check_call_func(current_game, "guess", GUESS_NUM_PARAMS, "guess X", handle_guess_command);
//
//		if(return_value < 0){
//			return 0;
//		}
//	}

	/*-------------------------------generate command------------------------------*/
	else if(strcmp(command_name, "generate") == 0){
		if(check_in_right_mode(edit, current_game) == false){/*not in the right mode*/
			print_flush("Error: this command is only available in edit mode\n");
			return 0;
		}
		return_value = check_call_func(current_game, "generate", GENERATE_NUM_PARAMS, "generate X Y", handle_generate_command);

		if(return_value < 0){
			return 0;
		}
	}

	/*-------------------------------undo command------------------------------*/
	else if(strcmp(command_name, "undo") == 0){
		if(check_in_right_mode(solve, current_game) == false &&
				check_in_right_mode(edit ,current_game) == false){
			print_flush("Error: this command is only available in solve mode or edit mode\n");
			return 0;
		}

		return_value = check_call_func(current_game, "undo", UNDO_NUM_PARAMS, "undo", handle_undo_command);

		if(return_value < 0){
			return 0;
		}
	}

	/*-------------------------------redo command------------------------------*/
	else if(strcmp(command_name, "redo") == 0){
		if(check_in_right_mode(solve, current_game) == false &&
				check_in_right_mode(edit ,current_game) == false){
			print_flush("Error: this command is only available in solve mode or edit mode\n");
			return 0;
		}

		return_value = check_call_func(current_game, "redo", REDO_NUM_PARAMS, "redo", handle_redo_command);

		if(return_value < 0){
			return 0;
		}
	}

	/*-------------------------------save command------------------------------*/
	else if(strcmp(command_name, "save") == 0){
		if(check_in_right_mode(solve, current_game) == false &&
				check_in_right_mode(edit ,current_game) == false){
			print_flush("Error: this command is only available in solve mode or edit mode\n");
			return 0;
		}

		return_value = check_call_func(current_game, "save", SAVE_NUM_PARAMS, "save X", handle_save_command);

		if(return_value < 0){
			return 0;
		}
	}


	/*-------------------------------hint command------------------------------*/
//	else if(strcmp(command_name, "hint") == 0){
//		if(check_in_right_mode(solve, current_game) == false){/*not in the right mode*/
//			print_flush("Error: this command is only available in solve mode\n");
//			return 0;
//		}
//
//		return_value = check_call_func(current_game, "hint", HINT_NUM_PARAMS, "hint X Y", handle_hint_command);
//
//		if(return_value < 0){
//			return 0;
//		}
//	}

	/*-------------------------------guess_hint command------------------------------*/
//	else if(strcmp(command_name, "guess_hint") == 0){
//		if(check_in_right_mode(solve, current_game) == false){/*not in the right mode*/
//			print_flush("Error: this command is only available in solve mode\n");
//			return 0;
//		}
//
//		for(int i = 0; i < 2; i++){
//			parameters[i] = strtok(NULL, delimitor);
//			if (check_param_missing("guess_hint", parameters[i], "guess_hint X Y") == true){
//				return 0;
//			}
//		}
//		return_value = check_call_func(current_game, "guess_hint", GUESS_HINT_NUM_PARAMS, "guess_hint X Y", handle_guess_hint_command);
//
//		if(return_value < 0){
//			return 0;
//		}
//	}

	/*-------------------------------num_solutions command------------------------------*/
//	else if(strcmp(command_name, "num_solutions") == 0){
//		if(check_in_right_mode(solve, current_game) == false &&
//				check_in_right_mode(edit ,current_game) == false){
//			print_flush("Error: this command is only available in solve mode or edit mode\n");
//			return 0;
//		}
//		return_value = check_call_func(current_game, "num_solutions", NUM_SOLUTIONS_NUM_PARAMS, "num_solutions", handle_num_solutions_command);
//
//		if(return_value < 0){
//			return 0;
//		}
//	}


	/*-------------------------------autofill command------------------------------*/
//	else if(strcmp(command_name, "autofill") == 0){
//		if(check_in_right_mode(solve, current_game) == false){/*not in the right mode*/
//			print_flush("Error: this command is only available in solve mode\n");
//			return 0;
//		}
//
//		return_value = check_call_func(current_game, "autofill", AUTOFILL_NUM_PARAMS, "autofill", handle_autofill_command);
//
//		if(return_value < 0){
//			return 0;
//		}
//	}


	/*-------------------------------reset command------------------------------*/
	else if(strcmp(command_name, "reset") == 0){
		if(check_in_right_mode(solve, current_game) == false &&
				check_in_right_mode(edit ,current_game) == false){
			print_flush("Error: this command is only available in solve mode or edit mode\n");
			return 0;
		}

		return_value = check_call_func(current_game, "reset", RESET_NUM_PARAMS, "reset", handle_reset_command);

		if(return_value < 0){
			return 0;
		}
	}

	/*-------------------------------exit command------------------------------*/
	else if(strcmp(command_name, "exit") == 0){

		return_value = check_call_func(current_game, "exit", EXIT_NUM_PARAMS, "exit", handle_exit_command);

		if(return_value < 0){
			return -1;
		}
	}


	/*-------------------------------another command------------------------------*/
	else{
		print_flush("This command does not exists\n");
	}
	return 0;
}

