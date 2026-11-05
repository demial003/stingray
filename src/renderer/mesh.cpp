#include "renderer/mesh.h"
#include <glad/glad.h>
#include <vector>

using namespace stingray::renderer;

void Mesh::setupMesh() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0],
               GL_STATIC_DRAW);

  if (!indices.empty()) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 &indices[0], GL_STATIC_DRAW);
  }

  glVertexAttribPointer(vertPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)0);
  glEnableVertexAttribArray(vertPosLoc);

  glVertexAttribPointer(vertNormLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, Normal));
  glEnableVertexAttribArray(vertNormLoc);

  glVertexAttribPointer(vertTexLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, TexCoords));
  glEnableVertexAttribArray(vertTexLoc);

  glBindVertexArray(0);
}

void Mesh::Draw(Shader &shader) {
  unsigned int diffuseNr = 1;
  unsigned int specularNr = 1;
  if (!textures.empty()) {
    for (unsigned int i = 0; i < textures.size(); i++) {
      glActiveTexture(GL_TEXTURE0 + i);
      std::string number;
      std::string name = textures[i].type;
      if (name == "texture_diffuse")
        number = std::to_string(diffuseNr++);
      else if (name == "texture_specular")
        number = std::to_string(specularNr++);

      shader.setInt(("material." + name + number).c_str(), i);
      glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);
  }

  glBindVertexArray(VAO);
  if (indices.empty()) {
    glDrawArrays(drawMode, 0, vertices.size());
  } else {
    glDrawElements(drawMode, indices.size(), GL_UNSIGNED_INT, 0);
  }
  glBindVertexArray(0);
}

void Mesh::genSphere(int slices, int stacks) {
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
    }
  }
  auto getIdx = [slices, stacks](int i, int j) {
    return (i % slices) * (stacks + 1) + j;
  };

  for (int i = 0; i < slices; i++) {
    indices.push_back(getIdx(i, 0));
    indices.push_back(getIdx(i, 1));
    indices.push_back(getIdx(i + 1, 1));
  }

  for (int i = 0; i < slices; i++) {
    for (int j = 1; j < stacks - 1; j++) {

      indices.push_back(getIdx(i, j + 1));
      indices.push_back(getIdx(i + 1, j + 1));
      indices.push_back(getIdx(i + 1, j));
    }
  }

  for (int i = 0; i < slices; i++) {
    indices.push_back(getIdx(i, stacks));
    indices.push_back(getIdx(i + 1, stacks - 1));
    indices.push_back(getIdx(i, stacks - 1));
  }
  setupMesh();
}

void Mesh::genCylinder(int slices, float height) {
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
  }
  setupMesh();
}

void Mesh::genQuad(int rows, int cols) {
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

  auto getIdx = [rows](int i, int j) { return i * rows + j; };

  for (int i = 0; i < cols - 1; i++) {
    for (int j = 0; j < rows - 1; j++) {
      unsigned int a = getIdx(i, j);
      unsigned int b = getIdx(i, j + 1);
      unsigned int c = getIdx(i + 1, j);
      unsigned int d = getIdx(i + 1, j + 1);

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

void Mesh::genCube() {
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

Mesh::~Mesh() {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
}
