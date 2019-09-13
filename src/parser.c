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

#define UNUSED(x) (void)(x);

struct command_desc;

typedef void (* handler_t)(struct command_desc* pcommand_descriptor, char** argv, int argc, game* current_game);

typedef struct command_desc{
	char *command_string;
	char *command_format;
	command_e command;
	int min_number_of_params;
	int max_number_of_params;
	int modes;
	handler_t handler;
}command_descriptor_t;


void parse_solve_command(command_descriptor_t* pcommand_descriptor, char** argv, int argc, game* current_game);

void parse_edit_command(command_descriptor_t* pcommand_descriptor, char** argv, int argc, game* current_game);

void parse_mark_errors_command(command_descriptor_t* pcommand_descriptor, char** argv, int argc, game* current_game);

void parse_print_board_command(command_descriptor_t* pcommand_descriptor, char** argv, int argc, game* current_game);

void parse_set_command(struct command_desc* pcommand_descriptor, char** argv, int argc, game* current_game);

void parse_validate_command(struct command_desc* pcommand_descriptor, char** argv, int argc, game* current_game);

void parse_guess_command(struct command_desc* pcommand_descriptor, char** argv, int argc, game* current_game);

void parse_generate_command(struct command_desc* pcommand_descriptor, char** argv, int argc, game* current_game);

void parse_undo_command(struct command_desc* pcommand_descriptor, char** argv, int argc, game* current_game);

void parse_redo_command(struct command_desc* pcommand_descriptor, char** argv, int argc, game* current_game);

void parse_save_command(struct command_desc* pcommand_descriptor, char** argv, int argc, game* current_game);

void parse_hint_command(struct command_desc* pcommand_descriptor, char** argv, int argc, game* current_game);

void parse_guess_hint_command(struct command_desc* pcommand_descriptor, char** argv, int argc, game* current_game);

void parse_num_solutions_command(struct command_desc* pcommand_descriptor, char** argv, int argc, game* current_game);

void parse_autofill_command(struct command_desc* pcommand_descriptor, char** argv, int argc, game* current_game);

void parse_reset_command(struct command_desc* pcommand_descriptor, char** argv, int argc, game* current_game);

void parse_exit_command(struct command_desc* pcommand_descriptor, char** argv, int argc, game* current_game);


static command_descriptor_t commands[] = {
		{"solve","solve X", E_SOLVE_CMD, 1, 1, (init | edit | solve), parse_solve_command},
		{"edit","edit X", E_EDIT_CMD, 0, 1, (init | edit | solve), parse_edit_command},
		{"mark_errors","mark_errors X", E_MARK_ERRORS_CMD, 1, 1, (solve), parse_mark_errors_command},
		{"print_board", "print_board", E_PRINT_BOARD_CMD, 0, 0, (edit | solve), parse_print_board_command},
		{"set", "set X Y Z", E_SET_CMD, 3, 3, (edit | solve), parse_set_command},
		{"validate", "validate", E_VALIDATE_CMD, 0, 0, (edit | solve), parse_validate_command},
		{"guess", "guess X", E_GUESS_CMD, 1, 1, (solve), parse_guess_command},
		{"generate", "generate X Y", E_GENERATE_CMD, 2, 2, (edit), parse_generate_command},
		{"undo", "undo", E_UNDO_CMD, 0, 0, (edit | solve), parse_undo_command},
		{"redo", "redo", E_REDO_CMD, 0, 0, (edit | solve), parse_redo_command},
		{"save", "save X", E_SAVE_CMD, 1, 1, (edit | solve), parse_save_command},
		{"hint", "hint X Y", E_HINT_CMD, 2, 2, (solve), parse_hint_command},
		{"guess_hint", "guess_hint X Y", E_GUESS_HINT_CMD, 2, 2, (solve), parse_guess_hint_command},
		{"num_solutions", "num_solutions", E_NUM_SOLUTIONS_CMD, 0, 0, (edit | solve), parse_num_solutions_command},
		{"autofill", "autofill", E_AUTOFILL_CMD, 0, 0, (solve), parse_autofill_command},
		{"reset", "reset", E_RESET_CMD, 0, 0, (edit | solve), parse_reset_command},
		{"exit","exit", E_EXIT_CMD, 0, 0, (init | edit | solve), parse_exit_command},
};

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

void clean(char** argv,int argc){
	int i;
	for(i = 0; i < argc; i++){
		free(argv[i]);
	}
}

int find_command_and_params(char* command, command_descriptor_t** command_descriptor, char** argv, int* argc){
	char* command_name;
	char* param;
	int j = 0;
	int unsigned i;
	command_name = strtok(command, delimitor);

	for (i = 0; i < sizeof(commands) / sizeof(command_descriptor_t); i++){
		if(strcmp(command_name, commands[i].command_string) == 0){
			*command_descriptor = &commands[i];
		}
	}

	if(*command_descriptor == NULL){
		printf("This command does not exists\n");
		return false;
	}

	param = strtok(NULL, delimitor);

	while(param != NULL){
		argv[j] = malloc(strlen(param) + 1);
		strcpy(argv[j], param);
		j++;
		param = strtok(NULL, delimitor);
	}

	*argc = j;
	return true;
}


int parse_command(char* command, game* current_game){
	command_descriptor_t* command_descriptor = NULL;
	char* argv[COMMAND_SIZE];
	int argc;

	if(find_command_and_params(command, &command_descriptor, argv, &argc) == false){
		return 0;
	}


	if((current_game->mode & command_descriptor->modes) == 0){
		printf("Error: %s command is only available in %s mode\n", command_descriptor->command_string, mode_for_message[command_descriptor->modes]);
		clean(argv, argc);
		return 0;
	}

	if(command_descriptor->min_number_of_params > argc){
		printf("Error: some parameters are missing in %s command, right format: %s.\n", command_descriptor->command_string, command_descriptor->command_format);
		clean(argv, argc);
		return 0;
	}
	else if(command_descriptor->max_number_of_params < argc){
		printf("Error: there are too many parameters in %s command, right format: %s.\n", command_descriptor->command_string, command_descriptor->command_format);
		clean(argv, argc);
		return 0;
	}

	command_descriptor->handler(command_descriptor, argv, argc, current_game);

	if(command_descriptor->command == E_EXIT_CMD){
		clean(argv, argc);
		return -1;
	}
	else{
		clean(argv, argc);
		return 0;
	}
}


void parse_solve_command(command_descriptor_t* pcommand_descriptor, char** argv, int argc, game* current_game){
	UNUSED(pcommand_descriptor);
	UNUSED(argc);

	handle_solve_command(current_game, argv[0]);

	return;
}

void parse_edit_command(command_descriptor_t* pcommand_descriptor, char** argv, int argc, game* current_game){
	UNUSED(argc);
	UNUSED(pcommand_descriptor);

	if(argc == 1){
		handle_edit_command(current_game, argv[0]);
	}
	else{
		handle_edit_command(current_game, NULL);
	}
	return;
}

void parse_mark_errors_command(command_descriptor_t* pcommand_descriptor, char** argv, int argc, game* current_game){
	int x;
	UNUSED(argc);
	UNUSED(pcommand_descriptor);

	x = atoi(argv[0]);
	if((x == 0 && strcmp(argv[0], "0") != 0) || (x != 0 && x != 1)){
		printf("Error: parameter is invalid, should be 0 or 1\n");
		return;
	}
	handle_mark_errors_command(current_game, x);
	return;
}

void parse_print_board_command(command_descriptor_t* pcommand_descriptor, char** argv, int argc, game* current_game){
	UNUSED(argc);
	UNUSED(argv);
	UNUSED(pcommand_descriptor);

	handle_print_board_command(current_game);
	return;
}

void parse_set_command(command_descriptor_t* pcommand_descriptor, char** argv, int argc, game* current_game){
	int x, y, z;
	board* current_board = current_game->undo_redo_list->head->data;
	int N = current_board->m * current_board->n;

	UNUSED(argc);
	UNUSED(pcommand_descriptor);

	x = atoi(argv[0]);
	y = atoi(argv[1]);
	z = atoi(argv[2]);

	if(check_atoi_error("X", x, argv[0]) || check_atoi_error("Y", y, argv[1])
		|| check_atoi_error("Z", z, argv[2])){
		return;
	}

	if(!check_range("X", x, 1, N) || !check_range("Y", y, 1, N) || !check_range("Z", z, 0, N)){
		return;
	}

	/*in solve mode user can't change fixed cells*/
	if(current_game->mode == solve){
		if(current_board->board[y - 1][x - 1].is_fixed == true){
			printf("Error: this cell is fixed and cannot be changed.\n");
			return;
	}

	handle_set_command(current_game, y - 1, x - 1, z);
	return;
}

void parse_validate_command(command_descriptor_t* pcommand_descriptor, char** argv, int argc, game* current_game){
	board* current_board = current_game->undo_redo_list->head->data;

	UNUSED(argc);
	UNUSED(argv);
	UNUSED(pcommand_descriptor);

	if(board_is_erroneous(current_board) == true){
		printf("Error: the board is erroneous\n");
		return;
	}
	handle_validate_command(current_game);
	return;
}

void parse_guess_command(command_descriptor_t* pcommand_descriptor, char** argv, int argc, game* current_game){
	board* current_board = current_game->undo_redo_list->head->data;
	float threshold;

	UNUSED(argc);
	UNUSED(pcommand_descriptor);

	threshold = atof(argv[0]);
	if(threshold == 0.0 && strcmp(argv[0], "0") != 0){
		printf("Error: the given threshold is invalid, should be a number.\n");
		return;
	}
	if(threshold > 1.0 || threshold < 0.0){
		printf("Error: the given threshold is invalid, should be a number between 0 and 1. format: guess X\n");
		return;
	}
	if(board_is_erroneous(current_board)){
		printf("board is erroneous\n");
		return;
	}
	handle_guess_command(current_game, threshold);
	return;
}

void parse_generate_command(command_descriptor_t* pcommand_descriptor, char** argv, int argc, game* current_game){

	board* current_board = current_game->undo_redo_list->head->data;
	int x, y;
	int N = current_board->m * current_board->n;

	UNUSED(argc);
	UNUSED(pcommand_descriptor);

	x = atoi(argv[0]);
	y = atoi(argv[1]);

	if(check_atoi_error("X", x, argv[0]) || check_atoi_error("Y", y, argv[1])){
		return;
	}
	if(check_range("X", x, 0, N*N) == false || check_range("Y", y, 0, N*N) == false){
		return;
	}

	handle_generate_command(current_game, x, y);
	return;
}

void parse_undo_command(command_descriptor_t* pcommand_descriptor, char** argv, int argc, game* current_game){
	UNUSED(argc);
	UNUSED(argv);
	UNUSED(pcommand_descriptor);

	handle_undo_redo_command(current_game, E_UNDO_CMD);
}

void parse_redo_command(command_descriptor_t* pcommand_descriptor, char** argv, int argc, game* current_game){
	UNUSED(argc);
	UNUSED(argv);
	UNUSED(pcommand_descriptor);

	handle_undo_redo_command(current_game, E_REDO_CMD);
}

void parse_save_command(command_descriptor_t* pcommand_descriptor, char** argv, int argc, game* current_game){
	board* current_board = current_game->undo_redo_list->head->data;

	UNUSED(argc);
	UNUSED(pcommand_descriptor);

	if(current_game->mode == edit){
		if(board_is_erroneous(current_board)){
			printf("Erroneous boards are not saved in edit mode\n");
			return;
		}
	}
	handle_save_command(current_game, argv[0]);
	return;
}

void parse_hint_command(command_descriptor_t* pcommand_descriptor, char** argv, int argc, game* current_game){
	board* current_board = current_game->undo_redo_list->head->data;
	int x,y;
	int N = current_board->m * current_board->n;

	UNUSED(argc);
	UNUSED(pcommand_descriptor);

	x = atoi(argv[0]);
	y = atoi(argv[1]);

	if(check_atoi_error("X", x, argv[0]) || check_atoi_error("Y", y, argv[1])){
		return;
	}

	if(!check_range("X", x, 1, N) || !check_range("Y", y, 1, N)){
		return;
	}

	if(board_is_erroneous(current_board) == true){
		printf("Error: board is erroneous\n");
		return;
	}

	if(current_board->board[y - 1][x - 1].is_fixed == 1){
		printf("Error: the provided cell is fixed\n");
		return;
	}

	if(current_board->board[y - 1][x - 1].value != 0){
		printf("Error: the provided cell contains a value already\n");
		return ;
	}

	handle_hint_and_ghint_command(current_game, y - 1, x - 1, E_HINT_CMD);
	return;
}

void parse_guess_hint_command(command_descriptor_t* pcommand_descriptor, char** argv, int argc, game* current_game){
	board* current_board = current_game->undo_redo_list->head->data;
	int x,y;
	int N = current_board->m * current_board->n;

	UNUSED(argc);
	UNUSED(pcommand_descriptor);

	x = atoi(argv[0]);
	y = atoi(argv[1]);

	if(check_atoi_error("X", x, argv[0]) || check_atoi_error("Y", y, argv[1])){
		return;
	}

	if(!check_range("X", x, 1, N) || !check_range("Y", y, 1, N)){
		return;
	}

	if(board_is_erroneous(current_board) == true){
		printf("Error: board is erroneous\n");
		return;
	}

	if(current_board->board[y - 1][x - 1].is_fixed == 1){
		printf("Error: the provided cell is fixed\n");
		return;
	}

	if(current_board->board[y - 1][x - 1].value != 0){
		printf("Error: the provided cell contains a value already\n");
		return ;
	}

	handle_hint_and_ghint_command(current_game, y - 1, x - 1, E_GUESS_HINT_CMD);
	return;
}

void parse_num_solutions_command(command_descriptor_t* pcommand_descriptor, char** argv, int argc, game* current_game){
	board* current_board = current_game->undo_redo_list->head->data;

	UNUSED(argc);
	UNUSED(argv);
	UNUSED(pcommand_descriptor);

	if(board_is_erroneous(current_board) == true){
		printf("Error: board is erroneous\n");
		return;
	}

	handle_num_solutions_command(current_game);
	return;
}


void parse_autofill_command(command_descriptor_t* pcommand_descriptor, char** argv, int argc, game* current_game){
	board* current_board = current_game->undo_redo_list->head->data;

	UNUSED(argc);
	UNUSED(argv);
	UNUSED(pcommand_descriptor);

	if(board_is_erroneous(current_board) == true){
		printf("Error: board is erroneous\n");
		return;
	}

	handle_autofill_command(current_game);
	return;
}

void parse_reset_command(command_descriptor_t* pcommand_descriptor, char** argv, int argc, game* current_game){
	UNUSED(argc);
	UNUSED(argv);
	UNUSED(pcommand_descriptor);

	handle_reset_command(current_game);
}

void parse_exit_command(command_descriptor_t* pcommand_descriptor, char** argv, int argc, game* current_game){
	UNUSED(argc);
	UNUSED(argv);
	UNUSED(pcommand_descriptor);

	handle_exit_command(current_game);
}


