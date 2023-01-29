#include <stdint.h>

#include "SDL.h"
#include <GL/glew.h>
#include <GL/glu.h>
#include "SDL_opengl.h"



struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

struct Point {
    float x;
    float y;
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

App app_init(const char *title, int window_width, int window_height);
void app_quit();

void app_update(App *app);

void app_clear(App *app, Color color);
void app_present(App *app);

void app_draw_rect(App *app, Rect rect, Color color);

GLuint gl_create_shader(GLenum type, const GLchar *src);
GLuint gl_create_program(const char *vert_src, const char *frag_src);
