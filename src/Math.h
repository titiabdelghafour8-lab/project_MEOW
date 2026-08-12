#pragma once

namespace Math
{
    template <typename T>
    struct Vec2 {
        T x, y;
    };

    template <typename T>
    struct Vec3 {
        T x, y, z;
    };

    // 3D coordinates into 2D pixel positions
    inline Vec2<int> viewport_projection(Vec3<float> v, const int width, const int height)
    {
        Vec2<int> projected_pixel_position {
            static_cast<int>((v.x + 1.) * width / 2),
            static_cast<int>(height - (v.y + 1.) * height / 2)
        };
        return projected_pixel_position;
    }
};
