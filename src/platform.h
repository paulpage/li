#ifndef _PLATFORM_H
#define _PLATFORM_H
#include <stdint.h>

#include "SDL.h"
#include <GL/glew.h>
#include <GL/glu.h>
#include "SDL_opengl.h"

struct Rect {
    float x, y, width, height;
};

struct Point {
    float x, y;
};

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

struct Texture {
    float width;
    float height;
    uint32_t id;
};

struct App {

    Point window;

    Point mouse;
    Point scroll;
    bool mouse_left_down;
    bool mouse_right_down;
    bool mouse_middle_down;
    bool mouse_left_pressed;
    bool mouse_right_pressed;
    bool mouse_middle_pressed;

    bool should_quit;

};

bool app_init(const char *title, int window_width, int window_height);
void app_quit();

void app_update(App *app);

void app_clear(Color color);
void app_present();

void app_draw_rotated_rects(Rect *rects, Color *colors, Point *origins, float *rotations, int count);
void app_draw_rect(Rect rect, Color color);

void app_draw_rotated_textures(Texture texture, Rect *src_rects, Rect *dest_rects, Point *origins, float *rotations, int count);
void app_draw_texture(Texture texture, Rect src_rect, Rect dest_rect);

Texture app_load_texture(unsigned char *data, int width, int height);
Texture app_load_texture_from_file(const char *filename);



void app_load_font(const char *filename);
void app_draw_rotated_text(const char *texts, Point positions, float sizes, Color colors, Point *origins, float *rotations, int count);
void app_draw_text(const char *text, Point pos, float size, Color color);

GLuint gl_create_shader(GLenum type, const GLchar *src);
GLuint gl_create_program(const char *vert_src, const char *frag_src);

uint8_t *read_file(const char *filename, long *out_len);

#endif // _PLATFORM_H
