// Utils ------------------------------------------------------------

#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "stb_image.h"

typedef struct GlVertex {
    Point position;
    Color color;
} GlVertex;

typedef struct GlTexVertex {
    Point position;
    Point uv;
} GlTexVertex;

typedef struct DrawState {
    int last_rect_count;
    GlVertex *tri_vertex_data;
    GLuint *tri_index_data;
    GLuint tri_vao;
    GLuint tri_vbo;
    GLuint tri_ibo;
    int last_texture_count;
    GlTexVertex *tex_vertex_data;
    GLuint *tex_index_data;
    GLuint tex_vao;
    GLuint tex_vbo;
    GLuint tex_ibo;
    stbtt_pack_context font_pack_ctx;
    stbtt_packedchar *font_packed_chars;
    GLuint font_texture_id;
    int last_text_count;
    GlTexVertex *text_vertex_data;
    Color *text_color_data;
    int *text_index_data;
    GLuint font_vao;
    GLuint font_vert_vbo;
    GLuint font_color_vbo;
    GLuint font_ibo;
} DrawState;
static DrawState draw_state = {0};

static float gl_x(float x) {
    return -1.0f + 2.0f * x / state.window_width;
}

static float gl_y(float y) {
    return 1.0f - 2.0f * y / state.window_height;
}

typedef struct GlRectVertices {
    Point p1, p2, p3, p4;
} GlRectVertices;

static GlRectVertices get_new_rect_vertices(Rect rect, Point origin, float rotation) {
    GlRectVertices out = {0};
    if (rotation == 0.0f) {
        float x = rect.x - origin.x;
        float y = rect.y - origin.y;
        out.p1.x = x;
        out.p1.y = y;
        out.p2.x = x + rect.width;
        out.p2.y = y;
        out.p3.x = x;
        out.p3.y = y + rect.height;
        out.p4.x = x + rect.width;
        out.p4.y = y + rect.height;
    } else {
        float rcos = cosf(rotation);
        float rsin = sinf(rotation);
        float dx = -origin.x;
        float dy = -origin.y;
        out.p1.x = rect.x + dx*rcos - dy*rsin;
        out.p1.y = rect.y + dx*rsin + dy*rcos;
        out.p2.x = rect.x + (dx + rect.width)*rcos - dy*rsin;
        out.p2.y = rect.y + (dx + rect.width)*rsin + dy*rcos;
        out.p3.x = rect.x + dx*rcos - (dy + rect.height)*rsin;
        out.p3.y = rect.y + dx*rsin + (dy + rect.height)*rcos;
        out.p4.x = rect.x + (dx + rect.width)*rcos - (dy + rect.height)*rsin;
        out.p4.y = rect.y + (dx + rect.width)*rsin + (dy + rect.height)*rcos;
    }
    return out;
}

static void get_rect_vertices(Rect rect, Point origin, float rotation, float *out) {

    float x = rect.x;
    float y = rect.y;
    float width = rect.width;
    float height = rect.height;
    float dx = -origin.x;
    float dy = -origin.y;

    if (rotation == 0.0f) {
        x = x + dx;
        y = y + dy;

        out[0] = x;
        out[1] = y;
        out[2] = x + width;
        out[3] = y;
        out[4] = x;
        out[5] = y + height;
        out[6] = x + width;
        out[7] = y + height;
    } else {
        float rcos = cosf(rotation);
        float rsin = sinf(rotation);

        out[0] = x + dx*rcos - dy*rsin;
        out[1] = y + dx*rsin + dy*rcos;
        out[2] = x + (dx + width)*rcos - dy*rsin;
        out[3] = y + (dx + width)*rsin + dy*rcos;
        out[4] = x + dx*rcos - (dy + height)*rsin;
        out[5] = y + dx*rsin + (dy + height)*rcos;
        out[6] = x + (dx + width)*rcos - (dy + height)*rsin;
        out[7] = y + (dx + width)*rsin + (dy + height)*rcos;
    }
}

// Shapes ------------------------------------------------------------

static void gl_draw_triangles(GlVertex vertex_data[], GLuint index_data[], int vertex_count, int triangle_count) {

    if (!draw_state.tri_vao) {
        glGenVertexArrays(1, &draw_state.tri_vao);
    }

    glGenBuffers(1, &draw_state.tri_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, draw_state.tri_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(GlVertex), vertex_data, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &draw_state.tri_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, draw_state.tri_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * triangle_count * sizeof(GLuint), index_data, GL_STATIC_DRAW);

    GLint position_location = -1, color_location = -1;
    position_location = glGetAttribLocation(state.tri_program_id, "position");
    color_location = glGetAttribLocation(state.tri_program_id, "color");
    if (position_location == -1 || color_location == -1) {
        return;
    }

    glBindVertexArray(draw_state.tri_vao);
    glUseProgram(state.tri_program_id);
    glBindBuffer(GL_ARRAY_BUFFER, draw_state.tri_vbo);

    glEnableVertexAttribArray(position_location);
    glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE, sizeof(GlVertex), 0);
    glEnableVertexAttribArray(color_location);
    glVertexAttribPointer(color_location, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(GlVertex), (void*)(2 * sizeof(float)));

    GLint uniform = glGetUniformLocation(state.tri_program_id, "screen");
    glUniform2f(uniform, state.window_width, state.window_height);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, draw_state.tri_ibo);
    glDrawElements(GL_TRIANGLES, 3 * triangle_count, GL_UNSIGNED_INT, NULL);

    glDisableVertexAttribArray(position_location);
    glDisableVertexAttribArray(color_location);
    glUseProgram(0);

    glBindVertexArray(0);
    glDeleteBuffers(1, &draw_state.tri_vbo);
    glDeleteBuffers(1, &draw_state.tri_ibo);
}

static void gl_draw_textures(Texture texture, GlTexVertex vertex_data[], GLuint index_data[], int vertex_count, int triangle_count) {

    GLint position_location = -1, tex_coords_location = -1;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    if (!draw_state.tri_vao) {
        glGenVertexArrays(1, &draw_state.tex_vao);
    }
    glGenBuffers(1, &draw_state.tex_vbo);
    glGenBuffers(1, &draw_state.tex_ibo);

    glBindBuffer(GL_ARRAY_BUFFER, draw_state.tex_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(GlTexVertex), vertex_data, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, draw_state.tex_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * triangle_count * sizeof(GLuint), index_data, GL_STATIC_DRAW);

    position_location = glGetAttribLocation(state.texture_program_id, "position");
    tex_coords_location = glGetAttribLocation(state.texture_program_id, "tex_coords");
    if (position_location == -1 || tex_coords_location == -1) {
        return;
    }

    glBindVertexArray(draw_state.tex_vao);
    glUseProgram(state.texture_program_id);
    glBindBuffer(GL_ARRAY_BUFFER, draw_state.tex_vbo);

    glEnableVertexAttribArray(position_location);
    glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE, sizeof(GlTexVertex), NULL);
    glEnableVertexAttribArray(tex_coords_location);
    glVertexAttribPointer(tex_coords_location, 2, GL_FLOAT, GL_FALSE, sizeof(GlTexVertex), (void*)(2 * sizeof(GLfloat)));

    GLint uniform_screen = glGetUniformLocation(state.texture_program_id, "screen");
    glUniform2f(uniform_screen, state.window_width, state.window_height);

    GLint uniform_tex = glGetUniformLocation(state.texture_program_id, "tex");
    glUseProgram(state.texture_program_id);
    glUniform1i(uniform_tex, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, draw_state.tex_ibo);
    glDrawElements(GL_TRIANGLES, 3 * triangle_count, GL_UNSIGNED_INT, NULL);

    glBindVertexArray(0);
    glDisableVertexAttribArray(position_location);
    glDisableVertexAttribArray(tex_coords_location);
    glUseProgram(0);

    glDeleteBuffers(1, &draw_state.tex_vbo);
    glDeleteBuffers(1, &draw_state.tex_ibo);
}

void app_draw_rotated_rects(Rect *rects, Color *colors, Point *origins, float *rotations, int count) {

    float v[8];

    if (count != draw_state.last_rect_count) {
        if (draw_state.tri_vertex_data) {
            printf("FREE VERTEX\n");
            free(draw_state.tri_vertex_data);
            draw_state.tri_vertex_data = 0;
        }
        if (draw_state.tri_index_data) {
            printf("FREE INDEX\n");
            free(draw_state.tri_index_data);
            draw_state.tri_index_data = 0;
        }
        printf("MALLOC\n");
        draw_state.tri_vertex_data = (GlVertex*)malloc(sizeof(GlVertex) * count * 4);
        draw_state.tri_index_data = (GLuint*)malloc(sizeof(GLuint) * count * 6);

        memset(draw_state.tri_index_data, 0, sizeof(GLuint) * count * 6);
        for (int i = 0; i < count; i++) {
            draw_state.tri_index_data[i * 6 + 0] = 0 + i * 4;
            draw_state.tri_index_data[i * 6 + 1] = 1 + i * 4;
            draw_state.tri_index_data[i * 6 + 2] = 2 + i * 4;
            draw_state.tri_index_data[i * 6 + 3] = 1 + i * 4;
            draw_state.tri_index_data[i * 6 + 4] = 2 + i * 4;
            draw_state.tri_index_data[i * 6 + 5] = 3 + i * 4;
        }
    }
    draw_state.last_rect_count = count;

    uint64_t start = app_get_performance_counter();

    memset(draw_state.tri_vertex_data, 0, sizeof(GlVertex) * count * 4);
    GlRectVertices out;
    for (int i = 0; i < count; i++) {
        out = get_new_rect_vertices(rects[i], origins[i], rotations[i]);
        draw_state.tri_vertex_data[i * 4 + 0].position = out.p1;
        draw_state.tri_vertex_data[i * 4 + 0].color = colors[i];
        draw_state.tri_vertex_data[i * 4 + 1].position = out.p2;
        draw_state.tri_vertex_data[i * 4 + 1].color = colors[i];
        draw_state.tri_vertex_data[i * 4 + 2].position = out.p3;
        draw_state.tri_vertex_data[i * 4 + 2].color = colors[i];
        draw_state.tri_vertex_data[i * 4 + 3].position = out.p4;
        draw_state.tri_vertex_data[i * 4 + 3].color = colors[i];
    }

    uint64_t end = app_get_performance_counter();
    float elapsed = (float)(end - start) / (float)app_get_performance_frequency() * 1000.0f;
    /* printf("loop time: %.4f ms\n", elapsed); */

    gl_draw_triangles(draw_state.tri_vertex_data, draw_state.tri_index_data, 4 * count, 2 * count);
}

void app_draw_rect(Rect rect, Color color) {
    Point origin = {0.0f, 0.0f};
    float rotation = 0.0f;
    app_draw_rotated_rects(&rect, &color, &origin, &rotation, 1);
}

// Textures ------------------------------------------------------------

void app_draw_rotated_textures(Texture texture, Rect *src_rects, Rect *dest_rects, Point *origins, float *rotations, int count) {

    if (count != draw_state.last_texture_count) {
        if (draw_state.tex_vertex_data) {
            free(draw_state.tex_vertex_data);
            draw_state.tex_vertex_data = 0;
        }
        if (draw_state.tex_index_data) {
            free(draw_state.tex_index_data);
            draw_state.tex_index_data = 0;
        }
        draw_state.tex_vertex_data = (GlTexVertex*)malloc(sizeof(GlTexVertex) * count * 4);
        draw_state.tex_index_data = (GLuint*)malloc(sizeof(GLuint) * count * 6);

        memset(draw_state.tex_index_data, 0, sizeof(GLuint) * count * 6);
        for (int i = 0; i < count; i++) {
            draw_state.tex_index_data[i * 6 + 0] = 0 + i * 4;
            draw_state.tex_index_data[i * 6 + 1] = 1 + i * 4;
            draw_state.tex_index_data[i * 6 + 2] = 2 + i * 4;
            draw_state.tex_index_data[i * 6 + 3] = 1 + i * 4;
            draw_state.tex_index_data[i * 6 + 4] = 2 + i * 4;
            draw_state.tex_index_data[i * 6 + 5] = 3 + i * 4;
        }
    }

    draw_state.last_texture_count = count;

    memset(draw_state.tex_vertex_data, 0, sizeof(GlTexVertex) * count * 4);
    GlRectVertices out;
    for (int i = 0; i < count; i++) {
        out = get_new_rect_vertices(dest_rects[i], origins[i], rotations[i]);
        float u0 = src_rects[i].x / texture.width;
        float u1 = (src_rects[i].x + src_rects[i].width) / texture.width;
        float v0 = src_rects[i].y / texture.height;
        float v1 = (src_rects[i].y + src_rects[i].height) / texture.height;

        draw_state.tex_vertex_data[i * 4 + 0].position = out.p1;
        draw_state.tex_vertex_data[i * 4 + 0].uv = (Point){u0, v0};
        draw_state.tex_vertex_data[i * 4 + 1].position = out.p2;
        draw_state.tex_vertex_data[i * 4 + 1].uv = (Point){u1, v0};
        draw_state.tex_vertex_data[i * 4 + 2].position = out.p3;
        draw_state.tex_vertex_data[i * 4 + 2].uv = (Point){u0, v1};
        draw_state.tex_vertex_data[i * 4 + 3].position = out.p4;
        draw_state.tex_vertex_data[i * 4 + 3].uv = (Point){u1, v1};
    }

    gl_draw_textures(texture, draw_state.tex_vertex_data, draw_state.tex_index_data, 4 * count, 2 * count);

    /* float xy[8]; */
    /* for (int i = 0; i < count; i++) { */
    /*     get_rect_vertices(dest_rects[i], origins[i], rotations[i], xy); */

    /*     float u0 = src_rects[i].x / texture.width; */
    /*     float u1 = (src_rects[i].x + src_rects[i].width) / texture.width; */
    /*     float v0 = src_rects[i].y / texture.height; */
    /*     float v1 = (src_rects[i].y + src_rects[i].height) / texture.height; */

    /*     GLfloat vertex_data[16] = { */
    /*         xy[0], xy[1], u0, v0, */
    /*         xy[2], xy[3], u1, v0, */
    /*         xy[4], xy[5], u0, v1, */
    /*         xy[6], xy[7], u1, v1, */
    /*     }; */
    /*     GLuint index_data[] = { 0, 1, 2, 1, 2, 3 }; */
    /*     gl_draw_textures(texture, vertex_data, index_data, 4, 2); */
    /*     // TODO */
    /* } */
}

void app_draw_texture(Texture texture, Rect src_rect, Rect dest_rect) {
    Point origin = {0.0f, 0.0f};
    float rotation = 0.0f;
    app_draw_rotated_textures(texture, &src_rect, &dest_rect, &origin, &rotation, 1);
}

Texture app_load_texture(unsigned char *data, int width, int height) {
    GLuint id = 0;
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return (Texture){(float)width, (float)height, id};
}

Texture app_load_texture_from_file(const char *filename) {
    int width = 0, height = 0;
    unsigned char *data = stbi_load(filename, &width, &height, NULL, 4);
    Texture texture = app_load_texture(data, width, height);
    stbi_image_free(data);
    return texture;
}

void app_load_font(const char *filename) {
    state.font = (Font){0};
    state.font.buf = read_file(filename, &state.font.buflen);
    stbtt_InitFont(&state.font.info, state.font.buf, 0);
    int x0, y0, x1, y1;
    /* stbtt_GetFontBoundingBox(&state.font.info, &x0, &y0, &x1, &y1); */
    /* state.font.bbox_width = x1 - x0; */
    /* state.font.bbox_height = y1 - y0; */

    unsigned char *temp_bitmap = malloc(512 * 512);
   stbtt_PackBegin(&draw_state.font_pack_ctx, temp_bitmap, 512, 512, 0, 1, NULL);
    // TODO properly handle font sizes, and get it from stbtt_ScaleForPixelHeight to pass into this function
    draw_state.font_packed_chars = malloc(sizeof(stbtt_packedchar) * 256-32);
    stbtt_PackFontRange(&draw_state.font_pack_ctx, state.font.buf, 0, 40.0f, 32, 256-32, draw_state.font_packed_chars);
    glGenTextures(1, &draw_state.font_texture_id);
    glBindTexture(GL_TEXTURE_2D, draw_state.font_texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, temp_bitmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void gl_draw_text(GLuint texture, GlTexVertex vertex_data[], GLuint index_data[], Color color_data[], int vertex_count, int triangle_count) {

    GLint position_location = glGetAttribLocation(state.text_program_id, "position");
    GLint tex_coords_location = glGetAttribLocation(state.text_program_id, "tex_coords");
    GLint color_location = glGetAttribLocation(state.text_program_id, "color");
    if (position_location == -1 || tex_coords_location == -1 || color_location == -1) {
        return;
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    if (!draw_state.font_vao) {
        glGenVertexArrays(1, &draw_state.font_vao);
    }
    glGenBuffers(1, &draw_state.font_vert_vbo);
    glGenBuffers(1, &draw_state.font_color_vbo);
    glGenBuffers(1, &draw_state.font_ibo);

    glBindBuffer(GL_ARRAY_BUFFER, draw_state.font_vert_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(GlTexVertex), vertex_data, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, draw_state.font_color_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(Color), color_data, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, draw_state.font_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * triangle_count * sizeof(GLuint), index_data, GL_STATIC_DRAW);

    glBindVertexArray(draw_state.font_vao);
    glUseProgram(state.text_program_id);

    glBindBuffer(GL_ARRAY_BUFFER, draw_state.font_vert_vbo);
    glEnableVertexAttribArray(position_location);
    glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE, sizeof(GlTexVertex), NULL);
    glEnableVertexAttribArray(tex_coords_location);
    glVertexAttribPointer(tex_coords_location, 2, GL_FLOAT, GL_FALSE, sizeof(GlTexVertex), (void*)(2 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, draw_state.font_color_vbo);
    glEnableVertexAttribArray(color_location);
    glVertexAttribPointer(color_location, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Color), NULL);

    glUseProgram(state.text_program_id);
    GLint uniform_screen = glGetUniformLocation(state.text_program_id, "screen");
    glUniform2f(uniform_screen, state.window_width, state.window_height);

    GLint uniform_tex = glGetUniformLocation(state.text_program_id, "tex");
    glUniform1i(uniform_tex, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, draw_state.font_ibo);
    glDrawElements(GL_TRIANGLES, 3 * triangle_count, GL_UNSIGNED_INT, NULL);

    glBindVertexArray(0);
    glDisableVertexAttribArray(position_location);
    glDisableVertexAttribArray(tex_coords_location);
    glDisableVertexAttribArray(color_location);
    glUseProgram(0);

    glDeleteBuffers(1, &draw_state.font_vert_vbo);
    glDeleteBuffers(1, &draw_state.font_color_vbo);
    glDeleteBuffers(1, &draw_state.font_ibo);
}

void app_draw_rotated_text(char **texts, Point *positions, float *sizes, Color *colors, Point *origins, float *rotations, int count) {

    int char_count = 0;
    for (int i = 0; i < count; i++) {
        char_count += strlen(texts[i]);
    }

    if (char_count != draw_state.last_text_count) {
        if (draw_state.text_vertex_data) {
            free(draw_state.text_vertex_data);
            draw_state.text_vertex_data = 0;
        }
        if (draw_state.text_index_data) {
            free(draw_state.text_index_data);
            draw_state.text_index_data = 0;
        }
        draw_state.text_vertex_data = (GlTexVertex*)malloc(sizeof(GlTexVertex) * char_count * 4);
        draw_state.text_color_data = (Color*)malloc(sizeof(Color) * char_count * 4);
        draw_state.text_index_data = (GLuint*)malloc(sizeof(GLuint) * char_count * 6);

        memset(draw_state.text_index_data, 0, sizeof(GLuint) * char_count * 6);
        for (int i = 0; i < char_count; i++) {
            draw_state.text_index_data[i * 6 + 0] = 0 + i * 4;
            draw_state.text_index_data[i * 6 + 1] = 1 + i * 4;
            draw_state.text_index_data[i * 6 + 2] = 2 + i * 4;
            draw_state.text_index_data[i * 6 + 3] = 1 + i * 4;
            draw_state.text_index_data[i * 6 + 4] = 2 + i * 4;
            draw_state.text_index_data[i * 6 + 5] = 3 + i * 4;
        }
    }
    draw_state.last_text_count = char_count;

    memset(draw_state.text_vertex_data, 0, sizeof(GlTexVertex) * char_count * 4);
    memset(draw_state.text_color_data, 0, sizeof(Color) * char_count * 4);
    GlRectVertices out;

    int ci = 0;
    for (int i = 0; i < count; i++) {

        int ch = 0;
        float posx = positions[i].x;
        float posy = positions[i].y;
        while (texts[i][ch]) {

            stbtt_aligned_quad q;
            stbtt_GetPackedQuad(draw_state.font_packed_chars, 512, 512, (int)texts[i][ch]-32, &posx, &posy, &q, 0);

            // TODO respect rotation
            GlTexVertex vertex_data[4];
            /* GLuint index_data[6]; */

            Point origin = (Point){origins[i].x - (q.x0 - positions[i].x), origins[i].y - (q.y0 - positions[i].y)};
            Rect rect = (Rect){q.x0 + origin.x, q.y0 + origin.y, (q.x1 - q.x0), (q.y1 - q.y0)};
            out = get_new_rect_vertices(rect, origin, rotations[i]);

            /* GLuint index_data[] = { 0, 1, 2, 1, 2, 3 }; */

            draw_state.text_vertex_data[ci * 4 + 0].position = out.p1;
            draw_state.text_vertex_data[ci * 4 + 0].uv =       (Point){q.s0, q.t0};
            draw_state.text_vertex_data[ci * 4 + 1].position = out.p2;
            draw_state.text_vertex_data[ci * 4 + 1].uv =       (Point){q.s1, q.t0};
            draw_state.text_vertex_data[ci * 4 + 2].position = out.p3;
            draw_state.text_vertex_data[ci * 4 + 2].uv =       (Point){q.s0, q.t1};
            draw_state.text_vertex_data[ci * 4 + 3].position = out.p4;
            draw_state.text_vertex_data[ci * 4 + 3].uv =       (Point){q.s1, q.t1};

            /* Color color_data[4] = {0}; */
            draw_state.text_color_data[ci * 4 + 0] = colors[i];
            draw_state.text_color_data[ci * 4 + 1] = colors[i];
            draw_state.text_color_data[ci * 4 + 2] = colors[i];
            draw_state.text_color_data[ci * 4 + 3] = colors[i];

            /* gl_draw_text(draw_state.font_texture_id, vertex_data, index_data, color_data, 4, 2); */

            ch++;
            ci++;
        }
    }

    gl_draw_text(draw_state.font_texture_id, draw_state.text_vertex_data, draw_state.text_index_data, draw_state.text_color_data, 4 * char_count, 2 * char_count);


    /* Font font = state.font; */

    /* for (int i = 0; i < count; i++) { */

    /*     // TODO respect font size */

    /*     int buf_width = (int)sizes[i] * strlen(texts[i]); */
    /*     int buf_height = (int)sizes[i]; */
    /*     unsigned char *buf = (unsigned char *)calloc(buf_width * buf_height, sizeof(unsigned char)); */

    /*     int ch = 0; */
    /*     float xpos = 2.0f; */

    /*     float scale = stbtt_ScaleForPixelHeight(&state.font.info, (int)sizes[i]); */
    /*     int ascent; */
    /*     stbtt_GetFontVMetrics(&state.font.info, &ascent, 0, 0); */
    /*     int baseline = (int)(ascent * scale); */

    /*     while (texts[i][ch]) { */
    /*         int advance, lsb, x0, y0, x1, y1; */
    /*         float x_shift = xpos - (float)floor(xpos); */
    /*         stbtt_GetCodepointHMetrics(&font.info, texts[i][ch], &advance, &lsb); */
    /*         stbtt_GetCodepointBitmapBoxSubpixel(&font.info, texts[i][ch], scale, scale, x_shift, 0, &x0, &y0, &x1, &y1); */

    /*         // TODO don't overwrite old data */

    /*         stbtt_MakeCodepointBitmapSubpixel(&font.info, &buf[((baseline + y0) * buf_width + (int)xpos + x0)], x1 - x0, y1 - y0, buf_width, scale, scale, x_shift, 0, texts[i][ch]); */

    /*         xpos += (advance * scale); */
    /*         if (texts[i][ch + 1]) { */
    /*             xpos += scale * stbtt_GetCodepointKernAdvance(&font.info, texts[i][ch], texts[i][ch + 1]); */
    /*         } */
    /*         ch++; */
    /*     } */

    /*     /1* printf("bbox: %d x %d\n", font.bbox_width, font.bbox_height); *1/ */
    /*     unsigned char *tex_buf = (unsigned char *)malloc(buf_width * buf_height * 4); */
    /*     for (int j = 0; j < buf_width * buf_height; j++) { */
    /*         tex_buf[j * 4 + 0] = colors[i].r; */
    /*         tex_buf[j * 4 + 1] = colors[i].g; */
    /*         tex_buf[j * 4 + 2] = colors[i].b; */
    /*         tex_buf[j * 4 + 3] = buf[j]; */
    /*     } */

    /*     Texture texture = app_load_texture(tex_buf, buf_width, buf_height); */

    /*     free(buf); */
    /*     free(tex_buf); */

    /*     Rect src_rect = {0, 0, (float)buf_width, (float)buf_height}; */
    /*     Rect dest_rect = {positions[i].x, positions[i].y, (float)buf_width, (float)buf_height}; */
    /*     app_draw_rotated_textures(texture, &src_rect, &dest_rect, &(origins[i]), &(rotations[i]), 1); */
    /* } */
}

void app_draw_text(char *text, Point pos, float size, Color color) {
    Point origin = {0.0f, 0.0f};
    float rotation = 0.0f;
    app_draw_rotated_text(&text, &pos, &size, &color, &origin, &rotation, 1);
}
