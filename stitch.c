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
    char *images[64] = {
        "text001.png",
        "text007.png",
        "text008.png",
        "text009.png",
        "text010.png",
        "text011.png",
        "text012.png",
        "text013.png",
        "text014.png",
        "text015.png",
        "text016.png",
        "text017.png",
        "text018.png",
        "text019.png",
        "text020.png",
        "text021.png",
        "text022.png",
        "text023.png",
        "text024.png",

        "text026.png",

        "text029.png",
        "text030.png",
        "text031.png",
        "text032.png",
        "text033.png",
        "text034.png",
        "text035.png",
        "text036.png",
        "text037.png",
        "text038.png",
        "text039.png",
        
        "text041.png",
        "text042.png",
        "text043.png",
        "text044.png",
        "text045.png",
        "text046.png",
        "text047.png",
        "text048.png",
        "text049.png",
        "text050.png",
        "text051.png",
        "text052.png",
        "text053.png",
        "text054.png",
        "text055.png",
        "text056.png",
        "text057.png",
        "text058.png",
        "text059.png",
        "text060.png",
        "text061.png",
        "text062.png",
        "text063.png",
        "text064.png",
        "text065.png",
        "text066.png",
        "text067.png",
        "text068.png",
        "text069.png",
        "text070.png",

        "text070.png", // Duplicates so we have an even 64
        "text070.png",
        "text070.png",
    };
    int map[64] = {
        0,  1,  2,  3,  4,  5,  6,  7,
        8,  9,  10, 11, 12, 13, 14, 15,
        16, 17, 18, 19, 20, 21, 22, 23,
        24, 25, 26, 27, 28, 29, 30, 31,
        32, 33, 34, 35, 36, 37, 38, 39,
        40, 41, 42, 43, 44, 45, 46, 47,
        48, 49, 50, 51, 52, 53, 54, 55,
        56, 57, 58, 59, 60, 61, 62, 63,
    };
    char *dir = "/home/paul/projects/lidata/z02LGI/textures/";

    stitch(dir, images, 64, map, 8, 8, "out.png");

    return 0;
}
