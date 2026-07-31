# Compiler and flags
CXX      := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -O2

# Include paths and libraries
INCLUDES := -I./src
LIBS     := -lglfw -lGL -lm -lpthread -ldl

# Directories
SRC_DIR  := src
BUILD_DIR:= build
TARGET   := $(BUILD_DIR)/rasterizer

# Find all .cpp files inside src/
SRCS     := $(wildcard $(SRC_DIR)/*.cpp)
OBJS     := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# Default rule
all: $(TARGET)

# Link final executable
$(TARGET): $(OBJS) | $(BUILD_DIR)
	$(CXX) $(OBJS) -o $@ $(LIBS)

# Compile C++ source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all run clean