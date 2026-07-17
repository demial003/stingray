#include "utils/mesh.h"
#include <glad/glad.h>
#include <iostream>

using namespace utils;

void Mesh::initializeAtrributeLocations(unsigned int posLoc,
                                        unsigned int normLoc,
                                        unsigned int texLoc) {

  if (VAO == 0) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
  }

  int numVerts = numVertices();
  int numElems = numElements();
  vertPosLoc = posLoc;
  vertNormLoc = normLoc;
  vertTexLoc = texLoc;

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, numVerts * sizeof(float), 0, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, numElems * sizeof(int), 0,
               GL_STATIC_DRAW);

  glVertexAttribPointer(vertPosLoc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)0);
  glEnableVertexAttribArray(vertPosLoc);

  glVertexAttribPointer(vertNormLoc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(vertNormLoc);
  glVertexAttribPointer(vertTexLoc, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(vertTexLoc);

  CalcVboAndEbo();
}

void Mesh::CalcVboAndEbo() const {

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

  float *vboBuffer = (float *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
  unsigned int *eboBuffer =
      (unsigned int *)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
  genVertices(vboBuffer, eboBuffer);
  glUnmapBuffer(GL_ARRAY_BUFFER);
  glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::RenderVBO(GLuint drawMode, unsigned int first,
                     unsigned int numVerts) {
  glBindVertexArray(VAO);
  glDrawArrays(drawMode, first, numVerts);
}

void Mesh::RenderEBO(GLuint drawMode, unsigned int numElems) const {
  glBindVertexArray(VAO);
  glDrawElements(drawMode, numElems, GL_UNSIGNED_INT, 0);
}

Mesh::~Mesh() {

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
}
