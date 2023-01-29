all: main stitch parse

SRC=src/main.cpp src/platform.cpp src/gl.cpp

# main: src/main.c
# 	${CC} src/main.c -g -o main -Ilib -lm -lraylib

main: ${SRC}
	g++ ${SRC} -o main `pkg-config sdl2 glew --cflags --libs`

stitch: src/stitch.c
	${CC} src/stitch.c -g -o stitch -Ilib -lm

parse:  src/parse.cpp
	g++ src/parse.cpp -g -o parse -Ilib

clean:
	rm -f main stitch parse

.PHONY: all
