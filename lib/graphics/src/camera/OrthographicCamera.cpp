#include "camera/OrthographicCamera.hpp"

#include <glm/gtc/matrix_transform.hpp>

OrthographicCamera::OrthographicCamera(
  float screenWidth,
  float screenHeight,
  float bottom,
  float top,
  float near,
  float far
)
  : Camera{screenWidth, screenHeight, near, far}, bottom{bottom}, top{top} {}

glm::mat4 OrthographicCamera::projection() const {
  glm::mat4 projection = glm::ortho(
    -(top - bottom) * screenWidth / screenHeight / 2.0f,
    (top - bottom) * screenWidth / screenHeight / 2.0f,
    bottom,
    top,
    near,
    far
  );
  return projection;
}
