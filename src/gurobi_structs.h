/*
 * gurobi_structs.h
 *
 *  Created on: Aug 10, 2019
 *      Author: GuyErez
 */

#ifndef GUROBI_STRUCTS_H_
#define GUROBI_STRUCTS_H_
 /*
  * This module contains custom Structs used in Gurobi models (Both LP and ILP),
  * to store the relevant variables for Linear Programming calculations.
  * These Structs spare us from traversing the game board multiple times.
  */

/*
 * The Gurobi variable - x_ijk.
 * i,j indicating the cell's coordinates and k the cell's value.
 */
typedef struct GRBvariable{
	int i;
	int j;
	int k;
} GRBvariable;

/*
 * A custom struct containing a matrix of Gurobi variables
 * and the amount of valid Gurobi variables to be used for optimization.
 */
typedef struct GRB_vars{
	int var_count;
	GRBvariable** vars;
} GRB_vars;

#endif /* GUROBI_STRUCTS_H_ */
