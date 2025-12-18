#version 460 core

uniform sampler2D elevation_map;

layout(location = 0) in vec3 xyz;
layout(location = 1) in vec3 normal;

out vec3 f_position;
out vec3 f_normal;

uniform mat4 view;
uniform mat4 projection;
uniform float xMin;
uniform float yMin;
uniform float zMin;
uniform float xMax;
uniform float yMax;
uniform float zMax;
uniform float epsilon;
uniform float step;

float sample_elevation(uint i, uint j) {
  uint m = uint(floor((xMax - xMin) / step));
  uint n = uint(floor((zMax - zMin) / step));

  return texture(elevation_map, vec2(float(j + 0.5) / m, float(n - (i + 0.5)) / n)).r *
           (yMax - yMin + epsilon) +
         yMin - 0.5 * epsilon;
}

void main() {
  uint iid = gl_BaseInstance + gl_InstanceID;

  // should flip m, n for convention
  uint m = uint(floor((xMax - xMin) / step));
  uint n = uint(floor((zMax - zMin) / step));

  // flip y-coord
  uint i = uint(floor(iid / m));
  uint j = uint(mod(iid, m));

  float height = sample_elevation(i, j);

  float x = step * mod(iid, m) - (xMax - xMin) / 2.0;
  float z = step * iid / m - (zMax - zMin) / 2.0;
  float y = height / 2.0;

  // clang-format off
  mat4 model = mat4(
    step / 2.0,          0.0,          0.0,          0.0,
           0.0, height / 2.0,          0.0,          0.0,
           0.0,          0.0,   step / 2.0,          0.0,
             x,            y,            z,          1.0
  );

  mat3 inversed = mat3(
    2.0 / step,          0.0,          0.0,
           0.0, 2.0 / height,          0.0,
           0.0,          0.0,   2.0 / step
  );
  // clang-format on

  gl_Position = projection * view * model * vec4(xyz, 1.0);

  vec4 transformed = model * vec4(xyz, 1.0);
  f_position = transformed.xyz / transformed.w;
  f_normal = transpose(inversed) * normal;
}
