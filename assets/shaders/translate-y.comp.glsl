#version 460 core

#define WORKGROUP_SIZE 256

layout(local_size_x = WORKGROUP_SIZE, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 0) readonly buffer Positions_0 {
  float g_positions_0[];
};

layout(std430, binding = 2) buffer Positions_tp1_Front {
  float g_positions_tp1_front[];
};

uniform uint vert_count;

void main() {
  uint g_tid = gl_GlobalInvocationID.x;
  if (g_tid >= vert_count) return;

  uint vid = g_tid;

  vec3 position_0_i = vec3(g_positions_0[3 * vid + 0],
                           g_positions_0[3 * vid + 1],
                           g_positions_0[3 * vid + 2]);

  vec3 position_tp1_i = position_0_i;
  position_tp1_i.y += 2.0;

  g_positions_tp1_front[3 * vid + 0] = position_tp1_i.x;
  g_positions_tp1_front[3 * vid + 1] = position_tp1_i.y;
  g_positions_tp1_front[3 * vid + 2] = position_tp1_i.z;
}
