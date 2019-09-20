#ifndef GAME_UTILS_H_
#define GAME_UTILS_H_

/*
 * This module contains various utility functions that are relevant
 * to the Sudoku game - including solving the board using exhaustive backtracking,
 * checking whether a given cell is legal for the current board, and so on.
 */


/* This method goes over rows, columns and blocks to determine if placing
 * "num" in a cell with coordinates [row,col] is a legal move in the current game board.
 */
int is_legal_cell(board* current_board, int row, int col, int num);

/*
 * This function goes over the current board and checks whether it's current cell placements
 * are legal. If they are - previous error marks (if there were any, are removed).
 * If the cell placement isn't legal however, it is marked as an error cell.
 */
void update_error_cells(board* current_board);

/*
 * This method uses the backtracking algorithm (with a stack implementation, instead of
 * regular recursion) in order to exhaustively check all possible solutions for the game board.
 * Returns the number of possible solutions (or 0 if there are none).
 */
int exhaustive_backtracking_solution_count(board* game_board);


#endif /* GAME_UTILS_H_ */
