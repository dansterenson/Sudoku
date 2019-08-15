/*
 * commands.h
 *
 *  Created on: Aug 12, 2019
 *      Author: dan
 */

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include "structures.h"

/*
 * this module is in charge of handling the different commands
 */


/*
 * handle the solve command, returns 0 in success, -1 otherwise.
 */
int handle_solve_command(game* current_game, char* path);

/*
 * handle the edit command, returns 0 in success, -1 otherwise.
 */
int handle_edit_command(game* current_game, char* path);

/*
 * handle the set command, returns 0 in success, -1 otherwise.
 */
int handle_set_command(game* current_game, char* parameters[3]);
#endif /* COMMANDS_H_ */
