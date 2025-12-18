#include "camera/PerspectiveCamera.hpp"

#include <glm/gtc/matrix_transform.hpp>

PerspectiveCamera::PerspectiveCamera(
  float fieldOfViewY,
  float screenWidth,
  float screenHeight,
  float near,
  float far
)
  : Camera{screenWidth, screenHeight, near, far}, fieldOfViewY{fieldOfViewY} {};

glm::mat4 PerspectiveCamera::projection() const {
  glm::mat4 projection =
    glm::perspective(glm::radians(fieldOfViewY), screenWidth / screenHeight, near, far);
  return projection;
}
