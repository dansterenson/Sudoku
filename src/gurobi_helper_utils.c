/*
 * gurobi_helper.utils.c
 *
 *  Created on: Aug 10, 2019
 *      Author: GuyErez
 */
#include "structures.h"
#include "gurobi_structs.h"
#include "gurobi_helper_utils.h"
#include "game_utils.h"
#include "gurobi_c.h"
#include <stdlib.h>
#include <stdio.h>

GRB_vars* get_variables(board* board){
	GRBvariable** vars;
	int count = 0;
	int N = board->m * board->n;
	int i = 0;
	int j = 0;
	int k = 1;

	GRB_vars* variables = (GRB_vars*)calloc(1, sizeof(GRB_vars));
	  if (variables==NULL){
		printf("ERROR: Problem in memory allocating");
		exit(1);
		}

	/*
	 * A variable Xijk is cell (i,  j) with value k.
	 * Create variables for cells that are not filled yet (content = 0), and only for legal k values.
	 * */
	for (; i < N; i++){
		j = 0;
		for (; j < N; j++){
			if (board->board[i][j].value == 0){
				k = 1;
				for (; k <= N; k++){
					if (is_legal_cell(board, i, j, k)){
						count++;
					}
				}
			}
		}
	}
	variables->var_count = count;
	vars = (GRBvariable**)calloc(count, sizeof(GRBvariable*));
	if (vars==NULL){
		printf("ERROR: Problem in memory allocating");
		exit(1);
	}
	count = 0;
	i = 0;

	/* Create a mapping from each variable to its cell + value */
	for (; i < N; i++){
		j = 0;
		for (; j < N; j++){
			if (board->board[i][j].value == 0){
				k = 1;
				for (; k <= N; k++){
					if (is_legal_cell(board, i, j, k)){
						GRBvariable* tmp_var = (GRBvariable*) calloc(1, sizeof(GRBvariable));
						tmp_var->i = i;
						tmp_var->j = j;
						tmp_var->k = k;
						vars[count] = tmp_var;
						count++;
					}
				}
			}
		}
	}

	variables->vars = vars;
	return variables;
}


int add_cell_single_value_constraints(GRBmodel *model, GRB_vars* vars) {
	/* We already scanned the board, we can just scan the vars to make the constraint that each cell has one value. */
	/* The vars are already sorted by cells */
	int* ind;
	double* val;

	int curr_ind = 0;
	int cnt = 0;
	int i = 0;
	int error = 0;
	GRBvariable* var;

	int celly = vars->vars[0]->j;

	for(; curr_ind < vars->var_count; curr_ind++) {
		var = vars->vars[curr_ind];
		/* Moved a cell */
		if (var->j != celly) {
			celly = var->j;
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
			for(; i < cnt; i++) {
				ind[i] = curr_ind - cnt + i;
				val[i] = 1.0;
			}

			error = GRBaddconstr(model, cnt, ind, val, GRB_EQUAL, 1.0, NULL);
			free(ind);
			free(val);
			if (error) {
			  printf("GUROBI ERROR.\n");
			  fflush(stdout);
			  return -1;
			}

			cnt = 1;
		} else {
			cnt++;
		}
	}

	/* last constraint */
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
	for(; i < cnt; i++) {
		ind[i] = curr_ind - cnt + i;
		val[i] = 1.0;
	}

	error = GRBaddconstr(model, cnt, ind, val, GRB_EQUAL, 1.0, NULL);
	free(ind);
	free(val);
	if (error) {
	  printf("GUROBI ERROR.\n");
	  fflush(stdout);
	  return -1;
	}



	return 1;
}


int add_row_constraints(GRBmodel *model, GRB_vars* vars, board* board){
	/*
	 * Add row constraints - each row should contain only one appearance of each value.
	 * Minimize the amount of constraints by not setting constraints for values that already exist in the row.
	 * */
	int* ind;
	double* val;

	int n = board->n;
	int m = board->m;
	int N = n*m;
	int curr_ind = 0;
	int curr_row = 0;
	int error = 0;
	int cnt = 0;
	int i = 0;
	int k = 1;


	for(; curr_row < N; curr_row++) {
		k = 1;
		for (; k <= N; k++){
			cnt = 0;
			curr_ind = 0;
			for(; curr_ind < vars->var_count; curr_ind++) {
				if(vars->vars[curr_ind]->i == curr_row && vars->vars[curr_ind]->k == k) {
					cnt++;
				}
			}
			if (cnt == 0) {
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

			for(; curr_ind < vars->var_count; curr_ind++) {
				if(vars->vars[curr_ind]->i == curr_row  && vars->vars[curr_ind]->k == k) {
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
	}

	return 1;
}


int add_col_constraints(GRBmodel *model, GRB_vars* vars, board* board){
	/*
	 * Add col constraints - each col should contain only one appearance of each value.
	 * Minimize the amount of constraints by not setting constraints for values that already exist in the col.
	 * */
	int* ind;
	double* val;

	int n = board->n;
	int m = board->m;
	int N = n*m;
	int curr_ind = 0;
	int curr_col = 0;
	int error = 0;
	int cnt = 0;
	int i = 0;
	int k = 1;


	for(; curr_col < N; curr_col++) {
		k = 1;
		for (; k <= N; k++){
			cnt = 0;
			curr_ind = 0;
			for(; curr_ind < vars->var_count; curr_ind++) {
				if(vars->vars[curr_ind]->j == curr_col && vars->vars[curr_ind]->k == k) {
					cnt++;
				}
			}
			if (cnt == 0) {
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

			for(; curr_ind < vars->var_count; curr_ind++) {
				if(vars->vars[curr_ind]->j == curr_col  && vars->vars[curr_ind]->k == k) {
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
	}

	return 1;
}

int add_block_constraints(GRBmodel *model, GRB_vars* vars, board* board) {
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
			for(; curr_ind < vars->var_count; curr_ind++) {
				var_col = vars->vars[curr_ind]->j;
				var_row = vars->vars[curr_ind]->i;
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

			for(; curr_ind < vars->var_count; curr_ind++) {
				var_col = vars->vars[curr_ind]->j;
				var_row = vars->vars[curr_ind]->i;
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



void release_vars_memory(GRBvariable** vars, int amount){
	int i = 0;
	for (; i < amount ; i++){
		free(vars[i]);
	}
	return;
}


void release_gurobi_vars_memory(GRB_vars* vars_ptr){
	release_vars_memory(vars_ptr->vars, vars_ptr->var_count);
	free(vars_ptr);
	return;
}


int add_LP_xijk_constraints(GRBmodel *model, GRB_vars* vars_ptr){
	int ind[1];
	double val[1];
	int i = 0;
	int error = 0;

	val[0] = 1.0;

	for (; i < vars_ptr->var_count; i++){
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
