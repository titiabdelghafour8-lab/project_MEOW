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
constexpr Uint32 COLOR_GREEN = 0xFF00FF00;
constexpr Uint32 COLOR_RED = 0xFF0000FF;
constexpr Uint32 COLOR_YELLOW = COLOR_RED | COLOR_GREEN;
constexpr Uint32 COLOR_BLUE = 0xFFFF0000;


void draw_line(int ax, int ay, int bx, int by, std::vector<Uint32>& pixels, Uint32 color);
void draw_wireframe(const ObjParser::ObjElementData& obj_data, std::vector<Uint32>& pixels, const Uint32& color);
void draw_triangle(int ax, int ay, int bx, int by, int cx, int cy, std::vector<Uint32>& pixels, const Uint32& color);
void fill_triangle(int ax, int ay, int bx, int by, int cx, int cy, std::vector<Uint32>& pixels, const Uint32& color);


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
        ObjParser::parse_obj_file(african_head_obj, african_head_data);
        draw_wireframe(african_head_data, pixels, COLOR_RED);

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
        std::uniform_int_distribution<int> distrib(0, 255);     // Update the SDL texture with our pixel buffer data
        int r {distrib(gen)}, g {distrib(gen)}, b {distrib(gen)}, a {distrib(gen)};
        draw_triangle(vertex_a.x, vertex_a.y, vertex_b.x, vertex_b.y, vertex_c.x, vertex_c.y, pixels, getColor(r, g, b, a));
    }
}

void draw_triangle(int ax, int ay, int bx, int by, int cx, int cy, std::vector<Uint32>& pixels, const Uint32& color)
{
    draw_line(ax, ay, bx, by, pixels, color); //draw AB
    draw_line(bx, by, cx, cy, pixels, color); //draw BC
    draw_line(cx, cy, ax, ay, pixels, color); //draw CA
    fill_triangle(ax, ay, bx, by, cx, cy, pixels, color);
}
void fill_triangle(int ax, int ay, int bx, int by, int cx, int cy, std::vector<Uint32>& pixels, const Uint32& color)
{
    std::array<std::tuple<int,int>, 3> vertices {
        std::tuple<int, int>{ax, ay},
         std::tuple<int, int> {bx, by},
         std::tuple<int, int> {cx, cy}
    };
    std::sort(
        vertices.begin(),
        vertices.end(),
        [](const auto& a, const auto& b) { return std::get<1>(a) < std::get<1>(b); }
        );
    int x0 = std::get<0>(vertices[0]); int y0 = std::get<1>(vertices[0]);
    int x1 = std::get<0>(vertices[1]); int y1 = std::get<1>(vertices[1]);
    int x2 = std::get<0>(vertices[2]); int y2 = std::get<1>(vertices[2]);
    int d1{ (y1 - y0) == 0 ? 1: y1 - y0 };
    int d2{ (y2 - y0) == 0 ? 1: y2 - y0 };
    int d3{ (y2 - y1) == 0 ? 1: y2 - y1 };

    int first_yposition { std::max(0, y0) };
    int final_yposition { std::min(SCREEN_HEIGHT - 1, y2) };
    for (int y { first_yposition };y <= final_yposition;y++)
    {
        if (y < y0 || y > y2) continue; // triangle bounds along the y axis
        if (y <= y1)
        {
            if (y1 == y0) continue; //flat surface no need to paint

            int first_xposition { x0 + (y - y0) *  (x1 - x0) / d1};
            int second_xposition { x0 + (y - y0) * (x2 - x0) / d2};
            if (first_xposition > second_xposition)
            {
                std::swap(first_xposition, second_xposition);
            }
            first_xposition = std::max(0, first_xposition);
            second_xposition = std::min(SCREEN_WIDTH - 1, second_xposition);
            for (int x {first_xposition}; x <= second_xposition;x++)
            {
                pixels[y * SCREEN_WIDTH + x] = color;
            }
        }
        else
        {
            if (x1 < x2)
            {
                if (y2 == y1) continue; //flat surface no need to paint

                int first_xposition { x1 + (y - y1) *  (x2 - x1) / d3};
                int second_xposition { x0 + (y - y0) *  (x2 - x0) / d2};

                if (first_xposition > second_xposition)
                {
                    std::swap(first_xposition, second_xposition);
                }
                first_xposition = std::max(0, first_xposition);
                second_xposition = std::min(SCREEN_WIDTH - 1, second_xposition);

                for (int x {first_xposition}; x <= second_xposition;x++)
                {
                    pixels[y * SCREEN_WIDTH + x] = color;
                }
            }
            else
            {
                int first_xposition { x0 + (y - y0) *  (x2 - x0) / d2};
                int second_xposition { x1 + (y - y1) * (x2 - x1) / d3};

                if (first_xposition > second_xposition)
                {
                    std::swap(first_xposition, second_xposition);
                }
                first_xposition = std::max(0, first_xposition);
                second_xposition = std::min(SCREEN_WIDTH - 1, second_xposition);

                for (int x {first_xposition}; x <= second_xposition;x++)
                {
                    pixels[y * SCREEN_WIDTH + x] = color;
                }

            }
        }
    }
}