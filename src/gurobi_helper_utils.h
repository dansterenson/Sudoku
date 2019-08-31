
#ifndef GUROBI_HELPER_UTILS_H_
#define GUROBI_HELPER_UTILS_H_

#include "gurobi_c.h"
#include "gurobi_structs.h"
#include "structures.h"

/*
 * This module includes utility functions that are used in the logic
 * concerning Gurobi - both for LP and ILP.
 */

/*
 * Returns Gurobi variables x_ijk to be used in the optimization process.
 * The variables constitute the cell's coordinates - (i,j) and the cell's value - k.
 * The method returns a sorted variable list with all legal k values,
 * (sorted by i,j,k) for cells that are still empty. The sorted list of variables allows us to
 * go over the board more quickly, since we can easily spot when a variable belongs to a different
 * cell than the previous one.
 */
GRB_vars* get_grb_vars(board* game_board);

/*
 * Handle constraints for rows and columns - each row and col should contain only one
 * copy of each possible value. Improve performance by not setting constraints for values
 * that already exist in the row/column.
 */
int handle_row_col_constraints(Constraints row_or_column, board* game_board, GRB_vars* vars, GRBmodel *model);

/*
 * Add block constraints - each block should contain only one appearance of each value.
 * Minimize the amount of constraints by not setting constraints for values that already exist in the block.
 */
int handle_block_constraints(board* game_board, GRB_vars* vars, GRBmodel *model);

/* Add the constraint such that each cell will have only a single value. */
int handle_single_value_constraints(GRB_vars* vars, GRBmodel *model);

/* Add the constraint such that every Gurobi variable x_ijk will represent
 * an approximation - e.g: probability, in the LP soluton.
 */
int handle_constraints_LP(GRB_vars* vars_ptr, GRBmodel *model);

/* Free all Gurobi variables related memory. */
void free_all_grb_vars(GRB_vars* vars_ptr);

#endif /* GUROBI_HELPER_UTILS_H_ */
