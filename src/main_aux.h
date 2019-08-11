/*
 * main_aux.h
 *
 *  Created on: Aug 11, 2019
 *      Author: dan
 */

#ifndef MAIN_AUX_H_
#define MAIN_AUX_H_

#include <stdbool.h>

#define COMMAND_SIZE 256


void memory_alloc_problem();

board* create_board(int n, int m);

game* create_game(int n, int m, game_modes mode, bool is_mark_errors);

void free_board_mem(board board_to_free);

void free_game_mem(game* game);

void print_separating_line(int n, int m);

void print_board(board* board_to_print, game* game);

#endif /* MAIN_AUX_H_ */
