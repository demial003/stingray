#pragma once

#include "renderer/mesh.h"
namespace stingray {
namespace renderer {
class Quad : public Mesh {
public:
  Quad() : rows(6), cols(6) {}
  Quad(int rows, int cols) : rows(rows), cols(cols) {}

  virtual void genVertices() override;

  int rows;
  int cols;
  unsigned int getVertexNumber(int i, int j) const;
};
} // namespace renderer
} // namespace stingray
