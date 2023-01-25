all: main stitch

main:
	${CC} src/main.c -g -o main -lm -lraylib

stitch: stitch.c
	${CC} stitch.c -lm -g -o stitch

clean:
	rm -f main stitch

.PHONY: all
