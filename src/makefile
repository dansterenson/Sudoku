CC = gcc
EXEC = sudoku-console

CFLAGS = -ansi -O3 -Wall -Wextra -Werror -pedantic-errors
GUROBI_COMP = #-I/usr/local/lib/gurobi563/include
GUROBI_LIB = #-L/usr/local/lib/gurobi563/lib -lgurobi56

CSRCS   =  commands.c  main_aux.c  stab.c game_utils.c main.c stack.c linked_list.c parser.c
OBJS    = commands.o  main_aux.o  stab.o game_utils.o main.o stack.o linked_list.o parser.o


$(EXEC): $(OBJS)
	$(CC) $(OBJS)  $(GUROBI_LIB) -o $@ -lm

%.o: %.c
	$(CC)  $(CFLAGS) $(GUROBI_COMP) -c $<

clean:
	rm -f *.o $(EXEC)