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
					if (is_legal_cell(board,i,j,k)) {
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
				for (k = 1 ; k <= dimensions; k++){
					if (is_legal_cell(board,i,j,k)) {
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
			free(val);
			free(index_ptr);


			if (error_occured) {
			  printf("GUROBI RELATED ERROR OCCURED.\n");
			  fflush(stdout);
			  return -1;
			}
		}
	}

	return 1;
}

int handle_block_constraints(GRBmodel *model, GRB_vars* vars, board* game_board) {
	/*
	 * Add block constraints - each block should contain only one appearance of each value.
	 * Minimize the amount of constraints by not setting constraints for values that already exist in the block.
	 * */

	int* index_ptr;
	double* val;
	int i,k;
	int count;
	int current_index;
	int current_block;
	int current_var_col;
	int current_var_row;
	int current_var_value;
	int block_start_row = 0;
	int block_start_col = 0;
	int error_occured = false;
	int current_position = 0;
	int board_rows = game_board->n;
	int board_cols = game_board->m;
	int dimensions = board_rows * board_cols;

	for(current_block = 0; current_block < dimensions; current_block++) {

		for (k = 1; k <= dimensions; k++){
			count = 0;
			for(current_index = 0; current_index < vars->var_count; current_index++) {
				current_var_col = vars->vars[current_index]->i;
				current_var_row = vars->vars[current_index]->j;
				current_var_value = vars->vars[current_index]->k;

				if(current_var_value == k && block_start_col <= current_var_col
						&& current_var_col < block_start_col + board_rows &&
						current_var_row < block_start_row + board_cols &&
						block_start_row <= current_var_row ) {
					count++;
				}
			}

			if (count == 0){
				continue;
			}

			val = (double*)calloc(cnt, sizeof(double));
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
				current_var_col = vars->vars[current_index]->i;
				current_var_row = vars->vars[current_index]->j;
				current_var_value = vars->vars[current_index]->k;

				if(current_var_value == k && block_start_col <= current_var_col
						&& current_var_col < block_start_col + board_rows &&
						current_var_row < block_start_row + board_cols &&
						block_start_row <= current_var_row ) {
					val[i] = 1.0;
					index_ptr[i] = current_index;
					i++;
				}
			}

			error_occured = GRBaddconstr(model, count, index_ptr, val, GRB_EQUAL, 1.0, NULL);
			free(val);
			free(index_ptr);

			if (error_occured) {
			  printf("GUROBI RELATED ERROR OCCURED.\n");
			  fflush(stdout);
			  return -1;
			}
		}

		block_start_col += board_rows;
		if (block_start_col == dimensions) {
			block_start_row += board_cols;
			block_start_col = 0;
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

void free_grb_vars_memory(GRBvariable** vars, int var_count){
	int i;
	for (i = 0; i < var_count ; i++){
		free(vars[i]);
	}

	return;
}


void free_grb_vars_pointer_memory(GRB_vars* vars_ptr){
	free_grb_vars_memory(vars_ptr->vars, vars_ptr->var_count);
	free(vars_ptr);
	return;
}


int add_constraints_LP(GRBmodel *model, GRB_vars* vars_ptr){
	int i;
	int index_ptr[1];
	double val[1];
	val[0] = 1.0;
	int error_occured = false;

	for (i = 0; i < vars_ptr->var_count; i++){
		index_ptr[0] = i;

		error_occured = GRBaddconstr(model, 1, index_ptr, val, GRB_GREATER_EQUAL, 0.0, NULL);
			if (error_occured) {
		      printf("GUROBI RELATED ERROR OCCURED.\n");
			  fflush(stdout);
			  return -1;
			}

		error_occured = GRBaddconstr(model, 1, index_ptr, val, GRB_LESS_EQUAL, 1.0, NULL);
		if (error_occured) {
		  printf("GUROBI RELATED ERROR OCCURED.\n");
		  fflush(stdout);
		  return -1;
		}
	}

	return 1;
}

