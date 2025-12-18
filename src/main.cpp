#define WORKGROUP_SIZE 256

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <cassert>
#include <cmath>
#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>

#include "Camera.hpp"
#include "ComputeShader.hpp"
#include "RenderShader.hpp"
#include "VBDParameterReader.hpp"

using json = nlohmann::json;

const float fovy = 45.0f;
int width = 1920;  // bad being overwritten
int height = 1080;
const float near = 0.01f;
const float far = 100.0f;
const bool throttle = true;
const bool fullscreen = false;

const float speed = 5.0f;
const float sensitivity = 0.05f;

struct State {
  bool isMovingForward = false;
  bool isMovingBackward = false;
  bool isMovingLeftward = false;
  bool isMovingRightward = false;
  bool isMovingUpward = false;
  bool isMovingDownward = false;
  bool isPaused = false;
  bool isPseudoPaused = true;

  Camera camera = Camera(fovy, width, height, near, far);
} state;

bool firstMouse = true;
double prevXCoord, prevYCoord;

void processFrameBufferState(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

void processMouse(GLFWwindow* window, double currXCoord, double currYCoord) {
  if (firstMouse) {
    prevXCoord = currXCoord;
    prevYCoord = currYCoord;
    firstMouse = false;
  }

  glm::vec2 delta = glm::vec2(-(currYCoord - prevYCoord), currXCoord - prevXCoord) * sensitivity;

  state.camera.rotateBy(delta);

  prevXCoord = currXCoord;
  prevYCoord = currYCoord;
}

void processKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_ESCAPE) {
      glfwSetWindowShouldClose(window, GL_TRUE);
      return;
    }

    if (key == GLFW_KEY_P) {
      state.isPseudoPaused = !state.isPseudoPaused;
      return;
    }

    if (key == GLFW_KEY_W) {
      state.isMovingForward = true;
      return;
    }
    if (key == GLFW_KEY_S) {
      state.isMovingBackward = true;
      return;
    }
    if (key == GLFW_KEY_D) {
      state.isMovingRightward = true;
      return;
    }
    if (key == GLFW_KEY_A) {
      state.isMovingLeftward = true;
      return;
    }
    if (key == GLFW_KEY_SPACE or key == GLFW_KEY_E) {
      state.isMovingUpward = true;
      return;
    }
    if (key == GLFW_KEY_LEFT_SHIFT or key == GLFW_KEY_Q) {
      state.isMovingDownward = true;
      return;
    }
  }

  if (action == GLFW_RELEASE) {
    if (key == GLFW_KEY_W) {
      state.isMovingForward = false;
      return;
    }
    if (key == GLFW_KEY_S) {
      state.isMovingBackward = false;
      return;
    }
    if (key == GLFW_KEY_D) {
      state.isMovingRightward = false;
      return;
    }
    if (key == GLFW_KEY_A) {
      state.isMovingLeftward = false;
      return;
    }
    if (key == GLFW_KEY_SPACE or key == GLFW_KEY_E) {
      state.isMovingUpward = false;
      return;
    }
    if (key == GLFW_KEY_LEFT_SHIFT or key == GLFW_KEY_Q) {
      state.isMovingDownward = false;
      return;
    }
  }
}

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  GLFWwindow* window;
  if (fullscreen) {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    width = mode->width;
    height = mode->height;
    window = glfwCreateWindow(width, height, "VBD", monitor, nullptr);
  } else {
    window = glfwCreateWindow(width, height, "VBD", nullptr, nullptr);
  }
  if (!window) {
    return 1;
  }
  glfwMakeContextCurrent(window);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetFramebufferSizeCallback(window, processFrameBufferState);
  glfwSetCursorPosCallback(window, processMouse);
  glfwSetKeyCallback(window, processKey);

  if (!throttle) {
    glfwSwapInterval(0);
  }

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    return 1;
  }

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_CULL_FACE);

  state.camera = Camera(fovy, width, height, near, far);  // bad
  state.camera.translateTo(glm::vec3(0.0, 0.0, 10.0));
  state.camera.rotateTo(glm::vec2(0.0, -90.0));

  // === BILLBOARDED QUAD ===
  unsigned int quadVAO, quadVBO, quadEBO;

  float quadVertices[] = {
    // clang-format off
    -0.005f,  0.005f, 0.0f, 0.0f, 1.0f,
    -0.005f, -0.005f, 0.0f, 0.0f, 0.0f,
     0.005f,  0.005f, 0.0f, 1.0f, 1.0f,
     0.005f, -0.005f, 0.0f, 1.0f, 0.0f
    // clang-format on
  };
  unsigned int quadIndices[] = {
    // clang-format off
    0, 1, 2,
    2, 1, 3
    // clang-format on
  };

  glGenVertexArrays(1, &quadVAO);
  glGenBuffers(1, &quadVBO);
  glGenBuffers(1, &quadEBO);

  glBindVertexArray(quadVAO);

  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

  // === COMPUTE PIPELINE SHADERS ===

  // === RENDER PIPELINE SHADERS ===
  RenderShader interiorShader;

  try {
    interiorShader.build(
      "./assets/shaders/interior.vert.glsl", "./assets/shaders/interior.frag.glsl"
    );

  } catch (const std::exception& err) {
    std::cerr << err.what();
    return 1;
  }

  VBD vbd{};

  const float deltaTime = 1.0f / 144.0f;
  float prevTime = glfwGetTime();
  float accumulatedTime = 0.0f;

  std::cout << "Before while loop" << std::endl;

  while (!glfwWindowShouldClose(window)) {
    auto origin = state.camera.origin();
    auto [u, v, w] = state.camera.basis();

    float currTime = glfwGetTime();
    accumulatedTime += currTime - prevTime;
    prevTime = currTime;

    while (accumulatedTime >= deltaTime) {
      glm::vec3 delta = glm::vec3(0.0);
      if (state.isMovingForward) delta -= w * speed * deltaTime;
      if (state.isMovingBackward) delta += w * speed * deltaTime;
      if (state.isMovingLeftward) delta -= u * speed * deltaTime;
      if (state.isMovingRightward) delta += u * speed * deltaTime;
      if (state.isMovingDownward) delta -= v * speed * deltaTime;
      if (state.isMovingUpward) delta += v * speed * deltaTime;
      state.camera.translateBy(delta);

      // physics update
      // --------------
      if (!state.isPaused) {
        accumulatedTime -= deltaTime;
      }
    }

    glClearColor(0.6f, 0.88f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // === INTERIOR SHADER ===
    interiorShader.use();
    interiorShader.uniform("camera.view", state.camera.view());
    interiorShader.uniform("camera.projection", state.camera.projection());
    interiorShader.uniform("camera.u", u);
    interiorShader.uniform("camera.v", v);
    interiorShader.uniform("camera.w", w);

    glBindVertexArray(quadVAO);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, vbd.vertCount);
    glBindVertexArray(0);

    // === INTERIOR SHADER ===

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
