#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "lib/stb_image_write.h"

char pathbuf[1000];

void set_path(char *dir, char *path) {
    memset(pathbuf, 0, strlen(pathbuf));
    strcat(pathbuf, dir);
    strcat(pathbuf, path);
}

void stitch(char *dir, char **paths, int image_count, int *map, int nx, int ny, char *outfile) {

    int iw, ih;
    unsigned char **images = malloc(sizeof(char*) * image_count);
    for (int i = 0; i < image_count; i++) {
        set_path(dir, paths[i]);
        images[i] = stbi_load(pathbuf, &iw, &ih, NULL, 4);
    }
    int width = iw * nx;
    int height = ih * ny;

    unsigned char *data = malloc(width * height * 4);

    for (int row = 0; row < ny; row++) {
        for (int col = 0; col < nx; col++) {
            unsigned char *idata = images[map[row * nx + col]];

            int xo = col * iw * 4;
            int yo = row * ih;

            for (int y = 0; y < ih; y++) {
                for (int x = 0; x < iw * 4; x++) {
                    data[(yo + y) * (width * 4) + (xo + x)] =
                        idata[y * (iw * 4) + x];
                }
            }
        }
    }

    stbi_write_png(outfile, width, height, 4, data, width * 4);
}

int main() {
    char *images[4] = {
        "text027.png",
        "text028.png",
        "text029.png",
        "text030.png",
    };
    int map[4] = {
        0, 1,
        2, 3,
    };
    char *dir = "/home/paul/projects/lidata/z04IFC/textures/";

    stitch(dir, images, 4, map, 2, 2, "out.png");

    return 0;
}
