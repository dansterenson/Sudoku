
#ifndef GUROBI_HELPER_UTILS_H_
#define GUROBI_HELPER_UTILS_H_

#include "gurobi_c.h"
#include "gurobi_structs.h"
#include "structures.h"
/*
 * This module includes utility functions that are used in the logic
 * concerning Gurobi - both for LP and ILP.
 * All functions returning int return 0 in case the condition is false, or an error occurred.
 * Otherwise, if everything went well, they return 1 (or TRUE).
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
int handle_row_col_constraints(Constraints row_or_column, GRB_vars* vars_ptr, GRBmodel *model, board* game_board);

/*
 * Add block constraints - each block should contain only one appearance of each value.
 * Minimize the amount of constraints by not setting constraints for values that already exist in the block.
 */
int handle_block_constraints(GRB_vars* vars_ptr, GRBmodel *model, board* game_board);

/* Add the constraint such that each cell will have only a single value. */
int handle_single_value_constraints(GRB_vars* vars_ptr, GRBmodel *model);

/* Add the constraint such that every Gurobi variable x_ijk will represent
 * an approximation for the given value - e.g: probability, for the LP solution.
 */
int handle_constraints_LP(GRB_vars* vars_ptr, GRBmodel *model);

/* Set all the linear constraint variables type to GRB_BINARY - (ILP). */
void set_vtype_grb_binary(char* vtype, int var_count);

/* Use the solution found using ILP to fill the board with legal values */
void fill_board_ILP(board_cell** board, GRB_vars* grb_vars_ptr, double* sol);

/* Free all Gurobi variables related memory. */
void free_all_grb_vars(GRB_vars* vars_ptr);

/* Free GRBmodel and GRBenv */
void free_grb_model_env(GRBmodel *model, GRBenv *env);

/* No GRB_vars created - that means there's no solution using ILP/LP */
int no_vars_created(GRB_vars* grb_vars_ptr);

/* During Gurobi LP - set vtype to GRB_CONTINUOUS and set obj array with random values
 * in the range of the boards dimensions - N
 */
void set_vtype_continuous_obj_rand(char* vtype, double* obj, int var_count, int N);

/* Use the attribute array to fill the board with legal values that passed the chosen threshold*/
void fill_board_LP(board* game_board, GRB_vars* grb_vars_ptr, double* sol, double chosen_threshold);

/* Use the attribute array to fill the guess_hint_array with possible values for the cell coordinates the user chose.
 * We get the user's chosen coordinates from the first cell in the array,
 * and return the possible values and their probability.
 */
void fill_guess_hint_array(board* game_board, cell_probability* guess_hint_array, GRB_vars* grb_vars_ptr, double* sol);

#endif /* GUROBI_HELPER_UTILS_H_ */
