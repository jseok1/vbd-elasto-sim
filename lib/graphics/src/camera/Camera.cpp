#include "camera/Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(float screenWidth, float screenHeight, float near, float far)
  : transform{},
    u{glm::vec3(0.0f, 0.0f, 1.0f)},
    v{glm::vec3(0.0f, 1.0f, 0.0f)},
    w{glm::vec3(-1.0f, 0.0f, 0.0f)},
    near{near},
    far{far},
    screenWidth{screenWidth},
    screenHeight{screenHeight} {};

glm::vec3 Camera::origin() const {
  return transform.translation;
}

std::tuple<glm::vec3, glm::vec3, glm::vec3> Camera::basis() const {
  return std::make_tuple(u, v, w);
}

void Camera::translateBy(glm::vec3 by) {
  transform.translateBy(by);
};

void Camera::translateTo(glm::vec3 to) {
  transform.translateTo(to);
};

void Camera::rotateBy(glm::vec2 by) {
  by.x = glm::clamp(transform.rotation.x + by.x, -89.0f, 89.0f) - transform.rotation.x;

  transform.rotateBy(glm::vec3(by, 0.0f));

  w = glm::vec3(
    -glm::cos(glm::radians(transform.rotation.y)) * glm::cos(glm::radians(transform.rotation.x)),
    -glm::sin(glm::radians(transform.rotation.x)),
    -glm::sin(glm::radians(transform.rotation.y)) * glm::cos(glm::radians(transform.rotation.x))
  );
  u = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), w);
  v = glm::cross(w, u);

  u = glm::normalize(u);
  v = glm::normalize(v);
  w = glm::normalize(w);
};

void Camera::rotateTo(glm::vec2 to) {
  to.x = glm::clamp(to.x, -89.999f, 89.999f);

  transform.rotateTo(glm::vec3(to, 0.0f));

  w = glm::vec3(
    -glm::cos(glm::radians(transform.rotation.y)) * glm::cos(glm::radians(transform.rotation.x)),
    -glm::sin(glm::radians(transform.rotation.x)),
    -glm::sin(glm::radians(transform.rotation.y)) * glm::cos(glm::radians(transform.rotation.x))
  );
  u = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), w);
  v = glm::cross(w, u);

  u = glm::normalize(u);
  v = glm::normalize(v);
  w = glm::normalize(w);
};

glm::mat4 Camera::view() const {
  glm::mat4 view = glm::lookAt(transform.translation, transform.translation - w, v);
  return view;
}
