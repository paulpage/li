#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>

#define STB_SPRINTF_IMPLEMENTATION
#define STB_SPRINTF_STATIC
#include "stb_sprintf.h"

#include "platform.h"

#define RECT_COUNT 10000

Rect rects[RECT_COUNT] = {0};
Color colors[RECT_COUNT] = {0};
Point origins[RECT_COUNT] = {0.0f};
float rotations[RECT_COUNT] = {0.0f};

Rect src_rects[RECT_COUNT] = {0};

Point positions[RECT_COUNT] = {0.0f};
float sizes[RECT_COUNT] = {0.0f};

char **text;

static float frand() {
    return (float)rand() / (float)RAND_MAX;
}

static int randint(int start, int stop) {
    return rand() % (stop - start + 1) + start;
}

typedef struct Camera {
    Vec3 focus;
    float distance;
    float rot_horizontal;
    float rot_vertical;
    float fovy;
} Camera;

static void init_rects() {
    for (int i = 0; i < RECT_COUNT; i++) {
        rects[i] = (Rect){frand() * 800.0f, frand() * 800.0f, frand() * 50.0f, frand() * 50.0f};
        colors[i] = (Color){rand() % 255, rand() % 255, rand() % 255, 255};
        origins[i] = (Point){rects[i].width / 2.0f, rects[i].height / 2.0f};
        /* rotations[i] = frand() * 10.0f; */
        rotations[i] = 0.0f;

        src_rects[i] = (Rect){0.0f, 0.0f, 128.0f, 128.0f};

        positions[i] = (Point){frand() * 800.0f, frand() * 800.0f};
        sizes[i] = 20.0f;
    }
}

static void draw_cube(App *app, Camera camera) {
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f,  0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f
    };
    unsigned int indices[] = {
        /* 0, 1, 2, 1, 2, 3, */
        /* 0, 1, 4, 1, 4, 5, */
        /* 0, 2, 4, 2, 4, 6, */
        /* 1, 3, 5, 3, 5, 7, */
        /* 2, 3, 6, 3, 6, 7, */
        /* 4, 6, 5, 6, 5, 7 */
        0, 1, 2, 2, 1, 3,
        0, 1, 4, 4, 1, 5,
        0, 2, 4, 4, 2, 6,
        1, 3, 5, 5, 3, 7,
        2, 3, 6, 6, 3, 7,
        4, 6, 5, 5, 6, 7
    };
    Color color = {255, 0, 0, 255};

    float light[] = {
        -5.0, -5.0, -5.0,
        1.0, 1.0, 1.0,

        0.1, 0.1, 0.1,
        0.8, 0.8, 0.8,
        1.0, 1.0, 1.0,
    };

    Mat4 transform = (Mat4){
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    // camera -----
    
    // look at -------

    Vec3 eye = (Vec3){
        camera.focus.z + camera.distance * m_sin(camera.rot_vertical) * m_sin(camera.rot_horizontal),
        camera.focus.y + camera.distance * m_cos(camera.rot_vertical),
        camera.focus.x + camera.distance * m_sin(camera.rot_vertical) * m_cos(camera.rot_horizontal)
    };
    Vec3 at = camera.focus;
    Vec3 up = V3(0.0f, 1.0f, 0.0f);

    Mat4 view = transform;
    /* Mat4 view = look_at_rh(eye, at, up); */

    /* Vec3 zaxis = norm_v3(sub_v3(at, eye)); */
    /* Vec3 xaxis = norm_v3(cross(zaxis, up)); */
    /* Vec3 yaxis = cross(xaxis, zaxis); */

    /* zaxis = mul_v3f(zaxis, -1.0f); */

    /* Mat4 view = { */
    /*     xaxis.x, xaxis.y, xaxis.z, -dot_v3(xaxis, eye), */
    /*     yaxis.x, yaxis.y, yaxis.z, -dot_v3(yaxis, eye), */
    /*     zaxis.x, zaxis.y, zaxis.z, -dot_v3(zaxis, eye), */
    /*     0, 0, 0, 1 */
    /* }; */

    // perspective -----

    /* float a = 1.0f; */
    /* float b = app->window.x / app->window.y; // aspect ratio */
    /* float c = 3.0f; // focal distance */
    /* float n = 0.01f; // near */
    /* float f = 100.0f; // far */
    /* float d = (n+f) / (n-f); */
    /* float e = (2*f*n) / (n-f); */

    /* float near = 0.01f; */
    /* float far = 100.0f; */
    /* float aspect_ratio = app->window.x / app->window.y; */
    /* float fov = camera.fovy; */

    /* float s = 1 / (tanf(fov / 2.0f * M_PI / 180.0f)); */

    float fov = camera.fovy;
    float aspect_ratio = app->window.x / app->window.y;
    float near = 0.001f;
    float far = 100.0f;
    Mat4 proj = perspective_rh_no(deg_to_rad*fov, aspect_ratio, near, far);
    /* Mat4 proj = transform; */


    for (int i = 0; i < 8; i++) {
        Vec4 v = V4(vertices[i * 3 + 0], vertices[i * 3 + 1], vertices[i * 3 + 2], 0.0f);
        /* Vec4 result = mul_m4v4(mul_m4(mul_m4(transform, view), proj), v); */
        Vec4 result = mul_m4v4(mul_m4(mul_m4(proj, view), transform), v);
        printf("vertex %d: %f %f %f\n", i, result.x, result.y, result.z);
    }

    /* Mat4 proj = { */
    /*     a*c, 0, 0, 0, */
    /*     0, b*c, 0, 0, */
    /*     0, 0, d, e, */
    /*     0, 0, -1, 0 */
    /* }; */

    /* Mat4 proj = { */
    /*     s,    0.0f, 0.0f, 0.0f, */
    /*     0.0f, s,    0.0f, 0.0f, */
    /*     0.0f, 0.0f, -far/(far-near), -1.0f, */
    /*     0.0f, 0.0f, 0.0f, -(far*near)/(far-near), */
    /* }; */

    // -----------------

    Vec3 view_position = eye;

    // TODO might have to correct some of the above for degrees

    app_draw_model(vertices, indices, 8, 36, transform, view, proj, view_position, light,color);
}

int main(int argc, char **argv) {
    /* srand(time(NULL)); */
    srand(500);

    App app = {0};
    if (!app_init("Sample", 400, 400)) {
        return 1;
    }

    init_rects();
    text = malloc(sizeof(char*) * RECT_COUNT);
    for (int i = 0; i < RECT_COUNT; i++) {
        text[i] = "Hello";
        /* text[i] = malloc(sizeof(char) * 10); */
        /* text[i][0] = (char)randint((int)'a', (int)'z'); */
        /* text[i][1] = (char)randint((int)'a', (int)'z'); */
        /* text[i][2] = (char)randint((int)'a', (int)'z'); */
        /* text[i][3] = (char)randint((int)'a', (int)'z'); */
        /* text[i][4] = (char)randint((int)'a', (int)'z'); */
        /* text[i][5] = (char)randint((int)'a', (int)'z'); */
        /* text[i][6] = (char)randint((int)'a', (int)'z'); */
        /* text[i][7] = (char)randint((int)'a', (int)'z'); */
        /* text[i][8] = (char)randint((int)'a', (int)'z'); */
        /* text[i][9] = '\0'; */
    }

    Color background = {0, 100, 0, 255};

    Color foreground = {0, 0, 255, 255};
    Rect rect = {10.0f, 10.0f, 50.0f, 50.0f};

    /* Rect destrect = {40.0f, 40.0f, 256.0f, 256.0f}; */
    /* Rect srcrect = {0.0f, 0.0f, 128.0f, 128.0f}; */
    Texture texture = app_load_texture_from_file("/usr/share/icons/hicolor/128x128/apps/firefox.png");
    /* Texture texture = app_load_texture_from_file("C:\\Users\\Paul\\Pictures\\Emoji\\joy.png"); */

    float rotation = 0.0f;

    app_load_font("/usr/share/fonts/TTF/DejaVuSans.ttf");
    /* app_load_font("C:\\Windows\\Fonts\\Inkfree.ttf"); */

    uint64_t start = app_get_performance_counter(), end = 0;

    Camera camera;
    camera.focus = V3(0.0f, 0.0f, 0.0f);
    camera.distance = 10.0f;
    camera.rot_horizontal = 0.5f;
    camera.rot_vertical = 0.5f;
    camera.fovy = 45.0f;

    while (!app.should_quit) {

        end = app_get_performance_counter();

        float elapsed = (float)(end - start) / (float)app_get_performance_frequency() * 1000.0f;
        char s[64] = {0};
        stbsp_snprintf(s, 64, "frame time: %.4f ms", elapsed);

        /* printf("frame time: %.4f ms\n", elapsed); */

        start = app_get_performance_counter();

        app_update(&app);

        /* printf("window: (%.2f, %.2f) mouse: (%.2f, %.2f)  scroll: (%.2f, %.2f) buttons: %d/%d %d/%d %d/%d\n", */
        /*         app.window.x, app.window.y, app.mouse.x, app.mouse.y, app.scroll.x, app.scroll.y, */
        /*         app.mouse_left_down, app.mouse_left_pressed, app.mouse_middle_down, app.mouse_middle_pressed, app.mouse_right_down, app.mouse_right_pressed); */

        app_clear(background);

        draw_cube(&app, camera);

        camera.rot_horizontal += 0.01f;

        Rect r = {app.mouse.x, app.mouse.y, 50, 50};
        Color c = {0, 0, 255, 255};
        app_draw_rect(r, c);

#if 0
        rect.x = app.mouse.x;
        rect.y = app.mouse.y;

        /* Point origin = {25.0f, 25.0f}; */
        rotation += 0.01f;

        for (int i = 0; i < RECT_COUNT; i++) {
            /* rotations[i] = app.mouse.x / 500.0f; */
            rotations[i] += 0.1f;
            /* app_draw_rotated_rects(&rects[i], &colors[i], &origins[i], &rotations[i], 1); */
        }

        /* app_draw_rotated_rects(rects, colors, origins, rotations, RECT_COUNT); */

        app_draw_rotated_textures(texture, src_rects, rects, origins, rotations, RECT_COUNT);
        /* app_draw_texture(texture, srcrect, destrect); */

        app_draw_rotated_text(text, positions, sizes, colors, origins, rotations, RECT_COUNT);

        app_draw_text("Heljo, World!", (Point){50.0f, 50.0f}, 100.0f, foreground);
        /* app_draw_text("Heljo, World!", (Point){50.0f, 400.0f}, 50.0f, foreground); */

        /* app_draw_text(s, (Point){100.0f, 200.0f}, 50.0f, foreground); */
#endif
        app_present();


    }

    app_quit();
    return 0;
}
