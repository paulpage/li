#include <stdio.h>

#include "platform.h"

int main(int argc, char **argv) {
    App app = {0};
    if (!app_init("Sample", 400, 400)) {
        return 1;
    }

    Color background = {0, 100, 0, 255};

    Color foreground = {0, 0, 255, 255};
    Rect rect = {10.0f, 10.0f, 50.0f, 50.0f};

    Rect destrect = {40.0f, 40.0f, 256.0f, 256.0f};
    Rect srcrect = {0.0f, 0.0f, 128.0f, 128.0f};
    Texture texture = app_load_texture_from_file("/usr/share/icons/hicolor/128x128/apps/firefox.png");

    float rotation = 0.0f;

    while (!app.should_quit) {
        app_update(&app);

        /* printf("window: (%.2f, %.2f) mouse: (%.2f, %.2f)  scroll: (%.2f, %.2f) buttons: %d/%d %d/%d %d/%d\n", */
        /*         app.window.x, app.window.y, app.mouse.x, app.mouse.y, app.scroll.x, app.scroll.y, */
        /*         app.mouse_left_down, app.mouse_left_pressed, app.mouse_middle_down, app.mouse_middle_pressed, app.mouse_right_down, app.mouse_right_pressed); */

        app_clear(background);

        rect.x = app.mouse.x;
        rect.y = app.mouse.y;

        Point origin = {25.0f, 25.0f};
        rotation += 0.01f;
        app_draw_rotated_rects(&rect, &foreground, &origin, &rotation, 1);

        app_draw_texture(texture, srcrect, destrect);

        app_present();
    }

    app_quit();
    return 0;
}
