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
    strcat(pathbuf, "/");
    strcat(pathbuf, path);
}

void stitch(char *dir, char **paths, int image_count, int *map, int nx, int ny, char *outfile) {

    int iw = 0, ih = 0, in = 0;
    unsigned char **images = malloc(sizeof(char*) * image_count);
    for (int i = 0; i < image_count; i++) {
        printf("%s %s\n", dir, paths[i]);
        set_path(dir, paths[i]);
        images[i] = stbi_load(pathbuf, &iw, &ih, &in, 4);
        printf("iw ih in: %d %d %d\n", iw, ih, in);
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
    printf("wrote png\n");
}

int main() {
    char *images[64] = {
        "text001.tga",
        "text007.tga",
        "text008.tga",
        "text009.tga",
        "text010.tga",
        "text011.tga",
        "text012.tga",
        "text013.tga",
        "text014.tga",
        "text015.tga",
        "text016.tga",
        "text017.tga",
        "text018.tga",
        "text019.tga",
        "text020.tga",
        "text021.tga",
        "text022.tga",
        "text023.tga",
        "text024.tga",

        "text026.tga",

        "text029.tga",
        "text030.tga",
        "text031.tga",
        "text032.tga",
        "text033.tga",
        "text034.tga",
        "text035.tga",
        "text036.tga",
        "text037.tga",
        "text038.tga",
        "text039.tga",
        
        "text041.tga",
        "text042.tga",
        "text043.tga",
        "text044.tga",
        "text045.tga",
        "text046.tga",
        "text047.tga",
        "text048.tga",
        "text049.tga",
        "text050.tga",
        "text051.tga",
        "text052.tga",
        "text053.tga",
        "text054.tga",
        "text055.tga",
        "text056.tga",
        "text057.tga",
        "text058.tga",
        "text059.tga",
        "text060.tga",
        "text061.tga",
        "text062.tga",
        "text063.tga",
        "text064.tga",
        "text065.tga",
        "text066.tga",
        "text067.tga",
        "text068.tga",
        "text069.tga",
        "text070.tga",

        "text070.tga", // Duplicates so we have an even 64
        "text070.tga",
        "text070.tga",
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
    char *dir = "_data/z02LGI/textures";

    stitch(dir, images, 64, map, 8, 8, "out.png");

    return 0;
}
