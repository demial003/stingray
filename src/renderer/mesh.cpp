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

  if (indices.size() != 0) {
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

  glBindVertexArray(VAO);
  if (indices.empty()) {
    glDrawArrays(drawMode, 0, vertices.size());
  } else {
    glDrawElements(drawMode, indices.size(), GL_UNSIGNED_INT, 0);
  }
  glBindVertexArray(0);
}

Mesh::~Mesh() {

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
}
