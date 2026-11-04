#include "renderer/quad.h"

using namespace stingray::renderer;
void Quad::genVertices() {
  for (int i = 0; i < cols; i++) {
    for (int j = 0; j < rows; j++) {
      float x = 2.0f * i / (cols - 1.0f) - 1.0f;
      float z = 2.0f * j / (rows - 1.0f) - 1.0f;
      float u = j / (rows - 1.0f);
      float v = i / (cols - 1.0f);

      vertices.push_back(
          Vertex{glm::vec3(x, 0, z), glm::vec3(0, 1, 0), glm::vec2(u, v)});
    }
  }

  for (int i = 0; i < cols - 1; i++) {
    for (int j = 0; j < rows - 1; j++) {
      unsigned int a = getVertexNumber(i, j);
      unsigned int b = getVertexNumber(i, j + 1);
      unsigned int c = getVertexNumber(i + 1, j);
      unsigned int d = getVertexNumber(i + 1, j + 1);

      indices.push_back(a);
      indices.push_back(b);
      indices.push_back(c);

      indices.push_back(c);
      indices.push_back(b);
      indices.push_back(d);
    }
  }
  setupMesh();
}

unsigned int Quad::getVertexNumber(int i, int j) const { return i * rows + j; }
