#include "renderer/sphere.h"
#include <cmath>

using namespace stingray::renderer;
void Sphere::genVertices() {
  for (auto i = 0; i < slices; i++) {
    float theta = 2.0 * M_PI * float(i % slices) / (float)slices;
    float costheta = cos(theta);
    float sintheta = sin(theta);
    for (auto j = 0; j <= stacks; j++) {
      float phi = M_PI * float(j) / (float)stacks;
      float sinphi = (j < stacks) ? sin(phi) : 0.0f;
      float cosphi = cos(phi);
      float x = sinphi * sintheta;
      float y = cosphi;
      float z = sinphi * costheta;
      float s = float(i % slices) / (float)slices;
      float t = (float)j / (float)stacks;

      vertices.push_back(
          Vertex{glm::vec3(x, y, z), glm::vec3(x, y, z), glm::vec2(s, t)});

      // int vertNumber = getVertexNumber(i, j);
      //
      // float *basePtr = vboIdx + vertNumber * 8;
      // *(basePtr++) = x;
      // *(basePtr++) = y;
      // *(basePtr++) = z;
      //
      // *(basePtr++) = x;
      // *(basePtr++) = y;
      // *(basePtr++) = z;
      //
      // *(basePtr++) = float(i % slices) / (float)slices;
      // *(basePtr++) = (float)j / (float)stacks;
    }
  }

  for (int i = 0; i < slices; i++) {
    indices.push_back(getVertexNumber(i, 0));
    indices.push_back(getVertexNumber(i, 1));
    indices.push_back(getVertexNumber(i + 1, 1));

    // *(eboIdx++) = ;
    // *(eboIdx++) = getVertexNumber(i, 1);
    // *(eboIdx++) = getVertexNumber(i + 1, 1);
  }

  for (int i = 0; i < slices; i++) {
    for (int j = 1; j < stacks - 1; j++) {
      indices.push_back(getVertexNumber(i, j));
      indices.push_back(getVertexNumber(i, j + 1));
      indices.push_back(getVertexNumber(i + 1, j));

      // *(eboIdx++) = getVertexNumber(i, j);
      // *(eboIdx++) = getVertexNumber(i, j + 1);
      // *(eboIdx++) = getVertexNumber(i + 1, j);

      indices.push_back(getVertexNumber(i, j + 1));
      indices.push_back(getVertexNumber(i + 1, j + 1));
      indices.push_back(getVertexNumber(i + 1, j));

      // *(eboIdx++) = getVertexNumber(i, j + 1);
      // *(eboIdx++) = getVertexNumber(i + 1, j + 1);
      // *(eboIdx++) = getVertexNumber(i + 1, j);
    }
  }

  for (int i = 0; i < slices; i++) {
    indices.push_back(getVertexNumber(i, stacks));
    indices.push_back(getVertexNumber(i + 1, stacks - 1));
    indices.push_back(getVertexNumber(i, stacks - 1));

    // *(eboIdx++) = getVertexNumber(i, stacks);
    // *(eboIdx++) = getVertexNumber(i + 1, stacks - 1);
    // *(eboIdx++) = getVertexNumber(i, stacks - 1);
  }
  setupMesh();
}

unsigned int Sphere::getVertexNumber(int i, int j) const {
  return (i % slices) * (stacks + 1) + j;
}
