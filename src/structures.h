#ifndef STRUCTURES_H_
#define STRUCTURES_H_

#include <stdbool.h>
#include "linked_list.h"

/*
 *this module conatains the necessary structures for the game.
 */
 
typedef enum {
	init = 1,
	edit = 2,
	solve = 4
} game_modes;

/*
 * structure of a board cell which contains:
 * A bit that represents if the cell is fixed
 * A bit that represents if the cell is an error
 * the value of the cell
 */
typedef struct cell{
	bool is_fixed;
	bool is_error;
	int value;
}board_cell;

/*
 * structure of a board which contains:
 * n rows of blocks
 * m columns of blocks
 * and a pointer to a 2D array of board cells
 */
typedef struct board {
	int n;
	int m;
	board_cell** board;
}board;

/*
 * structure of a game which contains:
 * the game's mode
 * mark errors bit
 * and a pointer to the game's undo redo list
 */
typedef struct game {
	game_modes mode;
	bool is_mark_errors;
	list* undo_redo_list;
} game;

/*
 * A cell node, to be used in the exhaustive backtracking
 * algorithm - in order to simulate recursion using a stack
 */

typedef struct cell_node {
	int row_index;
	int col_index;
	int value;
} cell_node;

/*
 * A custom struct to be used in the guess_hint command.
 * It contains the user chosen coordinates - row, col, and a board_cell array
 * and it's probability to be a part of a valid solution.
 */
typedef struct cell_probability {
	double* probability;
	int row;
	int col;
} cell_probability;


#endif /* STRUCTURES_H_ */
