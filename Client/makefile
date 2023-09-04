#цель: зависимость
#[tab] команда


BIN = ClientChat
SOURCES = main.c
HEADERS = main.h
CC := gcc

all:
	$(CC) $(SOURCES) -lncurses -lpthread -lrt -o $(BIN).out
#	$(CC) $(SOURCES) -lpthread -o $(BIN).out -lrt
$(BIN): $(HEADERS)

run: all
	./$(BIN).out
run2:
	./$(BIN).out


#clean:
#	rm -rf $(BIN) *.o $(BIN).out



















