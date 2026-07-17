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

  virtual void initializeAtrributeLocations(unsigned int posLoc,
                                            unsigned int normLoc,
                                            unsigned int texLoc);
  virtual void RenderVBO(GLuint drawMode, unsigned int first,
                         unsigned int numVerts);
  virtual void RenderEBO(GLuint drawMode, unsigned int numElems) const;
  virtual void genVertices(float *vboIdx, unsigned int *eboIdx) const = 0;
  virtual void CalcVboAndEbo() const;
  virtual int numVertices() const = 0;
  virtual int numElements() const = 0;
  virtual int getVAO() { return VAO; };

private:
  unsigned int VBO = 0;
  unsigned int VAO = 0;
  unsigned int EBO = 0;

  unsigned int vertPosLoc = 0;
  unsigned int vertNormLoc = 1;
  unsigned int vertTexLoc = 2;
};
} // namespace utils

#endif
