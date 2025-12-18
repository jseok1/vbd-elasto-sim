#pragma once

#include "Camera.hpp"

class OrthographicCamera : public Camera {
 public:
  OrthographicCamera(
    float screenWidth,
    float screenHeight,
    float bottom,
    float top,
    float near,
    float far
  );

  glm::mat4 projection() const override;

 private:
  float bottom;
  float top;
};
