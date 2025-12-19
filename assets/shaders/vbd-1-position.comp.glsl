#version 460 core

#define WORKGROUP_SIZE 256

layout(local_size_x = WORKGROUP_SIZE, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 0) readonly buffer Positions_0 {
  float g_positions_0[];
};

layout(std430, binding = 1) readonly buffer Positions_t {
  float g_positions_t[];
};

layout(std430, binding = 2) readonly buffer Positions_tp1_Front {
  float g_positions_tp1_front[];
};

layout(std430, binding = 11) buffer Positions_tp1_Back {
  float g_positions_tp1_back[];
};

// careful, this is tm1 (not o) in vbd-0-position-init.comp.glsl
layout(std430, binding = 3) readonly buffer Velocities_t {
  float g_velocities_t[];
};

layout(std430, binding = 4) readonly buffer Masses {
  float g_masses[];
};

layout(std430, binding = 5) readonly buffer TetrahedronIndices {
  uint g_tetrahedron_indices[];
};

layout(std430, binding = 7) readonly buffer VertexIndexToTetrahedra {
  uint g_vertex_index_to_tetrahedra[];
};

layout(std430, binding = 8) readonly buffer VertexIndexToTetrahedraOffsets {
  uint g_vertex_index_to_tetrahedra_offsets[];
};

layout(std430, binding = 6) readonly buffer Stiffnesses {
  float g_stiffnesses[];
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

const float gravity = 0.0;//-9.81;
const vec3 acceleration_ext = vec3(0.0, gravity, 0.0);

const float kc = 2e6;

vec3 force(uint vid, float mass, vec3 position_t, vec3 position_tp1, vec3 velocity_t) {
  vec3 force = mass / (h * h) * (position_tp1 - (position_t + h * velocity_t + h * h * acceleration_ext));

  // bounding box (quadratic) energy
  // force[1] += position_tp1.y < 0.0 ? kc * position_tp1.y : 0.0;

  // elastic potential energy
  uint start = g_vertex_index_to_tetrahedra_offsets[vid];
  uint end = g_vertex_index_to_tetrahedra_offsets[vid + 1];  // exclusive

  uint i = start;
  while (i < end) {
    uint tid = g_vertex_index_to_tetrahedra[i];

    // these are ordered consistently
    uint tetrahedron[4] = {
      g_tetrahedron_indices[4 * tid + 0],
      g_tetrahedron_indices[4 * tid + 1],
      g_tetrahedron_indices[4 * tid + 2],
      g_tetrahedron_indices[4 * tid + 3]
    };

    // which vertex is this in this tetrahedron?
    uint j;
    if (vid == tetrahedron[0]) j = 0;
    if (vid == tetrahedron[1]) j = 1;
    if (vid == tetrahedron[2]) j = 2;
    if (vid == tetrahedron[3]) j = 3;

    // probably better to compute this and store it with a separate compute shader + buffer
    // (displacements)
    // TODO: compute everything (forces, Hessians) PER tetrahedron

    // clang-format off
    vec3 positions_0[4] = {
      vec3(
        g_positions_0[3 * tetrahedron[0] + 0],
        g_positions_0[3 * tetrahedron[0] + 1],
        g_positions_0[3 * tetrahedron[0] + 2]
      ),
      vec3(
        g_positions_0[3 * tetrahedron[1] + 0],
        g_positions_0[3 * tetrahedron[1] + 1],
        g_positions_0[3 * tetrahedron[1] + 2]
      ),
      vec3(
        g_positions_0[3 * tetrahedron[2] + 0],
        g_positions_0[3 * tetrahedron[2] + 1],
        g_positions_0[3 * tetrahedron[2] + 2]
      ),
      vec3(
        g_positions_0[3 * tetrahedron[3] + 0],
        g_positions_0[3 * tetrahedron[3] + 1],
        g_positions_0[3 * tetrahedron[3] + 2]
      )
    };

    vec3 positions_tp1[4] = {
      vec3(
        g_positions_tp1_front[3 * tetrahedron[0] + 0],
        g_positions_tp1_front[3 * tetrahedron[0] + 1],
        g_positions_tp1_front[3 * tetrahedron[0] + 2]
      ),
      vec3(
        g_positions_tp1_front[3 * tetrahedron[1] + 0],
        g_positions_tp1_front[3 * tetrahedron[1] + 1],
        g_positions_tp1_front[3 * tetrahedron[1] + 2]
      ),
      vec3(
        g_positions_tp1_front[3 * tetrahedron[2] + 0],
        g_positions_tp1_front[3 * tetrahedron[2] + 1],
        g_positions_tp1_front[3 * tetrahedron[2] + 2]
      ),
      vec3(
        g_positions_tp1_front[3 * tetrahedron[3] + 0],
        g_positions_tp1_front[3 * tetrahedron[3] + 1],
        g_positions_tp1_front[3 * tetrahedron[3] + 2]
      )
    };

    uint o = 144 * tid + 36 * j;
    mat3 stiffnesses[4] = {
      mat3(
        g_stiffnesses[o +  0], g_stiffnesses[o + 12], g_stiffnesses[o + 24],
        g_stiffnesses[o +  1], g_stiffnesses[o + 13], g_stiffnesses[o + 25],
        g_stiffnesses[o +  2], g_stiffnesses[o + 14], g_stiffnesses[o + 26]
      ),
      mat3(
        g_stiffnesses[o +  3], g_stiffnesses[o + 15], g_stiffnesses[o + 27],
        g_stiffnesses[o +  4], g_stiffnesses[o + 16], g_stiffnesses[o + 28],
        g_stiffnesses[o +  5], g_stiffnesses[o + 17], g_stiffnesses[o + 29]
      ),
      mat3(
        g_stiffnesses[o +  6], g_stiffnesses[o + 18], g_stiffnesses[o + 30],
        g_stiffnesses[o +  7], g_stiffnesses[o + 19], g_stiffnesses[o + 31],
        g_stiffnesses[o +  8], g_stiffnesses[o + 20], g_stiffnesses[o + 32]
      ),
      mat3(
        g_stiffnesses[o +  9], g_stiffnesses[o + 21], g_stiffnesses[o + 33],
        g_stiffnesses[o + 10], g_stiffnesses[o + 22], g_stiffnesses[o + 34],
        g_stiffnesses[o + 11], g_stiffnesses[o + 23], g_stiffnesses[o + 35]
      ),
    };
    // clang-format on

    for (int k = 0; k < 4; k++) {
      // if displacement is positive, then force is negative (?)
      force += stiffnesses[k] * (positions_tp1[k] - positions_0[k]);
      // force += tetrahedron[0] - tetrahedron[0] + tetrahedron[1] - tetrahedron[1] + tetrahedron[2] - tetrahedron[2] + tetrahedron[3] - tetrahedron[3];
    }

    i++; 
  }
  
  return -force;
}

mat3 hessian(uint vid, float mass, vec3 position_tp1){
  mat3 hessian = mass / (h * h) * mat3(1.0);

  // bounding box (quadratic) energy
  // hessian[1][1] += position_tp1.y < 0.0 ? kc : 0.0;

  // elastic potential energy
  uint start = g_vertex_index_to_tetrahedra_offsets[vid];
  uint end = g_vertex_index_to_tetrahedra_offsets[vid + 1];  // exclusive

  uint i = start;
  while (i < end) {
    uint tid = g_vertex_index_to_tetrahedra[i];

    // these are ordered consistently
    uint tetrahedron[4] = {
      g_tetrahedron_indices[4 * tid + 0],
      g_tetrahedron_indices[4 * tid + 1],
      g_tetrahedron_indices[4 * tid + 2],
      g_tetrahedron_indices[4 * tid + 3]
    };

    // which vertex is this in this tetrahedron?
    uint j;
    if (vid == tetrahedron[0]) j = 0;
    if (vid == tetrahedron[1]) j = 1;
    if (vid == tetrahedron[2]) j = 2;
    if (vid == tetrahedron[3]) j = 3;

    // clang-format off
    uint o = 144 * tid + 36 * j + 3 * j;
    hessian += mat3(
      g_stiffnesses[o +  0], g_stiffnesses[o + 12], g_stiffnesses[o + 24],
      g_stiffnesses[o +  1], g_stiffnesses[o + 13], g_stiffnesses[o + 25],
      g_stiffnesses[o +  2], g_stiffnesses[o + 14], g_stiffnesses[o + 26]
    );
    // clang-format on

    i++; 
  }

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
  mat3 hessian = hessian(vid, mass_i, position_tp1_i);

  // positions get really far, so forces are really big

  // g_debug[3 * vid + 0] = hessian[0][0];
  // g_debug[3 * vid + 1] = hessian[1][0];
  // g_debug[3 * vid + 2] = hessian[2][0];
  
  g_debug[3 * vid + 0] = force.x;
  g_debug[3 * vid + 1] = force.y;
  g_debug[3 * vid + 2] = force.z;
  
  // if this vertex's Hessian is rank-deficient, skip it
  if (determinant(hessian) > 1e-6) position_tp1_i += inverse(hessian) * force;

  g_positions_tp1_back[3 * vid + 0] = position_tp1_i.x;
  g_positions_tp1_back[3 * vid + 1] = position_tp1_i.y;
  g_positions_tp1_back[3 * vid + 2] = position_tp1_i.z;
}
