/*
 * parser.h
 *
 *  Created on: Aug 12, 2019
 *      Author: dan
 */

#ifndef PARSER_H_
#define PARSER_H_

#include "structures.h"
/*
 * this module is in charge of parsing the given command from the user.
 */


#define SOLVE_NUM_PARAMS 1
#define EDIT_NUM_PARAMS 1
#define MARK_ERRORS_NUM_PARAMS 1
#define PRINT_BOARD_NUM_PARAMS 0
#define SET_NUM_PARAMS 3
#define VALIDATE_NUM_PARAMS 0
#define GUESS_NUM_PARAMS 1
#define GENERATE_NUM_PARAMS 2
#define UNDO_NUM_PARAMS 0
#define REDO_NUM_PARAMS 0
#define SAVE_NUM_PARAMS 1
#define HINT_NUM_PARAMS 2
#define GUESS_HINT_NUM_PARAMS 2
#define NUM_SOLUTIONS_NUM_PARAMS 0
#define AUTOFILL_NUM_PARAMS 0
#define RESET_NUM_PARAMS 0
#define EXIT_NUM_PARAMS 0

#define SOLVE_MODES (init | edit | solve)
#define EDIT_MODES (init | edit | solve)
#define MARK_ERRORS_MODES (solve)
#define PRINT_BOARD_MODES (edit | solve)
#define SET_MODES (edit | solve)
#define VALIDATE_MODES (edit | solve)
#define GUESS_MODES (solve)
#define GENERATE_MODES (edit)
#define UNDO_MODES (edit | solve)
#define REDO_MODES (edit | solve)
#define SAVE_MODES (edit | solve)
#define HINT_MODES (solve)
#define GUESS_HINT_MODES (solve)
#define NUM_SOLUTIONS_MODES (edit | solve)
#define AUTOFILL_MODES (solve)
#define RESET_MODES (edit | solve)
#define EXIT_MODES (edit | solve)

/*
 * checks if command is in the right mode.
 * returns true if it is, and flase otherwise.
 */
int right_mode(game* current_game, int modes_availabilty, char* command_name);

/*
 * checks if the given command has the right amount of parameters.
 * filling the parameters array with the given parameters.
 * returns false if number of parameters is not right.
 */
int check_num_params(int number_of_params, char* command, char* format, char* parameters[MAX_PARAMETERS]);

/*
 * parse the given command and sends it to the check_call_func.
 * returns -1 if needs to exit, 0 otherwise.
 */
int command_parser(char* command, game* current_game);

#endif /* PARSER_H_ */
