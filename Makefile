INCLUDE_DIR = src
SRC_DIR = src
CC = g++
CFLAGS = -I $(INCLUDE_DIR) --std=c++11

BUILD_DIR = build
BIN_DIR = bin

HEADERS = $(wildcard $(INCLUDE_DIR)/*.hpp)
SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(wildcard $(SRC_DIR)/*.cpp))

all: bin build $(BIN_DIR)/hr-calc-path

bin:
	mkdir $(BIN_DIR)

build:
	mkdir $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	$(CC) -c $< $(CFLAGS) -o $@

$(BIN_DIR)/hr-calc-path: $(OBJECTS)
	$(CC) $^ $(CFLAGS) -o $(BIN_DIR)/hr-calc-path

.PHONY: clean

clean:
	rm -f $(BUILD_DIR)/*.o
	rm -f $(BIN_DIR)/*
