/*
 * parser.h
 *
 *  Created on: Aug 12, 2019
 *      Author: dan
 */

#ifndef PARSER_H_
#define PARSER_H_

/*
 * this module is in charge of parsing the given command from the user.
 */


#define SOLVE_NUM_PARAMS 1
#define edit_NUM_PARAMS 0
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



/*
 * prints an error of missing parameters and shows the right format
 */
void param_missing_err(char* command,char* format);

/*
 * prints an error of too many parameters and shows the right format
 */
void too_many_param_err(char* command,char* format);

/*
 * parse the given command and commit the right handler.
 * returns -1 if needs to exit, 0 otherwise.
 */
int command_parser(char* command, game* current_game);

#endif /* PARSER_H_ */
