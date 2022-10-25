CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra -g

project: project.o ppm_io.o image_manip.o
	$(CC) -o project project.o ppm_io.o image_manip.o -lm

checkerboard: checkerboard.o ppm_io.o
	$(CC) -o checkerboard checkerboard.o ppm_io.o

img_cmp: img_cmp.o ppm_io.o
	$(CC) -o img_cmp img_cmp.o ppm_io.o

project.o: project.c ppm_io.h image_manip.h
	$(CC) $(CFLAGS) -c project.c

checkerboard.o: checkerboard.c ppm_io.h
	$(CC) $(CFLAGS) -c checkerboard.c

ppm_io.o: ppm_io.c ppm_io.h 
	$(CC) $(CFLAGS) -c ppm_io.c

image_manip.o: image_manip.c image_manip.h
	$(CC) $(CFLAGS) -c image_manip.c

clean:
	rm -f *.o main
