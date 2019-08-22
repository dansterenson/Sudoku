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


/*
 * this module is in charge of handling the different commands
 */


/*
 * handle the solve command, starts a puzzle in solve mode, loaded from file.
 * returns 0 in success, -1 otherwise.
 */
int handle_solve_command(game* current_game, char* parameters[MAX_PARAMETERS]);

/*
 * handle the edit command, starts a puzzle in edit mode, loaded from file.
 * returns 0 in success, -1 otherwise.
 */
int handle_edit_command(game* current_game, char* parameters[MAX_PARAMETERS]);

/*
 * handle the mark_errors command, sets the mark errors setting to X.
 * returns 0 in success, -1 otherwise.
 */
int handle_mark_errors_command(game* current_game, char* parameters[MAX_PARAMETERS]);

/*
 * handle the print_board command, prints the board to the user.
 * returns 0 in success, -1 otherwise.
 */
int handle_print_board_command(game* current_game, char* parameters[MAX_PARAMETERS]);

/*
 * handle the set command, sets the value of cell <X,Y> to Z.
 * returns 0 in success, -1 otherwise.
 */
int handle_set_command(game* current_game, char* parameters[MAX_PARAMETERS]);

/*
 * handle the validate command, validates the current board using ILP.
 * returns 0 in success, -1 otherwise.
 */
int handle_validate_command(game* current_game, char* parameters[MAX_PARAMETERS]);

/*
 * handle the guess command, guesses a solution to the current board using LP.
 * returns 0 in success, -1 otherwise.
 */
int handle_guess_command(game* current_game, char* parameters[MAX_PARAMETERS]);

/*
 * handle the generate command, generates a puzzle by randomly filling x empty
 * cells with legal value.
 * returns 0 in success, -1 otherwise.
 */
int handle_generate_command(game* current_game, char* parameters[3]);

/*
 * handle the undo command, undos the previous move done by the user.
 * returns 0 in success, -1 otherwise.
 */
int handle_undo_command(game* current_game, char* parameters[MAX_PARAMETERS]);

/*
 * handle the redo command, redos the move previosly undone by the user.
 * returns 0 in success, -1 otherwise.
 */
int handle_redo_command(game* current_game, char* parameters[MAX_PARAMETERS]);

/*
 * handle the save command, saves the current game board to a specified file.
 * returns 0 in success, -1 otherwise.
 */
int handle_save_command(game* current_game, char* parameters[MAX_PARAMETERS]);

/*
 * handle the hint command, give a hint to the user by showing
 * a solution of a single cell x,y.
 * returns 0 in success, -1 otherwise.
 */
int handle_hint_command(game* current_game, char* cells[MAX_PARAMETERS]);

/*
 * handle the guess_hint command, shows a guess to the user for single cell x,y.
 * returns 0 in success, -1 otherwise.
 */
int handle_guess_hint_command(game* current_game, char* cells[MAX_PARAMETERS]);

/*
 * handle the num_solutions command, prints the number of solutions
 * for the current board, returns 0 in success, -1 otherwise.
 */
int handle_num_solution_command(game* current_game, char* parameters[MAX_PARAMETERS]);

/*
 * handle the autofill command, automatically fill "obvious values
 * returns 0 in success, -1 otherwise.
 */
int handle_autofill_command(game* current_game, char* parameters[MAX_PARAMETERS]);

/*
 * handle the reset command undo all moves.
 */
int handle_reset_command(game* current_game, char* parameters[MAX_PARAMETERS]);

/*
 * handle the exit command, exits the program.
 */
int handle_exit_command(game* current_game, char* parameters[MAX_PARAMETERS]);

#endif /* COMMANDS_H_ */
