#цель: зависимость
#[tab] команда


BIN = ServerChat
SOURCES = main.c
HEADERS = main.h
CC := gcc

all:
	$(CC) $(SOURCES) -lrt -o $(BIN).out
#	$(CC) $(SOURCES) -lpthread -lrt -o $(BIN).out
#	$(CC) $(SOURCES) -lpthread -o $(BIN).out -lrt
$(BIN): $(HEADERS)

run: all
	./$(BIN).out
#clean:
#	rm -rf $(BIN) *.o $(BIN).out



















