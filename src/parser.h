/*
 * parser.h
 *
 *  Created on: Aug 12, 2019
 *      Author: dan
 */

#ifndef PARSER_H_
#define PARSER_H_


#include "structures.h"
#include "commands.h"
/*
 * this module is in charge of parsing the given command from the user.
 */


/*
 * parse the given command and sends it to the handler of this command.
 * returns -1 if needs to exit, 0 otherwise.
 */
int parse_command(char* command, game* current_game);



#endif /* PARSER_H_ */
