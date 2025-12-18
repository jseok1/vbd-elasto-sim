#pragma once

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/types.h>

#include <format>
#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "glad/glad.h"
#include "mesh/Mesh.hpp"
#include "texture/Texture.hpp"
#include "util/Util.hpp"

// ultimate meme: deferred pipeline + indirect rendering

enum class NormalType {
  __FACE_NORMAL,
  __VERT_NORMAL
};

class Model {
 public:
  std::vector<Mesh> meshes;
  std::unordered_map<std::string, Texture> textures;

  Model(const std::string &path, NormalType normalType = NormalType::__VERT_NORMAL) {
    aiPostProcessSteps normalFlag;
    switch (normalType) {
      case NormalType::__VERT_NORMAL: {
        normalFlag = aiProcess_GenSmoothNormals;
        break;
      }
      case NormalType::__FACE_NORMAL: {
        normalFlag = aiProcess_GenNormals;
        break;
      }
      default:
        break;
    }
  }

  void load(const std::string &path) {
    const aiScene *scene = aiImportFile(
      path.c_str(),
      aiProcess_Triangulate | normalFlag | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs
    );

    if (!scene || !scene->mRootNode) {
      const char *message = aiGetErrorString();
      throw std::runtime_error("[in Model]: " + std::string(message));
    }
    if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
      throw std::runtime_error("[in Model]: no meshes");
    }

    // embedded textures for .glb, .fbx, etc.
    for (int i = 0; i < scene->mNumTextures; i++) {
      // FIX: this should not only be specular
      textures.emplace(
        std::format("*{}", i), Texture(scene->mTextures[i], TextureType::__SPECULAR)
      );
    }

    load(scene, scene->mRootNode);

    aiReleaseImport(scene);
  }

  void draw() {
    for (auto &mesh : meshes) {
      // FIX: support multiple textures
      glBindTexture(GL_TEXTURE_2D, textures.at(mesh.texturePaths[0]).textureId);

      mesh.draw();
    }
  }

  void draw(int nInstances) {
    for (auto &mesh : meshes) {
      mesh.draw(nInstances);
    }
  }

 private:
  void load(const aiScene *scene, aiNode *node) {
    for (int i = 0; i < node->mNumMeshes; i++) {
      aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];

      std::vector<Vertex> vertices;
      for (int j = 0; j < mesh->mNumVertices; j++) {
        glm::vec3 xyz = glm::vec3(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z);

        glm::vec3 normal = glm::vec3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z);

        glm::vec2 uv = mesh->HasTextureCoords(0)
                         ? glm::vec2(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y)
                         : glm::vec2(0.0f);

        vertices.emplace_back(xyz, normal, uv);
      }

      std::vector<unsigned int> indices;
      for (int j = 0; j < mesh->mNumFaces; j++) {
        for (int k = 0; k < mesh->mFaces[j].mNumIndices; k++) {
          indices.push_back(mesh->mFaces[j].mIndices[k]);
        }
      }

      aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

      std::vector<std::string> texturePaths;
      std::vector<std::string> diffuseTexturePaths =
        loadMaterialTextures(material, TextureType::__DIFFUSE);
      texturePaths.insert(
        texturePaths.end(), diffuseTexturePaths.begin(), diffuseTexturePaths.end()
      );
      std::vector<std::string> specularTexturePaths =
        loadMaterialTextures(material, TextureType::__SPECULAR);
      texturePaths.insert(
        texturePaths.end(), specularTexturePaths.begin(), specularTexturePaths.end()
      );
      std::vector<std::string> ambientTexturePaths =
        loadMaterialTextures(material, TextureType::__AMBIENT);
      texturePaths.insert(
        texturePaths.end(), ambientTexturePaths.begin(), ambientTexturePaths.end()
      );

      meshes.emplace_back(vertices, indices, texturePaths);
    }

    for (int i = 0; i < node->mNumChildren; i++) {
      load(scene, node->mChildren[i]);
    }
  }

  std::vector<std::string> loadMaterialTextures(aiMaterial *material, TextureType textureType) {
    aiTextureType type;
    switch (textureType) {
      case TextureType::__DIFFUSE:
        // print("diffuse", aiGetMaterialTextureCount(material, type));
        type = aiTextureType_DIFFUSE;
        break;
      case TextureType::__SPECULAR:
        // print("specular", aiGetMaterialTextureCount(material, type));
        type = aiTextureType_SPECULAR;
        break;
      case TextureType::__AMBIENT:
        // print("ambient", aiGetMaterialTextureCount(material, type));
        type = aiTextureType_AMBIENT;
        break;
      default:
        std::cout << "UNKNOWN TextureType" << std::endl;
        break;
    }

    std::vector<std::string> texturePaths;
    for (int i = 0; i < aiGetMaterialTextureCount(material, type); i++) {
      aiString str;
      aiGetMaterialTexture(material, type, i, &str);

      const char *path = str.C_Str();

      if (!textures.contains(path)) {
        textures.emplace(path, Texture(path, textureType));
      }

      texturePaths.push_back(path);
    }

    return texturePaths;
  }
};
