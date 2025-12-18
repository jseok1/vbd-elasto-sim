#include "Transform.hpp"

#include <glm/gtc/matrix_transform.hpp>

Transform::Transform()
  : translation{glm::vec3(0.0f)}, rotation{glm::vec3(0.0f)}, scale{glm::vec3(1.0f)} {};

void Transform::translateBy(glm::vec3 by) {
  translation += by;
};

void Transform::translateTo(glm::vec3 to) {
  translation = to;
};

void Transform::rotateBy(glm::vec3 by) {
  rotation += by;
  rotation = glm::mod(rotation + 180.0f, 360.0f) - 180.0f;
};

void Transform::rotateTo(glm::vec3 to) {
  rotation = glm::mod(to + 180.0f, 360.0f) - 180.0f;
};

void Transform::scaleBy(glm::vec3 by) {
  scale *= by;
};

void Transform::scaleTo(glm::vec3 to) {
  scale *= to;
};

glm::mat4 Transform::model() const {
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, translation);
  model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
  model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
  model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
  model = glm::scale(model, scale);
  return model;
}
