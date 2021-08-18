wow:
	cc wow.c `sdl2-config --cflags --libs` -o wow -lpthread

run:
	./wow