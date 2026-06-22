#ifndef MESH_H
#define MESH_H
#include <glad/glad.h>

namespace utils {
class Mesh {

public:
  Mesh() {}
  ~Mesh();
  virtual void initializeAtrributeLocations(unsigned int posLoc);
  virtual void RenderVBO(GLuint drawMode, unsigned int first,
                         unsigned int numVerts);
  virtual void RenderEBO(GLuint drawMode, unsigned int numElems);
  virtual void genVertices(float *vboIdx, unsigned int *eboIdx) = 0;
  virtual void CalcVboAndEbo();
  virtual int numVertices() { return -1; };
  virtual int numElements() { return -1; };

private:
  unsigned int VBO;
  unsigned int VAO;
  unsigned int EBO;

  unsigned int vertPosLoc = 0;
  unsigned int vertColorLoc = 1;
};
} // namespace utils

#endif
