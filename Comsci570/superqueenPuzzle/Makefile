all: superqueens_puzzle
superqueens_puzzle: superqueens_puzzle.o 
	g++ -Wall superqueens_puzzle.o -o superqueens_puzzle
superqueens_puzzle.o: superqueens_puzzle.cpp
	g++ -Wall -c superqueens_puzzle.cpp

clean:
	\rm *~
