#include "pipeline/ComputePipeline.hpp"

#include <format>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>

ComputePipeline::ComputePipeline() {}

ComputePipeline::~ComputePipeline() {
  glDeleteProgram(programId);
}

void ComputePipeline::load(const std::string& compShaderPath) {
  GLuint compShaderId = compile(compShaderPath, GL_COMPUTE_SHADER);

  link(compShaderId);
}

GLuint ComputePipeline::compile(const std::string& shaderPath, GLenum shaderType) {
  std::ifstream file;
  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  file.open(shaderPath);
  std::string source{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
  file.close();

  const char* sources[1] = {source.c_str()};

  GLuint shaderId = glCreateShader(shaderType);
  glShaderSource(shaderId, 1, sources, nullptr);
  glCompileShader(shaderId);

  GLint isCompiled;
  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &isCompiled);
  if (!isCompiled) {
    GLint size;
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &size);
    std::string message;
    message.resize(size);
    glGetShaderInfoLog(shaderId, size, nullptr, message.data());
    throw std::runtime_error(
      std::format("[at ComputePipeline]: error compiling '{}'\n{}", shaderPath, message)
    );
  };

  return shaderId;
}

void ComputePipeline::link(GLuint compShaderId) {
  programId = glCreateProgram();

  glAttachShader(programId, compShaderId);

  glLinkProgram(programId);

  GLint isLinked;
  glGetProgramiv(programId, GL_LINK_STATUS, &isLinked);
  if (!isLinked) {
    GLint size;
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &size);
    std::string message;
    message.resize(size);
    glGetProgramInfoLog(programId, size, nullptr, message.data());
    throw std::runtime_error(std::format("[at ComputePipeline]: error linking\n{}", message));
  };

  glDeleteShader(compShaderId);
}

void ComputePipeline::use() {
  glUseProgram(programId);
}

void ComputePipeline::uniform(const std::string& uniformId, bool uniform) const {
  GLint uniformLocation = glGetUniformLocation(programId, uniformId.c_str());
  if (uniformLocation == -1) {
    throw std::runtime_error(
      std::format("[at ComputePipeline]: uniform '{}' not declared", uniformId)
    );
  }
  glUniform1i(uniformLocation, uniform ? GL_TRUE : GL_FALSE);
}

void ComputePipeline::uniform(const std::string& uniformId, int uniform) const {
  GLint uniformLocation = glGetUniformLocation(programId, uniformId.c_str());
  if (uniformLocation == -1) {
    throw std::runtime_error(
      std::format("[at ComputePipeline]: uniform '{}' not declared", uniformId)
    );
  }
  glUniform1i(uniformLocation, uniform);
}

void ComputePipeline::uniform(const std::string& uniformId, unsigned int uniform) const {
  GLint uniformLocation = glGetUniformLocation(programId, uniformId.c_str());
  if (uniformLocation == -1) {
    throw std::runtime_error(
      std::format("[at ComputePipeline]: uniform '{}' not declared", uniformId)
    );
  }
  glUniform1ui(uniformLocation, uniform);
}

void ComputePipeline::uniform(const std::string& uniformId, float uniform) const {
  GLint uniformLocation = glGetUniformLocation(programId, uniformId.c_str());
  if (uniformLocation == -1) {
    throw std::runtime_error(
      std::format("[at ComputePipeline]: uniform '{}' not declared", uniformId)
    );
  }
  glUniform1f(uniformLocation, uniform);
}

void ComputePipeline::uniform(const std::string& uniformId, glm::vec2 uniform) const {
  GLint uniformLocation = glGetUniformLocation(programId, uniformId.c_str());
  if (uniformLocation == -1) {
    throw std::runtime_error(
      std::format("[at ComputePipeline]: uniform '{}' not declared", uniformId)
    );
  }
  glUniform2fv(uniformLocation, 1, glm::value_ptr(uniform));
}

void ComputePipeline::uniform(const std::string& uniformId, glm::vec3 uniform) const {
  GLint uniformLocation = glGetUniformLocation(programId, uniformId.c_str());
  if (uniformLocation == -1) {
    throw std::runtime_error(
      std::format("[at ComputePipeline]: uniform '{}' not declared", uniformId)
    );
  }
  glUniform3fv(uniformLocation, 1, glm::value_ptr(uniform));
}

void ComputePipeline::uniform(const std::string& uniformId, glm::vec4 uniform) const {
  GLint uniformLocation = glGetUniformLocation(programId, uniformId.c_str());
  if (uniformLocation == -1) {
    throw std::runtime_error(
      std::format("[at ComputePipeline]: uniform '{}' not declared", uniformId)
    );
  }
  glUniform4fv(uniformLocation, 1, glm::value_ptr(uniform));
}

void ComputePipeline::uniform(const std::string& uniformId, glm::mat4 uniform) const {
  GLint uniformLocation = glGetUniformLocation(programId, uniformId.c_str());
  if (uniformLocation == -1) {
    throw std::runtime_error(
      std::format("[at ComputePipeline]: uniform '{}' not declared", uniformId)
    );
  }
  glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(uniform));
}
