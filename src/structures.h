/*
 * structures.h
 *
 *  Created on: Aug 7, 2019
 *      Author: dan
 */

#ifndef STRUCTURES_H_
#define STRUCTURES_H_

#include <stdbool.h>
#include "linked_list.h"

typedef enum {
	init,
	edit,
	solve
} game_modes;

typedef struct cell{
	bool is_fixed;
	bool is_error;
	int value;
}board_cell;

typedef struct board {
	int n;
	int m;
	board_cell** board;
}board;


typedef struct game {
	game_modes mode;
	bool is_mark_errors;
	list* undo_redo_list;
} game;

#endif /* STRUCTURES_H_ */
