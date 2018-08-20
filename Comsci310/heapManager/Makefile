# You can compile with either gcc or g++
# CC = g++
CC = gcc
CFLAGS = -I. -Wall -lm -DNDEBUG
# disable the -DNDEBUG flag for the printing the freelist
OPTFLAG = -O2
DEBUGFLAG = -g

all: dmm.o

dmm.o: dmm.c
	$(CC) $(CFLAGS) -c -g dmm.c 
clean:
	rm -f *.o a.out
