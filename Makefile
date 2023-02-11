all: main stitch parse mesh_loader

APP_SRC=src/main.c
LIB_SRC=src/platform.c
LIB_FAKESRC=src/drawing.c src/shaders.c

release: ${LIB_SRC} ${LIB_FAKESRC} ${APP_SRC}
	${CC} ${LIB_SRC} ${APP_SRC} -O2 -o release-main -Ilib -lm `pkg-config sdl2 --cflags --libs`

platform.o: ${LIB_SRC} ${LIB_FAKESRC}
	${CC} ${LIB_SRC} -c -g -Ilib -lm `pkg-config sdl2 --cflags --libs`

main: platform.o ${APP_SRC}
	${CC} platform.o ${APP_SRC} -g -o main -Ilib -lm `pkg-config sdl2 --cflags --libs`

stitch: src/stitch.c
	${CC} src/stitch.c -g -o stitch -Ilib -lm

parse:  src/parse.c
	${CC} src/parse.c -g -o parse -Ilib

mesh_loader: src/mesh_loader.c
	${CC} src/mesh_loader.c -g -o mesh_loader -Ilib

clean:
	rm -f main stitch parse mesh_loader platform.o

.PHONY: all
