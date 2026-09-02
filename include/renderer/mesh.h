#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <renderer/shader.h>

namespace stingray {
namespace renderer {
struct Vertex {
  glm::vec3 Position;
  glm::vec3 Normal;
  glm::vec2 TexCoords;
};

struct Texture {
  unsigned int id;
  std::string type;
};
class Mesh {

public:
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;

  Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
       std::vector<Texture> textures) {
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh();
  }
  Mesh() = default;
  ~Mesh();

  Mesh(const Mesh &) = delete;
  Mesh &operator=(const Mesh &) = delete;
  Mesh(Mesh &&) = delete;
  Mesh &operator=(Mesh &&) = delete;

  virtual void genVertices() = 0;

  virtual int getVAO() { return VAO; };

  virtual void Draw(Shader &shader);

protected:
  void setupMesh();

  unsigned int VBO = 0;
  unsigned int VAO = 0;
  unsigned int EBO = 0;

  unsigned int vertPosLoc = 0;
  unsigned int vertNormLoc = 1;
  unsigned int vertTexLoc = 2;

  GLenum drawMode = GL_TRIANGLES;
};
} // namespace renderer
} // namespace stingray
