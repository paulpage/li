#ifndef _PLATFORM_H
#define _PLATFORM_H
#include <stdint.h>
#include <stdbool.h>

#include "glad/gl.h"
#include "SDL.h"
#include "SDL_opengl.h"

#include "handmade_math.h"

typedef struct Rect {
    float x, y, width, height;
} Rect;

typedef struct Point {
    float x, y;
} Point;

typedef struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} Color;

typedef struct Texture {
    float width;
    float height;
    uint32_t id;
} Texture;

typedef struct App {

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

} App;

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
void app_draw_rotated_text(char **texts, Point *positions, float *sizes, Color *colors, Point *origins, float *rotations, int count);
void app_draw_text(char *text, Point pos, float size, Color color);

void app_draw_model(float *vertices, unsigned int *indices, int vertex_count, int index_count, Mat4 world, Mat4 view, Mat4 proj, Vec3 view_position, float light[15], Color color);

uint64_t app_get_performance_counter();
uint64_t app_get_performance_frequency();

#endif // _PLATFORM_H
