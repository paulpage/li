// Imported ------------------------------------------------------------

float gl_x(float x) {
    return -1.0f + 2.0f * x / state.window_width;
}

float gl_y(float y) {
    return 1.0f - 2.0f * y / state.window_height;
}

void gl_draw_triangles(GLfloat vertex_data[], GLuint index_data[], int vertex_count, int triangle_count) {
    GLuint vbo, vao, ibo;
    GLint vertex_pos_location = -1, vertex_color_location = -1;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 6 * vertex_count * sizeof(GLfloat), vertex_data, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * triangle_count * sizeof(GLuint), index_data, GL_STATIC_DRAW);

    vertex_pos_location = glGetAttribLocation(state.tri_program_id, "position");
    vertex_color_location = glGetAttribLocation(state.tri_program_id, "color");
    if (vertex_pos_location == -1 || vertex_color_location == -1) {
        return;
    }

    glBindVertexArray(vao);
    glUseProgram(state.tri_program_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(vertex_pos_location);
    glVertexAttribPointer(vertex_pos_location, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), NULL);
    glEnableVertexAttribArray(vertex_color_location);
    glVertexAttribPointer(vertex_color_location, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(2*sizeof(GLfloat)));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glDrawElements(GL_TRIANGLES, 3 * triangle_count, GL_UNSIGNED_INT, NULL);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vao);

    glDisableVertexAttribArray(vertex_pos_location);
    glUseProgram(0);
}

void gl_draw_rect(Rect rect, Color color) {
    float x1 = gl_x(rect.x);
    float y1 = gl_y(rect.y);
    float x2 = gl_x(rect.x + rect.width);
    float y2 = gl_y(rect.y + rect.height);

    float r = (float)color.r / 255.0f;
    float g = (float)color.g / 255.0f;
    float b = (float)color.b / 255.0f;
    float a = (float)color.a / 255.0f;

    GLfloat vertex_data[24] = {
        x1, y1, r, g, b, a,
        x2, y1, r, g, b, a,
        x2, y2, r, g, b, a,
        x1, y2, r, g, b, a,
    };
    GLuint index_data[] = { 0, 1, 2, 3, 0, 2 };

    gl_draw_triangles(vertex_data, index_data, 4, 2);
}

// Imported ------------------------------------------------------------

void app_draw_rotated_rects(Rect *rects, Color *colors, float *rotations, int count) {
    for (int i = 0; i < count; i++) {
        gl_draw_rect(rects[i], colors[i]);
    }
}

void app_draw_rect(Rect rect, Color color) {
    float rotation = 0.0f;
    app_draw_rotated_rects(&rect, &color, &rotation, 1);
}

