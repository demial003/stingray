#ifndef CYLINDER_H
#define CYLINDER_H

namespace utils {
class Cylinder {
public:
  Cylinder() : slices(50), height(2) {}
  void genVertices(float *vboIdx);
  int vboSize();

private:
  int slices;
  float height;
  int getVertexNumber(int i);
};
} // namespace utils

#endif
