#version 460 core

#define WORKGROUP_SIZE 256

layout(local_size_x = WORKGROUP_SIZE, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 1) readonly buffer PrevPositions {
  float g_prev_positions[];
};

layout(std430, binding = 2) readonly buffer CurrPositions {
  float g_curr_positions[];
};

layout(std430, binding = 9) readonly buffer ColorGroups {
  uint g_color_groups[];
};

layout(std430, binding = 10) readonly buffer ColorGroupOffsets {
  uint g_color_group_offsets[];
};

uniform uint color_group;
uniform uint color_group_size;

void main() {
  uint g_tid = gl_GlobalInvocationID.x;
  if (g_tid >= color_group_size) return;

  uint vid = g_color_groups[g_color_group_offsets[color_group] + g_tid];

  



}
