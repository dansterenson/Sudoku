#include "structures.h"
#include "main_aux.h"
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include <time.h>


int main() {
	srand(time(0));
	bool need_to_exit = false;
	char command[COMMAND_SIZE] = {0};
	int res;
	game* current_game = create_game(3, 3, init, true);


	printf("Welcome To Our Sudoku Game, Start Playing\n");

	while(!need_to_exit){
		res = get_command_from_user(command);
		if(res == -1){ /*command too long*/
			continue;
		}
		
		if(res == 0){/*EOF*/
			free_game_mem(current_game);
			printf("\n");
			break;

		}

		if(parse_command(command, current_game) == -1){
			need_to_exit = true;
			break;
		}
	}
	
	printf("Exiting...\n");

	return 0;

}
