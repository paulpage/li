#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>

#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"

#include "platform.h"

#define RECT_COUNT 1000000

Rect rects[RECT_COUNT] = {0};
Color colors[RECT_COUNT] = {0};
Point origins[RECT_COUNT] = {0.0f};
float rotations[RECT_COUNT] = {0.0f};

float frand() {
    return (float)rand() / (float)RAND_MAX;
}

int randint(int start, int stop) {
    return rand() % (stop - start + 1) + start;
}

void init_rects() {
    for (int i = 0; i < RECT_COUNT; i++) {
        rects[i] = {frand() * 800.0f, frand() * 800.0f, frand() * 50.0f, frand() * 50.0f};
        colors[i] = {rand() % 255, rand() % 255, rand() % 255, 255};
        origins[i] = {rects[i].width / 2.0f, rects[i].height / 2.0f};
        rotations[i] = frand() * 10.0f;
    }
}

int main(int argc, char **argv) {
    srand(time(NULL));

    App app = {0};
    if (!app_init("Sample", 400, 400)) {
        return 1;
    }

    init_rects();

    Color background = {0, 100, 0, 255};

    Color foreground = {0, 0, 255, 255};
    Rect rect = {10.0f, 10.0f, 50.0f, 50.0f};

    Rect destrect = {40.0f, 40.0f, 256.0f, 256.0f};
    Rect srcrect = {0.0f, 0.0f, 128.0f, 128.0f};
    Texture texture = app_load_texture_from_file("/usr/share/icons/hicolor/128x128/apps/firefox.png");

    float rotation = 0.0f;

    app_load_font("/usr/share/fonts/TTF/DejaVuSans.ttf");

    uint64_t start = app_get_performance_counter(), end = 0;

    while (!app.should_quit) {

        end = app_get_performance_counter();

        float elapsed = (float)(end - start) / (float)app_get_performance_frequency() * 1000.0f;
        char s[64] = {0};
        stbsp_snprintf(s, 64, "frame time: %.4f ms", elapsed);

        start = app_get_performance_counter();

        app_update(&app);

        /* printf("window: (%.2f, %.2f) mouse: (%.2f, %.2f)  scroll: (%.2f, %.2f) buttons: %d/%d %d/%d %d/%d\n", */
        /*         app.window.x, app.window.y, app.mouse.x, app.mouse.y, app.scroll.x, app.scroll.y, */
        /*         app.mouse_left_down, app.mouse_left_pressed, app.mouse_middle_down, app.mouse_middle_pressed, app.mouse_right_down, app.mouse_right_pressed); */

        app_clear(background);

        rect.x = app.mouse.x;
        rect.y = app.mouse.y;

        Point origin = {25.0f, 25.0f};
        rotation += 0.01f;

        for (int i = 0; i < RECT_COUNT; i++) {
            rotations[i] = app.mouse.x / 1000.0f;
            /* app_draw_rotated_rects(&rects[i], &colors[i], &origins[i], &rotations[i], 1); */
        }

        app_draw_rotated_rects(rects, colors, origins, rotations, RECT_COUNT);

        app_draw_texture(texture, srcrect, destrect);


        app_draw_text("Heljo, World!", (Point){50.0f, 50.0f}, 100.0f, foreground);
        app_draw_text("Heljo, World!", (Point){50.0f, 400.0f}, 50.0f, foreground);

        app_draw_text(s, (Point){100.0f, 200.0f}, 50.0f, foreground);

        app_present();


    }

    app_quit();
    return 0;
}
