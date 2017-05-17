CC = g++
DEBUG = -g
DEBUGGER = gdb
BIN = ../bin/
OBJS = main.o gameState.o
CFLAGS = -c -std=c++11 -pthread
OFLAGS = -std=c++11 -pthread

all: bin $(OBJS)
	$(CC) $(OFLAGS) $(OBJS) -o $(BIN)main.out

main.o: main.cpp gameState.h | bin
	$(CC) $(CFLAGS) main.cpp -o main.o

gameState.o: gameState.h gameState.cpp | bin
	$(CC) $(CFLAGS) gameState.cpp -o gameState.o

check.o: check.cpp gameState.h | bin
	$(CC) $(CFLAGS) check.cpp -o check.o

debug: $(OBJS)
	$(CC) $(OFLAGS) $(DEBUG) $(OBJS) -o $(BIN)mainDebug.out

test: bin $(OBJS) check.o
	$(CC) $(OFLAGS) $(OBJS) -o $(BIN)main.out
	$(CC) $(OFLAGS) check.o gameState.o -o $(BIN)check.out

run: all $(BIN)main.out
	$(BIN)main.out

run-g: debug $(BIN)mainDebug.out
	$(DEBUGGER) $(BIN)mainDebug.out

run-test: all $(BIN)main.out
	$(BIN)main.out ../data/autosokoban/15/1.txt 8 1

bin:
	mkdir -p $(BIN)

clean:
	rm main.o gameState.o
	rm -r ../bin
