CXX      := g++
CXXFLAGS = -Wall -O3 -std=c++23

SRC_DIR  := src
BUILD_DIR := build
EXEC     := rasterizer
SRCS     := $(wildcard $(SRC_DIR)/*.cpp)
OBJS     := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

ifeq ($(OS), Windows_NT)
	SDL_CFLAGS = -I/ucrt64/include/SDL3
    SDL_LIBS = -lSDL3 -mwindows -lm

    MKDIR = if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
    RM = if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S), Linux)
		SDL_CFLAGS = $(shell pkg-config --cflags sdl3)
		SDL_LIBS = $(shell pkg-config --libs sdl3) -lm
	endif
	ifeq ($(UNAME_S), Darwin)
		SDL_cFLAGS = -I/opt/homebrew/include/SDL3
		SDL_LIBS = -framework SDL3 -framework Cocoa -framework OpenGL -lm
	endif
	MKDIR = mkdir -p $(BUILD_DIR)
	RM = rm -rf $(BUILD_DIR)
endif

# DEFAULT TARGET
all: $(BUILD_DIR) $(EXEC)

$(BUILD_DIR):
	$(MKDIR)

# LINK
$(EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $(BUILD_DIR)/$(EXEC) $(SDL_LIBS)

# COMPILE
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) -c $(CXXFLAGS) $(SDL_CFLAGS) $< -o $@

# CLEAN
clean:
	$(RM)


# RUN
run: all
	./$(BUILD_DIR)/$(EXEC)

.PHONY: all clean run

