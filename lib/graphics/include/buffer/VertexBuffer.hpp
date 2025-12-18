#pragma once

#include <vector>

#include "buffer/Buffer.hpp"

template <typename T>
class VertexBuffer : public Buffer {
 public:
  VertexBuffer(const std::vector<T> &data, bool isMutable) : Buffer{} {
    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
    glBufferData(
      GL_ARRAY_BUFFER,
      sizeof(T) * data.size(),
      data.data(),
      isMutable ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW
    );
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  void use() const override {
    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
  };
};
