#pragma vs
#version 330 core
uniform vec2 screen;
in vec2 position;
in vec2 tex_coords;
in vec4 color;

out vec2 v_tex_coords;
out vec4 v_color;

void main() {
  gl_Position = vec4(-1 + 2 * position.x / screen.x, 1 - 2 * position.y / screen.y, 0, 1);
  v_tex_coords = tex_coords;
  v_color = color;
};
#pragma end

#pragma fs
#version 330 core
uniform sampler2D tex;
in vec2 v_tex_coords;
in vec4 v_color;
out vec4 f_color;

void main() {
  // f_color = v_color * vec4(1.0, 1.0, 1.0, texture(tex, v_tex_coords).r);
  f_color = v_color * 0.001 + vec4(1.0, 1.0, 1.0, texture(tex, v_tex_coords).r);
};
#pragma end
