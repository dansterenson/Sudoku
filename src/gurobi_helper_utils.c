/*
 * gurobi_helper.utils.c
 *
 *  Created on: Aug 10, 2019
 *      Author: GuyErez
 */
#include "structures.h"
#include "gurobi_structs.h"
#include "gurobi_helper_utils.h"
#include "gurobi_c.h"
#include <stdlib.h>
#include <stdio.h>

/*
 * Iterate over board cells and count the number of cells (or vars) we can fill
 * with valid values. return that amount.
 */
int get_valid_vars_count(board* game_board, int dimensions);

/* Map each variable to it's specific cell in the board
 * and it's appropriate value
 */
void map_vars_to_cell(board* game_board, GRBvariable** vars, int dimensions);

GRB_vars* get_grb_vars(board* game_board){
	int var_count;
	GRBvariable** vars;
	int dimensions = game_board->n * game_board->m;

	GRB_vars* grb_vars_ptr = (GRB_vars*)calloc(1, sizeof(GRB_vars));

	if (grb_vars_ptr == NULL){
		printf("ERROR: memory allocation failed");
		exit(1);
	}

	var_count = get_valid_vars_count(game_board, dimensions);
	grb_vars_ptr->var_count = var_count;

	vars = (GRBvariable**)calloc(var_count, sizeof(GRBvariable*));

	if (vars==NULL){
		printf("ERROR: memory allocation failed");
		exit(1);
	}

	map_vars_to_cell(game_board, vars, dimensions);

	grb_vars_ptr->vars = vars;
	return grb_vars_ptr;
}

int get_valid_vars_count(board* game_board, int dimensions) {
	int i,j,k;
	int count = 0;

	for (i = 0 ; i < dimensions; i++){
		for (j = 0 ; j < dimensions; j++){
			if (game_board->board[i][j].value == 0){
				for (k = 1 ; k <= dimensions; k++){
					if (!is_error_cell(k,i,j,board)){
						count++;
					}
				}
			}
		}
	}

	return count;
}

void map_vars_to_cell(board* game_board, GRBvariable** vars, int dimensions) {
	int i,j,k;
	int count = 0;

	for (i = 0 ; i < dimensions; i++){
		for (j = 0 ; j < dimensions; j++){
			if (game_board->board[j][i].value == 0){
				for (k = 1 ; k <= N; k++){
					if (!is_error_cell(k,i,j,board)){
						GRBvariable* tmp = (GRBvariable*) calloc(1, sizeof(GRBvariable));
						tmp->i = i;
						tmp->j = j;
						tmp->k = k;
						vars[count] = tmp;
						count++;
					}
				}
			}
		}
	}
}

/*
 * Handle constraints for rows and columns - each row and col should contain only one
 * copy of each possible value. Improve performance by not setting constraints for values
 * that already exist in the row/column.
 */
int handle_row_col_constraints(Constraints row_or_column, GRBmodel *model, GRB_vars* vars, board* game_board){
	int* index_ptr;
	double* val;
	int i,k;
	int current_index;
	int count = 0;
	int error_occured = false;
	int current_position = 0;
	int dimensions = game_board->n * game_board->m;

	for(i = 0; current_position < dimensions; current_position++) {
		for (k = 1; k <= dimensions; k++){
			count = 0;
			for(current_index = 0; current_index < vars->var_count; current_index++) {
				if(row_or_column == ROW){
					if(vars->vars[current_index]->j == current_position && vars->vars[current_index]->k == k) {
						count++;
					}
				}
				else{
					if(vars->vars[current_index]->i == current_position && vars->vars[current_index]->k == k) {
						count++;
					}
				}
			}
			if (count == 0) {
				continue;
			}

			val = (double*)calloc(count, sizeof(double));
			if (val==NULL){
				printf("ERROR: memory allocation failed");
				exit(1);
			}

			index_ptr = (int*)calloc(count, sizeof(int));
			if (index_ptr==NULL){
				printf("ERROR: memory allocation failed");
				exit(1);
			}

			i = 0;
			for(current_index = 0; current_index < vars->var_count; current_index++) {
				if(row_or_column == ROW){
					if(vars->vars[current_index]->j == current_position  && vars->vars[current_index]->k == k) {
						val[i] = 1.0;
						index_ptr[i] = current_index;
						i++;
					}
				}
				else{
					if(vars->vars[current_index]->i == current_position  && vars->vars[current_index]->k == k) {
						val[i] = 1.0;
						index_ptr[i] = current_index;
						i++;
					}
				}
			}

			error_occured = GRBaddconstr(model, count, index_ptr, val, GRB_EQUAL, 1.0, NULL);
			free(index_ptr);
			free(val);

			if (error_occured) {
			  printf("GUROBI RELATED ERROR OCCURED.\n");
			  fflush(stdout);
			  return -1;
			}
		}
	}

	return 1;
}

int handle_block_constraints(GRBmodel *model, Gurobi_vars* vars, Board* board) {
	/*
	 * Add block constraints - each block should contain only one appearance of each value.
	 * Minimize the amount of constraints by not setting constraints for values that already exist in the block.
	 * */
	int* ind;
	double* val;

	int n = board->n;
	int m = board->m;
	int N = n*m;
	int curr_ind = 0;
	int curr_block = 0;
	int error = 0;
	int cnt = 0;
	int i = 0;
	int k = 1;
	int var_row = 0;
	int var_col = 0;
	int var_val = 0;
	int block_start_row = 0;
	int block_start_col = 0;

	for(; curr_block < N; curr_block++) {
		k = 1;
		for (; k <= N; k++){
			cnt = 0;
			curr_ind = 0;
			for(; curr_ind < vars->amount; curr_ind++) {
				var_col = vars->vars[curr_ind]->i;
				var_row = vars->vars[curr_ind]->j;
				var_val = vars->vars[curr_ind]->k;

				if(block_start_col <= var_col && var_col < block_start_col + n && block_start_row <= var_row && var_row < block_start_row + m && var_val == k) {
					cnt++;
				}
			}

			if (cnt == 0){
				continue;
			}

			ind = (int*)calloc(cnt, sizeof(int));
			if (ind==NULL){
				printf("ERROR: Problem in memory allocating");
				exit(1);
			}
			val = (double*)calloc(cnt, sizeof(double));
			if (val==NULL){
				printf("ERROR: Problem in memory allocating");
				exit(1);
			}
			i = 0;
			curr_ind = 0;

			for(; curr_ind < vars->amount; curr_ind++) {
				var_col = vars->vars[curr_ind]->i;
				var_row = vars->vars[curr_ind]->j;
				var_val = vars->vars[curr_ind]->k;

				if(block_start_col <= var_col && var_col < block_start_col + n && block_start_row <= var_row && var_row < block_start_row + m && var_val == k) {
					ind[i] = curr_ind;
					val[i] = 1.0;
					i++;
				}
			}

			error = GRBaddconstr(model, cnt, ind, val, GRB_EQUAL, 1.0, NULL);
			free(ind);
			free(val);
			if (error) {
			  printf("GUROBI ERROR.\n");
			  fflush(stdout);
			  return -1;
			}
		}

		block_start_col += n;
		if (block_start_col == N) {
			block_start_col = 0;
			block_start_row += m;
		}
	}
	return 1;

}

int handle_single_value_constraints(GRBmodel *model, GRB_vars* vars) {

	int* index_ptr;
	double* val;
	GRBvariable* var;
	int i;
	int current_index = 0;
	int count = 0;
	int error_occured = false;

	int cell_y_coordinates = vars->vars[0]->j;

	/* Iterate over vars to handle the constraint so that each cell has only one value. */
	for(i = 0 ; current_index < vars->var_count; current_index++) {
		var = vars->vars[current_index];

		if (var->j != cell_y_coordinates) {
			cell_y_coordinates = var->j;

			val = (double*)calloc(count, sizeof(double));
			if (val==NULL){
				printf("ERROR: memory allocation failed");
				exit(1);
			}

			index_ptr = (int*)calloc(count, sizeof(int));
			if (index_ptr==NULL){
				printf("ERROR: memory allocation failed");
				exit(1);
			}

			for(i = 0; i < count; i++) {
				val[i] = 1.0;
				index_ptr[i] = current_index - count + i;
			}

			error_occured = GRBaddconstr(model, count, index_ptr, val, GRB_EQUAL, 1.0, NULL);
			free(index_ptr);
			free(val);

			if (error_occured) {
			  printf("GUROBI RELATED ERROR OCCURED.\n");
			  fflush(stdout);
			  return -1;
			}

			count = 1;
		}

		else {
			count++;
		}
	}

	/* handle last constraint separately */

	val = (double*)calloc(count, sizeof(double));
	if (val==NULL){
		printf("ERROR: memory allocation failed");
		exit(1);
	}

	index_ptr = (int*)calloc(count, sizeof(int));
	if (index_ptr==NULL){
		printf("ERROR: memory allocation failed");
		exit(1);
	}

	for(i = 0; i < count; i++) {
		val[i] = 1.0;
		index_ptr[i] = current_index - count + i;
	}

	error_occured = GRBaddconstr(model, count, index_ptr, val, GRB_EQUAL, 1.0, NULL);
	free(index_ptr);
	free(val);

	if (error_occured) {
	  printf("GUROBI RELATED ERROR OCCURED.\n");
	  fflush(stdout);
	  return -1;
	}

	return 1;
}

void release_vars_memory(Gurobi_variable** vars, int amount){
	int i = 0;
	for (; i < amount ; i++){
		free(vars[i]);
	}
	return;
}


void release_gurobi_vars_memory(Gurobi_vars* vars_ptr){
	release_vars_memory(vars_ptr->vars, vars_ptr->amount);
	free(vars_ptr);
	return;
}


int add_LP_xijk_constraints(GRBmodel *model, Gurobi_vars* vars_ptr){
	int ind[1];
	double val[1];
	int i = 0;
	int error = 0;

	val[0] = 1.0;

	for (; i < vars_ptr->amount; i++){
		ind[0] = i;
		error = GRBaddconstr(model, 1, ind, val, GRB_LESS_EQUAL, 1.0, NULL);
		if (error) {
		  printf("GUROBI ERROR.\n");
		  fflush(stdout);
		  return -1;
		}
		error = GRBaddconstr(model, 1, ind, val, GRB_GREATER_EQUAL, 0.0, NULL);
			if (error) {
			  printf("GUROBI ERROR.\n");
			  fflush(stdout);
			  return -1;
			}
	}
	return 1;
}

