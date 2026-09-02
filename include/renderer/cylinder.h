#pragma once

#include "renderer/mesh.h"
namespace stingray {
namespace renderer {
class Cylinder : public Mesh {
public:
  Cylinder() : slices(6), height(2.0f) { drawMode = GL_TRIANGLE_STRIP; }
  Cylinder(int slices, float height) : slices(slices), height(height) {
    drawMode = GL_TRIANGLE_STRIP;
  }

  virtual void genVertices() override;

  float getHeight() { return height; }

private:
  int slices;
  float height;
  int getVertexNumber(int i);
};
} // namespace renderer
} // namespace stingray
