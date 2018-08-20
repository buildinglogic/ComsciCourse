all: deli
deli: deli.o thread.o
	g++ -Wall deli.o thread.o libinterrupt.a -ldl -o deli
deli.o: deli.cc thread.h interrupt.h
	g++ -Wall -c deli.cc

clean:
	\rm *~
