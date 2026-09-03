#pragma once

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <renderer/mesh.h>
#include <renderer/shader.h>

namespace stingray {
namespace renderer {
class Model {
public:
  Model(std::string const &path) { loadModel(path); }
  void Draw(Shader &shader);

private:
  std::vector<Mesh> meshes;
  std::vector<Texture> textures_loaded;
  std::string directory;

  void loadModel(std::string path);
  void processNode(aiNode *node, const aiScene *scene);
  Mesh processMesh(aiMesh *mesh, const aiScene *scene);
  std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                            std::string typeName);
  unsigned int TextureFromFile(const char *path, const std::string &dir,
                               bool gamma = false);
};
} // namespace renderer
} // namespace stingray
