all: fifteen_puzzle
fifteen_puzzle: fifteen_puzzle.o 
	g++ -Wall fifteen_puzzle.o -o fifteen_puzzle
fifteen_puzzle.o: fifteen_puzzle.cpp
	g++ -Wall -c fifteen_puzzle.cpp

clean:
	\rm *~
