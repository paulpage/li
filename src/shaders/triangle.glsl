#pragma vs
#version 330 core
uniform vec2 screen;
in vec2 position;
in vec4 color;
out vec4 v_color;
void main() {
  v_color = color;
  gl_Position = vec4(-1 + 2 * position.x / screen.x, 1 - 2 * position.y / screen.y, 0, 1);
};
#pragma end

#pragma fs
#version 330 core
in vec4 v_color;
out vec4 LFragment;
void main() {
  LFragment = v_color;
};
#pragma end
