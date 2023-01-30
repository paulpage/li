#include "platform.h"

#include "SDL_opengl.h"

#include <GL/glew.h>
#include <GL/glu.h>

const GLchar *TRI_VERT_SRC =
    "#version 330 core\n"
    "in vec2 position;"
    "in vec4 color;"
    "out vec4 v_color;"
    "void main() {"
    "  v_color = color;"
    "  gl_Position = vec4(position.x, position.y, 0, 1);"
    "}";

const GLchar* TRI_FRAG_SRC =
    "#version 330 core\n"
    "in vec4 v_color;"
    "out vec4 LFragment;"
    "void main() {"
    "  LFragment = v_color;"
    "}";

const char *BASIC_VERT_SRC = "#version 330 core\n"
    "layout(location = 0) in vec4 position;\n"
    "void main() {\n"
    "  gl_Position = position;\n"
    "}\n";

const char *BASIC_FRAG_SRC = "#version 330 core\n"
    "out vec4 output_color;\n"
    "in vec4 v_color;\n"
    "void main() {\n"
    "  output_color = vec4(0.0f, 0.0f, 0.7f, 1.0f);\n"
    "}\n";

struct State {
    float window_width;
    float window_height;

    SDL_Window *window;
    SDL_GLContext context;
    GLuint tri_program_id;
    GLuint basic_program_id;
};

static State state = {0};

#include "shapes.cpp"

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


    glEnable(GL_MULTISAMPLE);

    // GL Programs
    state.tri_program_id = gl_create_program(TRI_VERT_SRC, TRI_FRAG_SRC);
    if (!state.tri_program_id) {
        return false;
    }
    state.basic_program_id = gl_create_program(BASIC_VERT_SRC, BASIC_FRAG_SRC);
    if (!state.tri_program_id) {
        return false;
    }

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(gl_error_callback, 0);

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
                    glViewport(0, 0, e.window.data1, e.window.data2);
                }
                break;
            case SDL_MOUSEWHEEL:
                s->scroll.x += (float)e.wheel.preciseX;
                s->scroll.y += (float)e.wheel.preciseY;
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
