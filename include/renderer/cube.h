#pragma once

#include "renderer/mesh.h"
namespace stingray {
namespace renderer {
class Cube : public Mesh {
public:
  virtual void genVertices() override;
};
} // namespace renderer
} // namespace stingray
