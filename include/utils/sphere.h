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
  virtual int numVertices() const;
  virtual int numElements() const;

  virtual void genVertices(float *vboIdx, unsigned int *eboIdx) const;

private:
  int slices;
  int stacks;
  int getVertexNumber(int i, int j) const;
};
} // namespace utils
#endif
