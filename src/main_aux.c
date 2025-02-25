#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "structures.h"
#include "linked_list.h"
#include "main_aux.h"
#include "game_utils.h"


void memory_alloc_problem() {
	printf("Error: problem in memory allocation.\n");
	exit(EXIT_FAILURE);
}

/*
 * check if the given string is empty.
 */
 
int is_empty_command(char* str_to_check){
	unsigned int i;
	for(i = 0; i < strlen(str_to_check); i++){
		if(str_to_check[i] != '\n' && str_to_check[i] != '\t' && str_to_check[i] != '\r' && str_to_check[i] != ' '){
			return false; /*not empty*/
		}
	}
	return true; /*empty string*/
}


int get_command_from_user(char* command){ /*from hw3*/
	int i;
	char c;
	do{
		printf("Enter command please: \n");
		for(i = 0; (c = fgetc(stdin)) != '\n'; i++){
			if(i > COMMAND_SIZE){
				printf("Command is too long\n");
				while((c = fgetc(stdin)) != '\n' && c != EOF); /*long command so flush buffer*/

				if (ferror(stdin)){
					printf("Error: fail in fgetc\n");
					exit (1);
				}
				return -1;
			}
			if(c == EOF){
				return 0;
			}
			command[i] = c;
		}
		command[i] = '\0';
		if(ferror(stdin)){
			printf("Error: fail in fgetc\n");
			exit(1);
		}
	}
	while(is_empty_command(command));
	return 1;
}

board* create_board(int n, int m){
	int N = n*m;
	int i,j;

	board* new_board = (board*)calloc(1, sizeof(board));
	if(new_board == NULL){
		memory_alloc_problem();
	}

	new_board->n = n;
	new_board->m = m;

	new_board->board = (board_cell**)calloc(N, sizeof(board_cell*));

	if(new_board->board == NULL){
		memory_alloc_problem();
	}

	for (i = 0; i < N; i++){
		new_board->board[i] = (board_cell*)calloc(N, sizeof(board_cell));
		if(new_board->board[i] == NULL){
			memory_alloc_problem();
		}
		for(j = 0; j < N; j++){
			new_board->board[i][j].is_error = false;
			new_board->board[i][j].is_fixed = false;
			new_board->board[i][j].value = 0;
		}
	}
	return new_board;
}

list* create_empty_list(){
	list* new_list;
	new_list = (list*) calloc(1, sizeof(list));
	if(new_list == NULL){
		memory_alloc_problem();
	}
	init_list(new_list);
	return new_list;
}

game* create_game(int n, int m, game_modes mode, bool is_mark_errors){
	board* new_board;
	list* undo_redo_list;
	game* game_new;

	game_new = (game*) calloc(1, sizeof(game));
	if(game_new == NULL){
		memory_alloc_problem();
	}


	undo_redo_list = create_empty_list();
	new_board = create_board(n, m);
	list_push(undo_redo_list, new_board);

	game_new->is_mark_errors = is_mark_errors;
	game_new->mode = mode;
	game_new->undo_redo_list = undo_redo_list;

	return game_new;
}

int cell_in_right_format(int n, int m, int cell){
	if(cell < 0 || cell > n*m){
		return false;
	}
	else{
		return true;
	}
}

void file_not_right_format(FILE *fp){
	printf("Error: file is not in the right format\n");
	if(fclose(fp) != 0){
		printf("Error: was not able to close file");
	}
}

/*
 *checks if the given file is empty, returns true if it is.
 */
int file_empty(FILE *fp){
	char c = '\0';
	int num_read = 0;
	num_read = fscanf(fp, " %c", &c);
	if(num_read == EOF){
		return true;
	}
	return false;
}

/*
 * updates the board cell's error value.
 */
void update_errors(board* loaded_board){
	int i;
	int j;
	int val;
	int N = loaded_board->n * loaded_board->m;

	for (i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			val = loaded_board->board[i][j].value;
			if(!is_legal_cell(loaded_board, i, j, val)){
				 loaded_board->board[i][j].is_error = true;
			}
		}
	}
}

/*
 * checks if the given board's fixed cells are erroneous. if so returns true.
 */
bool erroneus_fixed(board* board_to_check){
	board* new_board;
	int i;
	int j;
	int n = board_to_check->n;
	int m = board_to_check->m;
	int N = m * n;
	new_board = create_board(n, m);
	
	for (i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			if(board_to_check->board[i][j].is_fixed == true){
				new_board->board[i][j].value = board_to_check->board[i][j].value;
			}
			else{
				new_board->board[i][j].value = 0;
			}
		}
	}
	
	update_errors(new_board);

	if(board_is_erroneous(new_board) == true){
		free_board_mem(new_board);
		return true;
	}
	else{
		free_board_mem(new_board);	
		return false;
	}	
}

int load_game_from_file(char* path, board** loaded_board, int command){
	FILE *fp;
	int i,j;
	int new_m, new_n;
	int num_read = 0;
	int N;
	board* new_board;
	char is_fixed;

	fp = fopen(path, "r");

	if(fp == NULL){
		printf("Error: was not able to open file\n");
		return false;
	}

	num_read = fscanf(fp, " %d %d ", &new_m, &new_n);

	if(num_read != 2){
		printf("Error: could not parse board dimensions\n");
		fclose(fp);
		return false;
	}

	if(new_m < 0 || new_n < 0){
		printf("Error: board dimensions should be positive\n");
		fclose(fp);
		return false;
	}

	new_board = create_board(new_n, new_m);
	N = new_n * new_m;

	for(i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			num_read = fscanf(fp," %d%c ", &new_board->board[i][j].value, &is_fixed);

			if(num_read == 2){
				if(is_fixed == '.'){
					if(command == E_SOLVE_CMD){
						new_board->board[i][j].is_fixed = 1;
					}
					else{
						new_board->board[i][j].is_fixed = 0;
					}
				}
			}

			else if(num_read == 1){
				new_board->board[i][j].is_fixed = 0;
			}
			else if(num_read == -1){
				printf("Error: differences between board dimensions and number of cells.\n");
				fclose(fp);
				free_board_mem(new_board);
				return false;
			}
			else{
				printf("Error: failed reading the cells from loaded board.\n");
				fclose(fp);
				free_board_mem(new_board);
				return false;
			}

			if(new_board->board[i][j].value < 0 || new_board->board[i][j].value > N){
				printf("Error: file cells are not in the right range.\n");
				fclose(fp);
				free_board_mem(new_board);
				return false;
			}
		}
	}
	
	if(erroneus_fixed(new_board)){
		printf("Error: board contain fixed cells which are erroneous and therefore cannot be load\n");
		fclose(fp);
		free_board_mem(new_board);
		return false;
	}
	
	if(!file_empty(fp)){
		printf("Error: differences between board dimensions and number of cells.\n");
		fclose(fp);
		free_board_mem(new_board);
		return false;
	}

	*loaded_board = new_board;
	
	update_errors(*loaded_board);

	fclose(fp);
	return true;
}

int save_game_to_file(game* current_game, char* path){
	FILE *fp;
	board* current_board;
	int i,j,n,m;

	list* temp_list = current_game->undo_redo_list;
	current_board = (board*)(temp_list->head->data);
	n = current_board->n;
	m = current_board->m;

	fp = fopen(path, "w+");

	if(fp == NULL){
		printf("Error: was not able to open file\n");
		return false;
	}

	fprintf(fp, "%d %d\n", m, n);

	for(i = 0; i < n*m; i++){
		for(j = 0; j < n*m; j++){
			fprintf(fp, "%d",current_board->board[i][j].value);
			/*in edit mode every non empty cell is marked as fixed so needs a "."*/
			if(current_board->board[i][j].is_fixed == true
					|| (current_game->mode == edit && current_board->board[i][j].value != 0)){
				fprintf(fp, ".");
			}

			if(j == n*m - 1){
				fprintf(fp, "\n");
			}
			else fprintf(fp, " ");
		}
	}

	if(fclose(fp) != 0){
		printf("Error: was not able to close file\n");
		return false;
	}

	printf("Board saved to file\n");
	return true;
}


void free_board_mem(void* board_to_free){
	int i;
	board* temp_board = (board*)board_to_free;
	int N = temp_board->n * temp_board->m;

	for (i = 0; i < N; i++){
		free(temp_board->board[i]);
	}
	free(temp_board->board);
	free(temp_board);
}

void free_undo_redo_list(list* list_to_free){
	free_list_mem(list_to_free, free_board_mem);
}


void free_game_mem(game* game){
	free_undo_redo_list(game->undo_redo_list);
	free(game);
}

/*
 * prints a separating line.
 */
void print_separating_line(int n, int m){
	int N = n*m;
	int num_of_dots;
	int i;

	num_of_dots = 4*N + m + 1;

	for(i = 0; i < num_of_dots; i++){
		printf("-");
	}
	printf("\n");
}

void print_board(board* board_to_print, game* game){
	int m = board_to_print->m;
	int n = board_to_print->n;
	int N = n*m;
	int i, j;

	print_separating_line(n, m);
	for (i = 0; i < N; i++){
		printf("|");
		for (j = 0; j < N; j++){
			if(board_to_print->board[i][j].value == 0){
				printf("    ");
			}
			else{
				if(board_to_print->board[i][j].is_fixed == true){
					printf(" %2d.", board_to_print->board[i][j].value);
				}

				else if(board_to_print->board[i][j].is_error == true
						&& (game->is_mark_errors == true || game->mode == edit)){
					printf(" %2d*", board_to_print->board[i][j].value);
				}
				else{
					printf(" %2d ", board_to_print->board[i][j].value);
				}

			}
			if((j + 1) % (N/m) == 0){
				if((j + 1) != N){
					printf("|");
				}
				else{
					printf("|\n");
				}
			}
		}

		if((i + 1) % (N/n) == 0){
			print_separating_line(n,m);
		}
	}
}

board* copy_board(board* board_to_copy){
	int i,j,n,m;
	board* new_board;

	n = board_to_copy->n;
	m = board_to_copy->m;
	new_board = create_board(n, m);

	for (i = 0; i < n*m; i++){
		for(j = 0; j < n*m; j++){
			new_board->board[i][j].is_error = board_to_copy->board[i][j].is_error;
			new_board->board[i][j].is_fixed = board_to_copy->board[i][j].is_fixed;
			new_board->board[i][j].value = board_to_copy->board[i][j].value;
		}
	}
	return new_board;
}

int board_is_full(board* board_to_check){
	int i,j;

	int N = board_to_check->n*board_to_check->m;

	for (i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			if(board_to_check->board[i][j].value == 0){
				return false;
			}
		}
	}
	return true;
}

void init_cell_prob_struct(cell_probability* prob_struct, double* prob_arr, int row, int col){
	prob_struct->probability = prob_arr;
	prob_struct->row = row;
	prob_struct->col = col;
}
	
int board_is_erroneous(board* board_to_check){
	int i,j;

	int N = board_to_check->n*board_to_check->m;

	for (i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			if(board_to_check->board[i][j].is_error == 1){
				return true;
			}
		}
	}
	return false;
}

int num_empty_cells(board* board_to_check){
	int i,j;

	int N = board_to_check->n*board_to_check->m;
	int count = 0;

	for (i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			if(board_to_check->board[i][j].value == 0){
				count++;
			}
		}
	}
	return count;
}

int check_atoi_error(char* param_name, int after_convert, char* before_convert){
	if(after_convert == 0 && strcmp(before_convert, "0") != 0){
		printf("Error: parameter %s is invalid, should be a positive number\n", param_name);
		return true;
	}
	return false;
}

int check_range(char* param, int num_to_check, int lower_bound, int upper_bound){
	if(num_to_check < lower_bound || num_to_check > upper_bound){
		printf("Error: parameter %s is not in range, should be in range %d - %d\n",param, lower_bound, upper_bound);
		return false;
	}
	return true;
}

int check_board(board* current_board){
	int i,j;
	int n = current_board->n;
	int m = current_board->m;

	for(i = 0; i < m*n; i++){
		for(j = 0; j < m*n; j++){
			if(is_legal_cell(current_board,i,j,current_board->board[i][j].value) == 0) {
				return false;
			}
		}
	}
	return true;
}

void keep_y_cells(board* board_to_clear, int y){
	int N = board_to_clear->m * board_to_clear->n;
	int i = 0;
	int j = 0;
	int num_to_clear = N*N - y;

	while(num_to_clear > 0){
		i = rand()%N;
		j = rand()%N;

		if (board_to_clear->board[i][j].value != 0){
			board_to_clear->board[i][j].value = 0;
			num_to_clear--;
		}
	}
	return;
}

void print_changes_boards(board* first_board, board* second_board){
	int n = first_board->n;
	int m = first_board->m;
	int N = n*m;
	int i,j;

	for (i = 0; i < N; i++){
		for (j = 0; j < N; j++){
			if (first_board->board[i][j].value != second_board->board[i][j].value){
				printf("Cell (%d, %d) was changed from value %d to value %d.\n", j + 1, i + 1, first_board->board[i][j].value , second_board->board[i][j].value);
			}
		}
	}
	return;
}

void set_array_zero(int* arr, int size){
	int i;
	for(i = 0; i < size; i++){
		arr[i] = 0;
	}
}

int board_is_completed(board* current_board){
	if(check_board(current_board) == true){
		printf("Puzzle was solved successfully! Congratulations!\n");
		return true;
	}
	else{
		printf("The solution you provided is erroneous, you can use the undo command\n");
		return false;
	}
}

void cell_legal_values(board* current_board, int* legal_values, int N, int row, int col){
	int i;
	for(i = 0; i < N; i++){
		if (is_legal_cell(current_board, row, col, i + 1)){
			legal_values[i] = 1;
		}
	}
}

void find_empty_cell(board* current_board, int* empty_cell_row, int* empty_cell_col, int r, int N){
	int cnt = 0;
	int i,j;

	for(i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			if(current_board->board[i][j].value == 0){
				cnt++;
				if(cnt == r){
					*empty_cell_row = i;
					*empty_cell_col = j;
					return;
				}
			}
		}
	}
}
