#pragma once
#include <SDL3/SDL.h>
#include <iostream>

class Window
{
private:
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Window* window;

public:
    // for weird ownership stuff
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    Window(const char* title, int width, int height)
    {
        // Initialize SDL3 video subsystem
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            std::cout << "SDL_Init Error: " << SDL_GetError() << "\n";
            exit(1);
        }

        // Create Window and Renderer
        window = SDL_CreateWindow(title, width, height, 0);
        if (!window) {
            std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << "\n";
            SDL_Quit();
            exit(1);
        }

        renderer = SDL_CreateRenderer(window, nullptr);
        if (!renderer) {
            std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << "\n";
            SDL_DestroyWindow(window);
            SDL_Quit();
            exit(1);
        }
        // Create a streaming texture to hold our pixel buffer (RGBA format)
        texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_ABGR8888,
            SDL_TEXTUREACCESS_STREAMING,
            width,
            height
        );
    }

    ~Window()
    {
        // Cleanup
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    };

    bool poll_event(SDL_Event event)
    {
        return SDL_PollEvent(&event);
    }
    void present(const std::vector<Uint32>& pixels, int width) const
    {
        SDL_UpdateTexture(texture, nullptr, pixels.data(),width * sizeof(Uint32));
        SDL_RenderTexture(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }
};
