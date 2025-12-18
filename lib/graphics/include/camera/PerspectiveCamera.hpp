#pragma once

#include "Camera.hpp"

class PerspectiveCamera : public Camera {
 public:
  PerspectiveCamera(
    float fieldOfViewY,
    float screenWidth,
    float screenHeight,
    float near,
    float far
  );

  glm::mat4 projection() const override;

 private:
  float fieldOfViewY;
};
