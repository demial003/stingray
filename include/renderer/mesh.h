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
  std::string path;
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

  Mesh(Mesh &&other) noexcept
      : vertices(std::move(other.vertices)), indices(std::move(other.indices)),
        textures(std::move(other.textures)), VAO(other.VAO), VBO(other.VBO),
        EBO(other.EBO) {
    other.VAO = other.VBO = other.EBO = 0;
  }

  Mesh &operator=(Mesh &&other) noexcept {
    if (this != &other) {
      glDeleteVertexArrays(1, &VAO);
      glDeleteBuffers(1, &VBO);
      glDeleteBuffers(1, &EBO);
      vertices = std::move(other.vertices);
      indices = std::move(other.indices);
      textures = std::move(other.textures);
      VAO = other.VAO;
      VBO = other.VBO;
      EBO = other.EBO;
      other.VAO = other.VBO = other.EBO = 0;
    }
    return *this;
  }
  Mesh(const Mesh &) = delete;
  Mesh &operator=(const Mesh &) = delete;

  virtual void genVertices();

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
