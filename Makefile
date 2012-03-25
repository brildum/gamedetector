
CC = g++
FLAGS = -Wall -g
LDFLAGS = -lopencv_core -lopencv_highgui

all:: build

clean:
	rm *.o
	rm test

build: main.o
	$(CC) $(LDFLAGS) $(FLAGS) -o test main.o

main.o: main.cpp RunningStat.h
	$(CC) $(FLAGS) -c main.cpp
