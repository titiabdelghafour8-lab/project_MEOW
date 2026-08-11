#pragma once
#include <fstream>
#include <vector>
#include <sstream>
#include <array>
#include <algorithm>


struct Vec3
{
    float x, y, z;
};
namespace ObjParser
{
    struct ObjElementData
    {
        std::vector<Vec3> v;
        std::vector<std::array<std::array<int, 3>, 3>> f;
    };
    inline void parse_obj_file(std::ifstream& file_object, ObjElementData& obj_data)
    {
        std::string line;
        while (std::getline(file_object, line))
        {
            std::replace(line.begin(), line.end(), '/', ' ');
            std::istringstream ss(line);
            std::string token {};
            ss >> token;
            if (token == "v")
            {
                float x, y, z;
                ss >> x >> y >> z;
                obj_data.v.push_back(Vec3 { x, y, z });
            }
            else if (token == "f")
            {
                std::array<int, 3> indices1 {};
                std::array<int, 3> indices2 {};
                std::array<int, 3> indices3 {};
                ss >> indices1[0] >> indices1[1] >> indices1[2]
                   >> indices2[0] >> indices2[1] >> indices2[2]
                   >> indices3[0] >> indices3[1] >> indices3[2];
                obj_data.f.push_back({ indices1, indices2, indices3 });
            }
        }
    }
};