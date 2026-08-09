#include <cmath>
#include <SDL3/SDL.h>
#include <vector>
#include <iostream>

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;
// colors
constexpr Uint32 COLOR_GREEN = 0xFF00FF00;
constexpr Uint32 COLOR_RED = 0xFF0000FF;
constexpr Uint32 COLOR_YELLOW = COLOR_RED | COLOR_GREEN;
constexpr Uint32 COLOR_BLUE = 0xFFFF0000;


void draw_line(int ax, int ay, int bx, int by, std::vector<Uint32>& pixels, Uint32 color);

int main(int, char**) {
    // 1. Initialize SDL3 video subsystem
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << "SDL_Init Error: " << SDL_GetError() << "\n";
        return 1;
    }

    // 2. Create Window and Renderer
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

    // 3. Create a streaming texture to hold our pixel buffer (RGBA format)
    SDL_Texture* texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ABGR8888,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );

    // 4. Allocate a CPU-side pixel buffer for your rasterizer to draw into
    std::vector<Uint32> pixels(SCREEN_WIDTH * SCREEN_HEIGHT, 0xFF000000); // Initialized to opaque black

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
        draw_line(0, 0, 800, 600, pixels, COLOR_RED);
        draw_line(0, 600, 800, 0, pixels, COLOR_YELLOW);


        // Update the SDL texture with our pixel buffer data
        SDL_UpdateTexture(texture, nullptr, pixels.data(), SCREEN_WIDTH * sizeof(Uint32));
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

void draw_line(int ax, int ay, int bx, int by, std::vector<Uint32>& pixels, Uint32 color)
{
    int steep = abs(ay - by) - abs(ax - bx);
    if (steep > 0)
    {
        std::swap(ax, ay);
        std::swap(bx, by);
    }
    int a_precede_b = bx - ax;
    if (a_precede_b < 0)
    {
        std::swap(ay, by);
        std::swap(ax, bx);
    }
    for (int x { ax }; x <= bx; x++)
    {
        float t = static_cast<float>(x - ax) / static_cast<float>(bx - ax);
        int y { static_cast<int>(std::round(ay + t * (by - ay))) };
        if (steep > 0)
        {
            pixels[x * SCREEN_WIDTH + y] = color;
        }else
        {
            pixels[y * SCREEN_WIDTH + x] = color;
        }
    }
}
