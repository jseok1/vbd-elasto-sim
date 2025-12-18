#pragma once

#include <glm/glm.hpp>

class Transform {
 public:
  // food for thought about getters/setters:
  // https://stackoverflow.com/questions/51615363/how-to-write-c-getters-and-setters#answer-51616894
  glm::vec3 translation;
  glm::vec3 rotation;
  glm::vec3 scale;

  Transform();

  void translateBy(glm::vec3 by);
  void translateTo(glm::vec3 to);
  void rotateBy(glm::vec3 by);
  void rotateTo(glm::vec3 to);
  void scaleBy(glm::vec3 by);
  void scaleTo(glm::vec3 to);
  glm::mat4 model() const;
};
