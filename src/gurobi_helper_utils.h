
#ifndef GUROBI_HELPER_UTILS_H_
#define GUROBI_HELPER_UTILS_H_

#include "gurobi_c.h"
#include "gurobi_structs.h"
#include "structures.h"

/*
 * Creates Xijk variables for optimization: value k at cell <i, j>.
 * Creates variables only for cells <i, j> that are not filled yet, and for each such cell, only for legal k values.
 *
 * The returned variables list is sorted by Xijk, easily allowing to notice when a variable belongs to another cell than
 * the previous variable - this allows tranversing the board fewer times.
 */
GRB_vars* get_variables(board* board);

/* One value per cell: for each cell <i, j> that was not removed from the variables list: sum(Xijk) = 1. */
int add_cell_single_value_constraints(GRBmodel *model,  GRB_vars* vars);

/*
 * Each row contains each value once: for each j, k combination: sum(Xijk) = 1.
 * Only add constraints for values (k) that do not already appear in the row.
 */
int add_row_constraints(GRBmodel *model, GRB_vars* vars, board* board);
/*
 * Each column contains each value once: for each i, k combination: sum(Xijk) = 1.
 * Only add constraints for values (k) that do not already appear in the column.
 */
int add_col_constraints(GRBmodel *model, GRB_vars* vars, board* board);
/*
 * Each block contains each value once, use the board sizes to determine the blocks and set the contraints.
 * Only add constraints for values (k) that do not already appear in the block.
 */
int add_block_constraints(GRBmodel *model, GRB_vars* vars, board* board);

void release_vars_memory(GRBvariable** vars, int amount);
void release_gurobi_vars_memory(GRB_vars* vars_ptr);

/* Add constraints such that every Xijk will represent a "probability" in the LP guess solution. */
int add_LP_xijk_constraints(GRBmodel *model, GRB_vars* vars_ptr);


#endif /* GUROBI_HELPER_UTILS_H_ */
