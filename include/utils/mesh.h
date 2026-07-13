#ifndef MESH_H
#define MESH_H
#include <glad/glad.h>

namespace utils {
class Mesh {

public:
  Mesh() {}
  ~Mesh();

  Mesh(const Mesh &) = delete;
  Mesh &operator=(const Mesh &) = delete;
  Mesh(Mesh &&) = delete;
  Mesh &operator=(Mesh &&) = delete;

  virtual void initializeAtrributeLocations(unsigned int posLoc);
  virtual void RenderVBO(GLuint drawMode, unsigned int first,
                         unsigned int numVerts);
  virtual void RenderEBO(GLuint drawMode, unsigned int numElems);
  virtual void genVertices(float *vboIdx, unsigned int *eboIdx) = 0;
  virtual void CalcVboAndEbo();
  virtual int numVertices() { return -1; };
  virtual int numElements() { return -1; };
  virtual int getVAO() { return VAO; };

private:
  unsigned int VBO = 0;
  unsigned int VAO = 0;
  unsigned int EBO = 0;

  unsigned int vertPosLoc = 0;
  unsigned int vertColorLoc = 1;
};
} // namespace utils

#endif
