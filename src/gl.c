#include <stdio.h>

GLuint gl_create_shader(GLenum type, const GLchar *src) {
    GLuint id = glCreateShader(type);
    glShaderSource(id, 1, &src, NULL);
    glCompileShader(id);
    GLint success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        int len = 0;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
        char buffer[len]; // TODO do we have to zero this?
        glGetShaderInfoLog(id, len, NULL, buffer);
        printf("Error creating shader: %s\n", buffer);
        glDeleteShader(id);
        return 0;
    }
    return id;
}

GLuint gl_create_program(const char *vert_src, const char *frag_src) {
    GLuint vert = gl_create_shader(GL_VERTEX_SHADER, vert_src);
    GLuint frag = gl_create_shader(GL_FRAGMENT_SHADER, frag_src);
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
        char buffer[len];
        glGetProgramInfoLog(program, len, NULL, buffer);
        printf("Error creating program: %s\n", buffer);
        glDeleteProgram(program);
        return 0;
    }
    return program;
}
