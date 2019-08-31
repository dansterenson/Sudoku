#ifndef GAME_UTILS_H_
#define GAME_UTILS_H_

/*
 * This module contains various utility functions that are relevant
 * to Sudoku - including solving the board using exhaustive backtracking, etc.
 */

/* This method goes over rows, columns and blocks to determine if placing
 * "num" in a cell with coordinates [row,col] is a legal move in the current game board.
 */
int is_legal_cell(board* current_board, int row, int col, int num);




#endif /* GAME_UTILS_H_ */
