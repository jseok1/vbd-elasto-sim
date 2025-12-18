#include "pipeline/RenderPipeline.hpp"

#include <format>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>

RenderPipeline::RenderPipeline() : programId{0} {}

RenderPipeline::~RenderPipeline() {
  glDeleteProgram(programId);
}

void RenderPipeline::load(
  const std::string& vertShaderPath,
  const std::string& fragShaderPath,
  const std::string& tescShaderPath,
  const std::string& teseShaderPath
) {
  GLuint vertShaderId = compile(vertShaderPath, GL_VERTEX_SHADER);
  GLuint fragShaderId = compile(fragShaderPath, GL_FRAGMENT_SHADER);
  GLuint tescShaderId =
    tescShaderPath.empty() ? 0 : compile(tescShaderPath, GL_TESS_CONTROL_SHADER);
  GLuint teseShaderId =
    teseShaderPath.empty() ? 0 : compile(teseShaderPath, GL_TESS_EVALUATION_SHADER);

  link(vertShaderId, fragShaderId, tescShaderId = tescShaderId, teseShaderId = teseShaderId);
}

GLuint RenderPipeline::compile(const std::string& shaderPath, GLenum shaderType) {
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
      std::format("[at RenderPipeline]: error compiling '{}'\n{}", shaderPath, message)
    );
  };

  return shaderId;
}

void RenderPipeline::link(
  GLuint vertShaderId,
  GLuint fragShaderId,
  GLuint tescShaderId,
  GLuint teseShaderId
) {
  programId = glCreateProgram();

  glAttachShader(programId, vertShaderId);
  glAttachShader(programId, fragShaderId);
  if (tescShaderId) glAttachShader(programId, tescShaderId);
  if (teseShaderId) glAttachShader(programId, teseShaderId);

  glLinkProgram(programId);

  GLint isLinked;
  glGetProgramiv(programId, GL_LINK_STATUS, &isLinked);
  if (!isLinked) {
    GLint size;
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &size);
    std::string message;
    message.resize(size);
    glGetProgramInfoLog(programId, size, nullptr, message.data());
    throw std::runtime_error(std::format("[at RenderPipeline]: error linking\n{}", message));
  };

  glDeleteShader(vertShaderId);
  glDeleteShader(fragShaderId);
  if (tescShaderId) glDeleteShader(tescShaderId);
  if (teseShaderId) glDeleteShader(teseShaderId);
}

void RenderPipeline::use() {
  glUseProgram(programId);
}

void RenderPipeline::uniform(const std::string& uniformId, bool uniform) const {
  GLint uniformLocation = glGetUniformLocation(programId, uniformId.c_str());
  if (uniformLocation == -1) {
    throw std::runtime_error(
      std::format("[at RenderPipeline]: uniform '{}' not declared", uniformId)
    );
  }
  glUniform1i(uniformLocation, uniform ? GL_TRUE : GL_FALSE);
}

void RenderPipeline::uniform(const std::string& uniformId, int uniform) const {
  GLint uniformLocation = glGetUniformLocation(programId, uniformId.c_str());
  if (uniformLocation == -1) {
    throw std::runtime_error(
      std::format("[at RenderPipeline]: uniform '{}' not declared", uniformId)
    );
  }
  glUniform1i(uniformLocation, uniform);
}

void RenderPipeline::uniform(const std::string& uniformId, unsigned int uniform) const {
  GLint uniformLocation = glGetUniformLocation(programId, uniformId.c_str());
  if (uniformLocation == -1) {
    throw std::runtime_error(
      std::format("[at RenderPipeline]: uniform '{}' not declared", uniformId)
    );
  }
  glUniform1ui(uniformLocation, uniform);
}

void RenderPipeline::uniform(const std::string& uniformId, float uniform) const {
  GLint uniformLocation = glGetUniformLocation(programId, uniformId.c_str());
  if (uniformLocation == -1) {
    throw std::runtime_error(
      std::format("[at RenderPipeline]: uniform '{}' not declared", uniformId)
    );
  }
  glUniform1f(uniformLocation, uniform);
}

void RenderPipeline::uniform(const std::string& uniformId, glm::vec2 uniform) const {
  GLint uniformLocation = glGetUniformLocation(programId, uniformId.c_str());
  if (uniformLocation == -1) {
    throw std::runtime_error(
      std::format("[at RenderPipeline]: uniform '{}' not declared", uniformId)
    );
  }
  glUniform2fv(uniformLocation, 1, glm::value_ptr(uniform));
}

void RenderPipeline::uniform(const std::string& uniformId, glm::vec3 uniform) const {
  GLint uniformLocation = glGetUniformLocation(programId, uniformId.c_str());
  if (uniformLocation == -1) {
    throw std::runtime_error(
      std::format("[at RenderPipeline]: uniform '{}' not declared", uniformId)
    );
  }
  glUniform3fv(uniformLocation, 1, glm::value_ptr(uniform));
}

void RenderPipeline::uniform(const std::string& uniformId, glm::vec4 uniform) const {
  GLint uniformLocation = glGetUniformLocation(programId, uniformId.c_str());
  if (uniformLocation == -1) {
    throw std::runtime_error(
      std::format("[at RenderPipeline]: uniform '{}' not declared", uniformId)
    );
  }
  glUniform4fv(uniformLocation, 1, glm::value_ptr(uniform));
}

void RenderPipeline::uniform(const std::string& uniformId, glm::mat4 uniform) const {
  GLint uniformLocation = glGetUniformLocation(programId, uniformId.c_str());
  if (uniformLocation == -1) {
    throw std::runtime_error(
      std::format("[at RenderPipeline]: uniform '{}' not declared", uniformId)
    );
  }
  glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(uniform));
}
