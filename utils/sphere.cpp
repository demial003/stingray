#include "utils/sphere.h"
#include <cmath>
#include <iostream>
#include <vector>

namespace utils {
void Sphere::genVertices(float *vboIdx, int *eboIdx) {

  for (auto i = 0; i <= slices; i++) {
    float theta = 2.0 * M_PI * float(i % slices) / (float)slices;
    float costheta = cos(theta);
    float sintheta = sin(theta);
    for (auto j = 0; j <= stacks; j++) {
      float phi = M_PI * float(j) / (float)stacks;
      float sinphi = (j < stacks) ? sin(phi) : 0.0f;
      float cosphi = cos(phi);
      float x = sinphi * sintheta;
      float y = cosphi;
      float z = sinphi * costheta;

      int vertNumber = getVertexNumber(i, j);
      //      if (vertNumber == 1 || vertNumber == 0)
      //        continue;

      float *basePtr = vboIdx + vertNumber * 3;
      *(basePtr) = x;
      *(basePtr + 1) = y;
      *(basePtr + 2) = z;

      //      std::cout << vertNumber << ": " << x << ", " << y << ", " << z <<
      //      "\n";
      //
    }
  }

  for (auto i = 0; i < slices; i++) {
    unsigned int leftIdxOld = getVertexNumber(i, 0);
    unsigned int rightIdxOld = getVertexNumber(i + 1, 1);
    for (auto j = 0; j < stacks - 1; j++) {
      unsigned int leftIdxNew = getVertexNumber(i, j + 1);
      unsigned int rightIdxNew = getVertexNumber(i + 1, j + 2);

      *(eboIdx++) = leftIdxOld;
      *(eboIdx++) = rightIdxOld;
      *(eboIdx++) = leftIdxNew;

      *(eboIdx++) = leftIdxNew;
      *(eboIdx++) = rightIdxNew;
      *(eboIdx++) = rightIdxOld;

      leftIdxOld = leftIdxNew;
      rightIdxOld = rightIdxNew;
    }
  }
}

int Sphere::getVertexNumber(int i, int j) {
  if (j == 0) {
    return 0;
  } else if (j == stacks) {
    return 1;
  } else {
    return (stacks - 1) * (i % slices) + j + 1;
  }
}
} // namespace utils
