#include "texture/Texture.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// TODO: delete copy functionality

// texture unit {
//   GL_TEXTURE_2D
//   GL_TEXTURE_3D
//   ...
// }
Texture::Texture(const std::string &texturePath, TextureType textureType)
  : textureType{textureType} {
  glGenTextures(1, &textureId);
  glBindTexture(GL_TEXTURE_2D, textureId);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int width, height, channels;
  unsigned char *data = stbi_load(
    texturePath.c_str(), &width, &height, &channels, 0
  );  // 4 req channels for RGBA always?
  if (!data) {
    const char *msg = stbi_failure_reason();
    throw std::runtime_error("ERROR::TEXTURE::" + std::string(msg));
  }

  GLenum format;
  switch (channels) {
    case 1:
      format = GL_RED;
      break;
    case 3:
      format = GL_RGB;
      break;
    case 4:
      format = GL_RGBA;
      break;
    default:
      break;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, 0);

  stbi_image_free(data);
}

// for embedded textures loaded from assimp
Texture::Texture(const aiTexture *texture, TextureType textureType) : textureType{textureType} {
  glGenTextures(1, &textureId);
  glBindTexture(GL_TEXTURE_2D, textureId);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  if (!texture->mHeight) {
    unsigned char *buffer = reinterpret_cast<unsigned char *>(texture->pcData);

    int width, height, channels;
    unsigned char *data =
      stbi_load_from_memory(buffer, texture->mWidth, &width, &height, &channels, 4);
    if (!data) {
      const char *msg = stbi_failure_reason();
      throw std::runtime_error("ERROR::TEXTURE::" + std::string(msg));
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);
  } else {
    std::cout << "UNCOMPRESSED TEXTURE NOT IMPLEMENTED" << std::endl;
  }
}

Texture::~Texture() {
  // glDeleteTextures(1, &textureId);
}

void Texture::use(GLenum textureUnit) {
  glActiveTexture(textureUnit);
  glBindTexture(GL_TEXTURE_2D, textureId);
}
