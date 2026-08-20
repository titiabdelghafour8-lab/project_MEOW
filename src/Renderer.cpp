#include "Renderer.h"
#include "Color.h"
#include "Math.h"
#include <vector>
#include <algorithm>
#include "OBJParser.h"
#include <SDL3/SDL_stdinc.h>
#include <cmath>

using Vec3 = Math::Vec3<float>;

Renderer::Renderer(int width, int height)
    : screen_width(width),
      screen_height(height),
      pixels(width * height),
      z_buffer(width * height)
{
}

void Renderer::clear()
{
    std::ranges::fill(pixels, 0xFF000000);
    std::ranges::fill(z_buffer, -1);
}

void Renderer::draw_mesh(const ObjParser::ObjElementData& obj_data)
{
    std::vector<std::array<int, 3>> faces { obj_data.faces() };

    for (const auto& face_indices: faces)
    {
        Math::Vec3<float> vertex_a {
            Math::rotate_y(obj_data.vertices()[face_indices[0]], 0)
        };
        Math::Vec3<float> vertex_b {
            Math::rotate_y(obj_data.vertices()[face_indices[1]],0)
        };
        Math::Vec3<float> vertex_c {
            Math::rotate_y(obj_data.vertices()[face_indices[2]], 0)
        };
        Renderer::rasterize_triangle( {vertex_a.x, vertex_a.y, vertex_a.z} , {vertex_b.x, vertex_b.y, vertex_b.z} , {vertex_c.x, vertex_c.y, vertex_c.z});
    }
}

void Renderer::rasterize_triangle(const Vec3& A, const Vec3& B, const Vec3& C)
{
    float rotation_angle { PI/6 }; // In radian
    Vec3 camera_position { 0, 0, 3 };
    Vec3 transformed_A { Math::rotate_y(A, rotation_angle) };
    Vec3 transformed_B { Math::rotate_y(B, rotation_angle) };
    Vec3 transformed_C { Math::rotate_y(C, rotation_angle) };
    Math::Vec3<int> projected_A {
            Math::viewport_projection(Math::perspective_projection(transformed_A, camera_position),
            Renderer::screen_width,
            Renderer::screen_height)
    };
    Math::Vec3<int> projected_B {
            Math::viewport_projection(Math::perspective_projection(transformed_B, camera_position),
            Renderer::screen_width,
            Renderer::screen_height)
    };
    Math::Vec3<int> projected_C {
            Math::viewport_projection(Math::perspective_projection(transformed_C, camera_position),
            Renderer::screen_width,
            Renderer::screen_height
            )
    };
    int bounding_x_min { std::clamp(std::min(std::min(projected_A.x, projected_B.x), projected_C.x), 0, Renderer::screen_width - 1) };
    int bounding_x_max { std::clamp(std::max(std::max(projected_A.x, projected_B.x), projected_C.x), 0 ,Renderer::screen_width - 1) };
    int bounding_y_min { std::clamp(std::min(std::min(projected_A.y, projected_B.y), projected_C.y), 0, Renderer::screen_height - 1) };
    int bounding_y_max { std::clamp(std::max(std::max(projected_A.y, projected_B.y), projected_C.y), 0 ,Renderer::screen_height - 1) };

    for (int y { bounding_y_min }; y <= bounding_y_max; y++)
    {
        for (int x { bounding_x_min }; x <= bounding_x_max; x++)
        {
            Math::Vec3<float> bary_coords {
                Math::calculate_bary_coords(
                { projected_A.x, projected_A.y },
                { projected_B.x, projected_B.y },
                { projected_C.x, projected_C.y },
                { x, y }
                )
            };

            if (bary_coords.x < 0 || bary_coords.y < 0 || bary_coords.z < 0) continue;

            bary_coords.x = std::clamp(bary_coords.x, 0.0f, 1.0f);
            bary_coords.y = std::clamp(bary_coords.y, 0.0f, 1.0f);
            bary_coords.z = std::clamp(bary_coords.z, 0.0f, 1.0f);

            float z {
                bary_coords.x * transformed_A.z +
                bary_coords.y * transformed_B.z +
                bary_coords.z * transformed_C.z
            };

            float z_precision { 65535.f / 2.f };
            int projected_z { static_cast<int>((z + 1) * z_precision) };

            if (projected_z > z_buffer[y * Renderer::screen_width + x])
            {
                z_buffer[y * Renderer::screen_width + x] = projected_z;
                // auto r = static_cast<Uint8>(bary_coords.x * 255.0f);
                // auto g = static_cast<Uint8>(bary_coords.y * 255.0f);
                // auto b = static_cast<Uint8>(bary_coords.z * 255.0f);
                // Uint32 pixel_color { getColor(r, g, b, 255) };
                // if (std::min({bary_coords.x, bary_coords.y, bary_coords.z}) > 0.1f) continue;
                // Uint8 a = static_cast<Uint8>(r + g + b);
                auto depth_intensity = static_cast<Uint8>((z + 1) * 127.5f) ;
                Uint32 pixel_color = getColor(depth_intensity, depth_intensity, depth_intensity, 255);
                pixels[y * Renderer::screen_width + x] = pixel_color;
                // Uint32 pixel_color = getColor(r, g, b, 255);

            }
        }
    }
}
