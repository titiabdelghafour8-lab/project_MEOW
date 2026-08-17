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
    inline Vec3<int> viewport_projection(Vec3<float> v, const int width, const int height)
    {
        Vec3<int> projected_pixel_position {
            static_cast<int>((v.x + 1.) * width / 2),
            static_cast<int>(height - (v.y + 1.) * height / 2),
            static_cast<int>((v.z + 1.) * 65535 / 2) // 65535 is for 16 bit precision, completely arbitrary!
        };
        return projected_pixel_position;
    }

    inline int calculate_determinant3(const Vec3<int>& A, const Vec3<int>& B, const Vec3<int>& C)
    {
        //3x3 Sarrus's rule
        return (A.x * B.y * C.z + B.x * C.y * A.z + C.x * A.y * B.z)
              - (C.x * B.y * A.z + A.x * C.y * B.z + B.x * A.y * C.z);
    }

    inline float calculate_area_triangle(Vec2<int> A, Vec2<int> B, Vec2<int> C)
    {
        return 0.5f * static_cast<float>(calculate_determinant3(
            Vec3<int> {A.x, A.y, 1},
            Vec3<int> {B.x, B.y, 1},
            Vec3<int> {C.x, C.y, 1}
        ));
    }



    inline Vec3<float> calculate_bary_coords(const Vec2<int>& A, const Vec2<int>& B, const Vec2<int>& C, const Vec2<int>& P)
    {
        Vec3<float> bary_coords {};
        float total_area { calculate_area_triangle(A, B, C) };
        if( calculate_area_triangle(A, B, C) == 0.0f)
            return {-1.0f, -1.0f, -1.0f};
        float area_inverse { 1 / total_area };
        bary_coords.x = calculate_area_triangle(P, B, C) * area_inverse;
        bary_coords.y = calculate_area_triangle(P, C, A) * area_inverse;
        bary_coords.z = calculate_area_triangle(P, A, B) * area_inverse;

        return bary_coords;
    }
};
