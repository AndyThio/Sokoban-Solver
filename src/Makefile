CC = g++
DEBUG = -g
DEBUGGER = gdb
BIN = ../bin/
OBJS = main.o gameState.o
CFLAGS = -c -std=c++11 -o3 -pthread
OFLAGS = -std=c++11 -o3 -pthread

all: bin $(OBJS) 
	$(CC) $(OFLAGS) $(OBJS) -o $(BIN)main.out

main.o: main.cpp gameState.h | bin
	$(CC) $(CFLAGS) main.cpp -o main.o

gameState.o: gameState.h gameState.cpp | bin
	$(CC) $(CFLAGS) gameState.cpp -o gameState.o

debug: $(OBJS)
	$(CC) $(OFLAGS) $(DEBUG) $(OBJS) -o $(BIN)mainDebug.out

run: all $(BIN)main.out
	$(BIN)main.out
	
test: all
	$(BIN)main.out ../data/autosokoban/5/1.txt

run-g: debug $(BIN)mainDebug.out
	$(DEBUGGER) $(BIN)mainDebug.out

bin:
	mkdir -p $(BIN)

clean:
	rm main.o gameState.o
	rm -r ../bin