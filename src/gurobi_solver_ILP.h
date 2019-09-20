#ifndef GUROBI_SOLVER_ILP_H_
#define GUROBI_SOLVER_ILP_H_

#include "structures.h"

/* This module includes a method that calls the main Gurobi implementation.
 * This implementation uses Integer Linear Programming to solve the sudoku board.
 * It also receives a second parameter - should_fill_board, to determine if the user
 * wants to fill the board, or only validate that it's solvable.
 */

int gurobi_main_ILP(board* game_board, int should_fill_board);


#endif /* GUROBI_SOLVER_ILP_H_ */
