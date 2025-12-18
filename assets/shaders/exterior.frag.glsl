#version 460 core

struct Light {
  vec3 origin;
  vec3 color;
};

in vec3 f_xyz;
in vec3 f_normal;

out vec4 f_color;

void main() {
  Light light;
  light.origin = vec3(10.0, 100.0, 10.0);
  light.color = vec3(1.0, 1.0, 1.0);

  vec3 light_dir = normalize(light.origin - f_xyz);

  vec3 material_color = vec3(0.75, 0.75, 0.75);

  vec3 ambient = light.color * material_color * 0.5;
  vec3 diffuse = light.color * material_color * max(dot(normalize(f_normal), light_dir), 0.0);

  f_color = vec4(ambient + diffuse, 1.0);
}
