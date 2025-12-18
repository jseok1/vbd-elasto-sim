#version 460 core

layout(location = 0) in vec3 v_xyz;
// layout(location = 1) in vec3 v_normal;

out vec3 f_xyz;
out vec3 f_normal;

struct Camera {
  mat4 view;
  mat4 projection;
  vec3 u;
  vec3 v;
  vec3 w;
};

uniform Camera camera;
uniform mat4 model;
uniform mat4 model_inv;

void main() {
  gl_Position = camera.projection * camera.view * vec4(v_xyz, 1.0);

  // TODO: remove
  vec3 v_normal = vec3(1.0, 1.0, 1.0);

  f_xyz = vec3(model * vec4(v_xyz, 1.0));
  f_normal = transpose(mat3(model_inv)) * v_normal;
}
