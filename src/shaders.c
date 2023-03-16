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
    "uniform vec2 screen;\n"
    "layout (location = 0) in vec2 position;\n"
    "layout (location = 1) in vec2 tex_coords;\n"
    "\n"
    "out vec2 v_tex_coords;\n"
    "\n"
    "void main() {\n"
    "  gl_Position = vec4(-1 + 2 * position.x / screen.x, 1 - 2 * position.y / screen.y, 0, 1);\n"
    "  v_tex_coords = tex_coords;\n"
    "}\n";

const GLchar *TEXTURE_FRAG_SRC =
    "#version 330 core\n"
    "uniform sampler2D tex;\n"
    "in vec2 v_tex_coords;\n"
    "out vec4 f_color;\n"
    "\n"
    "void main() {\n"
    "  f_color = texture(tex, v_tex_coords);\n"
    "}\n";

const GLchar *TEXT_VERT_SRC =
    "#version 330 core\n"
    "uniform vec2 screen;\n"
    "in vec2 position;\n"
    "in vec2 tex_coords;\n"
    "in vec4 color;\n"
    "\n"
    "out vec2 v_tex_coords;\n"
    "out vec4 v_color;\n"
    "\n"
    "void main() {\n"
    "  gl_Position = vec4(-1 + 2 * position.x / screen.x, 1 - 2 * position.y / screen.y, 0, 1);\n"
    "  v_tex_coords = tex_coords;\n"
    "  v_color = color;\n"
    "}\n";

const GLchar *TEXT_FRAG_SRC =
    "#version 330 core\n"
    "uniform sampler2D tex;\n"
    "in vec2 v_tex_coords;\n"
    "in vec4 v_color;\n"
    "out vec4 f_color;\n"
    "\n"
    "void main() {\n"
    "  f_color = v_color * vec4(1.0, 1.0, 1.0, texture(tex, v_tex_coords).r);\n"
    "}\n";

/* const GLchar* MODEL_VERT_SRC = */
/*     "#version 330 core\n" */
/*     "uniform vec2 screen;\n" */
/*     "uniform vec2 offset;\n" */
/*     "uniform float z_near;\n" */
/*     "uniform float z_far;\n" */
/*     "uniform float frustum_scale;\n" */
/*     "in vec4 position;\n" */
/*     "in vec4 color;\n" */
/*     "out vec4 v_color;\n" */
/*     "void main() {\n" */
/*     "  vec4 camera_pos = position + vec4(offset.x, offset.y, 0.0, 0.0);\n" */
/*     "  vec4 clip_pos;\n" */
/*     "  clip_pos.xy = camera_pos.xy * frustum_scale;\n" */
/*     "  clip_pos.z = (camera_pos.z * (z_near + z_far) / (z_near - z_far);\n" */
/*     "  clip_pos.z += 2 * z_near * z_far / (z_near - z_far);\n" */
/*     "  clip_pos.w = -camera_pos.z;\n" */
/*     "  gl_Position = clip_pos;\n" */
/*     "  v_color = color;\n" */
/*     "}\n"; */

const GLchar* MODEL_VERT_SRC =
    "#version 330 core\n"
    "layout (location = 0) in vec3 position;\n"
    "layout (location = 1) in vec3 normal;\n"
    "layout (location = 2) in vec4 color;\n"
    "uniform mat4 world;\n"
    "uniform mat4 view;\n"
    "uniform mat4 proj;\n"
    "out vec3 v_normal;\n"
    "out vec4 v_color;\n"
    "out vec3 fragment_position;\n"
    "void main() {\n"
    "    mat4 worldview = view * world;\n"
    "    v_normal = transpose(inverse(mat3(worldview))) * normal;\n"
    "    v_color = color;\n"
    "    // TODO check if world is correct to use below, original said model\n"
    "    fragment_position = vec3(world * vec4(position, 1.0));\n"
    "    // fragment_position = position;\n"
    "    gl_Position = proj * worldview * vec4(position, 1.0);\n"
    "}\n";

const GLchar* MODEL_FRAG_SRC =
    "#version 330 core\n"
    "in vec3 v_normal;\n"
    "in vec4 v_color;\n"
    "in vec3 fragment_position;\n"
    "struct Light {\n"
    "    vec3 position;\n"
    "    vec3 direction;\n"
    "    vec3 ambient;\n"
    "    vec3 diffuse;\n"
    "    vec3 specular;\n"
    "};\n"
    "uniform vec3 view_position;\n"
    "uniform Light light;\n"
    "// const vec3 LIGHT = vec3(1.0, 1.0, 1.0);\n"
    "void main() {\n"
    "    vec3 norm = normalize(v_normal);\n"
    "    vec3 light_direction = normalize(light.position - fragment_position);\n"
    "    vec3 view_direction = normalize(view_position - fragment_position);\n"
    "    vec3 reflection_direction = reflect(-light_direction, norm);\n"
    "    vec3 ambient = light.ambient; \n"
    "    vec3 diffuse = light.diffuse * max(dot(norm, light_direction), 0.0);\n"
    "    vec3 specular = light.specular * pow(max(dot(view_direction, reflection_direction), 0.0), 32);\n"
    "    gl_FragColor = vec4((ambient + diffuse + specular), 1.0) * v_color;\n"
    "}\n";

/* const GLchar *MODEL_FRAG_SRC = */
/*     "#version 330 core\n" */
/*     "in vec4 v_color;\n" */
/*     "out vec4 LFragment;\n" */
/*     "void main() {\n" */
/*     "  LFragment = v_color;\n" */
/*     "}\n"; */

