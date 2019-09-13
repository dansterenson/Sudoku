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
#include <time.h>


int main() {
	srand(time(0));
	bool need_to_exit = false;
	char command[COMMAND_SIZE + 1];
	game* current_game = create_game(3, 3, init, true);


	printf("Welcome To Our Sudoku Game, start playing\n");

	while(!need_to_exit){

		if(feof(stdin)){
			need_to_exit = true;
			break;
		}

		if(get_command_from_user(command) == -1){
			printf("Exiting...\n");
			return EXIT_FAILURE;
		}

		if(parse_command(command, current_game) == -1){
			need_to_exit = true;
			break;
		}
	}

	printf("Exiting...\n");

	return 0;

}
