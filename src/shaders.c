const GLchar* TRI_VERT_SRC =
    "#version 330 core\n"
    "uniform vec2 screen;\n"
    "in vec2 position;\n"
    "in vec4 color;\n"
    "out vec4 v_color;\n"
    "void main() {\n"
    "  v_color = color;\n"
    "  gl_Position = vec4(-1 + 2 * position.x / screen.x, 1 - 2 * position.y / screen.y, 0, 1);\n"
    "}\n";

const GLchar *TRI_FRAG_SRC =
    "#version 330 core\n"
    "in vec4 v_color;\n"
    "out vec4 LFragment;\n"
    "void main() {\n"
    "  LFragment = v_color;\n"
    "}\n";

const GLchar *TEXTURE_VERT_SRC =
    "#version 330 core\n"
    "layout (location = 0) in vec2 position;\n"
    "layout (location = 1) in vec2 tex_coords;\n"
    "\n"
    "out vec2 v_tex_coords;\n"
    "\n"
    "void main() {\n"
    "    gl_Position = vec4(position, 0.0, 1.0);\n"
    "    v_tex_coords = tex_coords;\n"
    "}\n";

const GLchar *TEXTURE_FRAG_SRC =
    "#version 330 core\n"
    "uniform sampler2D tex;\n"
    "in vec2 v_tex_coords;\n"
    "out vec4 f_color;\n"
    "\n"
    "void main() {\n"
    "    f_color = texture(tex, v_tex_coords);\n"
    "}\n";

const GLchar *TEXT_VERT_SRC =
    "#version 330 core\n"
    "layout (location = 0) in vec2 position;\n"
    "layout (location = 1) in vec2 tex_coords;\n"
    "layout (location = 2) in vec4 color;\n"
    "\n"
    "out vec2 v_tex_coords;\n"
    "out vec4 v_color;\n"
    "\n"
    "void main() {\n"
    "    gl_Position = vec4(position, 0.0, 1.0);\n"
    "    v_tex_coords = tex_coords;\n"
    "    v_color = color;\n"
    "}\n";

const GLchar *TEXT_FRAG_SRC =
    "#version 330 core\n"
    "uniform sampler2D tex;\n"
    "in vec2 v_tex_coords;\n"
    "in vec4 v_color;\n"
    "out vec4 f_color;\n"
    "\n"
    "void main() {\n"
    "    f_color = v_color * vec4(1.0, 1.0, 1.0, texture(tex, v_tex_coords).r);\n"
    "}\n";
