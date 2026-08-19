#include "Color.h"
#include "Math.h"
#include "OBJParser.h"
#include "Renderer.h"
#include <random>
#include <cmath>
#include <SDL3/SDL.h>
#include <vector>
#include <iostream>
#include <fstream>

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;
// colors
const Uint32 COLOR_GREEN = getColor(0, 255, 0, 255);
const Uint32 COLOR_RED = getColor(255, 0, 0, 255);
const Uint32 COLOR_YELLOW = COLOR_RED | COLOR_GREEN;
const Uint32 COLOR_BLUE = getColor(0, 0, 255, 255);
constexpr float PI = 3.1415927;


int main(int, char**) {
    // Initialize SDL3 video subsystem
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << "SDL_Init Error: " << SDL_GetError() << "\n";
        return 1;
    }

    // Create Window and Renderer
    SDL_Window* window = SDL_CreateWindow("Software Rasterizer", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!window) {
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Create a streaming texture to hold our pixel buffer (RGBA format)
    SDL_Texture* texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ABGR8888,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );

    // Instantiating the renderer
    Renderer software_renderer { SCREEN_WIDTH, SCREEN_HEIGHT };

    // Load file and its data
    std::ifstream african_head_obj("./src/diablo3_pose.obj");
    if (!african_head_obj.is_open())
    {
        std::cout << "Could not open file: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }
    ObjParser::ObjElementData african_head_data {};
    ObjParser::parse_obj_file(african_head_obj, african_head_data);

    bool is_running = true;
    SDL_Event event;

    while (is_running) {
        // Handle window events (like closing the window)
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                is_running = false;
            }
        }

        // TODO: DRAW STUFF
        software_renderer.clear();
        software_renderer.draw_mesh(african_head_data);


        SDL_UpdateTexture(texture, nullptr, software_renderer.getPixels().data(), SCREEN_WIDTH * sizeof(Uint32));
        SDL_RenderTexture(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }


    // Cleanup
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
