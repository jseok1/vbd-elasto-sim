#pragma once

#include <assimp/scene.h>
#include <glad/glad.h>

#include <string>

enum class TextureType {
  __DIFFUSE,
  __SPECULAR,
  __AMBIENT
};

class Texture {
 public:
  Texture(const std::string& texturePath, TextureType textureType);
  Texture(const aiTexture* texture, TextureType textureType);
  ~Texture();

  void use(GLenum textureUnit);

  GLuint textureId;
  TextureType textureType;

 private:
};
