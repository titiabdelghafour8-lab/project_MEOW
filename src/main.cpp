#include <cmath>
#include <SDL3/SDL.h>
#include <vector>
#include <iostream>
#include <fstream>
#include "Color.h"
#include "Math.h"
#include "OBJParser.h"
#include <algorithm>
#include <random>

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;
// colors
const Uint32 COLOR_GREEN = getColor(0, 255, 0, 255);
const Uint32 COLOR_RED = getColor(255, 0, 0, 255);
const Uint32 COLOR_YELLOW = COLOR_RED | COLOR_GREEN;
const Uint32 COLOR_BLUE = getColor(0, 0, 255, 255);


void draw_line(int ax, int ay, int bx, int by, std::vector<Uint32>& pixels, Uint32 color);
void draw_wireframe(const ObjParser::ObjElementData& obj_data, std::vector<Uint32>& pixels, const Uint32& color);
void draw_triangle(int ax, int ay, int bx, int by, int cx, int cy, std::vector<Uint32>& pixels, const Uint32& color);


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

    // Allocate a CPU-side pixel buffer for your rasterizer to draw into
    std::vector<Uint32> pixels(SCREEN_WIDTH * SCREEN_HEIGHT, 0xFF000000); // Initialized to opaque black


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
        std::fill(pixels.begin(), pixels.end(), 0xFF000000);

        // TODO: DRAW STUFF
        int ax = 400, ay = 100;
        int bx = 150, by = 500;
        int cx = 650, cy = 500;
        draw_triangle(ax, ay, bx, by, cx, cy, pixels, COLOR_BLUE);

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
    int y { ay };
    int normalized_error { 0 };
    for (int x { ax }; x <= bx; x++)
    {
        if (steep > 0)
        {
            pixels[x * SCREEN_WIDTH + y] = color;
        }
        else
        {
            pixels[y * SCREEN_WIDTH + x] = color;
        }
        normalized_error += 2 * std::abs(by - ay);
        y += (normalized_error > (bx - ax)) * (ay < by ? 1 : -1);
        normalized_error -= (normalized_error > (bx - ax)) * 2 * (bx - ax);
    }
}
void draw_wireframe(const ObjParser::ObjElementData& obj_data, std::vector<Uint32>& pixels, const Uint32& color)
{
    for (std::array<int, 3> face_indices: obj_data.faces())
    {
        Math::Vec2<int> vertex_a {
            Math::viewport_projection(obj_data.vertices()[face_indices[0]],SCREEN_WIDTH, SCREEN_HEIGHT)
        };
        Math::Vec2<int> vertex_b {
            Math::viewport_projection(obj_data.vertices()[face_indices[1]], SCREEN_WIDTH, SCREEN_HEIGHT)
        };
        Math::Vec2<int> vertex_c {
            Math::viewport_projection(obj_data.vertices()[face_indices[2]], SCREEN_WIDTH, SCREEN_HEIGHT)
        };

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> distrib(0, 255);
        int r {distrib(gen)}, g {distrib(gen)}, b {distrib(gen)}, a {distrib(gen)};
        draw_triangle(vertex_a.x, vertex_a.y, vertex_b.x, vertex_b.y, vertex_c.x, vertex_c.y, pixels, getColor(r, g, b, a));
    }
}

void draw_triangle(int ax, int ay, int bx, int by, int cx, int cy, std::vector<Uint32>& pixels, const Uint32& color)
{
    Math::Vec2<int> A{ax, ay}, B{bx, by}, C{cx, cy};
    int bounding_x_min { std::clamp(std::min(std::min(A.x, B.x), C.x), 0, SCREEN_WIDTH - 1) };
    int bounding_x_max { std::clamp(std::max(std::max(A.x, B.x), C.x), 0 ,SCREEN_WIDTH - 1 ) };
    int bounding_y_min { std::clamp(std::min(std::min(A.y, B.y), C.y), 0, SCREEN_HEIGHT - 1) };
    int bounding_y_max { std::clamp(std::max(std::max(A.y, B.y), C.y), 0 ,SCREEN_HEIGHT - 1 ) };

    for (int y { bounding_y_min }; y <= bounding_y_max; y++)
    {
        for (int x { bounding_x_min }; x <= bounding_x_max; x++)
        {
            Math::Vec3<float> bary_coords {
                Math::calculate_bary_coords(
                    A, B, C, { x, y }
                )
            };

            if ( bary_coords.x < 0 || bary_coords.y < 0 || bary_coords.z < 0) continue;
            if (bary_coords.x > 0.001f && bary_coords.y > 0.001f && bary_coords.z > 0.001f) continue;
            Uint8 r = static_cast<Uint8>(std::clamp(bary_coords.x, 0.0f, 1.0f) * 255.0f);
            Uint8 g = static_cast<Uint8>(std::clamp(bary_coords.y, 0.0f, 1.0f) * 255.0f);
            Uint8 b = static_cast<Uint8>(std::clamp(bary_coords.z, 0.0f, 1.0f) * 255.0f);

            Uint32 pixel_color = getColor(r, g, b, 255.f);

            pixels[y * SCREEN_WIDTH + x] = pixel_color;
        }
    }
}