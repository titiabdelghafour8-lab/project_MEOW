#include "Renderer.h"
#include "Color.h"
#include "Math.h"
#include <vector>
#include <algorithm>
#include "OBJParser.h"
#include <SDL3/SDL_stdinc.h>
#include <limits>
#include <array>

using Vec3 = Math::Vec3<float>;
using Vec4 = Math::Vec4<float>;

namespace
{
    constexpr float near_clip_w { 0.01f };

    Vec4 interpolate_at_near_plane(const Vec4& a, const Vec4& b)
    {
        const float t { (near_clip_w - a.w) / (b.w - a.w) };
        return {
            a.x + (b.x - a.x) * t,
            a.y + (b.y - a.y) * t,
            a.z + (b.z - a.z) * t,
            a.w + (b.w - a.w) * t
        };
    }

    std::vector<Vec4> clip_triangle_against_near_plane(const std::array<Vec4, 3>& triangle)
    {
        std::vector<Vec4> clipped;
        clipped.reserve(4);

        Vec4 previous { triangle.back() };
        bool previous_inside { previous.w >= near_clip_w };

        for (const Vec4& current : triangle)
        {
            const bool current_inside { current.w >= near_clip_w };

            if (current_inside != previous_inside)
                clipped.push_back(interpolate_at_near_plane(previous, current));

            if (current_inside)
                clipped.push_back(current);

            previous = current;
            previous_inside = current_inside;
        }

        return clipped;
    }

    Vec3 perspective_divide(const Vec4& vertex)
    {
        return { vertex.x / vertex.w, vertex.y / vertex.w, vertex.z / vertex.w };
    }
}

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
    std::ranges::fill(z_buffer, -std::numeric_limits<double>::infinity());
}

void Renderer::draw_mesh(const ObjParser::ObjElementData& obj_data)
{
    std::vector<std::array<int, 3>> faces { obj_data.faces() };

    for (const auto& face_indices: faces)
    {
        Vec3   eye{-1,0,2}; // camera position
        Vec3    center{0,0,0};  // camera direction
        Vec3    up{0,1,0};  // camera up vector
        const float focal_distance { (eye - center).length() };
        const std::array<Vec4, 3> triangle {
            Math::perspective_projection(Math::look_at(center, eye, up, obj_data.vertices()[face_indices[0]]), focal_distance),
            Math::perspective_projection(Math::look_at(center, eye, up, obj_data.vertices()[face_indices[1]]), focal_distance),
            Math::perspective_projection(Math::look_at(center, eye, up, obj_data.vertices()[face_indices[2]]), focal_distance)
        };

        const std::vector<Vec4> clipped { clip_triangle_against_near_plane(triangle) };
        for (std::size_t i { 1 }; i + 1 < clipped.size(); ++i)
        {
            Renderer::rasterize_triangle(
                perspective_divide(clipped[0]),
                perspective_divide(clipped[i]),
                perspective_divide(clipped[i + 1])
            );
        }
    }
}
void Renderer::rasterize_triangle(const Vec3& A, const Vec3& B, const Vec3& C)
{
    Math::Vec3<int> projected_A {
            Math::viewport_projection(
                A,
                Renderer::screen_width,
                Renderer::screen_height
                )
    };
    Math::Vec3<int> projected_B {
            Math::viewport_projection(
                B,
                Renderer::screen_width,
                Renderer::screen_height
                )
    };
    Math::Vec3<int> projected_C {
            Math::viewport_projection(
                C,
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

            double z {
                bary_coords.x * A.z +
                bary_coords.y * B.z +
                bary_coords.z * C.z
            };


            if (z > z_buffer[y * Renderer::screen_width + x])
            {
                z_buffer[y * Renderer::screen_width + x] = z;
                // auto r = static_cast<Uint8>(bary_coords.x * 255.0f);
                // auto g = static_cast<Uint8>(bary_coords.y * 255.0f);
                // auto b = static_cast<Uint8>(bary_coords.z * 255.0f);
                // Uint32 pixel_color { getColor(r, g, b, 255) };
                // if (std::min({bary_coords.x, bary_coords.y, bary_coords.z}) > 0.1f) continue;
                // Uint8 a = static_cast<Uint8>(r + g + b);
                auto depth_intensity = static_cast<Uint8>(std::clamp((z + 1) * 127.5f, 0.0, 255.0)) ;
                Uint32 pixel_color = getColor(depth_intensity, depth_intensity, depth_intensity, 255);
                pixels[y * Renderer::screen_width + x] = pixel_color;
                // Uint32 pixel_color = getColor(r, g, b, 255);

            }
        }
    }
}
