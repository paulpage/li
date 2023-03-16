typedef struct GlModelVertex {
    float x, y, z, w;
    Color color;
} GlModelVertex;

void app_draw_model(float *vertices, unsigned int *indices, int vertex_count, int index_count, Mat4 world, Mat4 view, Mat4 proj, Vec3 view_position, float light[15], Color color) {

    GlModelVertex *vertex_data = malloc(sizeof(GlModelVertex) * vertex_count);
    for (int i = 0; i < vertex_count; i++) {
        vertex_data[i] = (GlModelVertex){
            vertices[i * 3 + 0],
            vertices[i * 3 + 1],
            vertices[i * 3 + 2],
            1.0f,
            color
        };
    }

    int triangle_count = index_count / 3;

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

    GLuint vao, vbo, ibo;

    glGenVertexArrays(1, &vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(GlModelVertex), vertex_data, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * triangle_count * sizeof(GLuint), indices, GL_STATIC_DRAW);

    GLint position_location = -1, color_location = -1;
    position_location = glGetAttribLocation(state.tri_program_id, "position");
    color_location = glGetAttribLocation(state.tri_program_id, "color");
    if (position_location == -1 || color_location == -1) {
        return;
    }

    glBindVertexArray(vao);
    glUseProgram(state.model_program_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glEnableVertexAttribArray(position_location);
    glVertexAttribPointer(position_location, 4, GL_FLOAT, GL_FALSE, sizeof(GlModelVertex), 0);
    glEnableVertexAttribArray(color_location);
    glVertexAttribPointer(color_location, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(GlVertex), (void*)(4 * sizeof(float)));

    /* GLint uniform_screen = glGetUniformLocation(state.model_program_id, "screen"); */
    /* GLint uniform_offset = glGetUniformLocation(state.model_program_id, "offset"); */
    /* GLint uniform_z_near = glGetUniformLocation(state.model_program_id, "z_near"); */
    /* GLint uniform_z_far = glGetUniformLocation(state.model_program_id, "z_far"); */
    /* GLint uniform_frustum_scale = glGetUniformLocation(state.model_program_id, "frustum_scale"); */

    GLint uniform_world = glGetUniformLocation(state.model_program_id, "world");
    GLint uniform_view = glGetUniformLocation(state.model_program_id, "view");
    GLint uniform_proj = glGetUniformLocation(state.model_program_id, "proj");
    GLint uniform_view_position = glGetUniformLocation(state.model_program_id, "view_position");
    GLint uniform_light_position = glGetUniformLocation(state.model_program_id, "light.position");
    GLint uniform_light_direction = glGetUniformLocation(state.model_program_id, "light.direction");
    GLint uniform_light_ambient = glGetUniformLocation(state.model_program_id, "light.ambient");
    GLint uniform_light_diffuse = glGetUniformLocation(state.model_program_id, "light.diffuse");
    GLint uniform_light_specular = glGetUniformLocation(state.model_program_id, "light.specular");

    GLfloat offset[2] = {0.0f, 0.0f};

    /* glUniform2f(uniform_screen, state.window_width, state.window_height); */
    /* glUniform2f(uniform_offset, offset[0], offset[1]); */
    /* glUniform1f(uniform_z_near, 1.0f); */
    /* glUniform1f(uniform_z_far, 3.0f); */
    /* glUniform1f(uniform_frustum_scale, 1.0f); */

    /* glUniformMatrix4fv(uniform_world, 1, GL_FALSE, world.points); */
    /* glUniformMatrix4fv(uniform_view, 1, GL_FALSE, view.points); */
    /* glUniformMatrix4fv(uniform_proj, 1, GL_FALSE, proj.points); */
    glUniformMatrix4fv(uniform_world, 1, GL_TRUE, &world.elements[0][0]);
    glUniformMatrix4fv(uniform_view, 1, GL_TRUE, &view.elements[0][0]);
    glUniformMatrix4fv(uniform_proj, 1, GL_TRUE, &proj.elements[0][0]);
    glUniform3f(uniform_view_position, view_position.x, view_position.y, view_position.z);
    // TODO might have to switch row/col
    glUniform3f(uniform_light_position, light[0], light[1], light[2]);
    glUniform3f(uniform_light_direction, light[3], light[4], light[5]);
    glUniform3f(uniform_light_ambient, light[6], light[7], light[8]);
    glUniform3f(uniform_light_diffuse, light[9], light[10], light[11]);
    glUniform3f(uniform_light_specular, light[12], light[13], light[14]);

    /* SDL_TriggerBreakpoint(); */


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glDrawElements(GL_TRIANGLES, 3 * triangle_count, GL_UNSIGNED_INT, NULL);

    glDisableVertexAttribArray(position_location);
    glDisableVertexAttribArray(color_location);
    glUseProgram(0);
    glBindVertexArray(0);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
    glDeleteVertexArrays(1, &vao);

    glDisable(GL_CULL_FACE);
}
