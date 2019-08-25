/*
 * game_utils.c
 *
 *  Created on: Aug 17, 2019
 *      Author: GuyErez
 */
#include "structures.h"
#include "game_utils.h"
#include <stdlib.h>
#include <stdio.h>

/*
 * Checks if entering the current num at the specified cell
 * is a legal placement for that row.
 */
int is_row_legal(board_cell** game_board, int row, int col, int num, int dimensions);

/*
 * Checks if entering the current num at the specified cell
 * is a legal placement for that col.
 */
int is_col_legal(board_cell** game_board, int row, int col, int num, int dimensions);

/*
 * Checks if entering the current num at the specified cell
 * is a legal placement for that block.
 */
int is_block_legal(board_cell** game_board, int row, int col, int num, int board_rows, int board_cols);

/* Check number of empty cells we have in the game board */
int get_empty_cell_count(board* game_board);

/* Create a new stack node with relevant details about the cell it simulates */
cell_node* create_stack_node(int row, int col, int value);

int is_legal_cell(board* current_board, int row, int col, int num) {
	int block_start_row;
	int block_start_col;
	int i,j;
	int board_rows = current_board->n;
	int board_cols = current_board->m;
	int dimensions = board_rows * board_cols;
	board_cell** game_board = current_board->board;

	return is_row_legal(game_board, row, col, num, dimensions)
			&& is_col_legal(game_board, row, col, num, dimensions)
			&& is_block_legal(game_board, row, col, num, board_rows, board_cols);
}

int is_row_legal(board_cell** game_board, int row, int col, int num, int dimensions) {
	int i;

	for (i = 0; i < dimensions; i++) {
		if (game_board[i][row].value == num && i != col) {
			return 0;
		}
	}
	return 1;
}

int is_col_legal(board_cell** game_board, int row, int col, int num, int dimensions) {
	int i;

	for (i = 0; i < dimensions; i++) {
		if (game_board[col][i].value == num && i != row) {
			return 0;
		}
	}
	return 1;
}

int is_block_legal(board_cell** game_board, int row, int col, int num, int board_rows, int board_cols) {
	int i,j;
	int block_start_row = row - row % board_rows;
	int block_start_col = col - col % board_cols;
	int i = block_start_row;
	int j = block_start_col;

	for(i = block_start_row; i < block_start_row + board_rows; i++) {
		for(j = block_start_col; j < block_start_col + board_cols; j++) {
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
	int i,j;
	int empty_cell_count;
	int solution_count = 0;
	int dimensions = game_board->n * game_board->m;

	empty_cell_count = get_empty_cell_count(game_board);

	/* Init stack and push first stack node with the first possible value - 1. */
	stack_init(helper_stack);
	tmp_stack_node = create_stack_node(0,0,1);
	stack_push(helper_stack, tmp_stack_node);

	while(get_stack_size(helper_stack) > 0) {
		tmp_stack_node = get_stack_top_element(helper_stack);

		/* there are still legal values for this cell */
		if (tmp_stack_node->value <= dimensions) {
			/* In case cell is fixed */
			if (game_board->board[tmp_stack_node->row_index][tmp_stack_node->col_index].is_fixed == 1) {
				/* Check if the fixed cell has the stack's top element's value */
				if (game_board->board[tmp_stack_node->row_index][tmp_stack_node->col_index].value == tmp_stack_node->value){
					/* the cell's placement is legal for this board */
					if(is_legal_cell(game_board,tmp_stack_node->row_index, tmp_stack_node->col_index, tmp_stack_node->value)){
						/* If it's a legal placement and we've reached the last cell - we have a solution! */
						if (tmp_stack_node->row_index == dimensions-1 && tmp_stack_node->col_index == dimensions-1){
							solution_count++;

							/* Use stack to simulate recursive backtracking */
							stack_pop(helper_stack);
							increment_top_node_value(helper_stack);
						}
						/* Cell is legal, but we haven't reached the board's last cell */
						else {
							/* Check if we're at the last column */
							if (tmp_stack_node->col_index == dimensions-1){
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
					if (tmp_stack_node->row_index == dimensions-1 && tmp_stack_node->col_index == dimensions-1){
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
						if (tmp_stack_node->col_index == dimensions-1){
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

}

int get_empty_cell_count(board* game_board) {
	int i,j;
	int dimensions = game_board->n * game_board->m;
	int empty_cell_count = 0;

	for (i = 0; i < dimensions; i++){
		for (j = 0; j < dimensions; j++){
			if (game_board->board[i][j].value == 0){
				empty_cell_count++;
			}
		}
	}

	return empty_cell_count;
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
	cell_node* tmp_stack_node = get_stack_top_element(stack);
	tmp_stack_node->value +=1;
}


