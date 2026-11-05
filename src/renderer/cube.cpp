#include <renderer/cube.h>

using namespace stingray::renderer;
void Cube::genVertices() {
  const glm::vec3 p[8] = {
      {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
      {-1, -1, 1},  {1, -1, 1},  {1, 1, 1},  {-1, 1, 1},
  };
  for (const glm::vec3 &pos : p)
    vertices.push_back(Vertex{pos, pos, glm::vec2(0.f)}); // normal/uv unused

  indices.insert(indices.end(),
                 {
                     6, 5, 4, 7, 6, 4, 3, 0, 1, 2, 3, 1, 2, 1, 5, 6, 2, 5,
                     7, 4, 0, 3, 7, 0, 6, 7, 3, 2, 6, 3, 5, 1, 0, 4, 5, 0,
                 });
  setupMesh();
}
