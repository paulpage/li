#include "platform.h"

#include "SDL_opengl.h"

#include <GL/glew.h>
#include <GL/glu.h>

#include <stdio.h>

#define STB_RECT_PACK_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include "stb_rect_pack.h"
#include "stb_truetype.h"

struct Font {
    stbtt_fontinfo info;
    unsigned char *buf;
    long buflen;
    /* int bbox_width; */
    /* int bbox_height; */
};

struct State {
    float window_width;
    float window_height;

    SDL_Window *window;
    SDL_GLContext context;
    GLuint tri_program_id;
    GLuint texture_program_id;
    /* GLuint basic_program_id; */

    Font font;
};

static State state = {0};

#include "drawing.cpp"
#include "shaders.cpp"

void GLAPIENTRY gl_error_callback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam
) {
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}

bool app_init(const char *title, int window_width, int window_height) {

    state.window_width = (float)window_width;
    state.window_height = (float)window_height;

    // Window
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    /* SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); */
    /* SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2); */

    state.window = SDL_CreateWindow("App", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, (int)state.window_width, (int)state.window_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (state.window == NULL) {
        printf("Error creating window: %s\n", SDL_GetError());
        return false;
    }

    state.context = SDL_GL_CreateContext(state.window);
    if (state.context == NULL) {
        printf("Error creating OpenGL context: %s\n", SDL_GetError());
        return false;
    }
    glewExperimental = GL_TRUE; 
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        printf("Error initializing GLEW: %s\n", glewGetErrorString(glewError));
        return false;
    }

    if (SDL_GL_SetSwapInterval(1) < 0) {
        printf("Warning: Unable to set VSync: %s\n", SDL_GetError());
    }

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glDebugMessageCallback(gl_error_callback, 0);

    // GL Programs
    state.tri_program_id = gl_create_program(TRI_VERT_SRC, TRI_FRAG_SRC);
    if (!state.tri_program_id) {
        return false;
    }
    state.texture_program_id = gl_create_program(TEXTURE_VERT_SRC, TEXTURE_FRAG_SRC);
    if (!state.texture_program_id) {
        return false;
    }
    /* state.basic_program_id = gl_create_program(BASIC_VERT_SRC, BASIC_FRAG_SRC); */
    /* if (!state.basic_program_id) { */
    /*     return false; */
    /* } */

    return true;
}

void app_quit() {
    glDeleteProgram(state.tri_program_id);
    SDL_DestroyWindow(state.window);
    state.window = NULL;
    SDL_Quit();
}

void app_clear(Color color) {
    glClearColor(
        color.r / 255.0f,
        color.g / 255.0f,
        color.b / 255.0f,
        color.a / 255.0f
    );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void app_present() {
    SDL_GL_SwapWindow(state.window);
}

void app_update(App *s) {
    SDL_Event e;

    s->mouse_left_pressed = false;
    s->mouse_right_pressed = false;
    s->mouse_middle_pressed = false;

    while (SDL_PollEvent(&e) != 0) {
        switch (e.type) {
            case SDL_QUIT:
                s->should_quit = true;
                break;
            case SDL_WINDOWEVENT:
                if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                    s->window.x = (float)e.window.data1;
                    s->window.y = (float)e.window.data2;
                    state.window_width = (float)e.window.data1;
                    state.window_height = (float)e.window.data2;
                    glViewport(0, 0, e.window.data1, e.window.data2);
                }
                break;
            case SDL_MOUSEWHEEL:
                s->scroll.x += (float)e.wheel.x;
                s->scroll.y += (float)e.wheel.y;
                break;
            case SDL_MOUSEMOTION:
                s->mouse.x = (float)e.motion.x;
                s->mouse.y = (float)e.motion.y;
                break;
            case SDL_MOUSEBUTTONUP:
                switch (e.button.button) {
                    case SDL_BUTTON_LEFT:
                        s->mouse_left_down = false;
                        break;
                    case SDL_BUTTON_RIGHT:
                        s->mouse_right_down = false;
                        break;
                    case SDL_BUTTON_MIDDLE:
                        s->mouse_middle_down = false;
                        break;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                switch (e.button.button) {
                    case SDL_BUTTON_LEFT:
                        s->mouse_left_down = true;
                        s->mouse_left_pressed = true;
                        break;
                    case SDL_BUTTON_RIGHT:
                        s->mouse_right_down = true;
                        s->mouse_right_pressed = true;
                        break;
                    case SDL_BUTTON_MIDDLE:
                        s->mouse_middle_down = true;
                        s->mouse_middle_pressed = true;
                        break;
                }
                break;
        }
    }
}

uint8_t *read_file(const char *filename, long *out_len) {
    FILE *f;
    uint8_t *buf;

    f = fopen(filename, "rb");
    fseek(f, 0, SEEK_END);
    *out_len = ftell(f);
    rewind(f);

    buf = (uint8_t*)malloc(*out_len * sizeof(uint8_t));
    fread(buf, *out_len, 1, f);
    fclose(f);
    return buf;
}

uint64_t app_get_performance_counter() {
    return SDL_GetPerformanceCounter();
}

uint64_t app_get_performance_frequency() {
    return SDL_GetPerformanceFrequency();
}
