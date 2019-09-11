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
#include "main_aux.h"
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

/*
 * creates an array of size number_of_empty_cells of empty cells.
 */
cell_node* empty_cells_indexes(board* game_board, int number_of_empty_cells);


int is_legal_cell(board* current_board, int row, int col, int num) {
	int rows_of_blocks = current_board->n;
	int cols_of_blocks = current_board->m;
	int N = rows_of_blocks * cols_of_blocks;
	board_cell** game_board = current_board->board;

	if(num == 0){
		return 1;
	}

	return is_row_legal(game_board, row, col, num, N)
			&& is_col_legal(game_board, row, col, num, N)
			&& is_block_legal(game_board, row, col, num, rows_of_blocks, cols_of_blocks);
}

int is_row_legal(board_cell** game_board, int row, int col, int num, int N) {
	int i;

	for (i = 0; i < N; i++) {
		if (game_board[row][i].value == num && i != col) {
			return 0;
		}
	}
	return 1;
}

int is_col_legal(board_cell** game_board, int row, int col, int num, int N) {
	int i;

	for (i = 0; i < N; i++) {
		if (game_board[i][col].value == num && i != row) {
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
			else if (game_board[i][j].value == num) {
				return 0;
			}
		}
	}
	return 1;
}

cell_node* empty_cells_indexes(board* game_board, int number_of_empty_cells){
	int N = game_board->m * game_board->n;
	int i;
	int j;
	int k = 0;
	cell_node* ret = (cell_node*)calloc(number_of_empty_cells, sizeof(cell_node));

	if (ret == NULL){
		printf("ERROR: memory allocation failed");
		exit(1);
	}

	for (i = 0; i < N; i++){
		for (j = 0; j < N; j++){
			if(game_board->board[i][j].value == 0){
				ret[k].row_index = i;
				ret[k].col_index = j;
				ret[k].value = 1;
				k++;
			}
		}
	}
	return ret;
}

int exhaustive_backtracking_solution_count(board* game_board) {
	int N = game_board->m * game_board->n;
	int current_index = 0;
	int num_solutions = 0;
	Stack* backtrack_stack = NULL;
	cell_node* current_cell_node;
	int number_of_empty_cells;
	cell_node* array_empty_indexes;

	number_of_empty_cells = num_empty_cells(game_board); /*get number of empty cells in the board*/
	if (number_of_empty_cells == 0){ /*no empty cells found so the board is solved and has 1 solution*/
		return 1;
	}

	/*get empty cells indexes and put it in a array*/
	array_empty_indexes = empty_cells_indexes(game_board, number_of_empty_cells);
	backtrack_stack = stack_init();

	stack_push(backtrack_stack, &array_empty_indexes[0]); /*insert first empty cell to stack*/

	while(get_stack_size(backtrack_stack) > 0){
		current_cell_node = (cell_node*)get_stack_top_element(backtrack_stack);

		if(current_cell_node->value > N){ /*all possible values has been checked and none is legal*/
			array_empty_indexes[current_index].value = 1;
			game_board->board[current_cell_node->row_index][current_cell_node->col_index].value = 0;
			stack_pop(backtrack_stack); /*pop from stack*/
			current_index -= 1;
			if(get_stack_size(backtrack_stack) > 0){
				current_cell_node = (cell_node*)get_stack_top_element(backtrack_stack);
				current_cell_node->value += 1;
			}
		}
		/*if this value is legal*/
		else if(is_legal_cell(game_board, current_cell_node->row_index, current_cell_node->col_index, current_cell_node->value)){
			game_board->board[current_cell_node->row_index][current_cell_node->col_index].value = current_cell_node->value;
			if(current_index < number_of_empty_cells - 1){
				current_index += 1;
				current_cell_node = &array_empty_indexes[current_index];
				stack_push(backtrack_stack, current_cell_node);
				continue;
			}
			num_solutions++;
			current_cell_node->value += 1;
		}
		/*value is not legal and is smaller than N*/
		else{
			current_cell_node->value += 1;
		}
	}


	free(array_empty_indexes);
	free_stack_mem(backtrack_stack);

	return num_solutions;

}
