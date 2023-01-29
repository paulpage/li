#include <stdio.h>

#include "platform.h"

int main(int argc, char **argv) {
    AppState app = {0};
    if (!app_init("Sample", 400, 400)) {
        return 1;
    }

    Color background = {0, 100, 0, 255};

    Color foreground = {0, 0, 255, 255};
    Rect rect = {10.0f, 10.0f, 50.0f, 50.0f};

    while (!state.should_quit) {
        app_update(&app);

        printf("window: (%.2f, %.2f) mouse: (%.2f, %.2f)  scroll: (%.2f, %.2f) buttons: %d/%d %d/%d %d/%d\n",
                state.window.x, state.window.y, state.mouse.x, state.mouse.y, state.scroll.x, state.scroll.y,
                state.mouse_left_down, state.mouse_left_pressed, state.mouse_middle_down, state.mouse_middle_pressed, state.mouse_right_down, state.mouse_right_pressed);

        app_clear(background);

        app_draw_rect(rect, color);

        app_present();
    }

    app_quit();
    return 0;
}
