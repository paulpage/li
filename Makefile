all: main stitch parse mesh_loader

SRC=src/main.cpp src/platform.cpp src/gl.cpp
FAKESRC=src/drawing.cpp src/shaders.cpp

# main: src/main.c
# 	${CC} src/main.c -g -o main -Ilib -lm -lraylib

main: ${SRC} ${FAKESRC}
	g++ ${SRC} -g -o main -Ilib `pkg-config sdl2 glew --cflags --libs`

stitch: src/stitch.c
	${CC} src/stitch.c -g -o stitch -Ilib -lm

parse:  src/parse.cpp
	g++ src/parse.cpp -g -o parse -Ilib

mesh_loader: src/mesh_loader.cpp
	g++ src/mesh_loader.cpp -g -o parse -Ilib

clean:
	rm -f main stitch parse mesh_loader

.PHONY: all
