/*
 * main_aux.h
 *
 *  Created on: Aug 11, 2019
 *      Author: dan
 */

#ifndef MAIN_AUX_H_
#define MAIN_AUX_H_

#include <stdlib.h>
#include <stdio.h>
#include "structures.h"

#define MAX_PARAMETERS 3
#define COMMAND_SIZE 256
#define NUM_COMMANDS 17
#define TRUE 1
#define FALSE 0


/*
 * prints a right message when memory allocation failed
 * exits the program.
 */
void memory_alloc_problem();

/*
 * gets the command from the user, returns 0 in success, -1 in EOF.
 */
int get_command_from_user(char command[COMMAND_SIZE + 1]);

/*
 * creates a board with n rows of blocks and m columns of blocks.
 * returns a pointer to the board that was created
 */
board* create_board(int n, int m);

/*
 * creates empty list
 */
list* create_empty_list();

/*
 * creates a new game with the given mode
 * create a board and inserts it to the head of the list.
 * returns a pointer to the game that was created.
 */
game* create_game(int n, int m, game_modes mode, bool is_mark_errors);

/*
 * checks if the board cell is in the right format
 * value > 0 and value < n*m.
 */
int cell_in_right_format(int n, int m, int cell);

/*
 * prints a message that a file is not in the right format
 * returns -1.
 */
void file_not_right_format(FILE *fp);

/*
 * loads a game from a file.
 * returns false in error
 * otherwise returns true.
 */
int load_game_from_file(char* path, board** loaded_board);

/*
 * saves a game from a file.
 * returns false in error
 * otherwise returns true.
 */
int save_game_to_file(game* current_game, char* path);

/*
 * frees the board's memory.
 */
void free_board_mem(void* board_to_free);

/*
 * frees the game's list.
 */
void free_undo_redo_list(list* list_to_free);

/*
 * frees the game memory.
 */
void free_game_mem(game* game);

/*
 * prints a separating line.
 */
void print_separating_line(int n, int m);

/*
 * prints the given board
 */
void print_board(board* board_to_print, game* game);

/*
 * copies the given board and returns a new board similar to the given one.
 */
board* copy_board(board* board_to_copy);

/*
 * checks whether the given board is full or not.
 */
int board_is_full(board* board_to_check);

/*
 * checks whether the given board is erroneous or not.
 * returns true if erroneous.
 */
int board_is_erroneous(board* board_to_check);

/*
 * counts the number of empty cells in a given board.
 * returns the number of empty cells.
 */
int num_empty_cells(board* board_to_check);


/*
 * checks if a converted number was originally zero or the atoi function had a problem
 * returns true if the atoi function had a problem and prints an error message.
 * otherwise returns false.
 */
int check_atoi_error(char* param_name, int after_convert, char* before_convert);

/*
 * checks whether a given number is between given lower bound and upper bound.
 * returns error if not and prints an error message.
 * otherwise returns true.
 */
int check_range(char* param, int num_to_check, int lower_bound, int upper_bound);

/*
 * Goes over all cells in board and checks if they are legal.
 * If all cells are legal (and contain legal values - not 0 or empty) return true.
 * Else - if we have an illegal cell, return false.
 */
int check_board(board* current_board);

/*
 * Clear all non-empty cells from the board, except y (parameter given from the user) cells
 * which will remain filled.
 */
void keep_y_cells(board* board_to_clear, int y);

/*
 * prints differences between two boards
 */
void print_changes_boards(board* first_board, board* second_board);

/*
 * sets array to zeros.
 */
void set_array_zero(int* arr, int size);

/*
 * checks if a given board is completed and writes a message.
 */
int board_is_completed(board* current_board);

/*
 * fills 1 in the array index where the index is a legal value for the given cell.
 */
void cell_legal_values(board* current_board, int* legal_values, int N, int row, int col);

/*
 * finds the r empty cell.
 */
void find_empty_cell(board* current_board, int* empty_cell_row, int* empty_cell_col, int r, int N);

#endif /* MAIN_AUX_H_ */
