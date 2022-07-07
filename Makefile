CC=g++
CFLAGS= -std=c++11 -Wall -g -w -Werror -O2

all: tictactoe

tictactoe: tictactoe.cpp
	$(CC) tictactoe.cpp -o tictactoe $(CFLAGS)

clean: 
	$(RM) tictactoe.cpp