#pragma once
#include <vector>
#include <SDL3/SDL_stdinc.h>
#include "Math.h"
#include "OBJParser.h"


class Renderer
{
private:
    int screen_width { 800 };
    int screen_height { 600 };
    std::vector<Uint32> pixels;
    std::vector<double> z_buffer;
    void rasterize_triangle(const Math::Vec3<float>& vertex_a, const Math::Vec3<float>& vertex_b, const Math::Vec3<float>& vertex_c);

public:
    Renderer(int screen_width, int screen_height);
    ~Renderer() = default;
    void draw_mesh(const ObjParser::ObjElementData& obj_data);
    void clear();
    [[nodiscard]] const std::vector<Uint32>& getPixels() const { return pixels; }
    [[nodiscard]] const std::vector<double>& getZBuffer() const { return z_buffer; }
};
