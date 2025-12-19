#version 460 core

#define WORKGROUP_SIZE 256

layout(local_size_x = WORKGROUP_SIZE, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 1) readonly buffer Positions_t {
  float g_positions_t[];
};

layout(std430, binding = 2) readonly buffer Positions_tp1_Front {
  float g_positions_tp1_front[];
};

// careful, this is t (not tp1) in vbd-0-position-init.comp.glsl
layout(std430, binding = 3) buffer Velocities_tp1 {
  float g_velocities_tp1[];
};

uniform uint vert_count;
uniform float h;

void main() {
  uint g_tid = gl_GlobalInvocationID.x;
  if (g_tid >= vert_count) return;

  uint vid = g_tid;

  vec3 position_t_i = vec3(g_positions_t[3 * vid + 0],
                           g_positions_t[3 * vid + 1],
                           g_positions_t[3 * vid + 2]);
  vec3 position_tp1_i = vec3(g_positions_tp1_front[3 * vid + 0],
                             g_positions_tp1_front[3 * vid + 1],
                             g_positions_tp1_front[3 * vid + 2]);

  vec3 velocity_tp1_i = (position_tp1_i - position_t_i) / h;

  g_velocities_tp1[3 * vid + 0] = velocity_tp1_i.x;
  g_velocities_tp1[3 * vid + 1] = velocity_tp1_i.y;
  g_velocities_tp1[3 * vid + 2] = velocity_tp1_i.z;
}
