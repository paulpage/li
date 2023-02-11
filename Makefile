all: main stitch parse mesh_loader

SRC=src/main.c src/platform.c src/gl.c
FAKESRC=src/drawing.c src/shaders.c

# main: src/main.c
# 	${CC} src/main.c -g -o main -Ilib -lm -lraylib

release: ${SRC} ${FAKESRC}
	${CC} ${SRC} -o release-main -Ilib -lm -O2 `pkg-config sdl2 --cflags --libs`


main: ${SRC} ${FAKESRC}
	${CC} ${SRC} -g -o main -Ilib -lm `pkg-config sdl2 --cflags --libs`

stitch: src/stitch.c
	${CC} src/stitch.c -g -o stitch -Ilib -lm

parse:  src/parse.c
	${CC} src/parse.c -g -o parse -Ilib

mesh_loader: src/mesh_loader.c
	${CC} src/mesh_loader.c -g -o parse -Ilib

clean:
	rm -f main stitch parse mesh_loader

.PHONY: all
