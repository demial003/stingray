#include "utils/sphere.h"
#include <cmath>
#include <iostream>

namespace utils {
void genVertices() {
  int numVertices = slices * stacks + 2;
  float vert[numVertices];

  for (auto i = 0; i < stacks - 1; i++) {
    auto phi = M_PI * ((float)i + 1) / (float)stacks;
    for (auto j = 0; j < slices; j++) {
      float theta = 2.0 * M_PI * (float)j / (float)slices;
      float x = sin(phi) * sin(theta);
      float y = cos(phi);
      float z = sin(phi) * cos(theta);
      std::cout << x << ", " << y << ", " << z << "\n";
    }
  }
}
} // namespace utils
