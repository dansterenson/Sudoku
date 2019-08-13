/*
 * main_aux.h
 *
 *  Created on: Aug 11, 2019
 *      Author: dan
 */

#ifndef MAIN_AUX_H_
#define MAIN_AUX_H_

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>


#define COMMAND_SIZE 256

void print_flush(char* str);

void memory_alloc_problem();

int get_command_from_user(char command[COMMAND_SIZE + 1]);

board* create_board(int n, int m);

game* create_game(int n, int m, game_modes mode, bool is_mark_errors);

bool cell_in_right_format(int n, int m, int cell);

int file_not_right_format(FILE *fp);

int load_game_from_file(game* current_game, char* path);

int save_game_to_file(game* current_game, char* path);

void free_board_mem(void* board_to_free);

void free_game_mem(game* game);

void print_separating_line(int n, int m);

void print_board(board* board_to_print, game* game);

#endif /* MAIN_AUX_H_ */
