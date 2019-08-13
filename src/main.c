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


int main() {
	bool still_playing = true;
	char command[COMMAND_SIZE + 1];
	game* current_game = create_game(3, 3, init, true);

	print_flush("Welcome To Our Sudoku Game, start playing\n");

	while(still_playing == true){

		if(feof(stdin)){
			still_playing = false;
			break;
		}

		if(get_command_from_user(command) == -1){
			print_flush("Exiting...\n");
			return EXIT_FAILURE;
		}

		if(command_parser(command, current_game) == -1){
			still_playing = false;
			break;
		}
	}

	print_flush("Exiting...\n");

	return 0;

}
