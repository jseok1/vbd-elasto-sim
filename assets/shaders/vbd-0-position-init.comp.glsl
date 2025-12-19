#version 460 core

#define WORKGROUP_SIZE 256

layout(local_size_x = WORKGROUP_SIZE, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 1) readonly buffer Positions_t {
  float g_positions_t[];
};

layout(std430, binding = 2) buffer Positions_tp1_Front {
  float g_positions_tp1_front[];
};

layout(std430, binding = 3) readonly buffer Velocities_tm1 {
  float g_velocities_tm1[];
};

layout(std430, binding = 12) readonly buffer Velocities_t {
  float g_velocities_t[];
};


uniform uint vert_count;
uniform float h;

const float gravity = -9.81;
const vec3 acceleration_ext = vec3(0.0, gravity, 0.0);

void main() {
  uint g_tid = gl_GlobalInvocationID.x;
  if (g_tid >= vert_count) return;

  uint vid = g_tid;

  vec3 position_t_i = vec3(g_positions_t[3 * vid + 0],
                           g_positions_t[3 * vid + 1],
                           g_positions_t[3 * vid + 2]);

  vec3 velocity_tm1_i = vec3(g_velocities_tm1[3 * vid + 0],
                             g_velocities_tm1[3 * vid + 1],
                             g_velocities_tm1[3 * vid + 2]);
  vec3 velocity_t_i = vec3(g_velocities_t[3 * vid + 0],
                           g_velocities_t[3 * vid + 1],
                           g_velocities_t[3 * vid + 2]);

  vec3 acceleration_t_i = (velocity_t_i - velocity_tm1_i) / h;

  // === ADAPTIVE ===
  // // only an estimate
  // vec3 acceleration_tp1_i = clamp(
  //   dot(acceleration_t_i, normalize(acceleration_ext)), 0.0, length(acceleration_ext)
  // ) * normalize(acceleration_ext);
  //
  // vec3 position_tp1_i = position_t_i + h * velocity_t_i + h * h * acceleration_tp1_i;

  // === BASIC ===
  vec3 position_tp1_i = position_t_i;

  g_positions_tp1_front[3 * vid + 0] = position_tp1_i.x;
  g_positions_tp1_front[3 * vid + 1] = position_tp1_i.y;
  g_positions_tp1_front[3 * vid + 2] = position_tp1_i.z;
}
