#define WORKGROUP_SIZE 256

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <cassert>
#include <cmath>
#include <glm/matrix.hpp>
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
const bool fullscreen = true;

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
  bool isResetting = true;

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
    if (key == GLFW_KEY_ESCAPE or key == GLFW_KEY_CAPS_LOCK) {
      glfwSetWindowShouldClose(window, GL_TRUE);
      return;
    }

    if (key == GLFW_KEY_P) {
      state.isPseudoPaused = !state.isPseudoPaused;
      return;
    }

    if (key == GLFW_KEY_R) {
      state.isResetting = true;
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
  glfwSetErrorCallback([](int error, const char* description) {
    fprintf(stderr, "GLFW Error (%d): %s\n", error, description);
  });

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
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

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

  glBindVertexArray(0);

  // === COMPUTE PIPELINE SHADERS ===
  ComputeShader vbdPositionInit, vbdPositionUpdate, vbdVelocityUpdate, stretchY, debugCS;
  try {
    vbdPositionInit.build("./assets/shaders/vbd-0-position-init.comp.glsl");
    vbdPositionUpdate.build("./assets/shaders/vbd-1-position.comp.glsl");
    vbdVelocityUpdate.build("./assets/shaders/vbd-2-velocity.comp.glsl");
    stretchY.build("./assets/shaders/stretch-y.comp.glsl");
    debugCS.build("./assets/shaders/debug.comp.glsl");

  } catch (const std::exception& err) {
    std::cerr << err.what();
    return 1;
  }

  // === RENDER PIPELINE SHADERS ===
  RenderShader interiorShader, exteriorShader;
  try {
    interiorShader.build(
      "./assets/shaders/interior.vert.glsl", "./assets/shaders/interior.frag.glsl"
    );
    exteriorShader.build(
      "./assets/shaders/exterior.vert.glsl", "./assets/shaders/exterior.frag.glsl"
    );

  } catch (const std::exception& err) {
    std::cerr << err.what();
    return 1;
  }

  VBD vbd{};

  // === DEBUG ===
  unsigned int __SSBO_DEBUG;
  glGenBuffers(1, &__SSBO_DEBUG);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, __SSBO_DEBUG);
  glBufferData(
    GL_SHADER_STORAGE_BUFFER, sizeof(float) * 3 * vbd.vertCount, nullptr, GL_DYNAMIC_DRAW
  );
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 14, __SSBO_DEBUG);
  glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32F, GL_RED, GL_FLOAT, nullptr);
  glObjectLabel(GL_BUFFER, __SSBO_DEBUG, -1, "DEBUG");

  const float deltaTime = 1.0f / 144.0f;
  float prevTime = glfwGetTime();
  float accumulatedTime = 0.0f;

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

    // stretch
    if (state.isResetting) {
      stretchY.use();
      stretchY.uniform("vert_count", vbd.vertCount);
      stretchY.uniform("stretch_factor", 1.5f);
      glDispatchCompute((vbd.vertCount + WORKGROUP_SIZE - 1) / WORKGROUP_SIZE, 1, 1);
      glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

      state.isResetting = false;
    }

    if (!state.isPseudoPaused) {
      // === VBD (not synced with real-time) ===
      float h = deltaTime;

      // positions: t-1, t -> t, t+1
      glBindBuffer(GL_COPY_READ_BUFFER, vbd.__SSBO_POSITIONS_tp1_FRONT);
      glBindBuffer(GL_COPY_WRITE_BUFFER, vbd.__SSBO_POSITIONS_t);
      glCopyBufferSubData(
        GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, sizeof(float) * 3 * vbd.vertCount
      );
      glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

      // 0. position initialization
      vbdPositionInit.use();
      vbdPositionInit.uniform("vert_count", vbd.vertCount);
      vbdPositionInit.uniform("h", h);
      glDispatchCompute((vbd.vertCount + WORKGROUP_SIZE - 1) / WORKGROUP_SIZE, 1, 1);
      glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

      // velocities: t-1, t -> t, t+1
      glBindBuffer(GL_COPY_READ_BUFFER, vbd.__SSBO_VELOCITIES_tp1);
      glBindBuffer(GL_COPY_WRITE_BUFFER, vbd.__SSBO_VELOCITIES_t);
      glCopyBufferSubData(
        GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, sizeof(float) * 3 * vbd.vertCount
      );
      glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

      // y
      // DCD with x^t
      // adaptive init for x
      int iters = 1;
      for (int iter = 0; iter < iters; iter++) {
        // CCD every n_col iters
        for (unsigned int colorGroup = 0; colorGroup < vbd.colorGroupCount; colorGroup++) {
          unsigned int colorGroupSize = vbd.colorGroupSizes[colorGroup];

          // 1. position update
          vbdPositionUpdate.use();
          vbdPositionUpdate.uniform("color_group", colorGroup);
          vbdPositionUpdate.uniform("color_group_size", colorGroupSize);
          vbdPositionUpdate.uniform("h", h);
          glDispatchCompute((colorGroupSize + WORKGROUP_SIZE - 1) / WORKGROUP_SIZE, 1, 1);
          glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

          // 2. ping-pong
          glBindBuffer(GL_COPY_READ_BUFFER, vbd.__SSBO_POSITIONS_tp1_BACK);
          glBindBuffer(GL_COPY_WRITE_BUFFER, vbd.__SSBO_POSITIONS_tp1_FRONT);
          glCopyBufferSubData(
            GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, sizeof(float) * 3 * vbd.vertCount
          );
          glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        }
      }

      // 3. velocity update
      vbdVelocityUpdate.use();
      vbdVelocityUpdate.uniform("vert_count", vbd.vertCount);
      vbdVelocityUpdate.uniform("h", h);
      glDispatchCompute((vbd.vertCount + WORKGROUP_SIZE - 1) / WORKGROUP_SIZE, 1, 1);
      glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    debugCS.use();
    debugCS.uniform("vert_count", vbd.vertCount);
    glDispatchCompute((vbd.vertCount + WORKGROUP_SIZE - 1) / WORKGROUP_SIZE, 1, 1);

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

    // glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

    // === EXTERIOR SHADER ===
    glm::mat4 model = vbd.transform.model();
    exteriorShader.use();
    exteriorShader.uniform("camera.view", state.camera.view());
    exteriorShader.uniform("camera.projection", state.camera.projection());
    exteriorShader.uniform("camera.u", u);
    exteriorShader.uniform("camera.v", v);
    exteriorShader.uniform("camera.w", w);
    exteriorShader.uniform("model", model);
    exteriorShader.uniform("model_inv", glm::inverse(model));
    vbd.draw();

    // glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
