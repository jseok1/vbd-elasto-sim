#pragma once

#include <glm/glm.hpp>
#include <tuple>

#include "transform/Transform.hpp"

class Camera {
 public:
  Camera(float screenWidth, float screenHeight, float near, float far);
  virtual ~Camera() = default;

  glm::vec3 origin() const;
  std::tuple<glm::vec3, glm::vec3, glm::vec3> basis() const;
  void translateBy(glm::vec3 by);
  void translateTo(glm::vec3 to);
  void rotateBy(glm::vec2 by);
  void rotateTo(glm::vec2 to);
  glm::mat4 view() const;
  virtual glm::mat4 projection() const = 0;

 protected:
  Transform transform;
  glm::vec3 u, v, w;
  float screenWidth;
  float screenHeight;
  float near;
  float far;
};
