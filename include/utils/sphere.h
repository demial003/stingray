#ifndef SPHERE_H
#define SPHERE_H

namespace utils {
class Sphere {
public:
  Sphere() : slices(6), stacks(6) {}
  Sphere(const int val1, const int val2) {
    slices = val1;
    stacks = val2;
  }

  void genVertices(float *vboIdx, int *eboIdx);
  int vboSize();
  int eboSize();

private:
  int slices;
  int stacks;
  int getVertexNumber(int i, int j);
};
} // namespace utils
#endif
