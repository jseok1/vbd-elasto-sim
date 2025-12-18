#pragma once

#include <glad/glad.h>

class Buffer {
 public:
  Buffer() {
    glGenBuffers(1, &bufferId);
  };
  virtual ~Buffer() = default;

  virtual void use() const = 0;

 protected:
  GLuint bufferId;
};

// FBO, I(E)BO, RBO, SSBO, VBO

/*
 * VAO: glVertexAttribFormat, glVertexAttribBinding, glBindVertexBuffer (OpenGL 4.3+)
 */

//  - indirect command buffer
//
// SSBO (bound to GL_DRAW_INDIRECT_BUFFER) for draw parameters (num vertices, base vertex, base
// index)
// SSBO (generic) for uniforms
