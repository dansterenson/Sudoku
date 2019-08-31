#ifndef GUROBI_SOLVER_LP_H_
#define GUROBI_SOLVER_LP_H_

#include "structures.h"

/* This module includes a method that calls the main Gurobi implementation.
 * This implementation uses Linear Programming to solve the sudoku board, with a given
 * threshold to determine the appropriate solution.
 */

int gurobi_main_LP(board* game_board, double chosen_threshold);


#endif /* GUROBI_SOLVER_LP_H_ */
