CC = g++
CPPFLAGS = 
# CPPFLAGS = -Wall -Werror -ansi -lm

SRC_DIR = src
INC_DIR = include
BIN_DIR = bin

TARGET = $(BIN_DIR)/main.out

# $(TARGET): graph.o
# 	$(CC) $(CPPFLAGS) $^ -o $@

# Only compile the .cpp, not the .h
# Define object files
OBJS = graph.o main.o PuzzleManager.o PuzzleSolver.o  

$(TARGET): $(OBJS)
	$(CC) $(CPPFLAGS) $^ -o $@

# Pattern rule for object files
%.o: $(SRC_DIR)/%.cpp $(INC_DIR)/%.h
	$(CC) $(CPPFLAGS) -c $< -o $@

# Special case for main.cpp if it doesn't have a header
main.o: $(SRC_DIR)/main.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)
	
clean:
	rm -f *.o $(TARGET)

.PHONY: clean

