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


const char * mode_for_message[] = {
		"",
		"init",
		"edit",
		"init or edit",
		"solve",
		"init or solve",
		"edit or solve",
};

const char delimitor[8] = " \t\r\n";

void clean(char* command){
	free(command);
}

int right_mode(game* current_game, int modes_availabilty, char* command_name){
	if((current_game->mode & modes_availabilty) == 0){
		printf("Error: %s command is only available in %s\n", command_name, mode_for_message[modes_availabilty]);
		return false;
	}
	return true;
}

int check_num_params(int number_of_params, char* command, char* format, char* parameters[MAX_PARAMETERS]){
	char* too_many;
	char* param;
	char* command_name;

	command_name = strtok(command, delimitor);

	for(int i = 0; i < number_of_params; i++){
		param = strtok(NULL, delimitor);

		if(param == NULL && strcmp(command_name, "edit") != 0){
			printf("Error: some parameters are missing in %s command, right format: %s.\n", command_name, format);
			return false;
		}
		parameters[i] = param;
	}
	too_many = strtok(NULL, delimitor);
	if(too_many != NULL){
		printf("Error: there are too many parameters, right format: %s.\n", format);
		return false;
	}
	return true;
}


int command_parser(char* origin_command, game* current_game){
	char* command = calloc(strlen(origin_command)+1, sizeof(char));
	char* command_name;
	int x, y, z;
	float threshold;
	char* parameters[MAX_PARAMETERS];
	board* current_board = current_game->undo_redo_list->head->data;
	int N = current_board->m*current_board->n;

	strcpy(command, origin_command);
	command_name = strtok(origin_command, delimitor); /*get the command name*/

	if(command_name == NULL){ /*empty command*/
		clean(command);
		return 0;
	}

	/*-------------------------------solve command------------------------------*/
	else if(strcmp(command_name, "solve") == 0){

		if(check_num_params(SOLVE_NUM_PARAMS, command, "solve X", parameters) == false){
			clean(command);
			return 0;
		}

		handle_solve_command(current_game, parameters[0]);

		print_board((board*)current_game->undo_redo_list->head->data, current_game);
		clean(command);
		return 0;
	}

	/*-------------------------------edit command------------------------------*/
	else if(strcmp(command_name, "edit") == 0){

		if(check_num_params(EDIT_NUM_PARAMS, command, "edit [X]", parameters) == false){
			clean(command);
			return 0;
		}

		handle_edit_command(current_game, parameters[0]);
		print_board((board*)current_game->undo_redo_list->head->data, current_game);
		clean(command);
		return 0;
	}

	/*-------------------------------mark errors command------------------------------*/
	else if(strcmp(command_name, "mark_errors") == 0){

		if(!right_mode(current_game, MARK_ERRORS_MODES, "mark_errors")){
			clean(command);
			return 0;
		}

		if (check_num_params(MARK_ERRORS_NUM_PARAMS, command, "mark_errors X", parameters) == false){
			clean(command);
			return 0;
		}


		x = atoi(parameters[0]);
		if((x == 0 && strcmp(parameters[0], "0") != 0) || (x != 0 && x != 1)){
			printf("Error: parameter is invalid, should be 0 or 1\n");
			clean(command);
			return 0;
		}

		handle_mark_errors_command(current_game, x);
		clean(command);
		return 0;
	}

	/*-------------------------------print board command------------------------------*/
	else if(strcmp(command_name, "print_board") == 0){

		if(!right_mode(current_game, PRINT_BOARD_MODES, "print_board")){
			clean(command);
			return 0;
		}

		if(check_num_params(PRINT_BOARD_NUM_PARAMS, command, "print_board", parameters) == false){
			clean(command);
			return 0;
		}

		handle_print_board_command(current_game);
		clean(command);
		return 0;
	}

	/*-------------------------------set command------------------------------*/
	else if(strcmp(command_name, "set") == 0){

		if(!right_mode(current_game, SET_MODES, "set")){
			clean(command);
			return 0;
		}

		if(check_num_params(SET_NUM_PARAMS, command, "set X Y Z", parameters) == false){
			clean(command);
			return 0;
		}

		x = atoi(parameters[0]);
		y = atoi(parameters[1]);
		z = atoi(parameters[2]);

		if(check_atoi_error("X", x, parameters[0]) || check_atoi_error("Y", y, parameters[1])
				|| check_atoi_error("Z", z, parameters[2])){
			clean(command);
			return 0;
		}

		if(!check_range("X", x, 1, N) || !check_range("Y", y, 1, N) || !check_range("Z", z, 0, N)){
			clean(command);
			return 0;
		}

		/*in solve mode user can't change fixed cells*/
		if((current_game->mode == solve) && (current_board->board[x - 1][y - 1].is_fixed == true)){
			printf("Error: this cell is fixed and cannot be changed.\n");
			clean(command);
			return 0;
		}

		handle_set_command(current_game, y - 1, x - 1, z);
		clean(command);
		return 0;
	}


	/*-------------------------------edit command------------------------------*/
	else if(strcmp(command_name, "validate") == 0){

		if(!right_mode(current_game, VALIDATE_MODES, "validate")){
			clean(command);
			return 0;
		}

		if(check_num_params(VALIDATE_NUM_PARAMS, command, "validate", parameters) == false){
			clean(command);
			return 0;
		}

		if(board_is_erroneous(current_board) == true){
			printf("Error: the board is erroneous\n");
			clean(command);
			return 0;
		}

		handle_validate_command(current_game);
		clean(command);
		return 0;
	}

	/*-------------------------------guess command------------------------------*/
	else if(strcmp(command_name, "guess") == 0){

		if(!right_mode(current_game, GUESS_MODES, "guess")){
			clean(command);
			return 0;
		}

		if(check_num_params(GUESS_NUM_PARAMS, command, "guess X", parameters) == false){
			clean(command);
			return 0;
		}

		threshold = atof(parameters[0]);
		if(threshold == 0.0 && strcmp(parameters[0], "0") != 0){
			printf("Error: the given threshold is invalid, should be a number.\n");
			clean(command);
			return 0;
		}

		if(threshold > 1.0 || threshold < 0.0){
			printf("Error: the given threshold is invalid, should be a number between 0 and 1. format: guess X\n");
			clean(command);
			return 0;
		}

		if(board_is_erroneous(current_board)){
			printf("board is erroneous\n");
			clean(command);
			return 0;
		}

		handle_guess_command(current_game, threshold);
		clean(command);
		return 0;
	}

	/*-------------------------------generate command------------------------------*/
	else if(strcmp(command_name, "generate") == 0){
		if(!right_mode(current_game, GENERATE_MODES, "generate")){
			clean(command);
			return 0;
		}

		if(check_num_params(GENERATE_NUM_PARAMS, command, "generate X Y", parameters) == false){
			clean(command);
			return 0;
		}

		x = atoi(parameters[0]);
		y = atoi(parameters[1]);

		if(check_atoi_error("X", x, parameters[0]) || check_atoi_error("Y", y, parameters[1])){
			clean(command);
			return 0;
		}

		if(!check_range("X", x, 0, N*N) == false || !check_range("Y", y, 0, N*N)){
			clean(command);
			return 0;
		}

		handle_generate_command(current_game, x, y);
		clean(command);
		return 0;
	}

	/*-------------------------------undo command------------------------------*/
	else if(strcmp(command_name, "undo") == 0){
		if(!right_mode(current_game, UNDO_MODES, "undo")){
			clean(command);
			return 0;
		}

		if(check_num_params(UNDO_NUM_PARAMS, command, "undo", parameters) == false){
			clean(command);
			return 0;
		}

		handle_undo_redo_command(current_game, 0);
		clean(command);
		return 0;
	}

	/*-------------------------------redo command------------------------------*/
	else if(strcmp(command_name, "redo") == 0){
		if(!right_mode(current_game, REDO_MODES, "redo")){
			clean(command);
			return 0;
		}

		if(check_num_params(REDO_NUM_PARAMS, command, "redo", parameters) == false){
			clean(command);
			return 0;
		}

		handle_undo_redo_command(current_game, 1);
		clean(command);
		return 0;
	}

	/*-------------------------------save command------------------------------*/
	else if(strcmp(command_name, "save") == 0){
		if(!right_mode(current_game, SAVE_MODES, "save")){
			clean(command);
			return 0;
		}

		if(check_num_params(SAVE_NUM_PARAMS, command, "save X", parameters) == false){
			clean(command);
			return 0;
		}

		if(current_game->mode == edit){
			if(board_is_erroneous(current_board)){
				printf("Erroneous boards are not saved in edit mode\n");
				clean(command);
				return 0;
			}
		}

		handle_save_command(current_game, parameters[0]);
		clean(command);
		return 0;
	}

	/*-------------------------------hint command------------------------------*/
	else if(strcmp(command_name, "hint") == 0){
		if(!right_mode(current_game, HINT_MODES, "hint")){
			clean(command);
			return 0;
		}

		if(check_num_params(HINT_NUM_PARAMS, command, "hint X Y", parameters) == false){
			clean(command);
			return 0;
		}

		x = atoi(parameters[0]);
		y = atoi(parameters[1]);

		if(check_atoi_error("X", x, parameters[0]) || check_atoi_error("Y", y, parameters[1])){
			clean(command);
			return 0;
		}

		if(!check_range("X", x, 1, N) || !check_range("Y", y, 1, N)){
			clean(command);
			return 0;
		}

		if(board_is_erroneous(current_board) == true){
			printf("Error: board is erroneous\n");
			clean(command);
			return 0;
		}

		if(current_board->board[y - 1][x - 1].is_fixed == 1){
			printf("Error: the provided cell is fixed\n");
			clean(command);
			return 0;
		}

		if(current_board->board[y - 1][x - 1].value != 0){
			printf("Error: the provided cell contains a value already\n");
			clean(command);
			return 0;
		}

		handle_hint_and_ghint_command(current_game, y - 1, x - 1, 0);
		clean(command);
		return 0;
	}

	/*-------------------------------guess_hint command------------------------------*/
	else if(strcmp(command_name, "guess_hint") == 0){

		if(!right_mode(current_game, GUESS_HINT_MODES, "guess_hint")){
			clean(command);
			return 0;
		}

		if(check_num_params(GUESS_HINT_NUM_PARAMS, command, "guess_hint X Y", parameters) == false){
			clean(command);
			return 0;
		}

		x = atoi(parameters[0]);
		y = atoi(parameters[1]);

		if(check_atoi_error("X", x, parameters[0]) || check_atoi_error("Y", y, parameters[1])){
			clean(command);
			return 0;
		}

		if(!check_range("X", x, 1, N) || !check_range("Y", y, 1, N)){
			clean(command);
			return 0;
		}

		if(board_is_erroneous(current_board) == true){
			printf("Error: board is erroneous\n");
			clean(command);
			return 0;
		}

		if(current_board->board[y - 1][x - 1].is_fixed == 1){
			printf("Error: the provided cell is fixed\n");
			clean(command);
			return 0;
		}

		if(current_board->board[y - 1][x - 1].value != 0){
			printf("Error: the provided cell contains a value already\n");
			clean(command);
			return 0;
		}

		handle_hint_and_ghint_command(current_game, y - 1, x - 1, 0);
		clean(command);
		return 0;
	}

	/*-------------------------------num_solutions command------------------------------*/
	else if(strcmp(command_name, "num_solutions") == 0){
		if(!right_mode(current_game, NUM_SOLUTIONS_MODES, "num_solutions")){
			clean(command);
			return 0;
		}

		if(check_num_params(NUM_SOLUTIONS_NUM_PARAMS, command, "num_solutions", parameters) == false){
			clean(command);
			return 0;
		}

		if(board_is_erroneous(current_board) == true){
			printf("Error: board is erroneous\n");
			clean(command);
			return 0;
		}

		handle_num_solution_command(current_game);
		clean(command);
		return 0;
	}


	/*-------------------------------autofill command------------------------------*/
	else if(strcmp(command_name, "autofill") == 0){
		if(!right_mode(current_game, AUTOFILL_MODES, "autofill")){
			clean(command);
			return 0;
		}

		if(check_num_params(AUTOFILL_NUM_PARAMS, command, "autofill", parameters) == false){
			clean(command);
			return 0;
		}

		if(board_is_erroneous(current_board) == true){
			printf("Error: board is erroneous\n");
			clean(command);
			return 0;
		}

		handle_autofill_command(current_game);
		clean(command);
		return 0;
	}


	/*-------------------------------reset command------------------------------*/
	else if(strcmp(command_name, "reset") == 0){
		if(!right_mode(current_game, RESET_MODES, "reset")){
			clean(command);
			return 0;
		}

		if(check_num_params(RESET_NUM_PARAMS, command, "reset", parameters) == false){
			clean(command);
			return 0;
		}

		handle_reset_command(current_game);
		clean(command);
		return 0;
	}

	/*-------------------------------exit command------------------------------*/
	else if(strcmp(command_name, "exit") == 0){
		if(!right_mode(current_game, EXIT_MODES, "exit")){
			clean(command);
			return 0;
		}

		if(check_num_params(EXIT_NUM_PARAMS, command, "exit", parameters) == false){
			clean(command);
			return 0;
		}

		handle_exit_command(current_game);
		clean(command);
		return -1;
	}

	/*-------------------------------another command------------------------------*/
	else{
		printf("This command does not exists\n");
	}
	clean(command);
	return 0;
}

