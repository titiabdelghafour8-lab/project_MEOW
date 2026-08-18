#pragma once
#include <cassert>

namespace Math
{
    template <typename T>
    struct Vec2 {
        T x, y;
    };

    template <typename T>
    T calculate_determinant2(const Vec2<T>& A, const Vec2<T>& B)
    {
        return (A.x * B.y - B.x * A.y);
    }

    template <typename T>
    struct Vec3 {
        T x, y, z;
        Vec3<T> operator+ (const Vec3<T>& v2) { return {  this->x + v2.x,  this->y + v2.y, this->z + v2.z }; }
        T operator* (const Vec3<T>& v2) { return (this->x * v2.x + this->y * v2.y + this->z * v2.z); } // this one is for dot product
        Vec3<T> operator^ (const Vec3<T>& v2)
        {
            return Vec3<T> {
                calculate_determinant2<T>({this->y, this->z}, { v2.y, v2.z }),
                -calculate_determinant2<T>({this->x, this->z}, { v2.x, v2.z }),
                calculate_determinant2<T>({this->x, this->y}, { v2.x, v2.y })
            };
        }

        T operator[] (int i) const
        {
            assert(i >= 0 && i < 3);
            return i ? (i == 1 ? y: z) : x;
        }
    };
    template <typename T>
    struct Vec4
    {
        T x, y, z, w;
    };
    template <typename T>
    struct Matrix3x3
    {
        T mat3x3[3][3];

        Matrix3x3() = default;

        Matrix3x3(const Matrix3x3<T>& M)
        {
            for (int j = 0; j < 3; j++)
            {
                for (int i = 0; i < 3; i++)
                {
                    this->mat3x3[i][j] = M.mat3x3[i][j];
                }
            }
        }
        Matrix3x3<T>& operator=(const Matrix3x3<T>& M) = default;

        Matrix3x3(std::initializer_list<std::initializer_list<T>> list)
        {
            int i = 0;
            for (const auto& row : list) {
                int j = 0;
                for (const auto& val : row) {
                    if (i < 3 && j < 3) {
                        mat3x3[i][j] = val;
                    }
                    j++;
                }
                i++;
            }
        }
        Matrix3x3<T> operator+ (const Matrix3x3<T>& M) const
        {
            Matrix3x3<T> sum {};
            for (int j = 0; j < 3; j++)
            {
                for (int i = 0; i < 3; i++)
                {
                    sum.mat3x3[i][j] = this->mat3x3[i][j] + M.mat3x3[i][j];
                }
            }
            return sum;
        }
        Matrix3x3<T>& operator=(std::initializer_list<Vec3<T>> list)
        {
            int j { 0 };
            for (const auto& column: list)
            {
                for (int i = 0; i < 3; i++)
                {
                    this->mat3x3[i][j] = column[i];
                }
                j++;
            }
            return *this;
        }
        // Matrix3x3<T>& operator= (const Matrix3x3<T>& M)
        // {
        //     for (int j = 0; j < 3; j++)
        //     {
        //         for (int i = 0; i < 3; i++)
        //         {
        //             this->mat3x3[i][j] = M.mat3x3[i][j];
        //         }
        //     }
        //     return (*this);
        // }
        Matrix3x3<T> operator* (const Matrix3x3<T>& M2)
        {
            Matrix3x3<T> product {};

            for (int j = 0; j < 3; j++)
            {
                for (int i = 0; i < 3; i++)
                {
                    product.mat3x3[i][j] = 0;
                    for (int k = 0; k < 3; k++)
                    {
                        product.mat3x3[i][j] += this->mat3x3[i][k] * M2.mat3x3[k][j];
                    }
                }
            }
            return product;
        }
        Matrix3x3<T> operator-() const {
            Matrix3x3<T> result{};
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; ++j) {
                    result.mat3x3[i][j] = -mat3x3[i][j];
                }
            }
            return result;
        }
        Matrix3x3<T> transpose() const
        {
            Matrix3x3<T> transpose {};
            for (int j = 0; j < 3; ++j)
            {
                for (int i = 0; i < 3; ++i)
                {
                    transpose.mat3x3[i][j] = this->mat3x3[j][i];
                }
            }
            return transpose;
        }
        Matrix3x3<T> inverse() const
        {
            Matrix3x3<T> inverse {};

            Vec3<T> a { this->mat3x3[0][0], this->mat3x3[1][0], this->mat3x3[2][0] };
            Vec3<T> b { this->mat3x3[0][1], this->mat3x3[1][1], this->mat3x3[2][1] };
            Vec3<T> c { this->mat3x3[0][2], this->mat3x3[1][2], this->mat3x3[2][2] };
            T det { a * (b ^ c) };
            Vec3<T> inverse_a { (b ^ c) };
            Vec3<T> inverse_b { (a ^ c) };
            Vec3<T> inverse_c { (a ^ b) };

            inverse = { inverse_a, inverse_b, inverse_c };

            return inverse;
        }
    };

    /**********************************just for testing*************************************/
    template <typename T>
    std::ostream& operator<< (std::ostream& o, const Vec3<T>& v)
    {
        o << "{ " <<v.x << ", " << v.y << ", " << v.z << "}";
        return o;
    }
    template<typename T>
    std::ostream& operator<< (std::ostream& o, const Matrix3x3<T>& M)
    {
        int i { 0 };
        o << "{\n";
        for (const auto& row: M.mat3x3)
        {
            for (const auto& element: row)
            {
                std::cout << element << " " << (!(++i % 3) ? "\n": "");
            }
        }
        o << "}";
        return o;
    }


    /*************************************************************************************/


    // 3D coordinates into 2D pixel positins

inline Vec3<int> viewport_projection(Vec3<float> v, const int width, const int height)
    {
        Vec3<int> projected_pixel_position {
            static_cast<int>((v.x + 1.) * width / 2),
            static_cast<int>(height - (v.y + 1.) * height / 2),
            static_cast<int>((v.z + 1.) * 65535 / 2) // 65535 is for 16 bit precision, completely arbitrary!
        };
        return projected_pixel_position;
    }

    template <typename T>
    T calculate_determinant3(const Vec3<T>& A, const Vec3<T>& B, const Vec3<T>& C)
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
