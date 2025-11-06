CC = g++
CPPFLAGS = 
# CPPFLAGS = -Wall -Werror -ansi -lm

SRC_DIR = src
INC_DIR = include
BIN_DIR = bin

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/%: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

TARGET = $(BIN_DIR)/main.out
EXPERIMENTS_TARGET = $(BIN_DIR)/experiments.out

# $(TARGET): graph.o
# 	$(CC) $(CPPFLAGS) $^ -o $@

# Only compile the .cpp, not the .h
# Define object files
OBJS = graph.o main.o PuzzleManager.o PuzzleSolver.o
EXPERIMENTS_OBJS = graph.o main_experiments.o PuzzleManager.o PuzzleSolver.o

$(TARGET): $(OBJS)
	$(CC) $(CPPFLAGS) $^ -o $@

$(EXPERIMENTS_TARGET): $(EXPERIMENTS_OBJS)
	$(CC) $(CPPFLAGS) $^ -o $@

# Pattern rule for object files
%.o: $(SRC_DIR)/%.cpp $(INC_DIR)/%.h
	$(CC) $(CPPFLAGS) -c $< -o $@

# Special case for main.cpp if it doesn't have a header
main.o: $(SRC_DIR)/main.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@

# Special case for main_experiments.cpp if it doesn't have a header
main_experiments.o: $(SRC_DIR)/main_experiments.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

experiments: $(EXPERIMENTS_TARGET)
	./$(EXPERIMENTS_TARGET)

run-experiments: $(EXPERIMENTS_TARGET)
	./$(EXPERIMENTS_TARGET)
	
clean:
	rm -f *.o $(TARGET) $(EXPERIMENTS_TARGET)

.PHONY: clean run experiments run-experiments

