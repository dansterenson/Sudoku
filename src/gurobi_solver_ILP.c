#include <stdlib.h>
#include <stdio.h>
#include "gurobi_structs.h"
#include "gurobi_helper_utils.h"
#include "gurobi_solver_ILP.h"
#include "gurobi_c.h"

int gurobi_main_ILP(board* game_board, int should_fill_board)
{
  double*   sol;
  char*     vtype;
  int       optimstatus;
  GRB_vars* grb_vars_ptr;
  GRBenv   *env   = NULL;
  GRBmodel *model = NULL;
  int       error_occurred = FALSE;

  /* Create an environment. log file is: sudoku_log.log */
  error_occurred = GRBloadenv(&env, "sudoku_log.log");
  if (error_occurred) {
	  printf("Error: gurobi related error occured.\n");
	  printf("Error: %d GRBloadenv(): %s\n", error_occurred, GRBgeterrormsg(env));
	  free_grb_model_env(model, env);
	  return -1;
  }

  /* disable log to console */
  error_occurred = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
  if (error_occurred) {
	  printf("Error: Gurobi related error occurred.\n");
	  printf("Error: %d GRBsetintattr(): %s\n", error_occurred, GRBgeterrormsg(env));
	  free_grb_model_env(model, env);
	  return -1;
  }

  /* Create an empty model for the game named "sudoku" */
  error_occurred = GRBnewmodel(env, &model, "sudoku", 0, NULL, NULL, NULL, NULL, NULL);
  if (error_occurred) {
	  printf("Error: Gurobi related error occurred.\n");
	  printf("Error: %d GRBnewmodel(): %s\n", error_occurred, GRBgeterrormsg(env));
	  free_grb_model_env(model, env);
	  return -1;
  }


  grb_vars_ptr = get_grb_vars(game_board);

  /* No vars created - no possible solutions */
  if(no_vars_created(grb_vars_ptr)) {
	  free_grb_model_env(model, env);
	  free_all_grb_vars(grb_vars_ptr);
	  return 0;
  }

  vtype = (char*)calloc(grb_vars_ptr->var_count, sizeof(char));

  if (vtype == NULL){
	printf("Error: problem in memory allocation.\n");
	exit(1);
  }

  set_vtype_grb_binary(vtype, grb_vars_ptr->var_count);

  /* add variables created to GRB model */
  error_occurred = GRBaddvars(model, grb_vars_ptr->var_count, 0, NULL, NULL, NULL, NULL, NULL, NULL, vtype, NULL);
  if (error_occurred) {
	  printf("Error: Gurobi related error occurred.\n");
	  printf("Error: %d GRBaddvars(): %s\n", error_occurred, GRBgeterrormsg(env));
	  free_grb_model_env(model, env);
	  free_all_grb_vars(grb_vars_ptr);
	  free(vtype);
	  return -1;
  }

  /* Set the objective to MAX, it's arbitrary because we simply want a feasible solution. */
  error_occurred = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
  if (error_occurred) {
	  printf("Error: Gurobi related error occurred.\n");
	  printf("Error: %d GRBsetintattr(): %s\n", error_occurred, GRBgeterrormsg(env));
	  free_grb_model_env(model, env);
	  free_all_grb_vars(grb_vars_ptr);
	  free(vtype);
	  return -1;
  }

  /* Update model to integrate new variables into the solution */
  error_occurred = GRBupdatemodel(model);
  if (error_occurred) {
	  printf("Error: Gurobi related error occurred.\n");
	  printf("Error: %d GRBupdatemodel(): %s\n", error_occurred, GRBgeterrormsg(env));
	  free_grb_model_env(model, env);
	  free_all_grb_vars(grb_vars_ptr);
	  free(vtype);
	  return -1;
  }

  /* Handle ILP constraints */

  error_occurred = handle_single_value_constraints(grb_vars_ptr, model);
  if (error_occurred == -1) {
	  printf("Error: Gurobi related error - setting constraints single.\n");
	  free_grb_model_env(model, env);
	  free_all_grb_vars(grb_vars_ptr);
	  free(vtype);
	  return -1;

  }

  error_occurred = handle_row_col_constraints(ROW, grb_vars_ptr, model, game_board);
  if (error_occurred == -1) {
	  printf("Error: Gurobi related error - setting constraints row.\n");
	  free_grb_model_env(model, env);
	  free_all_grb_vars(grb_vars_ptr);
	  free(vtype);
	  return -1;
  }

  error_occurred = handle_row_col_constraints(COLUMN, grb_vars_ptr, model, game_board);
  if (error_occurred == -1) {
	  printf("Error: Gurobi related error - setting constraints col.\n");
	  free_grb_model_env(model, env);
	  free_all_grb_vars(grb_vars_ptr);
	  free(vtype);
	  return -1;
  }

  error_occurred = handle_block_constraints(grb_vars_ptr, model, game_board);
  if (error_occurred == -1) {
	  printf("Error: Gurobi related error - setting constraints block.\n");
	  free_grb_model_env(model, env);
	  free_all_grb_vars(grb_vars_ptr);
	  free(vtype);
	  return -1;
  }

  /* Optimize model */
  error_occurred = GRBoptimize(model);
  if (error_occurred) {
	  printf("Error: Gurobi related error occurred.\n");
	  printf("Error: %d GRBoptimize(): %s\n", error_occurred, GRBgeterrormsg(env));
	  free_grb_model_env(model, env);
	  free_all_grb_vars(grb_vars_ptr);
	  free(vtype);
	  return -1;
  }

  error_occurred = GRBwrite(model, "sudoku.lp");
  if (error_occurred) {
	  printf("Error: Gurobi related error occurred.\n");
	  printf("Error: %d GRBwrite(): %s\n", error_occurred, GRBgeterrormsg(env));
	  free_grb_model_env(model, env);
	  free_all_grb_vars(grb_vars_ptr);
	  free(vtype);
	  return -1;
  }


  /* Get solution information */
  error_occurred = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
   if (error_occurred) {
	  printf("Error: Gurobi related error occurred.\n");
 	  printf("Error: %d GRBgetintattr(): %s\n", error_occurred, GRBgeterrormsg(env));
	  free_grb_model_env(model, env);
	  free_all_grb_vars(grb_vars_ptr);
	  free(vtype);
 	  return -1;
   }

  if(should_fill_board) {
	  if(optimstatus == GRB_OPTIMAL) {
		  sol = (double*)calloc(grb_vars_ptr->var_count, sizeof(double));
		  if (sol == NULL){
			  printf("Error: problem in memory allocation.\n");
			  exit(1);
		  }

		   /* Get attribute array and assign it to each variable. */
		  error_occurred = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, grb_vars_ptr->var_count, sol);
		  if (error_occurred) {
			  printf("Error: %d GRBgetdblattrarray(): %s\n", error_occurred, GRBgeterrormsg(env));
			  free_grb_model_env(model, env);
			  free_all_grb_vars(grb_vars_ptr);
			  free(vtype);
			  free(sol);
			  return -1;
		   }

		   fill_board_ILP(game_board->board, grb_vars_ptr, sol);
		   free(sol);
	  }

  }

  /* Optimal solution found */
  if (optimstatus == GRB_OPTIMAL) {
	  free_grb_model_env(model, env);
	  free_all_grb_vars(grb_vars_ptr);
	  free(vtype);
	  return 1;
  }
  /* No solution found - solution is either infeasible or unbounded  */
  else if (optimstatus == GRB_INF_OR_UNBD || optimstatus  == GRB_INFEASIBLE || optimstatus == GRB_UNBOUNDED) {
	  free_grb_model_env(model, env);
	  free_all_grb_vars(grb_vars_ptr);
	  free(vtype);
    return 0;
  }
  /* error occurred or calculation stopped for an unknown reason */
  else {
	 free_grb_model_env(model, env);
	 free_all_grb_vars(grb_vars_ptr);
	 free(vtype);
	 return -1;
  }
}
