#ifndef SERIALFILE
#define SERAILFILE

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#include <stdlib.h>
#include <string.h>
#include <sys/select.h>

typedef struct termios Serial;

int serial_begin(const char *serial_name, speed_t baud_rate);

int serial_read(int fd, char *buffer);

int serial_wait_for_data(int fd, int timeout_ms);

#endif