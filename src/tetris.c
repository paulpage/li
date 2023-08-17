#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>

#include "platform.h"

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

static float frand() {
    return (float)rand() / (float)RAND_MAX;
}

static int randint(int start, int stop) {
    return rand() % (stop - start + 1) + start;
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

    int piece = 0;
    int rotation = 0;
    int xpos = 3;
    int ypos = 18;

    while (!app.should_quit) {
        app_update(&app);

        // ------------------------------------------------------------

        if (app.mouse_left_pressed) {
            piece = (piece + 1) % 7;
        }
        if (app.mouse_right_pressed) {
            rotation = (rotation + 1) % 4;
        }

        for (int y = 0; y < 40; y++) {
            for (int x = 0; x < 10; x++) {
                grid[y][x] = 0;
            }
        }

        for (int i = 0; i < 16; i++) {
            uint8_t p = pieces[piece][rotation][i];
            if (p == 1) {
                grid[ypos + i / 4][xpos + i % 4] = p;
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
