#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <string>

class ComputePipeline {
 public:
  ComputePipeline();
  ComputePipeline(const ComputePipeline&) = delete;
  ComputePipeline& operator=(const ComputePipeline&) = delete;
  ~ComputePipeline();

  void load(const std::string& compShaderPath);
  void use();
  void uniform(const std::string& uniformId, bool uniform) const;
  void uniform(const std::string& uniformId, int uniform) const;
  void uniform(const std::string& uniformId, unsigned int uniform) const;
  void uniform(const std::string& uniformId, float uniform) const;
  void uniform(const std::string& uniformId, glm::vec2 uniform) const;
  void uniform(const std::string& uniformId, glm::vec3 uniform) const;
  void uniform(const std::string& uniformId, glm::vec4 uniform) const;
  void uniform(const std::string& uniformId, glm::mat4 uniform) const;

 private:
  GLuint programId;

  GLuint compile(const std::string& shaderPath, GLenum shaderType);
  void link(GLuint compShaderId);
};
