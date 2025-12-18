#version 460 core

struct Light {
  vec3 origin;
  vec3 color;
};

in vec3 f_position;
in vec3 f_normal;

out vec4 out_color;

void main() {
  Light light;
  light.origin = vec3(10.0, 100.0, 10.0);
  light.color = vec3(1.0, 1.0, 1.0);

  vec3 lightDir = normalize(light.origin - f_position);

  vec3 f_color = vec3(0.8, 0.8, 0.8);

  vec3 ambient = light.color * f_color * 0.5;
  vec3 diffuse = light.color * f_color * max(dot(normalize(f_normal), lightDir), 0.0);

  out_color = vec4(ambient + diffuse, 1.0);
}
