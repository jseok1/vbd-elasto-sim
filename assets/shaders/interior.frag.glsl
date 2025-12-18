#version 460 core

in vec2 f_uv;

out vec4 f_color;

void main() {
  if (length(f_uv - vec2(0.5)) > 0.5) discard;

  f_color = vec4(0.1, 0.1, 0.1, 1.0);
}
