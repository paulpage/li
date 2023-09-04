#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>

#include "platform.h"

#define INIT_X 3
#define INIT_Y 18
#define INIT_ROTATION 0
#define GRID_WIDTH 10
#define GRID_HEIGHT 20
#define FULL_GRID_HEIGHT 40

#define DEFAULT_TICK_MS 200
#define FAST_TICK_MS 16

#define PIECE_COUNT 7
uint8_t pieces[PIECE_COUNT][4][16] = {
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

uint8_t floating_grid[FULL_GRID_HEIGHT][GRID_WIDTH] = {0};
uint8_t grid[FULL_GRID_HEIGHT][GRID_WIDTH] = {0};
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

bool hits_wall(int new_x, int new_y, int new_rotation) {
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            if (pieces[piece_idx][new_rotation][y * 4 + x] == 1) {
                if (new_x + x < 0 || new_x + x >= GRID_WIDTH) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool hits_ceiling(int new_x, int new_y, int new_rotation) {
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            if (pieces[piece_idx][new_rotation][y * 4 + x] == 1) {
                // TODO
            }
        }
    }
    return false;
}

bool hits_floor(int new_x, int new_y, int new_rotation) {
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            if (pieces[piece_idx][new_rotation][y * 4 + x] == 1) {
                // TODO hitting pieces
                if (y + new_y >= FULL_GRID_HEIGHT) {
                    return true;
                }

                if (grid[new_y + y][new_x + x] == 1) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool update() {
    if (hits_floor(piece_x, piece_y + 1, piece_rotation)) {
        for (int i = 0; i < 16; i++) {
            uint8_t p = pieces[piece_idx][piece_rotation][i];
            if (p == 1) {
                grid[piece_y + i / 4][piece_x + i % 4] = p;
            }
        }
        piece_x = INIT_X;
        piece_y = INIT_Y;
        piece_idx = randint(0, PIECE_COUNT - 1);
        return true;
    } else {
        piece_y += 1;
        return false;
    }
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
    bool soft_drop_reset = false;

    while (!app.should_quit) {
        app_update(&app);

        if (app.keys_pressed[KEY_LEFT]
                && !hits_wall(piece_x - 1, piece_y, piece_rotation)
                && !hits_floor(piece_x - 1, piece_y, piece_rotation)) {
            piece_x -= 1;
        }
        if (app.keys_pressed[KEY_RIGHT]
                && !hits_wall(piece_x + 1, piece_y, piece_rotation)
                && !hits_floor(piece_x + 1, piece_y, piece_rotation)) {
            piece_x += 1;
        }

        if (app.keys_pressed[KEY_SPACE]) {
            bool hits_floor = false;
            while (!hits_floor) {
                hits_floor = update();
            }
        }

        int elapsed_ms = app_get_ms();
        if (elapsed_ms > last_tick_ms + DEFAULT_TICK_MS ||
                (app.keys_down[KEY_DOWN] && elapsed_ms > last_tick_ms + FAST_TICK_MS && !soft_drop_reset)) {
            last_tick_ms = elapsed_ms;
            if (update()) {
                soft_drop_reset = true;
            }
        }

        if (!app.keys_down[KEY_DOWN]) {
            soft_drop_reset = false;
        }

        // ------------------------------------------------------------

        /* if (app.mouse_left_pressed) { */
        /*     piece_idx = randint(0, PIECE_COUNT - 1); */
        /* } */
        if ((app.mouse_right_pressed || app.keys_pressed[KEY_UP])
                && !hits_wall(piece_x + 1, piece_y, piece_rotation + 1 % 4)
                && !hits_floor(piece_x + 1, piece_y, piece_rotation + 1 % 4)) {
            piece_rotation = (piece_rotation + 1) % 4;
        }

        for (int y = 0; y < 40; y++) {
            for (int x = 0; x < 10; x++) {
                floating_grid[y][x] = 0;
            }
        }

        for (int i = 0; i < 16; i++) {
            uint8_t p = pieces[piece_idx][piece_rotation][i];
            if (p == 1) {
                floating_grid[piece_y + i / 4][piece_x + i % 4] = p;
            }
        }
        // ------------------------------------------------------------


        app_clear(color_bg);

        for (int y = 18; y < 40; y++) {
            for (int x = 0; x < 10; x++) {
                Color c = grid[y][x] == 0 ? color_grid : color_placed;
                Rect r = {x * 20.0f, (y - 18.0f) * 20.0f, 18.0f, 18.0f};
                app_draw_rect(r, c);

                if (floating_grid[y][x] != 0) {
                    r = (Rect){x * 20.0f, (y - 18.0f) * 20.0f, 18.0f, 18.0f};
                    app_draw_rect(r, color_floating);
                }
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
