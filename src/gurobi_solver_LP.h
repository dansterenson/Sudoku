#ifndef GUROBI_SOLVER_LP_H_
#define GUROBI_SOLVER_LP_H_

#include "structures.h"

/* This module includes a method that calls the main Gurobi implementation.
 * This implementation uses Linear Programming to solve the sudoku board. It also takes into
 * account a threshold input, to determine the most appropriate solution (out of those that pass
 * the given threshold).
 * The final parameter - the guess_hint_array is used in the guess_hint command, to return
 * a list of all the possible values for the given cell and their probabilities.
 */

int gurobi_main_LP(board* game_board, double chosen_threshold, cell_probability* guess_hint_array);


#endif /* GUROBI_SOLVER_LP_H_ */
