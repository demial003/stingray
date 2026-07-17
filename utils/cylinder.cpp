#include <cmath>
#include <utils/cylinder.h>

namespace utils {
void Cylinder::genVertices(float *vboIdx, unsigned int *eboIdx) const {
  for (auto j = 0; j <= slices; j++) {
    float phi = (float)(j % slices) / float(slices) * M_PI * 2.0f;
    float x = cos(phi);
    float z = sin(phi);

    float *basePtr = vboIdx + j * 16;
    *(basePtr++) = x;
    *(basePtr++) = (float)height;
    *(basePtr++) = z;

    *(basePtr++) = x;
    *(basePtr++) = 0;
    *(basePtr++) = z;

    *(basePtr++) = (float)j / float(slices);
    *(basePtr++) = height;

    *(basePtr++) = x;
    *(basePtr++) = -(float)height;
    *(basePtr++) = z;

    *(basePtr++) = x;
    *(basePtr++) = 0;
    *(basePtr++) = z;

    *(basePtr++) = (float)(j % slices) / float(slices);
    *(basePtr++) = -height;
  }
}

int Cylinder::numVertices() const { return 16 * (slices + 1); }
int Cylinder::numElements() const { return 0; };
} // namespace utils
