CC = gcc
CFLAGS = -Wall -Wextra -Werror -I.
LDFLAGS = 
BUILD_DIR = build
BIN_DIR = bin

SRC_MAIN = ProcData.c RandInsertData.c checks.c
OBJ_MAIN = $(addprefix $(BUILD_DIR)/, $(SRC_MAIN:.c=.o))
TARGET_MAIN = $(BIN_DIR)/test

DATA_FILES = $(BIN_DIR)/file1 $(BIN_DIR)/file2 $(BIN_DIR)/fileout

.PHONY: all clean create_files

all: create_dirs $(TARGET_MAIN) create_files

create_dirs:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR)

$(TARGET_MAIN): $(OBJ_MAIN)
	$(CC) $(LDFLAGS) $^ -o $@

create_files: create_dirs
	@touch $(DATA_FILES)

$(BUILD_DIR)/%.o: %.c ProcData.h checks.h | create_dirs
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)