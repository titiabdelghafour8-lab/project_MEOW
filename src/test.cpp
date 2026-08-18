#include <iostream>
#include "Math.h"

int main(int argc, char** argv)
{
    std::cout << "=== Matrix3x3 Full Operations & Print Test ===\n\n";

    Math::Matrix3x3<float> m1{{{1.0f, 2.0f, 3.0f},
                              {0.0f, 1.0f, 4.0f},
                              {5.0f, 6.0f, 0.0f}}};

    Math::Matrix3x3<float> m2{{{2.0f, 0.0f, 0.0f},
                              {1.0f, 3.0f, 2.0f},
                              {0.0f, 0.0f, 1.0f}}};

    std::cout << "Matrix m1:\n" << m1 << "\n\n";
    std::cout << "Matrix m2:\n" << m2 << "\n\n";

    Math::Matrix3x3<float> sum_m = m1 + m2;
    std::cout << "Sum (m1 + m2):\n" << sum_m << "\n\n";

    Math::Matrix3x3<float> prod_m = m1 * m2;
    std::cout << "Product (m1 * m2):\n" << prod_m << "\n\n";

    Math::Matrix3x3<float> neg_m = -m1;
    std::cout << "Negation (-m1):\n" << neg_m << "\n\n";

    Math::Matrix3x3<float> trans_m = m1.transpose();
    std::cout << "Transpose of m1:\n" << trans_m << "\n\n";

    Math::Matrix3x3<float> inv_m = m1.inverse();
    std::cout << "Inverse of m1:\n" << inv_m << "\n";

    return 0;
}