#ifndef SPHERE_H
#define SPHERE_H

#include "utils/mesh.h"
namespace utils {
class Sphere : public Mesh {
public:
  Sphere() : slices(6), stacks(6) {}
  Sphere(const int val1, const int val2) {
    slices = val1;
    stacks = val2;
  }
  int numVertices();
  int numElements();

  void genVertices(float *vboIdx, unsigned int *eboIdx);

private:
  int slices;
  int stacks;
  int getVertexNumber(int i, int j);
};
} // namespace utils
#endif
