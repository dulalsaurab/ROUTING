INCLUDE_DIR = src
SRC_DIR = src
CC = g++
CFLAGS = -I $(INCLUDE_DIR)

BUILD_DIR = build

SRC = $(wildcard $(SRC_DIR)/*.cpp) 
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(wildcard $(SRC_DIR)/*.cpp))

$(BUILD_DIR)/%.o: $(SRC) $(DEPS)
	$(CC) -c $< $(CFLAGS) -o $@

hr-calc-path: $(OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) -o bin/hr-calc-path

.PHONY: clean

clean:
	rm -f $(BUILD_DIR)/*.o
