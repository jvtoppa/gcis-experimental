CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -O3 -Iinclude

SRC_DIR  := src
BIN_DIR  := build
INC_DIR  := include

SRCS     := $(SRC_DIR)/gcis.cpp $(SRC_DIR)/main.cpp $(INC_DIR)/bitvector.cpp

TARGET   := $(BIN_DIR)/gcis_app

.PHONY: all
all: $(TARGET)

$(TARGET): $(SRCS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $@

$(BIN_DIR):
	mkdir -p $@

.PHONY: clean
clean:
	
	rm -rf $(BIN_DIR)