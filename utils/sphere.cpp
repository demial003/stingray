#include "utils/sphere.h"
#include <cmath>

namespace utils {
void Sphere::genVertices(float *vboIdx, int *eboIdx) {

  for (auto i = 0; i < slices; i++) {
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

      float *basePtr = vboIdx + vertNumber * 3;
      *(basePtr) = x;
      *(basePtr + 1) = y;
      *(basePtr + 2) = z;
    }
  }

  for (int i = 0; i < slices; i++) {
    *(eboIdx++) = getVertexNumber(i, 0);
    *(eboIdx++) = getVertexNumber(i, 1);
    *(eboIdx++) = getVertexNumber(i + 1, 1);
  }

  for (int i = 0; i < slices; i++) {
    for (int j = 1; j < stacks - 1; j++) {
      *(eboIdx++) = getVertexNumber(i, j);
      *(eboIdx++) = getVertexNumber(i, j + 1);
      *(eboIdx++) = getVertexNumber(i + 1, j);

      *(eboIdx++) = getVertexNumber(i, j + 1);
      *(eboIdx++) = getVertexNumber(i + 1, j + 1);
      *(eboIdx++) = getVertexNumber(i + 1, j);
    }
  }

  for (int i = 0; i < slices; i++) {
    *(eboIdx++) = getVertexNumber(i, stacks);
    *(eboIdx++) = getVertexNumber(i + 1, stacks - 1);
    *(eboIdx++) = getVertexNumber(i, stacks - 1);
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

int Sphere::vboSize() { return 3 * (slices * (stacks - 1) + 2); }
int Sphere::eboSize() { return 6 * slices * (stacks - 1); }
} // namespace utils
