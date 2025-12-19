#version 460 core

#define WORKGROUP_SIZE 256

layout(local_size_x = WORKGROUP_SIZE, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 0) readonly buffer Positions_0 {
  float g_positions_0[];
};

layout(std430, binding = 1) readonly buffer Positions_t {
  float g_positions_t[];
};

layout(std430, binding = 2) buffer Positions_tp1_Front {
  float g_positions_tp1_front[];
};

layout(std430, binding = 11) buffer Positions_tp1_Back {
  float g_positions_tp1_back[];
};

// careful, this is tm1 (not t) in vbd-0-position-init.comp.glsl
layout(std430, binding = 12) buffer Velocities_t {
  float g_velocities_t[];
};

layout(std430, binding = 4) buffer Masses {
  float g_masses[];
};

layout(std430, binding = 9) readonly buffer ColorGroups {
  uint g_color_groups[];
};

layout(std430, binding = 10) readonly buffer ColorGroupOffsets {
  uint g_color_group_offsets[];
};

layout(std430, binding = 14) buffer Debug {
  float g_debug[];
};

uniform uint color_group;
uniform uint color_group_size;
uniform float h;

const float gravity = -9.81;
const vec3 acceleration_ext = vec3(0.0, gravity, 0.0);

const float k = 1e6;

vec3 force(uint vid, float mass, vec3 position_t, vec3 position_tp1, vec3 velocity_t) {
  vec3 force = mass / (h * h) * (position_tp1 - (position_t + h * velocity_t + h * h * acceleration_ext));

  // bounding box (y = 0) quadratic energy
  force[1] += position_tp1.y < 0.0 ? k * position_tp1.y : 0.0;

  return -force;
}

mat3 hessian(uint vid, float mass, vec3 position_tp1){
  mat3 hessian = mass / (h * h) * mat3(1.0);

  // bounding box (y = 0) quadratic energy
  hessian[1][1] += position_tp1.y < 0.0 ? k : 0.0;

  // elastic potential


  return hessian;
}

void main() {
  uint g_tid = gl_GlobalInvocationID.x;
  if (g_tid >= color_group_size) return;

  uint vid = g_color_groups[g_color_group_offsets[color_group] + g_tid];

  float mass_i = g_masses[vid];
  vec3 position_t_i = vec3(g_positions_t[3 * vid + 0],
                           g_positions_t[3 * vid + 1],
                           g_positions_t[3 * vid + 2]);
  vec3 position_tp1_i = vec3(g_positions_tp1_front[3 * vid + 0],
                             g_positions_tp1_front[3 * vid + 1],
                             g_positions_tp1_front[3 * vid + 2]);
  vec3 velocity_t_i = vec3(g_velocities_t[3 * vid + 0],
                           g_velocities_t[3 * vid + 1],
                           g_velocities_t[3 * vid + 2]);

  vec3 force = force(vid, mass_i, position_t_i, position_tp1_i, velocity_t_i);
  mat3 hessian = hessian(vid, mass_i, position_tp1_i); // is this right?

  // if this vertex's Hessian is rank-deficient, skip it
  if (determinant(hessian) > 1e-8) position_tp1_i += inverse(hessian) * force;

  float res = determinant(hessian);
  g_debug[3 * vid + 0] = res;
  g_debug[3 * vid + 1] = 0.0;
  g_debug[3 * vid + 2] = 0.0;

  g_positions_tp1_back[3 * vid + 0] = position_tp1_i.x;
  g_positions_tp1_back[3 * vid + 1] = position_tp1_i.y;
  g_positions_tp1_back[3 * vid + 2] = position_tp1_i.z;
}
