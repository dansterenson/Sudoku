/*
 * gurobi_solver_ILP.h
 *
 *  Created on: Aug 10, 2019
 *      Author: GuyErez
 */

#ifndef GUROBI_SOLVER_ILP_H_
#define GUROBI_SOLVER_ILP_H_

#include "structures.h"

/* This module includes a method that calls the main Gurobi implementation.
 * This implementation uses Integer Linear Programming to solve the sudoku board.
 */

int gurobi_main_ILP(board* game_board, int should_fill_board);


#endif /* GUROBI_SOLVER_ILP_H_ */
