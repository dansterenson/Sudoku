/*
 * main.c
 *
 *  Created on: Aug 9, 2019
 *      Author: GuyErez
 */

#include "structures.h"
#include "main_aux.h"
#include <stdio.h>

int main() {
	bool still_playing = true;
	char command[COMMAND_SIZE + 1];
	game* current_game = create_game(3, 3, init, true);

	printf("Welcome To Our Sudoku Game, start playing\n");
	fflush(stdout);

	while(still_playing == true){

		if(feof(stdin)){
			still_playing = false;
			break;
		}

		get_command_from_user(command);
		if(parser(command, current_game) == -1){
			still_playing = false;
			break;
		}
	}

	printf("Exiting...\n");
	fflush(stdout);

	return 0;

}
