#version 460 core

#define WORKGROUP_SIZE 256

layout(local_size_x = WORKGROUP_SIZE, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 1) readonly buffer PrevPositions {
  float g_prev_positions[];
};

layout(std430, binding = 2) readonly buffer CurrPositionsFront {
  float g_curr_positions_front[];
};

layout(std430, binding = 3) buffer Velocities {
  float g_velocities[];
};

uniform uint vert_count;
uniform float time_step;

void main() {
  uint g_tid = gl_GlobalInvocationID.x;
  if (g_tid >= vert_count) return;

  uint vid = g_tid;

  vec3 prev_position_i = vec3(g_prev_positions[3 * vid + 0],
                              g_prev_positions[3 * vid + 1],
                              g_prev_positions[3 * vid + 2]);
  vec3 curr_position_i = vec3(g_curr_positions_front[3 * vid + 0],
                              g_curr_positions_front[3 * vid + 1],
                              g_curr_positions_front[3 * vid + 2]);

  vec3 velocity_i = (curr_position_i - prev_position_i) / time_step;

  g_velocities[3 * vid + 0] = velocity_i.x;
  g_velocities[3 * vid + 1] = velocity_i.y;
  g_velocities[3 * vid + 2] = velocity_i.z;
}
