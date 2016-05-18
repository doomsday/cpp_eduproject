CC=g++
CFLAGS=-c -Wall -std=c++14

all: eduproject.exe clean

eduproject.exe: main.o file1.o file2.o
	$(CC) main.o file1.o file2.o -o make-build/eduproject.exe

main.o: eduproject/main.cpp
	$(CC) -c eduproject/main.cpp

file1.o: eduproject/file1.cpp
	$(CC) -c eduproject/file1.cpp

file2.o: eduproject/file2.cpp
	$(CC) -c eduproject/file2.cpp

clean:
	del *o eduproject.exe