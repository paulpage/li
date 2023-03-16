#include <stdio.h>

GLuint gl_create_shader(GLenum type, Str src) {
    GLuint id = glCreateShader(type);
    glShaderSource(id, 1, (const GLchar**)(&src.s), (GLint*)(&src.len));
    glCompileShader(id);
    GLint success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        int len = 0;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
        char *buffer = malloc(sizeof(char) * len);
        glGetShaderInfoLog(id, len, NULL, buffer);
        printf("Error creating shader: %s\n", buffer);
        glDeleteShader(id);
        return 0;
    }
    return id;
}

GLuint gl_create_program(const char *filename) {

    Str data = read_file(filename);

    Str vs_str = S("#pragma vs");
    Str fs_str = S("#pragma fs");
    Str end_str = S("#pragma end");

    int vs_start = -1;
    int vs_end = -1;
    int fs_start = -1;
    int fs_end = -1;

    for (int i = 0; i < data.len; i++) {

        if (data.s[i] == '#') {

            Str o = str_off(data, i);

            if (str_startswith(o, vs_str)) {
                vs_start = i + vs_str.len;
            } else if (str_startswith(o, fs_str)) {
                fs_start = i + fs_str.len;
            } else if (str_startswith(o, end_str)) {
                if (vs_end == -1) {
                    vs_end = i - 1;
                } else {
                    fs_end = i - 1;
                }
            }
        }
    }

    Str vs_src = str_cut(data, vs_start, vs_end);
    Str fs_src = str_cut(data, fs_start, fs_end);

    GLuint vert = gl_create_shader(GL_VERTEX_SHADER, vs_src);
    GLuint frag = gl_create_shader(GL_FRAGMENT_SHADER, fs_src);
    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);
    glDeleteShader(vert);
    glDeleteShader(frag);
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        int len = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
        char *buffer = malloc(sizeof(char) * len);
        glGetProgramInfoLog(program, len, NULL, buffer);
        printf("Error creating program: %s\n", buffer);
        glDeleteProgram(program);
        return 0;
    }
    return program;
}
