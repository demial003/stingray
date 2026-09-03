#pragma once

#include "renderer/mesh.h"
namespace stingray {
namespace renderer {
class Sphere : public Mesh {
public:
  Sphere() : slices(6), stacks(6) {}
  Sphere(int slices, int stacks) : slices(slices), stacks(stacks) {}

  virtual void genVertices() override;

private:
  int slices;
  int stacks;
  unsigned int getVertexNumber(int i, int j) const;
};
} // namespace renderer
} // namespace stingray
