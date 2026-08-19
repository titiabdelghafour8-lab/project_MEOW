#pragma once
#include <algorithm>
#include <fstream>
#include <vector>
#include <sstream>
#include <array>
#include "Math.h"

using Vec3 = Math::Vec3<float>;

namespace ObjParser
{
    struct ObjElementData
    {
    private:
        std::vector<Vec3> m_v {};
        std::vector<std::array<int, 3>> m_f {};
        friend void parse_obj_file(std::ifstream& obj_file, ObjElementData& obj_data);
    public:
        ObjElementData(const std::vector<Vec3>& v, const std::vector<std::array<int, 3>>& f)
            : m_v(v), m_f(f) {}
        ObjElementData() = default;
        [[nodiscard]] size_t vertices_count() const { return m_v.size(); }
        [[nodiscard]] const std::vector<std::array<int, 3>>& faces() const { return m_f; }
        [[nodiscard]] const std::vector<Vec3>& vertices() const { return m_v; }

    };

    inline void parse_obj_file(std::ifstream& obj_file, ObjElementData& obj_data)
    {
        std::string line;
        while (std::getline(obj_file, line))
        {
            std::istringstream ss(line);
            std::string token {};
            ss >> token;
            if (token == "v")
            {
                float x, y, z;
                ss >> x >> y >> z;
                obj_data.m_v.push_back(Vec3 { x, y, z });
            }
            else if (token == "f")
            {
                std::array<int, 3> indices {};
                std::string index_token {};
                int i { 0 };
                while (ss >> index_token)
                {
                    std::istringstream index_token_ss { index_token };
                    std::string  index_str {};
                    std::getline(index_token_ss, index_str, '/');
                    indices[i++] = std::stoi(index_str) - 1; // obj files are 1-indexed
                }
                obj_data.m_f.push_back(indices);
            }
        }

    }
};