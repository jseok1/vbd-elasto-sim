#pragma once

#include <vector>

#include "buffer/Buffer.hpp"

class TriangleBuffer : public Buffer {
 public:
  TriangleBuffer(const std::vector<unsigned int> &data, bool isMutable) : Buffer{} {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId);
    glBufferData(
      GL_ELEMENT_ARRAY_BUFFER,
      sizeof(unsigned int) * data.size(),
      data.data(),
      isMutable ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW
    );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }

  void use() const override {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId);
  };
};
