CC := gcc
CFLAGS := -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

main: main.c
	$(CC) *.c $(CFLAGS) -o udp_snake

