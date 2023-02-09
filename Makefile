all: main stitch parse mesh_loader

SRC=src/main.c src/platform.c src/gl.c
FAKESRC=src/drawing.c src/shaders.c

# main: src/main.c
# 	${CC} src/main.c -g -o main -Ilib -lm -lraylib

main: ${SRC} ${FAKESRC}
	g++ ${SRC} -g -o main -Ilib `pkg-config sdl2 glew --cflags --libs`

stitch: src/stitch.c
	${CC} src/stitch.c -g -o stitch -Ilib -lm

parse:  src/parse.c
	g++ src/parse.c -g -o parse -Ilib

mesh_loader: src/mesh_loader.c
	g++ src/mesh_loader.c -g -o parse -Ilib

clean:
	rm -f main stitch parse mesh_loader

.PHONY: all
