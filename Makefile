CC = g++
CPPFLAGS = 
# CPPFLAGS = -Wall -Werror -ansi -lm

SRC_DIR = src
INC_DIR = include
BIN_DIR = bin

TARGET = $(BIN_DIR)/main.out

$(TARGET): graph.o
	$(CC) $(CPPFLAGS) $^ -o $@

# Only compile the .cpp, not the .h
graph.o: $(SRC_DIR)/graph.cpp $(INC_DIR)/graph.h
	$(CC) $(CPPFLAGS) -c $< -o $@ 

run: $(TARGET)
	./$(TARGET)
	
clean:
	rm -f graph.o $(TARGET)

.PHONY: clean
