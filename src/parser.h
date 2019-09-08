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

/*
 * parse the given command and sends it to the handler of this command.
 * returns -1 if needs to exit, 0 otherwise.
 */
int parse_command(char* command, game* current_game);



#endif /* PARSER_H_ */
