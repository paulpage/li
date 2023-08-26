#include "platform.h"

#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"

#include <stdio.h>

#define STB_RECT_PACK_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#define STBRP_STATIC
#define STBTT_STATIC
#include "stb_rect_pack.h"
#include "stb_truetype.h"

typedef struct Font {
    stbtt_fontinfo info;
    unsigned char *buf;
    long buflen;
} Font;

typedef struct State {
    float window_width;
    float window_height;

    SDL_Window *window;
    SDL_GLContext context;
    GLuint tri_program_id;
    GLuint texture_program_id;
    GLuint text_program_id;
    GLuint model_program_id;

    Font font;
} State;

static State state = {0};

#include "string.c"
/* #include "math.c" */
#include "gl.c"
#include "shaders.c"
#include "drawing.c"
#include "models.c"

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

    gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);

    if (SDL_GL_SetSwapInterval(1) < 0) {
        printf("Warning: Unable to set VSync: %s\n", SDL_GetError());
    }

    /* glEnable(GL_DEBUG_OUTPUT); */
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    /* glDebugMessageCallback(gl_error_callback, 0); */

    // GL Programs
    state.tri_program_id = gl_create_program("src/shaders/triangle.glsl");
    if (!state.tri_program_id) {
        return false;
    }
    state.texture_program_id = gl_create_program("src/shaders/texture.glsl");
    if (!state.texture_program_id) {
        return false;
    }
    state.text_program_id = gl_create_program("src/shaders/text.glsl");
    if (!state.text_program_id) {
        return false;
    }
    state.model_program_id = gl_create_program("src/shaders/model.glsl");
    if (!state.model_program_id) {
        return false;
    }

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

    s->scroll.x = 0.0f;
    s->scroll.y = 0.0f;
    s->precise_scroll.x = 0.0f;
    s->precise_scroll.y = 0.0f;

    for (int i = 0; i < KEY_COUNT; i++) {
        s->keys_pressed[i] = false;
    }

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
                s->precise_scroll.x += e.wheel.preciseX;
                s->precise_scroll.y += e.wheel.preciseY;
                printf("%f %f\n", e.wheel.preciseX, e.wheel.preciseY);
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
            case SDL_KEYDOWN:
                s->keys_down[e.key.keysym.scancode] = true;
                s->keys_pressed[e.key.keysym.scancode] = true;
                break;
            case SDL_KEYUP:
                s->keys_down[e.key.keysym.scancode] = false;
                break;
        }
    }
}

uint64_t app_get_performance_counter() {
    return SDL_GetPerformanceCounter();
}

uint64_t app_get_performance_frequency() {
    return SDL_GetPerformanceFrequency();
}

int app_get_ms() {
    return SDL_GetTicks();
}
