/*
 * game_utils.c
 *
 *  Created on: Aug 17, 2019
 *      Author: GuyErez
 */
#include "structures.h"
#include "game_utils.h"
#include <stdlib.h>
#include <stdio.h>

/*
 * Checks if entering the current num at the specified cell
 * is a legal placement for that row.
 */
int is_row_legal(board_cell** game_board, int row, int col, int num, int dimensions);

/*
 * Checks if entering the current num at the specified cell
 * is a legal placement for that col.
 */
int is_col_legal(board_cell** game_board, int row, int col, int num, int dimensions);

/*
 * Checks if entering the current num at the specified cell
 * is a legal placement for that block.
 */
int is_block_legal(board_cell** game_board, int row, int col, int num, int board_rows, int board_cols);

int is_legal_cell(board* current_board, int row, int col, int num) {
	int block_start_row;
	int block_start_col;
	int i,j;
	int board_rows = current_board->n;
	int board_cols = current_board->m;
	int dimensions = board_rows * board_cols;
	board_cell** game_board = current_board->board;

	return is_row_legal(game_board, row, col, num, dimensions)
			&& is_col_legal(game_board, row, col, num, dimensions)
			&& is_block_legal(game_board, row, col, num, board_rows, board_cols);
}

int is_row_legal(board_cell** game_board, int row, int col, int num, int dimensions) {
	int i;

	for (i = 0; i < dimensions; i++) {
		if (game_board[i][row].value == num && i != col) {
			return 0;
		}
	}
	return 1;
}

int is_col_legal(board_cell** game_board, int row, int col, int num, int dimensions) {
	int i;

	for (i = 0; i < dimensions; i++) {
		if (game_board[col][i].value == num && i != row) {
			return 0;
		}
	}
	return 1;
}

int is_block_legal(board_cell** game_board, int row, int col, int num, int board_rows, int board_cols) {
	int i,j;
	int block_start_row = row - row % board_rows;
	int block_start_col = col - col % board_cols;
	int i = block_start_row;
	int j = block_start_col;

	for(i = block_start_row; i < block_start_row + board_rows; i++) {
		for(j = block_start_col; j < block_start_col + board_cols; j++) {
			if (i == row && j == col) {
				continue;
			}
			else if (game_board[j][i].value == num) {
				return 0;
			}
		}
	}
	return 1;
}

