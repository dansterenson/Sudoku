/*
 * game_utils.c
 *
 *  Created on: Aug 17, 2019
 *      Author: GuyErez
 */
#include "structures.h"
#include "game_utils.h"
#include "stack.h"
#include <stdlib.h>
#include <stdio.h>

/*
 * Checks if entering the current num at the specified cell
 * is a legal placement for that row.
 */
int is_row_legal(board_cell** game_board, int row, int col, int num, int N);

/*
 * Checks if entering the current num at the specified cell
 * is a legal placement for that col.
 */
int is_col_legal(board_cell** game_board, int row, int col, int num, int N);

/*
 * Checks if entering the current num at the specified cell
 * is a legal placement for that block.
 */
int is_block_legal(board_cell** game_board, int row, int col, int num, int rows_of_blocks, int cols_of_blocks);

/* Create a new stack node with relevant details about the cell it simulates */
cell_node* create_stack_node(int row, int col, int value);

/* Get the top stack node, and increment it's value (using cell_node* implementation for stack node) */
void increment_top_node_value(Stack* stack);

int is_legal_cell(board* current_board, int row, int col, int num) {
	int rows_of_blocks = current_board->n;
	int cols_of_blocks = current_board->m;
	int N = rows_of_blocks * cols_of_blocks;
	board_cell** game_board = current_board->board;

	return is_row_legal(game_board, row, col, num, N)
			&& is_col_legal(game_board, row, col, num, N)
			&& is_block_legal(game_board, row, col, num, rows_of_blocks, cols_of_blocks);
}

int is_row_legal(board_cell** game_board, int row, int col, int num, int N) {
	int i;

	for (i = 0; i < N; i++) {
		if (game_board[i][row].value == num && i != col) {
			return 0;
		}
	}
	return 1;
}

int is_col_legal(board_cell** game_board, int row, int col, int num, int N) {
	int i;

	for (i = 0; i < N; i++) {
		if (game_board[col][i].value == num && i != row) {
			return 0;
		}
	}
	return 1;
}

int is_block_legal(board_cell** game_board, int row, int col, int num, int rows_of_blocks, int cols_of_blocks) {
	int block_start_row = row - row % rows_of_blocks;
	int block_start_col = col - col % cols_of_blocks;
	int i = block_start_row;
	int j = block_start_col;

	for(i = block_start_row; i < block_start_row + rows_of_blocks; i++) {
		for(j = block_start_col; j < block_start_col + cols_of_blocks; j++) {
			if (i == row && j == col) {
				continue;
			}
			else if (game_board[j][i].value == num) {
				return 0;
			}
		}
	}
	return 1;
}

int exhaustive_backtracking_solution_count(board* game_board) {
	Stack* helper_stack;
	cell_node* tmp_stack_node;
	int solution_count = 0;
	int N = game_board->n * game_board->m;

	/* Init stack and push first stack node with the first possible value: 1. */
	helper_stack = stack_init();
	tmp_stack_node = create_stack_node(0,0,1);
	stack_push(helper_stack, tmp_stack_node);

	while(get_stack_size(helper_stack) > 0) {
		tmp_stack_node = (cell_node*)get_stack_top_element(helper_stack);

		/* there are still legal values for this cell */
		if (tmp_stack_node->value <= N) {
			/* In case cell is fixed */
			if (game_board->board[tmp_stack_node->row_index][tmp_stack_node->col_index].is_fixed == 1) {
				/* Check if the fixed cell has the stack's top element's value */
				if (game_board->board[tmp_stack_node->row_index][tmp_stack_node->col_index].value == tmp_stack_node->value){
					/* the cell's placement is legal for this board */
					if(is_legal_cell(game_board,tmp_stack_node->row_index, tmp_stack_node->col_index, tmp_stack_node->value)){
						/* If it's a legal placement and we've reached the last cell - we have a solution! */
						if (tmp_stack_node->row_index == N-1 && tmp_stack_node->col_index == N-1){
							solution_count++;

							/* Use stack to simulate recursive backtracking */
							stack_pop(helper_stack);
							increment_top_node_value(helper_stack);
						}
						/* Cell is legal, but we haven't reached the board's last cell */
						else {
							/* Check if we're at the last column */
							if (tmp_stack_node->col_index == N-1){
								tmp_stack_node->row_index +=1;
								tmp_stack_node->col_index = 0;
								tmp_stack_node->value = 1;
								stack_push(helper_stack, tmp_stack_node);

							} else {
								tmp_stack_node->col_index += 1;
								tmp_stack_node->value = 1;
								stack_push(helper_stack, tmp_stack_node);
							 }
						 }
					 }
					/* Cell placement isn't legal */
					else{
						increment_top_node_value(helper_stack);
					 }
				}
				else {
					increment_top_node_value(helper_stack);
				}

			}
			/* Cell isn't fixed */
			else {
				if(is_legal_cell(game_board,tmp_stack_node->row_index, tmp_stack_node->col_index, tmp_stack_node->value)){
					/* If it's a legal placement and we've reached the last cell - we have a solution! */
					if (tmp_stack_node->row_index == N-1 && tmp_stack_node->col_index == N-1){
						solution_count++;

						/*backtrack*/
						game_board->board[tmp_stack_node->row_index][tmp_stack_node->col_index].value = 0;
						stack_pop(helper_stack);
						increment_top_node_value(helper_stack);
					}
					/* Cell is legal, but we haven't reached the board's last cell */
					else {
						game_board->board[tmp_stack_node->row_index][tmp_stack_node->col_index].value = tmp_stack_node->value;
						/* Check if we're at the last column */
						if (tmp_stack_node->col_index == N-1){
							tmp_stack_node->row_index +=1;
							tmp_stack_node->col_index = 0;
							tmp_stack_node->value = 1;
							stack_push(helper_stack, tmp_stack_node);

						} else {
							tmp_stack_node->col_index += 1;
							tmp_stack_node->value = 1;
							stack_push(helper_stack, tmp_stack_node);
						 }
					 }
				 }
				/* Cell placement isn't legal */
				else {
					increment_top_node_value(helper_stack);
				 }
			}
		}
		/* all possible legal values for this cell have been used */
		else {
			/* If we're back at 0,0 we finished going through the entire board */
			if (tmp_stack_node->row_index == 0 && tmp_stack_node->col_index == 0){
				break;
			}
			else {
				 /* If cell isn't fixed - change it's value to 0 and backtrack.*/
				if (game_board->board[tmp_stack_node->row_index][tmp_stack_node->col_index].is_fixed != 1){
					game_board->board[tmp_stack_node->row_index][tmp_stack_node->col_index].value = 0;
				}

				stack_pop(helper_stack);
				increment_top_node_value(helper_stack);
			}
		}
	}

	free_stack_mem(helper_stack);

	return solution_count;
}

cell_node* create_stack_node(int row, int col, int value) {
	cell_node* stack_node;

	stack_node = (cell_node*)calloc(1, sizeof(cell_node));

	if (stack_node == NULL){
		printf("ERROR: memory allocation failed");
		exit(1);
	}

	stack_node->row_index = row;
	stack_node->col_index = col;
	stack_node->value = value;

	return stack_node;
}

void increment_top_node_value(Stack* stack) {
	cell_node* tmp_stack_node = (cell_node*)get_stack_top_element(stack);
	tmp_stack_node->value +=1;
}



