#pragma once

#include <glm/glm.hpp>
#include <iostream>

void print(const glm::vec3& value) {
  std::cout << "glm::vec3(" << value.x << ", " << value.y << ", " << value.z << ")" << std::endl;
}

void print(const glm::vec4& value) {
  std::cout << "glm::vec4(" << value.x << ", " << value.y << ", " << value.z << ", " << value.w
            << ")" << std::endl;
}

template <typename T>
void print(const T& value) {
  std::cout << value << std::endl;
}

template <typename T, typename... Args>
void print(const T& first, const Args&... rest) {
  std::cout << first;
  if constexpr (sizeof...(rest)) {
    std::cout << " ";
    print(rest...);
  } else {
    std::cout << std::endl;
  }
}
