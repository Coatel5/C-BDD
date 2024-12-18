CC = gcc
CFLAGS = -Wall -Wextra -g
SRC_DIR = src
OBJ_DIR = obj
OBJ = $(OBJ_DIR)/main.o $(OBJ_DIR)/btree.o $(OBJ_DIR)/repl.o
TARGET = btree_test

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c $(SRC_DIR)/btree.h $(SRC_DIR)/repl.h
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/main.c -o $(OBJ_DIR)/main.o

$(OBJ_DIR)/btree.o: $(SRC_DIR)/btree.c $(SRC_DIR)/btree.h
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/btree.c -o $(OBJ_DIR)/btree.o

$(OBJ_DIR)/repl.o: $(SRC_DIR)/repl.c $(SRC_DIR)/btree.h $(SRC_DIR)/repl.h
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/repl.c -o $(OBJ_DIR)/repl.o

clean:
	rm -rf $(OBJ_DIR) $(TARGET) btree.txt

rebuild: clean all
