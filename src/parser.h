/*
 * parser.h
 *
 *  Created on: Aug 12, 2019
 *      Author: dan
 */

#ifndef PARSER_H_
#define PARSER_H_
void param_missing_err(char* command,char* format);

void too_many_param_err(char* command,char* format);

int command_parser(char* command, game* current_game);

#endif /* PARSER_H_ */
