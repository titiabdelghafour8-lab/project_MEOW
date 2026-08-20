#include "OBJParser.h"
#include "Renderer.h"
#include <random>
#include <SDL3/SDL.h>
#include <iostream>
#include <fstream>
#include "Window.h"


constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;


int main(int, char**) {

    Window window = Window("software rasterizer", SCREEN_WIDTH, SCREEN_HEIGHT);
    Renderer software_renderer { SCREEN_WIDTH, SCREEN_HEIGHT };

    // Load file and its data
    std::ifstream african_head_obj("./assets/diablo3_pose.obj");
    if (!african_head_obj.is_open())
    {
        std::cout << "Could not open file: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }
    ObjParser::ObjElementData african_head_data {};
    ObjParser::parse_obj_file(african_head_obj, african_head_data);

    bool is_running = true;

    while (is_running) {
        SDL_Event event;
        while (Window::poll_event(event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                is_running = false;
            }
        }
        // TODO: DRAW STUFF
        software_renderer.clear();
        software_renderer.draw_mesh(african_head_data);

        //present on the window
        window.present(software_renderer.getPixels());
    }
    return 0;
}
