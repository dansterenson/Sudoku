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
