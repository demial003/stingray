#include "utils/cylinder.h"
#include <cmath>
#include <iostream>

namespace utils {
void Cylinder::genVertices(float *vboIdx) {
  for (auto j = 0; j <= slices; j++) {
    float phi = (float)(j % slices) / float(slices) * M_PI * 2.0f;
    float x = sin(phi);
    float z = cos(phi);

    float *basePtr = vboIdx + j * 6;
    *(basePtr) = x;
    *(basePtr + 1) = height / 1.0f;
    *(basePtr + 2) = z;

    *(basePtr + 3) = x;
    *(basePtr + 4) = -height / 1.0f;
    *(basePtr + 5) = z;

    std::cout << j << ": " << x << ", " << height << ", " << z << "\n";
    std::cout << j << ": " << x << ", " << -height << ", " << z << "\n";
  }

  // for(auto i = 0; i < slices; i++){
  //
  // }
}

int Cylinder::vboSize() { return 7 * slices + 1; }
} // namespace utils
