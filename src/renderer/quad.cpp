#include "renderer/quad.h"

using namespace stingray::renderer;
void Quad::genVertices() {
  for (int i = 0; i < cols; i++) {
    for (int j = 0; j < rows; j++) {
      int x = 2.0f * i / (cols - 1.0f) - 1.0f;
      int z = 2.0f * j / (rows - 1.0f) - 1.0f;

      vertices.push_back(
          Vertex{glm::vec3(x, 0, z), glm::vec3(x, 0, z), glm::vec2(x, z)});
    }
  }
  setupMesh();
}
