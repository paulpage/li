// Utils ------------------------------------------------------------

float gl_x(float x) {
    return -1.0f + 2.0f * x / state.window_width;
}

float gl_y(float y) {
    return 1.0f - 2.0f * y / state.window_height;
}

void get_rect_vertices(Rect rect, Point origin, float rotation, float out[8]) {
    float x = rect.x;
    float y = rect.y;
    float width = rect.width;
    float height = rect.height;
    float dx = -origin.x;
    float dy = -origin.y;

    if (rotation == 0.0f) {
        x = x + dx;
        y = y + dy;

        out[0] = gl_x(x);
        out[1] = gl_y(y);
        out[2] = gl_x(x + width);
        out[3] = gl_y(y);
        out[4] = gl_x(x);
        out[5] = gl_y(y + height);
        out[6] = gl_x(x + width);
        out[7] = gl_y(y + height);
    } else {
        float rcos = cosf(rotation);
        float rsin = sinf(rotation);

        out[0] = gl_x(x + dx*rcos - dy*rsin);
        out[1] = gl_y(y + dx*rsin + dy*rcos);
        out[2] = gl_x(x + (dx + width)*rcos - dy*rsin);
        out[3] = gl_y(y + (dx + width)*rsin + dy*rcos);
        out[4] = gl_x(x + dx*rcos - (dy + height)*rsin);
        out[5] = gl_y(y + dx*rsin + (dy + height)*rcos);
        out[6] = gl_x(x + (dx + width)*rcos - (dy + height)*rsin);
        out[7] = gl_y(y + (dx + width)*rsin + (dy + height)*rcos);
    }
}

// Shapes ------------------------------------------------------------

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

void app_draw_rotated_rects(Rect *rects, Color *colors, Point *origins, float *rotations, int count) {
    float v[8];
    for (int i = 0; i < count; i++) {
        float r = (float)colors[i].r / 255.0f;
        float g = (float)colors[i].g / 255.0f;
        float b = (float)colors[i].b / 255.0f;
        float a = (float)colors[i].a / 255.0f;
        get_rect_vertices(rects[i], origins[i], rotations[i], v);
        GLfloat vertex_data[24] = {
            v[0], v[1], r, g, b, a,
            v[2], v[3], r, g, b, a,
            v[4], v[5], r, g, b, a,
            v[6], v[7], r, g, b, a,
        };
        GLuint index_data[] = { 0, 1, 2, 1, 2, 3 };
        gl_draw_triangles(vertex_data, index_data, 4, 2);
    }
}

void app_draw_rect(Rect rect, Color color) {
    float rotation = 0.0f;
    Point origin = {0.0f, 0.0f};
    app_draw_rotated_rects(&rect, &color, &origin, &rotation, 1);
}

// Textures ------------------------------------------------------------

void app_draw_rotated_textures() {
}
