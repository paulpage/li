#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>

#include "platform.h"

#define INIT_X 3
#define INIT_Y 18
#define INIT_ROTATION 0

uint8_t pieces[7][4][16] = {
    {
        {0,0,0,0, 1,1,1,1, 0,0,0,0, 0,0,0,0},
        {0,0,1,0, 0,0,1,0, 0,0,1,0, 0,0,1,0},
        {0,0,0,0, 0,0,0,0, 1,1,1,1, 0,0,0,0},
        {0,1,0,0, 0,1,0,0, 0,1,0,0, 0,1,0,0},
    }, {
        {1,0,0,0, 1,1,1,0, 0,0,0,0, 0,0,0,0},
        {0,1,1,0, 0,1,0,0, 0,1,0,0, 0,0,0,0},
        {0,0,0,0, 1,1,1,0, 0,0,1,0, 0,0,0,0},
        {0,1,0,0, 0,1,0,0, 1,1,0,0, 0,0,0,0},
    }, {
        {0,0,1,0, 1,1,1,0, 0,0,0,0, 0,0,0,0},
        {0,1,0,0, 0,1,0,0, 0,1,1,0, 0,0,0,0},
        {0,0,0,0, 1,1,1,0, 1,0,0,0, 0,0,0,0},
        {1,1,0,0, 0,1,0,0, 0,1,0,0, 0,0,0,0},
    }, {
        {0,1,1,0, 0,1,1,0, 0,0,0,0, 0,0,0,0},
        {0,1,1,0, 0,1,1,0, 0,0,0,0, 0,0,0,0},
        {0,1,1,0, 0,1,1,0, 0,0,0,0, 0,0,0,0},
        {0,1,1,0, 0,1,1,0, 0,0,0,0, 0,0,0,0},
    }, {
        {0,1,1,0, 1,1,0,0, 0,0,0,0, 0,0,0,0},
        {0,1,0,0, 0,1,1,0, 0,0,1,0, 0,0,0,0},
        {0,0,0,0, 0,1,1,0, 1,1,0,0, 0,0,0,0},
        {1,0,0,0, 1,1,0,0, 0,1,0,0, 0,0,0,0},
    }, {
        {0,1,0,0, 1,1,1,0, 0,0,0,0, 0,0,0,0},
        {0,1,0,0, 0,1,1,0, 0,1,0,0, 0,0,0,0},
        {0,0,0,0, 1,1,1,0, 0,1,0,0, 0,0,0,0},
        {0,1,0,0, 1,1,0,0, 0,1,0,0, 0,0,0,0},
    }, {
        {1,1,0,0, 0,1,1,0, 0,0,0,0, 0,0,0,0},
        {0,0,1,0, 0,1,1,0, 0,1,0,0, 0,0,0,0},
        {0,0,0,0, 1,1,0,0, 0,1,1,0, 0,0,0,0},
        {0,1,0,0, 1,1,0,0, 1,0,0,0, 0,0,0,0}
    }
};

uint8_t grid[40][10] = {0};
int piece_x = INIT_X;
int piece_y = INIT_Y;
int piece_rotation = INIT_ROTATION;
int piece_idx = 0;

static float frand() {
    return (float)rand() / (float)RAND_MAX;
}

static int randint(int start, int stop) {
    return rand() % (stop - start + 1) + start;
}

void update() {
    piece_y += 1;
}

int main(int argc, char **argv) {
    /* srand(time(NULL)); */
    srand(500);

    App app = {0};
    if (!app_init("Sample", 400, 800)) {
        return 1;
    }

    Color color_bg = {0, 100, 0, 255};
    Color color_grid = {50, 50, 50, 255};
    Color color_floating = {0, 0, 255, 255};
    Color color_placed = {0, 0, 100, 255};

    Color black = {0, 0, 0, 255};

    int last_tick_ms = 0;

    while (!app.should_quit) {
        app_update(&app);

        if (app.keys_pressed[KEY_LEFT]) {
            piece_x -= 1;
        }
        if (app.keys_pressed[KEY_RIGHT]) {
            piece_x += 1;
        }

        int elapsed_ms = app_get_ms();
        if (elapsed_ms > last_tick_ms + 500) {
            last_tick_ms = elapsed_ms;
            update();
        }

        // ------------------------------------------------------------

        if (app.mouse_left_pressed) {
            piece_idx = randint(0, 6);
        }
        if (app.mouse_right_pressed) {
            piece_rotation = (piece_rotation + 1) % 4;
        }

        for (int y = 0; y < 40; y++) {
            for (int x = 0; x < 10; x++) {
                grid[y][x] = 0;
            }
        }

        for (int i = 0; i < 16; i++) {
            uint8_t p = pieces[piece_idx][piece_rotation][i];
            if (p == 1) {
                grid[piece_y + i / 4][piece_x + i % 4] = p;
            }
        }
        // ------------------------------------------------------------


        app_clear(color_bg);

        for (int y = 18; y < 40; y++) {
            for (int x = 0; x < 10; x++) {
                Color c = grid[y][x] == 0 ? color_grid : color_placed;
                Rect r = {x * 20.0f, (y - 18.0f) * 20.0f, 18.0f, 18.0f};
                app_draw_rect(r, c);
            }
        }

        /* for (int p = 0; p < 7; p++) { */
        /*     for (int r = 0; r < 4; r++) { */
        /*         Rect bg = {(float)(r * 100), (float)(p * 100), 80.0f, 80.0f}; */
        /*         app_draw_rect(bg, black); */
        /*         for (int i = 0; i < 16; i++) { */
        /*             int x = r * 100 + (i % 4 * 20); */
        /*             int y = p * 100 + (i / 4 * 20); */
        /*             Rect rect = {(float)x, (float)y, 20.0f, 20.0f}; */
        /*             if (pieces[p][r][i]) { */
        /*                 app_draw_rect(rect, color_placed); */
        /*             } */
        /*         } */
        /*     } */
        /* } */

        /* app_draw_rect(rect, foreground); */

        app_present();
    }

    app_quit();
    return 0;
}
