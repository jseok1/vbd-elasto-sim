#pragma once
#include <glad/glad.h>

#include <fstream>
#include <nlohmann/json.hpp>
#include <vector>

#include "Transform.hpp"

using json = nlohmann::json;

class VBD {
 public:
  unsigned int vertCount;
  unsigned int triCount;
  unsigned int tetCount;
  unsigned int colorGroupCount;

  unsigned int __SSBO_REST_POSITIONS, __SSBO_PREV_POSITIONS, __SSBO_CURR_POSITIONS;
  unsigned int __SSBO_VELOCITIES;
  unsigned int __SSBO_MASSES;
  unsigned int __EBO_TRI_INDICES;
  unsigned int __SSBO_TET_INDICES;
  unsigned int __SSBO_STIFFNESSES;
  unsigned int __VAO_SURFACE;
  unsigned int __SSBO_VERTEX_INDEX_TO_TETRAHEDRA;
  unsigned int __SSBO_VERTEX_INDEX_TO_TETRAHEDRA_OFFSETS;
  unsigned int __SSBO_COLOR_GROUPS;
  unsigned int __SSBO_COLOR_GROUP_OFFSETS;

  Transform transform;

  std::vector<unsigned int> colorGroupSizes;

  VBD()
    : vertCount{},
      triCount{},
      tetCount{},
      __SSBO_REST_POSITIONS{},
      __SSBO_PREV_POSITIONS{},
      __SSBO_CURR_POSITIONS{},
      __SSBO_VELOCITIES{},
      __SSBO_MASSES{},
      __EBO_TRI_INDICES{},
      __SSBO_TET_INDICES{},
      __SSBO_STIFFNESSES{},
      __VAO_SURFACE{},
      __SSBO_VERTEX_INDEX_TO_TETRAHEDRA{},
      __SSBO_VERTEX_INDEX_TO_TETRAHEDRA_OFFSETS{},
      __SSBO_COLOR_GROUPS{},
      __SSBO_COLOR_GROUP_OFFSETS{},
      transform{},
      colorGroupSizes{} {
    init_positions();
    init_masses();
    init_triangle_indices();
    init_tetrahedron_indices();
    init_stiffnesses();
    init_vertex_to_tetrahedra();
    init_vertex_to_tetrahedra_offsets();
    init_color_groups();
    init_color_group_offsets();

    // === DRAWING ===
    glGenVertexArrays(1, &__VAO_SURFACE);
    glBindVertexArray(__VAO_SURFACE);
    glBindBuffer(GL_ARRAY_BUFFER, __SSBO_CURR_POSITIONS);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, __EBO_TRI_INDICES);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
  }

  void draw() {
    glBindVertexArray(__VAO_SURFACE);
    glDrawElements(GL_TRIANGLES, 3 * triCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }

 private:
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

    glGenBuffers(1, &__EBO_TRI_INDICES);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, __EBO_TRI_INDICES);
    glBufferData(
      GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 3 * triCount, triangles.data(), GL_STATIC_DRAW
    );
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
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, __SSBO_TET_INDICES);
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
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, __SSBO_STIFFNESSES);
  }

  void init_vertex_to_tetrahedra() {
    std::ifstream f("./assets/experiments/01/vertex-to-tetrahedra.json");
    json j = json::parse(f);

    std::vector<float> mapping = j.get<std::vector<float>>();

    glGenBuffers(1, &__SSBO_VERTEX_INDEX_TO_TETRAHEDRA);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, __SSBO_VERTEX_INDEX_TO_TETRAHEDRA);
    glBufferData(
      GL_SHADER_STORAGE_BUFFER, sizeof(float) * mapping.size(), mapping.data(), GL_STATIC_DRAW
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, __SSBO_VERTEX_INDEX_TO_TETRAHEDRA);
  }

  void init_vertex_to_tetrahedra_offsets() {
    std::ifstream f("./assets/experiments/01/vertex-to-tetrahedra-offsets.json");
    json j = json::parse(f);

    std::vector<float> mapping = j.get<std::vector<float>>();

    glGenBuffers(1, &__SSBO_VERTEX_INDEX_TO_TETRAHEDRA_OFFSETS);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, __SSBO_VERTEX_INDEX_TO_TETRAHEDRA_OFFSETS);
    glBufferData(
      GL_SHADER_STORAGE_BUFFER, sizeof(float) * mapping.size(), mapping.data(), GL_STATIC_DRAW
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, __SSBO_VERTEX_INDEX_TO_TETRAHEDRA_OFFSETS);
  }

  void init_color_groups() {
    std::ifstream f("./assets/experiments/01/color-groups.json");
    json j = json::parse(f);

    std::vector<unsigned int> mapping = j.get<std::vector<unsigned int>>();

    glGenBuffers(1, &__SSBO_COLOR_GROUPS);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, __SSBO_COLOR_GROUPS);
    glBufferData(
      GL_SHADER_STORAGE_BUFFER,
      sizeof(unsigned int) * mapping.size(),
      mapping.data(),
      GL_STATIC_DRAW
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, __SSBO_COLOR_GROUPS);
  }

  void init_color_group_offsets() {
    std::ifstream f("./assets/experiments/01/color-group-offsets.json");
    json j = json::parse(f);

    std::vector<unsigned int> mapping = j.get<std::vector<unsigned int>>();
    colorGroupCount = mapping.size() - 1;

    glGenBuffers(1, &__SSBO_COLOR_GROUP_OFFSETS);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, __SSBO_COLOR_GROUP_OFFSETS);
    glBufferData(
      GL_SHADER_STORAGE_BUFFER,
      sizeof(unsigned int) * mapping.size(),
      mapping.data(),
      GL_STATIC_DRAW
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, __SSBO_COLOR_GROUP_OFFSETS);

    // [[*,*], [*,*,*], [*,*,*,*]]
    // [0, 2, 5, 9]

    for (int colorGroup = 0; colorGroup < colorGroupCount; colorGroup++) {
      colorGroupSizes.push_back(mapping[colorGroup + 1] - mapping[colorGroup]);
    }
  }
};
