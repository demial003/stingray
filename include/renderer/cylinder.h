#ifndef CYLINDER_H
#define CYLINDER_H

#include "renderer/mesh.h"
namespace stingray {
namespace renderer {
class Cylinder : public Mesh {
public:
  Cylinder() : slices(6), height(2.0f) {}
  Cylinder(int slices, float height) : slices(slices), height(height) {}
  virtual void genVertices(float *vboIdx, unsigned int *eboIdx = 0) const;
  virtual int numVertices() const;
  virtual int numElements() const;
  float getHeight() { return height; }

private:
  int slices;
  float height;
  int getVertexNumber(int i);
};
} // namespace renderer
} // namespace stingray
#endif
