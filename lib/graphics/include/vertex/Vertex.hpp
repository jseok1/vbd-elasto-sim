#pragma once

#include <glm/glm.hpp>

struct Vertex {
  glm::vec3 xyz;
  glm::vec3 normal;
  glm::vec2 uv;
  // possibly uvw for 3D textures
  // possibly rgba for fixed colors
};

// maybe different Vertex types could be done with some preprocessor evil
