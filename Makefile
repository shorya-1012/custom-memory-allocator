CC = gcc
CFLAGS = -Wall -Wextra -g
SRC_DIR = src
OBJ_DIR = obj
BIN = heap

SRCS = $(SRC_DIR)/heap.c $(SRC_DIR)/main.c
OBJS = $(SRCS:.c=.o)

$(BIN): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(BIN)

clean:
	rm -f $(BIN)


