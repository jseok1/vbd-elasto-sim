#pragma once
#include <glad/glad.h>

#include <fstream>
#include <nlohmann/json.hpp>
#include <vector>

using json = nlohmann::json;

class VBD {
 public:
  unsigned int vertCount;
  unsigned int triCount;
  unsigned int tetCount;

  VBD()
    : vertCount{},
      triCount{},
      tetCount{},
      __SSBO_REST_POSITIONS{},
      __SSBO_PREV_POSITIONS{},
      __SSBO_CURR_POSITIONS{},
      __SSBO_VELOCITIES{},
      __SSBO_MASSES{},
      __SSBO_TRI_INDICES{},
      __SSBO_TET_INDICES{},
      __SSBO_STIFFNESSES{} {
    init_positions();
    init_masses();
    init_triangle_indices();
    init_tetrahedron_indices();
    init_stiffnesses();
  }

 private:
  unsigned int __SSBO_REST_POSITIONS, __SSBO_PREV_POSITIONS, __SSBO_CURR_POSITIONS;
  unsigned int __SSBO_VELOCITIES;
  unsigned int __SSBO_MASSES;
  unsigned int __SSBO_TRI_INDICES;
  unsigned int __SSBO_TET_INDICES;
  unsigned int __SSBO_STIFFNESSES;

  void init_positions() {
    std::ifstream f("./assets/experiments/01/positions.json");
    json j = json::parse(f);

    std::vector<float> positions = j.get<std::vector<float>>();

    vertCount = positions.size() / 3;

    glGenBuffers(1, &__SSBO_REST_POSITIONS);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, __SSBO_REST_POSITIONS);
    glBufferData(
      GL_SHADER_STORAGE_BUFFER, sizeof(float) * 3 * vertCount, positions.data(), GL_STATIC_DRAW
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, __SSBO_REST_POSITIONS);

    glGenBuffers(1, &__SSBO_PREV_POSITIONS);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, __SSBO_PREV_POSITIONS);
    glBufferData(
      GL_SHADER_STORAGE_BUFFER, sizeof(float) * 3 * vertCount, positions.data(), GL_DYNAMIC_DRAW
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, __SSBO_PREV_POSITIONS);

    glGenBuffers(1, &__SSBO_CURR_POSITIONS);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, __SSBO_CURR_POSITIONS);
    glBufferData(
      GL_SHADER_STORAGE_BUFFER, sizeof(float) * 3 * vertCount, positions.data(), GL_DYNAMIC_DRAW
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, __SSBO_CURR_POSITIONS);
  }

  void init_velocities() {
    glGenBuffers(1, &__SSBO_VELOCITIES);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, __SSBO_VELOCITIES);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * 3 * vertCount, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, __SSBO_VELOCITIES);
    glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32F, GL_RED, GL_FLOAT, nullptr);
  }

  void init_masses() {
    std::ifstream f("./assets/experiments/01/masses.json");
    json j = json::parse(f);

    std::vector<float> masses = j.get<std::vector<float>>();

    glGenBuffers(1, &__SSBO_MASSES);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, __SSBO_MASSES);
    glBufferData(
      GL_SHADER_STORAGE_BUFFER, sizeof(float) * vertCount, masses.data(), GL_STATIC_DRAW
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, __SSBO_MASSES);
  }

  void init_triangle_indices() {
    std::ifstream f("./assets/experiments/01/triangle-indices.json");
    json data = json::parse(f);

    std::vector<unsigned int> triangles = data.get<std::vector<unsigned int>>();

    triCount = triangles.size() / 3;

    glGenBuffers(1, &__SSBO_TRI_INDICES);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, __SSBO_TRI_INDICES);
    glBufferData(
      GL_SHADER_STORAGE_BUFFER,
      sizeof(unsigned int) * 3 * triCount,
      triangles.data(),
      GL_STATIC_DRAW
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, __SSBO_TRI_INDICES);
  }

  void init_tetrahedron_indices() {
    std::ifstream f("./assets/experiments/01/tetrahedron-indices.json");
    json data = json::parse(f);

    std::vector<unsigned int> tetrahedra = data.get<std::vector<unsigned int>>();

    tetCount = tetrahedra.size() / 4;

    glGenBuffers(1, &__SSBO_TET_INDICES);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, __SSBO_TET_INDICES);
    glBufferData(
      GL_SHADER_STORAGE_BUFFER,
      sizeof(unsigned int) * 4 * tetCount,
      tetrahedra.data(),
      GL_STATIC_DRAW
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, __SSBO_TET_INDICES);
  }

  void init_stiffnesses() {
    std::ifstream f("./assets/experiments/01/stiffnesses.json");
    json j = json::parse(f);

    std::vector<float> stiffnesses = j.get<std::vector<float>>();

    glGenBuffers(1, &__SSBO_STIFFNESSES);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, __SSBO_STIFFNESSES);
    glBufferData(
      GL_SHADER_STORAGE_BUFFER, sizeof(float) * 144 * tetCount, stiffnesses.data(), GL_STATIC_DRAW
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, __SSBO_STIFFNESSES);
  }
};
