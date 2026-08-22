#pragma once
#include <cassert>
#include <cmath>
#include <type_traits>
#include <iostream>

/*               CONVENTIONS
 *
 *   World: right-handed, X right, Y up, Z toward viewer
 *   Screen: X right, Y down
 *   Matrix storage/indexing: [row][column]
 *   Matrix initializer lists: rows
 *   Vectors: column vectors
 *   Transformation: matrix * vector
 *   Positive rotations: right hand rule
 *   Depth: larger Z is closer
 *
 */

constexpr float PI = 3.1415927;

template<typename T>
concept Arithmetic = std::is_arithmetic_v<T>;


namespace Math
{
    template <Arithmetic T>
    struct Vec2 {
        T x, y;
        Vec2 (T x_value, T  y_value): x{x_value}, y{y_value} {}
        Vec2(const Vec2<T>& v) = default;
    };

    template <Arithmetic T>
    T calculate_determinant2(const Vec2<T>& A, const Vec2<T>& B)
    {
        return (A.x * B.y - B.x * A.y);
    }

    template <Arithmetic  T>
    struct Vec3
    {
        T x {};
        T y {};
        T z {};
        Vec3(const T& x_value, const T& y_value, const T& z_value): x{x_value}, y{y_value}, z{z_value} {}

        Vec3() = default;

        Vec3<T> operator+ (const Vec3<T>& v2) const { return { this->x + v2.x, this->y + v2.y, this->z + v2.z }; }
        Vec3<T> operator- (const Vec3<T>& v2) const { return { this->x - v2.x, this->y - v2.y, this->z - v2.z }; }
        T operator* (const Vec3<T>& v2) { return (this->x * v2.x + this->y * v2.y + this->z * v2.z); } // this one is for dot product
        [[nodiscard]] Vec3<T> scalar (const T& lambda) const { return {this->x * lambda, this->y * lambda, this->z * lambda}; } // this one is for dot product
        Vec3<T> operator^ (const Vec3<T>& v2) const
        {
            return Vec3<T> {
                calculate_determinant2<T>({this->y, this->z}, { v2.y, v2.z }),
                -calculate_determinant2<T>({this->x, this->z}, { v2.x, v2.z }),
                calculate_determinant2<T>({this->x, this->y}, { v2.x, v2.y })
            };
        }
        Vec3& operator= (const Vec3<T>& v) = default;
        float length()
        {
            return std::sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
        }
        void normalize()
        {
            float length_inverse { this->length() == 0 ? 1 : 1 / this->length() };
            this->x = this->x * length_inverse;
            this->y = this->y * length_inverse;
            this->z = this->z * length_inverse;
        };
        T operator[] (int i) const
        {
            assert(i >= 0 && i < 3);
            return i ? (i == 1 ? y: z) : x;
        }

    };
    template <Arithmetic  T>
    struct Vec4
    {
        T x{};
        T y{};
        T z{};
        T w{};
        Vec4(const T& x_value, const T& y_value, const T& z_value, const T& w_value)
            : x{x_value}, y{y_value}, z{z_value}, w{w_value} {}

        Vec4() = default;

        Vec4<T> operator+(const Vec4<T>& v2) const
        {
            return { x + v2.x, y + v2.y, z + v2.z, w + v2.w };
        }

        T operator*(const Vec4<T>& v2) const
        {
            return x * v2.x + y * v2.y + z * v2.z + w * v2.w;
        }

        [[nodiscard]] Vec4<T> scalar(const T& lambda) const
        {
            return { x * lambda, y * lambda, z * lambda, w * lambda };
        }

        T operator[](int i) const
        {
            assert(i >= 0 && i < 4);
            return i == 0 ? x : (i == 1 ? y : (i == 2 ? z : w));
        }
    };
    template <Arithmetic   T>
    struct Matrix3x3
    {
        T mat3x3[3][3] {};

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
            assert(list.size() == 3);
            int i { 0 };
            for (const auto& row: list)
            {
                for (int j = 0; j < 3; j++)
                {
                    this->mat3x3[i][j] = row[j];
                }
                i++;
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
        Vec3<T> operator* (const Vec3<T>& v) const
        {
            return Vec3<T> {
                mat3x3[0][0] * v.x + mat3x3[0][1] * v.y + mat3x3[0][2] * v.z,
                mat3x3[1][0] * v.x + mat3x3[1][1] * v.y + mat3x3[1][2] * v.z,
                mat3x3[2][0] * v.x + mat3x3[2][1] * v.y + mat3x3[2][2] * v.z
            };
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
        [[nodiscard]] Matrix3x3<T> transpose() const
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
        [[nodiscard]]Matrix3x3<T> inverse() const
        {
            Matrix3x3<T> inverse {};

            Vec3<T> a { this->mat3x3[0][0], this->mat3x3[1][0], this->mat3x3[2][0] };
            Vec3<T> b { this->mat3x3[0][1], this->mat3x3[1][1], this->mat3x3[2][1] };
            Vec3<T> c { this->mat3x3[0][2], this->mat3x3[1][2], this->mat3x3[2][2] };
            T det_inverse{ T{1} / (a * (b ^ c)) };
            Vec3<T> inverse_a { (b ^ c).scalar(det_inverse) };
            Vec3<T> inverse_b { (c ^ a).scalar(det_inverse) };
            Vec3<T> inverse_c { (a ^ b).scalar(det_inverse) };

            inverse = { inverse_a, inverse_b, inverse_c };

            return inverse;
        }
    };

    template <Arithmetic T>
    struct Matrix4x4
    {
        T mat4x4[4][4] {};

        Matrix4x4() = default;
        Matrix4x4(const Matrix4x4<T>& M) = default;
        Matrix4x4<T>& operator=(const Matrix4x4<T>& M) = default;

        Matrix4x4(std::initializer_list<std::initializer_list<T>> list)
        {
            int row { 0 };
            for (const auto& row_values: list)
            {
                if (row >= 4) break;

                int column { 0 };
                for (const auto& value: row_values)
                {
                    if (column >= 4) break;
                    mat4x4[row][column] = value;
                    column++;
                }
                row++;
            }
        }

        Matrix4x4<T>& operator=(std::initializer_list<Vec4<T>> list)
        {
            int row { 0 };
            for (const auto& row_vector: list)
            {
                if (row >= 4) break;

                mat4x4[row][0] = row_vector.x;
                mat4x4[row][1] = row_vector.y;
                mat4x4[row][2] = row_vector.z;
                mat4x4[row][3] = row_vector.w;
                row++;
            }
            return *this;
        }

        Matrix4x4<T> operator+(const Matrix4x4<T>& M) const
        {
            Matrix4x4<T> sum {};
            for (int row = 0; row < 4; row++)
            {
                for (int column = 0; column < 4; column++)
                {
                    sum.mat4x4[row][column] = mat4x4[row][column] + M.mat4x4[row][column];
                }
            }
            return sum;
        }

        Matrix4x4<T> operator*(const Matrix4x4<T>& M) const
        {
            Matrix4x4<T> product {};
            for (int row = 0; row < 4; row++)
            {
                for (int column = 0; column < 4; column++)
                {
                    for (int k = 0; k < 4; k++)
                    {
                        product.mat4x4[row][column] += mat4x4[row][k] * M.mat4x4[k][column];
                    }
                }
            }
            return product;
        }

        Vec4<T> operator*(const Vec4<T>& v) const
        {
            return {
                mat4x4[0][0] * v.x + mat4x4[0][1] * v.y + mat4x4[0][2] * v.z + mat4x4[0][3] * v.w,
                mat4x4[1][0] * v.x + mat4x4[1][1] * v.y + mat4x4[1][2] * v.z + mat4x4[1][3] * v.w,
                mat4x4[2][0] * v.x + mat4x4[2][1] * v.y + mat4x4[2][2] * v.z + mat4x4[2][3] * v.w,
                mat4x4[3][0] * v.x + mat4x4[3][1] * v.y + mat4x4[3][2] * v.z + mat4x4[3][3] * v.w
            };
        }

        Matrix4x4<T> operator-() const
        {
            Matrix4x4<T> result {};
            for (int row = 0; row < 4; row++)
            {
                for (int column = 0; column < 4; column++)
                {
                    result.mat4x4[row][column] = -mat4x4[row][column];
                }
            }
            return result;
        }

        [[nodiscard]] Matrix4x4<T> transpose() const
        {
            Matrix4x4<T> result {};
            for (int row = 0; row < 4; row++)
            {
                for (int column = 0; column < 4; column++)
                {
                    result.mat4x4[row][column] = mat4x4[column][row];
                }
            }
            return result;
        }
    };

    /**********************************just for testing*************************************/
    template <Arithmetic T>
    std::ostream& operator<< (std::ostream& o, const Vec3<T>& v)
    {
        o << "{ " <<v.x << ", " << v.y << ", " << v.z << "}";
        return o;
    }
    template <Arithmetic T>
    std::ostream& operator<< (std::ostream& o, const Vec4<T>& v)
    {
        o << "{ " << v.x << ", " << v.y << ", " << v.z << ", " << v.w << "}";
        return o;
    }

    template<Arithmetic T>
    std::ostream& operator<< (std::ostream& o, const Matrix3x3<T>& M)
    {
        int i { 0 };
        o << "{\n";
        for (const auto& row: M.mat3x3)
        {
            for (const auto& element: row)
            {
                o << element << " " << (!(++i % 3) ? "\n": "");
            }
        }
        o << "}";
        return o;
    }
    template<Arithmetic T>
    std::ostream& operator<< (std::ostream& o, const Matrix4x4<T>& M)
    {
        o << "{\n";
        for (const auto& row: M.mat4x4)
        {
            for (const auto& element: row)
            {
                o << element << " ";
            }
            o << "\n";
        }
        o << "}";
        return o;
    }


    /*************************************************************************************/


    // 3D coordinates into 2D pixel positins

// PROJECTIONS

    inline Vec3<int> viewport_projection(Vec3<float> v, const int width, const int height)
    {
        v.y = -v.y; //flipping y before projecting
        Matrix4x4<float> viewport_matrix
        {
            { static_cast<float>(width) * 0.5f, 0, 0, static_cast<float>(width) * 0.5f},
            { 0, static_cast<float>(height) * 0.5f, 0, static_cast<float>(height) * 0.5f},
            { 0, 0, 1, 0 },
            { 0, 0, 0, 1 }
        };
        const Vec4<float> projected {
            viewport_matrix * Vec4<float> { v.x, v.y, v.z, 1 }
        };
        return {
            static_cast<int>(projected.x),
            static_cast<int>(projected.y),
            static_cast<int>(projected.z)
        };
    }

    inline Vec4<float> perspective_projection(const Vec3<float>& v, float f)
    {
        Matrix4x4<float> perspective_matrix
        {
            { 1, 0, 0, 0 },
            { 0, 1, 0, 0 },
            { 0, 0, 1, 0 },
            { 0, 0, -1/f, 1 }
        };
        Vec4<float> projected
        {
            perspective_matrix * Vec4<float>{ v.x, v.y, v.z, 1 }
        };
        return projected;
    }

    inline Vec3<float> look_at(const Vec3<float>& center, const Vec3<float>& camera_eye, const Vec3<float>& up, const Vec3<float>& point)
    {
        Vec3<float> n { camera_eye - center };
        n.normalize();
        Vec3<float> l { (up ^ n) };
        l.normalize();
        Vec3<float> m { (n ^ l) };
        m.normalize();

        Matrix4x4<float> change_of_basis{
            {l.x, l.y, l.z, 0.0f},
            {m.x, m.y, m.z, 0.0f},
            {n.x, n.y, n.z, 0.0f},
            {0.0f, 0.0f, 0.0f, 1.0f}
        };
        Matrix4x4<float> translation{
            {1.0f, 0.0f, 0.0f, -center.x},
            {0.0f, 1.0f, 0.0f, -center.y},
            {0.0f, 0.0f, 1.0f, -center.z},
            {0.0f, 0.0f, 0.0f, 1.0f}
        };
        Matrix4x4<float> look_at_matrix { change_of_basis * translation };

        Vec4<float> projected { look_at_matrix * Vec4<float> { point.x, point.y, point.z, 1 } };
        return { projected.x, projected.y, projected.z };
    }

// CALCULATIONS

    template <Arithmetic T>
    float calculate_determinant3(const Vec3<T>& A, const Vec3<T>& B, const Vec3<T>& C)
    {

        //3x3 Sarrus's rule
        return (A.x * B.y * C.z + B.x * C.y * A.z + C.x * A.y * B.z)
              - (C.x * B.y * A.z + A.x * C.y * B.z + B.x * A.y * C.z);
    }

    inline float calculate_area_triangle(const Vec2<int>& A, const Vec2<int>& B, const Vec2<int>& C)
    {
        return 0.5f * calculate_determinant3(
            Vec3<int> {A.x, A.y, 1},
            Vec3<int> {B.x, B.y, 1},
            Vec3<int> {C.x, C.y, 1}
        );
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

// TRANSFORMATIONS

    inline Vec3<float> rotate_y(const Vec3<float>& Point, float angle)
    {
        Matrix3x3<float> rotation_matrix_y {
            {std::cos(angle), 0, std::sin(angle)},
            {0, 1, 0},
            {-std::sin(angle), 0, std::cos(angle)} };
        return rotation_matrix_y * Point;
    }

    inline Vec3<float> rotate_x(const Vec3<float>& Point, float angle)
    {
        Matrix3x3<float> rotation_matrix_x {
            {1, 0, 0},
            {0, std::cos(angle), -std::sin(angle)},
            {0, std::sin(angle), std::cos(angle)}
        };
        return rotation_matrix_x * Point;
    }

    inline Vec3<float> rotate_z(const Vec3<float>& Point, float angle)
    {
        Matrix3x3<float> rotation_matrix_z {
            {std::cos(angle), -std::sin(angle), 0},
            {std::sin(angle), std::cos(angle), 0},
            {0, 0, 1}
        };
        return rotation_matrix_z * Point;
    }
};
