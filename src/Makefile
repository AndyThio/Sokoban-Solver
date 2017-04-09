CC = g++
DEBUG = -g
DEBUGGER = gdb
BIN = ../bin/
OBJS = main.o gameState.o
CFLAGS = -c -std=c++11 -pthread
OFLAGS = -std=c++11 -pthread

all: $(OBJS)
	$(CC) $(OFLAGS) $(OBJS) -o $(BIN)main.out

main.o: main.cpp gameState.h
	$(CC) $(CFLAGS) main.cpp -o main.o

gameState.o: gameState.h gameState.cpp
	$(CC) $(CFLAGS) gameState.cpp -o gameState.o

debug: $(OBJS)
	$(CC) $(OFLAGS) $(DEBUG) $(OBJS) -o $(BIN)mainDebug.out

run: all $(BIN)main.out
	$(BIN)main.out

run-g: debug $(BIN)mainDebug.out
	$(DEBUGGER) $(BIN)mainDebug.out
