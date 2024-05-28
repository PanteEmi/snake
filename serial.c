#include "serial.h"

int serial_begin(const char *serial_name, speed_t baud_rate){
    int fd;
    struct termios tty;
    printf("the port is: %s\n", serial_name);
    // Open the serial port
    fd = open(serial_name, O_RDWR);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    // Get the current serial port settings
    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        close(fd);
        return 1;
    }

    // Set baud rate
    cfsetospeed(&tty, baud_rate);
    cfsetispeed(&tty, baud_rate);

    // Set other serial port parameters
    tty.c_cflag &= ~PARENB; // No parity
    tty.c_cflag &= ~CSTOPB; // 1 stop bit
    tty.c_cflag &= ~CSIZE; // Clear data size bits
    tty.c_cflag |= CS8; // 8 data bits
    tty.c_cflag &= ~CRTSCTS; // No hardware flow control
    tty.c_cflag |= CREAD | CLOCAL; // Enable receiver, ignore modem control lines

    // Set input mode (non-canonical, no echo)
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    // Set output mode (raw output)
    tty.c_oflag &= ~OPOST;

    // Set the new serial port settings
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        close(fd);
        return 1;
    }
    return fd;
}

int serial_read(int fd, char *buffer){
    int bytes_read = read(fd, buffer, sizeof(buffer));
    if (bytes_read == -1) {
        perror("read");
        close(fd);
        return 1;
    }
    //printf("Received %d bytes: %s\n", bytes_read, buffer);
    return bytes_read;
}

int serial_wait_for_data(int fd, int timeout_ms) {
    fd_set readfds;
    struct timeval timeout;

    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);

    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;

    int select_result = select(fd + 1, &readfds, NULL, NULL, &timeout);
    if (select_result == -1) {
        perror("select");
        return -1;
    } else if (select_result == 0) {
        // Timeout occurred, no data available
        return 0;
    } else if (FD_ISSET(fd, &readfds)) {
        // Data is available
        return 1;
    }

    return -1;
}