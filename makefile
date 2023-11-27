CC = gcc
STD = gnu9x
LIBS = -lmq -lc
DEFINE = -D_POSIX_C_SOURCE=200112L
ARGS = -ansi -pedantic -Wall

SRC_DIR = ./src
BLD_DIR = ./build
INC_DIR = ./include
OUT_DIR = ./bin

FLAGS = -std=$(STD) $(LIBS) -I$(INC_DIR) $(DEFINE) $(ARGS)

all: builddir $(OUT_DIR)/uart $(OUT_DIR)/terminal

# UART
$(OUT_DIR)/uart: $(BLD_DIR)/uart/main.o $(BLD_DIR)/uart/threads.o
	$(CC) $(FLAGS) -o $@ $^

$(BLD_DIR)/uart/main.o: $(SRC_DIR)/uart/main.c $(SRC_DIR)/uart/uart.h
	$(CC) $(FLAGS) -o $@ -c $<

$(BLD_DIR)/uart/threads.o: $(SRC_DIR)/uart/threads.c $(SRC_DIR)/uart/uart.h
	$(CC) $(FLAGS) -o $@ -c $<

# Terminal
$(OUT_DIR)/terminal: $(BLD_DIR)/terminal/main.o $(BLD_DIR)/terminal/threads.o
	$(CC) $(FLAGS) -o $@ $^

$(BLD_DIR)/terminal/main.o: $(SRC_DIR)/terminal/main.c $(SRC_DIR)/terminal/file.h
	$(CC) $(FLAGS) -o $@ -c $<

$(BLD_DIR)/terminal/threads.o: $(SRC_DIR)/terminal/threads.c $(SRC_DIR)/terminal/file.h
	$(CC) $(FLAGS) -o $@ -c $<

.PHONY: builddir
builddir:
	@mkdir -p $(OUT_DIR)
	@mkdir -p $(BLD_DIR)/uart
	@mkdir -p $(BLD_DIR)/terminal

.PHONY: clean
clean:
	@rm -rf $(BLD_DIR)
	@rm -rf $(OUT_DIR)
