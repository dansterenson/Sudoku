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

void freeGRB_vtype_obj (GRBenv *env, GRBmodel *model, GRB_vars* vars_ptr, char* vtype, double* obj);

int gurobi_main_LP(board* game_board, double chosen_threshold)
{
  double*    sol;
  int       optimstatus;
  GRB_vars* vars_ptr;
  int       i;
  GRBenv   *env   = NULL;
  GRBmodel *model = NULL;
  char*      vtype = NULL;
  double* 	obj = NULL;
  int       error_occurred = FALSE;
  int board_dimensions = game_board->n * game_board->m;

  /* Create environment and a log file for the game log */
  error_occurred = GRBloadenv(&env, "game_log.log");
  if (error_occurred) {
		printf("ERROR %d GRBloadenv(): %s\n", error_occurred, GRBgeterrormsg(env));
		printf("GUROBI RELATED ERROR OCCURED.\n");
		fflush(stdout);
		return -1;
  }

  /* disable log to console */
  error_occurred = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
  if (error_occurred) {
		printf("ERROR %d GRBsetintattr(): %s\n", error_occurred, GRBgeterrormsg(env));
		printf("GUROBI RELATED ERROR OCCURED.\n");
		fflush(stdout);
		GRBfreeenv(env);
		return -1;
  }

  /* Create an empty model for the game named "sudoku" */
  error_occurred = GRBnewmodel(env, &model, "sudoku", 0, NULL, NULL, NULL, NULL, NULL);
  if (error_occurred) {
		printf("ERROR %d GRBnewmodel(): %s\n", error_occurred, GRBgeterrormsg(env));
		printf("GUROBI RELATED ERROR OCCURED.\n");
		fflush(stdout);
		GRBfreeenv(env);
		return -1;
  }

  vars_ptr = get_grb_vars(game_board);

  /* no vars created - that means there are no possible solutions for the board */
  if (vars_ptr->var_count == 0) {
	    freeGRB_vtype_obj(env, model, vars_ptr, vtype, obj);
		return 0;
  }

  vtype = (char*)calloc(vars_ptr->var_count, sizeof(char));

  if (vtype == NULL) {
		printf("ERROR: memory allocation failed");
	    freeGRB_vtype_obj(env, model, vars_ptr, vtype, obj);
		exit(1);
  }

  obj = (double*)calloc(vars_ptr->var_count, sizeof(double));

  if (obj == NULL){
		printf("ERROR: memory allocation failed");
	    freeGRB_vtype_obj(env, model, vars_ptr, vtype, obj);
		exit(1);
  }

  /* Use random weights for objective function to determine a good one */
  for (i = 0 ; i < vars_ptr->var_count; i++){
		vtype[i] = GRB_CONTINUOUS;
		obj[i] =(double)(rand()%board_dimensions);
  }

  /* add vars to GRB model */
  error_occurred = GRBaddvars(model, vars_ptr->var_count, 0, NULL, NULL, NULL, obj, NULL, NULL, vtype, NULL);

  if (error_occurred) {
		printf("ERROR %d GRBaddvars(): %s\n", error_occurred, GRBgeterrormsg(env));
		printf("GUROBI RELATED ERROR OCCURED.\n");
		fflush(stdout);
	    freeGRB_vtype_obj(env, model, vars_ptr, vtype, obj);
		return -1;
  }

  /* Set the objective to MAX, it's arbitrary because we simply want a feasible solution. */
  error_occurred = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
  if (error_occurred) {
		printf("ERROR %d GRBsetintattr(): %s\n", error_occurred, GRBgeterrormsg(env));
		printf("GUROBI RELATED ERROR OCCURED.\n");
		fflush(stdout);
	    freeGRB_vtype_obj(env, model, vars_ptr, vtype, obj);
		return -1;
  }

  /* Update model to integrate new variables into the solution */

  error_occurred = GRBupdatemodel(model);
  if (error_occurred) {
		printf("ERROR %d GRBupdatemodel(): %s\n", error_occurred, GRBgeterrormsg(env));
		printf("GUROBI RELATED ERROR OCCURED.\n");
		fflush(stdout);
	    freeGRB_vtype_obj(env, model, vars_ptr, vtype, obj);
		return -1;
  }

  /* Handle single value constraints for each cell. */
  error_occurred = handle_single_value_constraints(vars_ptr, model);
  if (error_occurred == -1) {
	    freeGRB_vtype_obj(env, model, vars_ptr, vtype, obj);
		return -1;

  }

  error_occurred = handle_row_col_constraints(ROW, game_board, vars_ptr, model);
  if (error_occurred == -1) {
	    freeGRB_vtype_obj(env, model, vars_ptr, vtype, obj);
		return -1;
  }

  error_occurred = handle_row_col_constraints(COLUMN, game_board, vars_ptr, model);
  if (error_occurred == -1) {
	    freeGRB_vtype_obj(env, model, vars_ptr, vtype, obj);
		return -1;
  }


  error_occurred = handle_block_constraints(game_board, vars_ptr, model);
  if (error_occurred == -1) {
	    freeGRB_vtype_obj(env, model, vars_ptr, vtype, obj);
		return -1;
  }

  /* Handle constraints for LP guess solution - each var represents a probability. */
  error_occurred = handle_constraints_LP(vars_ptr, model);
  if (error_occurred == -1) {
	    freeGRB_vtype_obj(env, model, vars_ptr, vtype, obj);
		return -1;
  }

  /* Optimize model - need to call this before actual calculation */
  error_occurred = GRBoptimize(model);
  if (error_occurred) {
		printf("ERROR %d GRBoptimize(): %s\n", error_occurred, GRBgeterrormsg(env));
	    freeGRB_vtype_obj(env, model, vars_ptr, vtype, obj);
		return -1;
  }

  /* Write model to 'sudoku.lp' */
  error_occurred = GRBwrite(model, "sudoku.lp");
  if (error_occurred) {
		printf("ERROR %d GRBwrite(): %s\n", error_occurred, GRBgeterrormsg(env));
	    freeGRB_vtype_obj(env, model, vars_ptr, vtype, obj);
		return -1;
  }

  /* Get solution information */
  error_occurred = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
   if (error_occurred) {
		printf("ERROR %d GRBgetintattr(): %s\n", error_occurred, GRBgeterrormsg(env));
	    freeGRB_vtype_obj(env, model, vars_ptr, vtype, obj);
		return -1;
   }

   /* Optimal solution found */
   if (optimstatus == GRB_OPTIMAL) {
	  printf("Optimal Solution found!\n");
	  sol = (double*)calloc(vars_ptr->var_count, sizeof(double));

	  if (sol == NULL){
		 printf("ERROR: memory allocation failed");
		 freeGRB_vtype_obj(env, model, vars_ptr, vtype, obj);
		 exit(1);
	  }

	   /* Get attribute array and assign it to each variable. */
	  error_occurred = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, vars_ptr->var_count, sol);

	   if (error_occurred) {
		  printf("ERROR %d GRBgetdblattrarray(): %s\n", error_occurred, GRBgeterrormsg(env));
		  freeGRB_vtype_obj(env, model, vars_ptr, vtype, obj);
		  free(sol);
		  return -1;
	   }

	   /* If no error occurred - use the attributes to solve the board */
	   for(i = 0; i < vars_ptr->var_count; i++){
		   if (is_legal_cell(game_board, vars_ptr->vars[i]->i, vars_ptr->vars[i]->j, vars_ptr->vars[i]->k)
				   && sol[i] >= chosen_threshold){
			   game_board->board[vars_ptr->vars[i]->i][vars_ptr->vars[i]->j].value = vars_ptr->vars[i]->k;
		   }
	   }

	   free(sol);
	   freeGRB_vtype_obj(env, model, vars_ptr, vtype, obj);
	   return 1;
	}

    /* No solution found - solution is either infeasible or unbounded  */
	else if (optimstatus == GRB_INF_OR_UNBD) {
		printf("No solution found :(\n");
		freeGRB_vtype_obj(env, model, vars_ptr, vtype, obj);
		return 0;
	}
	/* Something else happened - that stopped the optimization process */
	else {
		printf("Optimization process stopped early, some odd error occurred.\n");
		fflush(stdout);
		freeGRB_vtype_obj(env, model, vars_ptr, vtype, obj);
		return -1;
	}
}

void freeGRB_vtype_obj (GRBenv *env, GRBmodel *model, GRB_vars* vars_ptr, char* vtype, double* obj) {
		GRBfreemodel(model);
		GRBfreeenv(env);
		free_all_grb_vars(vars_ptr);
		free(vtype);
		free(obj);
}


