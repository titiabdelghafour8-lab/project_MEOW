CXX      := g++
CXXFLAGS := -std=c++23 -Wall -Wextra -O3 -Werror
SDL_CFLAGS := $(shell pkg-config --cflags sdl3)
SDL_LIBS   := $(shell pkg-config --libs sdl3)

SRC_DIR  := src
BUILD_DIR := build
EXEC     := rasterizer
SRCS     := $(wildcard $(SRC_DIR)/*.cpp)
OBJS     := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# DEFAULT TARGET
all: $(BUILD_DIR) $(EXEC)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# LINK
$(EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $(BUILD_DIR)/$(EXEC) $(SDL_LIBS)

# COMPILE
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) -c $(CXXFLAGS) $(SDL_CFLAGS) $< -o $@

# CLEAN
clean:
	rm -rf $(BUILD_DIR)

# RUN
run: all
	./$(BUILD_DIR)/$(EXEC)

.PHONY: all clean run

