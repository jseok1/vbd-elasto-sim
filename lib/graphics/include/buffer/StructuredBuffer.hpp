#pragma once

#include <vector>

#include "buffer/Buffer.hpp"

template <typename T>
class StructuredBuffer : public Buffer {
 public:
  StructuredBuffer(const std::vector<T>& data, bool isMutable, unsigned int binding) : Buffer{} {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferId);
    glBufferData(
      GL_SHADER_STORAGE_BUFFER,
      sizeof(T) * data.size(),
      data.data(),
      isMutable ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW
    );
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, bufferId);
  }

  StructuredBuffer(unsigned int size, bool isMutable, unsigned int binding) : Buffer{} {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferId);
    glBufferData(
      GL_SHADER_STORAGE_BUFFER, size, nullptr, isMutable ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW
    );
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, bufferId);
  }

  void use() const override {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferId);
  };
};
