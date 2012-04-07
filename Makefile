
CC = g++
FLAGS = -Wall -g
LDFLAGS = -lopencv_core -lopencv_highgui -lavutil -lavformat -lavfilter

all:: build

clean:
	rm *.o
	rm test

build: testcv testfilter

testfilter: filter.o
	$(CC) $(LDFLAGS) $(FLAGS) -o filter filter.o

filter.o: filter.cpp
	$(CC) $(FLAGS) -c filter.cpp

testcv: main.o
	$(CC) $(LDFLAGS) $(FLAGS) -o test main.o

main.o: main.cpp RunningStat.h
	$(CC) $(FLAGS) -c main.cpp
