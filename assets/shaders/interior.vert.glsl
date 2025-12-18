#version 460 core

layout(std430, binding = 2) readonly buffer CurrPositions {
  float g_curr_positions[];
};

layout(location = 0) in vec3 v_xyz;  // quad
layout(location = 1) in vec2 v_uv;   // quad

out vec2 f_uv;

struct Camera {
  mat4 view;
  mat4 projection;
  vec3 u;
  vec3 v;
  vec3 w;
};

uniform Camera camera;

void main() {
  uint g_tid = gl_BaseInstance + gl_InstanceID;

  uint i = g_tid;
  vec3 position_i = vec3(g_curr_positions[3 * i], g_curr_positions[3 * i + 1], g_curr_positions[3 * i + 2]);

  gl_Position = camera.projection * camera.view * vec4(position_i + camera.u * v_xyz.x + camera.v * v_xyz.y, 1.0);

  f_uv = v_uv;
}
