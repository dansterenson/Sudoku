/*
 * main.c
 *
 *  Created on: Aug 9, 2019
 *      Author: GuyErez
 */

#include "structures.h"
#include "main_aux.h"
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "SPBufferset.h"


int main() {
	bool need_to_exit = false;
	char command[COMMAND_SIZE + 1];
	game* current_game = create_game(3, 3, init, true);

	SP_BUFF_SET();

	print_flush("Welcome To Our Sudoku Game, start playing\n");

	while(!need_to_exit){

		if(feof(stdin)){
			need_to_exit = true;
			break;
		}

		if(get_command_from_user(command) == -1){
			print_flush("Exiting...\n");
			return EXIT_FAILURE;
		}

		if(command_parser(command, current_game) == -1){
			need_to_exit = true;
			break;
		}
	}

	print_flush("Exiting...\n");

	return 0;

}
