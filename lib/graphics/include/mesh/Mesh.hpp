#include <glad/glad.h>

#include <glm/glm.hpp>
#include <vector>

#include "buffer/TriangleBuffer.hpp"
#include "buffer/VertexBuffer.hpp"
#include "vertex/Vertex.hpp"

// one VAO per shader/material, and since 1 material per mesh, 1 VAO per mesh
//
// Eventually, the goal with batching is to have 1 DRAW CALL per shader

class Mesh {
 public:
  Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &faces)
    : vertexBuffer{vertices, false}, faceBuffer{faces, false} {
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    vertexBuffer.use();
    faceBuffer.use();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
      0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<void *>(offsetof(Vertex, xyz))
    );
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
      1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<void *>(offsetof(Vertex, normal))
    );
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
      2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<void *>(offsetof(Vertex, uv))
    );

    glBindVertexArray(0);
  }

  void draw() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, faces.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }

  void draw(unsigned int instances) {
    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, faces.size(), GL_UNSIGNED_INT, 0, instances);
    glBindVertexArray(0);
  }

 private:
  unsigned int VAO;
  VertexBuffer<Vertex> vertexBuffer;
  TriangleBuffer faceBuffer;
};

// shader has once VAO but different shaders may share a VAO
