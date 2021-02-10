all: main stitch

main: main.c
	${CC} main.c -lm -lraylib -g -o main

stitch: stitch.c
	${CC} stitch.c -lm -g -o stitch

clean:
	rm -f main stitch

.PHONY: all
