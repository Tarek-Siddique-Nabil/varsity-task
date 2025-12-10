CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -g -Iinclude
SRC_DIR := src
OBJ_DIR := build
BIN_DIR := bin
TARGET := $(BIN_DIR)/parking

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

.PHONY: all clean run test format dirs

all: dirs $(TARGET)

dirs:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR) data

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@
	@echo "Built $(TARGET)"

run: all
	@echo "Starting Parking Lot Management System..."
	@$(TARGET)

clean:
	@rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "Cleaned build artifacts"

test: all
	@echo "Running tests..."
	# If you have test binaries, run them here. Example (uncomment when available):
	# ./tests/run_tests.sh

format:
	# Optional: run clang-format if installed
	@command -v clang-format >/dev/null 2>&1 && find src include -name "*.h" -o -name "*.c" | xargs clang-format -i || echo "clang-format not found"
