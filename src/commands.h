/*
 * commands.h
 *
 *  Created on: Aug 12, 2019
 *      Author: dan
 */

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include "structures.h"
#include "main_aux.h"

#define GENERATE_MAX_ITERATIONS 1000


/*
 * this module is in charge of handling the different commands
 */


/*
 * handle the solve command, starts a puzzle in solve mode, loaded from file.
 */
void handle_solve_command(game* current_game, char* path);

/*
 * handle the edit command, starts a puzzle in edit mode, loaded from file.
 */
void handle_edit_command(game* current_game, char* path);

/*
 * handle the mark_errors command, sets the mark errors setting to X.
 */

void handle_mark_errors_command(game* current_game, int setting);
/*
 * handle the print_board command, prints the board to the user.
 */
void handle_print_board_command(game* current_game);

/*
 * handle the set command, sets the value of cell <X,Y> to Z.
 */
void handle_set_command(game* current_game,int row, int col, int new_value);

/*
 * handle the validate command, validates the current board using ILP.
 */
void handle_validate_command(game* current_game);

/*
 * handle the guess command, guesses a solution to the current board using LP.
 */
void handle_guess_command(game* current_game, float threshold);

/*
 * handle the generate command, generates a puzzle by randomly filling x empty
 * cells with legal value.
 */
void handle_generate_command(game* current_game, int x, int y);

/*
 * handle the undo command and the redo commands.
 * undos or redos the previous move done by the user.
 */
void handle_undo_redo_command(game* current_game, int command);

/*
 * handle the save command, saves the current game board to a specified file.
 * returns 0 in success, -1 otherwise.
 */
void handle_save_command(game* current_game, char* path);

/*
 * handle the hint command and the guess hint command.
 * hint -  give a hint to the user by showing a solution of a single cell x,y.
 * guessHint - shows a guess to the user for single cell x,y
 */
void handle_hint_and_ghint_command(game* current_game, int row, int col,int command);

/*
 * handle the num_solutions command, prints the number of solutions
 * for the current board.
 */
void handle_num_solution_command(game* current_game);

/*
 * handle the autofill command, automatically fill "obvious values
 */
void handle_autofill_command(game* current_game);

/*
 * handle the reset command undo all moves.
 */
void handle_reset_command(game* current_game);

/*
 * handle the exit command, exits the program. (returns -1 if exits successfully, otherwise
 */
void handle_exit_command(game* current_game);

#endif /* COMMANDS_H_ */
