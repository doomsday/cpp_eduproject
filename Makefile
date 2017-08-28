CC=g++
CFLAGS=-c -Wall -std=c++14

all: eduproject.exe clean

eduproject.exe: main.o file1.o file2.o
	$(CC) main.o file1.o file2.o -o make-build/eduproject.exe

main.o: src
	$(CC) -c eduproject/main.cpp

file1.o: src
	$(CC) -c eduproject/file1.cpp

file2.o: src
	$(CC) -c eduproject/file2.cpp

clean:
	del *o eduproject.exe