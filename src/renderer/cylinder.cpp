#include <cmath>
#include <renderer/cylinder.h>

using namespace stingray::renderer;

void Cylinder::genVertices() {
  for (auto j = 0; j <= slices; j++) {
    float phi = (float)(j % slices) / float(slices) * M_PI * 2.0f;
    float x = cos(phi);
    float z = sin(phi);

    float s = (float)j / float(slices);

    vertices.push_back(Vertex{glm::vec3(x, -(float)height, z),
                              glm::vec3(x, 0, z),
                              glm::vec2(s, -(float)height)});
    vertices.push_back(
        Vertex{glm::vec3(x, 0, z), glm::vec3(x, 0, z), glm::vec2(s, 0.0f)});
    // *(basePtr++) = x;
    // *(basePtr++) = -(float)height;
    // *(basePtr++) = z;
    //
    // *(basePtr++) = x;
    // *(basePtr++) = 0;
    // *(basePtr++) = z;
    //
    // *(basePtr++) = (float)j / float(slices);
    // *(basePtr++) = -height;
    //
    // *(basePtr++) = x;
    // *(basePtr++) = 0;
    // *(basePtr++) = z;
    //
    // *(basePtr++) = x;
    // *(basePtr++) = 0;
    // *(basePtr++) = z;
    //
    // *(basePtr++) = (float)j / float(slices);
    // *(basePtr++) = 0;
  }
  setupMesh();
}
