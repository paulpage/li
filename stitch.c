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

unsigned char stitch(char *dir, char **images, int nx, int ny, char *outpath) {
    int iw, ih;
    set_path(dir, images[0]);
    stbi_info(pathbuf, &iw, &ih, NULL);
    int width = iw * nx;
    int height = ih * ny;
    unsigned char *data = malloc(width * height * 4);
    memset(data, 0, width * height * 4);

    printf("%d x %d\n", width, height);

    set_path(dir, "text033.png");
    for (int y = 0; y < ny; y++) {
        for (int x = 0; x < nx; x++) {
            /* set_path(dir, images[y * nx + x]); */
            /* printf("%s\n", images[y * nx + x]); */
            unsigned char *idata = stbi_load(pathbuf, &iw, &ih, NULL, 4);

            int xo = x * iw * 4;
            int yo = y * ih;

            printf("%d: %d, %d offset (%d %d)\n", y * nx + x, x, y, xo, yo);

            for (int y1 = 0; y1 < ih; y1++) {
                for (int x1 = 0; x1 < iw * 4; x1++) {
                    data[(yo + y1) * (width * 4) + (xo + x1)] =
                        idata[y1 * (iw * 4) + x1];
                }
            }
        }
    }

    stbi_write_png(outpath, width, height, 4, data, width * 4);
}

int main() {

    char *images[9] = {
"text028.png",
"text029.png",
"text030.png",

"text031.png",
"text032.png",
"text033.png",

"text034.png",
"text035.png",
"text036.png",
    };
    char *dir = "/home/paul/projects/lidata/Game/_data/z04IFC/textures/";
    stitch(dir, images, 3, 3, "out.png");
    return 0;
}

