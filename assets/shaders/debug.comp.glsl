#version 460 core

#define WORKGROUP_SIZE 256

layout(local_size_x = WORKGROUP_SIZE, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 0) buffer Positions_0 {
  float g_positions_0[];
};

layout(std430, binding = 2) buffer Positions_tp1_Front {
  float g_positions_tp1_front[];
};

layout(std430, binding = 14) buffer Debug {
  float g_debug[];
};

uniform uint vert_count;

float zero() {
  return 0.0;
}

void main() {
  uint g_tid = gl_GlobalInvocationID.x;
  if (g_tid >= vert_count) return;

  uint vid = g_tid;

  if (vid == vert_count - 1) {
    float temp = g_debug[3 * vid + 0];
    g_debug[3 * vid + 0] = 1.0;
    g_debug[3 * vid + 0] = temp;

    float tempa = g_positions_tp1_front[3 * vid + 0];
    g_positions_tp1_front[3 * vid + 0] = 1.0;
    g_positions_tp1_front[3 * vid + 0] = tempa;

    float tempb = g_positions_0[3 * vid + 0];
    g_positions_0[3 * vid + 0] = 1.0;
    g_positions_0[3 * vid + 0] = tempb;

  }
}
