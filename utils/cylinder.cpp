#include <cmath>
#include <utils/cylinder.h>

namespace utils {
void Cylinder::genVertices(float *vboIdx, unsigned int *eboIdx) {
  for (auto j = 0; j <= slices; j++) {
    float phi = (float)(j % slices) / float(slices) * M_PI * 2.0f;
    float x = cos(phi);
    float z = sin(phi);

    float *basePtr = vboIdx + j * 6;
    *(basePtr) = x;
    *(basePtr + 1) = (float)height / 1.0f;
    *(basePtr + 2) = z;

    *(basePtr + 3) = x;
    *(basePtr + 4) = -(float)height / 1.0f;
    *(basePtr + 5) = z;
  }
}

int Cylinder::numVertices() { return 6 * (slices + 1); }
int Cylidner::numElems() { return 0; };
} // namespace utils
