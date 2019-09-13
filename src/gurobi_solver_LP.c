/*
 * gurobi_solver_LP.c
 *
 *  Created on: Aug 10, 2019
 *      Author: GuyErez
 */
#include <stdlib.h>
#include <stdio.h>
#include "gurobi_structs.h"
#include "gurobi_helper_utils.h"
#include "gurobi_solver_LP.h"
#include "game_utils.h"
#include "gurobi_c.h"

int gurobi_main_LP(board* board, double threshold)
{
	 GRBenv   *env   = NULL;
	  GRBmodel *model = NULL;
	  GRB_vars* vars_ptr;
	  double*   sol;
	  double* 	obj;
	  char*     vtype;
	  int       optimstatus;
	  int       error = 0;
	  int n = board->n;
	  int m = board->m;
	  int N = n*m;
	  int i = 0;


	  /* Create environment */
	  error = GRBloadenv(&env, "sudoku_log.log");
	  if (error) {
		  printf("ERROR %d GRBloadenv(): %s\n", error, GRBgeterrormsg(env));
		  printf("GUROBI ERROR.\n");
		  fflush(stdout);
		  GRBfreemodel(model);
		  GRBfreeenv(env);
		  return -1;
	  }

	  /* cancel printing */
	  error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
	  if (error) {
		  printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
		  printf("GUROBI ERROR.\n");
		  fflush(stdout);
		  GRBfreemodel(model);
		  GRBfreeenv(env);
		  return -1;
	  }

	  /* Create an empty model named "sudoku" */
	  error = GRBnewmodel(env, &model, "sudoku", 0, NULL, NULL, NULL, NULL, NULL);
	  if (error) {
		  printf("ERROR %d GRBnewmodel(): %s\n", error, GRBgeterrormsg(env));
		  printf("GUROBI ERROR.\n");
		  fflush(stdout);
		  GRBfreemodel(model);
		  GRBfreeenv(env);
		  return -1;
	  }

	  vars_ptr = get_variables(board);

	  /* no vars created - no possible solutions */
	  if (vars_ptr->var_count == 0) {
		  GRBfreemodel(model);
		  GRBfreeenv(env);
		  release_gurobi_vars_memory(vars_ptr);
		  return 0;
	  }

	  vtype = (char*)calloc(vars_ptr->var_count, sizeof(char));
	  if (vtype == NULL){
		printf("ERROR: Problem in memory allocating");
		exit(1);
	  }
	  obj = (double*)calloc(vars_ptr->var_count, sizeof(double));
	  if (obj == NULL){
		printf("ERROR: Problem in memory allocating");
		exit(1);
	  }
	  for (; i < vars_ptr->var_count; i++){
		  vtype[i] = GRB_CONTINUOUS;
		  obj[i] =(double)(rand()%N);
	  }

	  /* add variables to model */
	  error = GRBaddvars(model, vars_ptr->var_count, 0, NULL, NULL, NULL, obj, NULL, NULL, vtype, NULL);
	  if (error) {
		  printf("ERROR %d GRBaddvars(): %s\n", error, GRBgeterrormsg(env));
		  printf("GUROBI ERROR.\n");
		  fflush(stdout);
		  GRBfreemodel(model);
		  GRBfreeenv(env);
		  release_gurobi_vars_memory(vars_ptr);
		  free(vtype);
		  return -1;
	  }

	  /* Set an arbitrary objective to MAX, we just want a feasible solution. */
	  error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
	  if (error) {
		  printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
		  printf("GUROBI ERROR.\n");
		  fflush(stdout);
		  GRBfreemodel(model);
		  GRBfreeenv(env);
		  release_gurobi_vars_memory(vars_ptr);
		  free(vtype);
		  return -1;
	  }

	  /* update the model - to integrate new variables */

	  error = GRBupdatemodel(model);
	  if (error) {
		  printf("ERROR %d GRBupdatemodel(): %s\n", error, GRBgeterrormsg(env));
		  printf("GUROBI ERROR.\n");
		  fflush(stdout);
		  GRBfreemodel(model);
		  GRBfreeenv(env);
		  release_gurobi_vars_memory(vars_ptr);
		  free(vtype);
		  return -1;
	  }

	  error = add_cell_single_value_constraints(model, vars_ptr);
	  if (error == -1) {
		  GRBfreemodel(model);
		  GRBfreeenv(env);
		  release_gurobi_vars_memory(vars_ptr);
		  free(vtype);
		  return -1;

	  }

	  error = add_LP_xijk_constraints(model, vars_ptr);
	  if (error == -1) {
		  GRBfreemodel(model);
		  GRBfreeenv(env);
		  release_gurobi_vars_memory(vars_ptr);
		  free(vtype);
		  return -1;

	  }

	  error = add_row_constraints(model, vars_ptr, board);
	  if (error == -1) {
		  GRBfreemodel(model);
		  GRBfreeenv(env);
		  release_gurobi_vars_memory(vars_ptr);
		  free(vtype);
		  return -1;
	  }

	  error = add_col_constraints(model, vars_ptr, board);
	  if (error == -1) {
		  GRBfreemodel(model);
		  GRBfreeenv(env);
		  release_gurobi_vars_memory(vars_ptr);
		  free(vtype);
		  return -1;
	  }


	  error = add_block_constraints(model, vars_ptr, board);
	  if (error == -1) {
		  GRBfreemodel(model);
		  GRBfreeenv(env);
		  release_gurobi_vars_memory(vars_ptr);
		  free(vtype);
		  return -1;
	  }



	  /* Optimize model - need to call this before calculation */
	  error = GRBoptimize(model);
	  if (error) {
		  printf("ERROR %d GRBoptimize(): %s\n", error, GRBgeterrormsg(env));
		  GRBfreemodel(model);
		  GRBfreeenv(env);
		  release_gurobi_vars_memory(vars_ptr);
		  free(vtype);
		  return -1;
	  }

	  error = GRBwrite(model, "sudoku.lp");
	  if (error) {
		  printf("ERROR %d GRBwrite(): %s\n", error, GRBgeterrormsg(env));
		  GRBfreemodel(model);
		  GRBfreeenv(env);
		  release_gurobi_vars_memory(vars_ptr);
		  free(vtype);
		  return -1;
	  }

	  /* print results */
	  printf("\nOptimization complete\n");


	  /* Get solution information */
	  error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
	   if (error) {
	 	  printf("ERROR %d GRBgetintattr(): %s\n", error, GRBgeterrormsg(env));
		  GRBfreemodel(model);
		  GRBfreeenv(env);
		  release_gurobi_vars_memory(vars_ptr);
		  free(vtype);
	 	  return -1;
	   }


	   /* solution found */
	   if (optimstatus == GRB_OPTIMAL) {
		   sol = (double*)calloc(vars_ptr->var_count, sizeof(double));
		   if (sol == NULL){
			   printf("ERROR: Problem in memory allocating");
			   exit(1);
		   }

		  /* get the solution - the assignment to each variable */
		  error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, vars_ptr->var_count, sol);
		  if (error) {
			  printf("ERROR %d GRBgetdblattrarray(): %s\n", error, GRBgeterrormsg(env));
			  GRBfreemodel(model);
			  GRBfreeenv(env);
			  release_gurobi_vars_memory(vars_ptr);
			  free(vtype);
			  free(sol);
			  return -1;
		  }

		  i = 0;
		  for (;i< vars_ptr->var_count; i++) {
			  /*make sure not to fill illegal values created along the way for "guess" command.*/
			  if (sol[i] >= threshold && is_legal_cell(board, vars_ptr->vars[i]->i, vars_ptr->vars[i]->j, vars_ptr->vars[i]->k)){
				  board->board[vars_ptr->vars[i]->i][vars_ptr->vars[i]->j].value = vars_ptr->vars[i]->k;
			  }
		  }



		  free(sol);

	 	  GRBfreemodel(model);
	 	  GRBfreeenv(env);
	 	  release_gurobi_vars_memory(vars_ptr);
	 	  free(vtype);
	 	  return 1;
	   }
	   /* no solution found */
	   else if (optimstatus == GRB_INF_OR_UNBD || optimstatus == GRB_INFEASIBLE || optimstatus == GRB_UNBOUNDED) {
	 	  GRBfreemodel(model);
	 	  GRBfreeenv(env);
	 	  release_gurobi_vars_memory(vars_ptr);
	 	  free(vtype);
	     return 0;
	   }
	   /* error or calculation stopped */
	   else {
	      printf("Optimization was stopped early\n");
	      fflush(stdout);
	 	 GRBfreemodel(model);
	 	 GRBfreeenv(env);
	 	 release_gurobi_vars_memory(vars_ptr);
	 	 free(vtype);
	 	 return -1;
	   }
	}
