# Project settings
PROJECT_NAME = mat_inv
VERSION = 1.0.0

# Detect OS and set appropriate commands
ifeq ($(OS),Windows_NT)
    RM = rmdir /s /q
    MKDIR = mkdir
    RMDIR = rmdir /s /q
    CP = copy
    RM_CMD = if exist
    RM_END = rd /s /q
    MKDIR_CMD = if not exist
    MKDIR_END = mkdir
else
    RM = rm -rf
    MKDIR = mkdir -p
    RMDIR = rm -rf
    CP = cp
    RM_CMD = rm -rf
    RM_END = 
    MKDIR_CMD = mkdir -p
    MKDIR_END = 
endif

# Directory structure
SRC_DIR = src
INC_DIR = inc
BUILD_DIR = build
OBJ_DIR = obj
TEST_DIR = test
TEST_INC_DIR = test/inc
TEST_BUILD_DIR = build/test

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99
CFLAGS += -I$(INC_DIR) -I$(TEST_INC_DIR)
LDFLAGS = 

# Source files (excluding main)
SRCS = $(filter-out $(SRC_DIR)/main.c, $(wildcard $(SRC_DIR)/*.c))
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Main program source
MAIN_SRC = $(SRC_DIR)/main.c
MAIN_OBJ = $(MAIN_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Test files
TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS = $(TEST_SRCS:$(TEST_DIR)/%.c=$(OBJ_DIR)/test_%.o)

# Target executables
TARGET = $(BUILD_DIR)/$(PROJECT_NAME)
TEST_TARGET = $(TEST_BUILD_DIR)/$(PROJECT_NAME)_test

# Create necessary directories
$(shell $(MKDIR) $(BUILD_DIR) $(OBJ_DIR) $(TEST_BUILD_DIR))

# Default target
all: $(TARGET)

# Test target
test: $(TEST_TARGET)

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Compile main program
$(MAIN_OBJ): $(MAIN_SRC)
	@echo "Compiling main program $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Compile test files
$(OBJ_DIR)/test_%.o: $(TEST_DIR)/%.c
	@echo "Compiling test $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Link main program
$(TARGET): $(OBJS) $(MAIN_OBJ)
	@echo "Linking main program $@"
	@$(CC) $(CFLAGS) $(OBJS) $(MAIN_OBJ) -o $@ $(LDFLAGS)

# Link test program
$(TEST_TARGET): $(OBJS) $(TEST_OBJS)
	@echo "Linking test program $@"
	@$(CC) $(CFLAGS) $(OBJS) $(TEST_OBJS) -o $@ $(LDFLAGS)

# Clean build files
clean:
ifeq ($(OS),Windows_NT)
	@if exist $(OBJ_DIR) $(RM_END) $(OBJ_DIR)
	@if exist $(BUILD_DIR) $(RM_END) $(BUILD_DIR)
	@if not exist $(OBJ_DIR) $(MKDIR_END) $(OBJ_DIR)
	@if not exist $(BUILD_DIR) $(MKDIR_END) $(BUILD_DIR)
	@if not exist $(TEST_BUILD_DIR) $(MKDIR_END) $(TEST_BUILD_DIR)
else
	@$(RM) $(OBJ_DIR) $(BUILD_DIR)
	@$(MKDIR) $(OBJ_DIR) $(BUILD_DIR) $(TEST_BUILD_DIR)
endif

# Clean and rebuild
rebuild: clean all

# Debug build
debug: CFLAGS += -g -DDEBUG
debug: clean all

# Release build with maximum optimization
release: CFLAGS += -O3
release: clean all

# Size information
size: $(TARGET)
	@echo "Size information for $(TARGET):"
ifeq ($(OS),Windows_NT)
	@size.exe $<
else
	@size $<
endif

# Dependencies
deps: $(SRCS)
	@echo "Generating dependencies..."
	@$(CC) -MM $(CFLAGS) $^ > $(BUILD_DIR)/dependencies.mk

# Run tests
run_tests: test
	@echo "Running tests..."
	@$(TEST_TARGET)

# Help target
help:
	@echo "Project: $(PROJECT_NAME) v$(VERSION)"
	@echo "Available targets:"
	@echo "  all        - Build the project (default)"
	@echo "  test       - Build the test program"
	@echo "  run_tests  - Build and run the tests"
	@echo "  clean      - Remove all built files"
	@echo "  rebuild    - Clean and rebuild the project"
	@echo "  debug      - Build with debug information"
	@echo "  release    - Build with maximum optimization"
	@echo "  size       - Show size information for the binary"
	@echo "  deps       - Generate dependency information"
	@echo "  help       - Show this help message"

.PHONY: all test run_tests clean rebuild debug release size deps help 