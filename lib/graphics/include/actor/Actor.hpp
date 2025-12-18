#pragma once

#include <vector>

#include "mesh/Mesh.hpp"
#include "transform/Transform.hpp"

class Actor {
 public:
  Actor();

  void load(const std::string &path);
  void draw() const;
  void draw(unsigned int instances) const;

 private:
  Transform transform;
  std::vector<Mesh> meshes;

  void load();
};
