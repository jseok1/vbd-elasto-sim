#version 460 core

#define WORKGROUP_SIZE 256

layout(local_size_x = WORKGROUP_SIZE, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 0) readonly buffer RestPositions {
  float g_rest_positions[];
};

layout(std430, binding = 2) readonly buffer CurrPositionsFront {
  float g_curr_positions_front[];
};

layout(std430, binding = 11) buffer CurrPositionsBack {
  float g_curr_positions_back[];
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

  vec3 position_i = vec3(g_curr_positions_front[3 * vid + 0],
                         g_curr_positions_front[3 * vid + 1],
                         g_curr_positions_front[3 * vid + 2]);



  


  g_curr_positions_back[3 * vid + 0] = position_i.x;
  g_curr_positions_back[3 * vid + 1] = position_i.y;
  g_curr_positions_back[3 * vid + 2] = position_i.z;
}
