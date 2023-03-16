#pragma vs
#version 330 core
uniform vec2 screen;
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 tex_coords;

out vec2 v_tex_coords;

void main() {
  gl_Position = vec4(-1 + 2 * position.x / screen.x, 1 - 2 * position.y / screen.y, 0, 1);
  v_tex_coords = tex_coords;
};
#pragma end

#pragma fs
#version 330 core
uniform sampler2D tex;
in vec2 v_tex_coords;
out vec4 f_color;

void main() {
  f_color = texture(tex, v_tex_coords);
};
#pragma end
