/******************************************************************************
 * Copyright 2018 Douglas V Johnson
 * Licensed under the BSD License. See LICENSE for details
 *****************************************************************************/
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <stdlib.h>
#include "gpiovuart.h"
#include "gpiovuart_backend.h"

struct gpiovuart_device_t {
    int fd; //Handle for serial port in POSIX
};

//Platform Specific in _posix or _win
gpiovuart_read_val gpiovuart_read_byte(gpiovuart_device* device) {
    gpiovuart_read_val val;
    int ret = read(device->fd, &(val.byte), 1);
    if (ret < 0) {
        perror("read");
        val.status = GPIOVUART_ERR_DEVICE;
    }
    else if (ret == 0) {
        val.status = GPIOVUART_ERR_TIMEOUT;
    }
    else {
        val.status = GPIOVUART_SUCCESS;
    }
    #ifdef DEBUG_COMM
    printf("RX: %x\n", val.byte);       
    #endif
    return val;
}

int gpiovuart_write_byte(gpiovuart_device* device, char byte) {
    if (write(device->fd, &byte, 1) < 1) {
        return GPIOVUART_ERR_DEVICE;
    }
    #ifdef DEBUG_COMM
    printf("TX: %x\n", byte);    
    #endif
    return GPIOVUART_SUCCESS;
}

#define MATCH(baud) case baud: return B ## baud

static int _map_baud_to_posix(int baud) {
    switch (baud) {
        MATCH(50);
        MATCH(75);
        MATCH(110);
        MATCH(134);
        MATCH(150);
        MATCH(200);
        MATCH(300);
        MATCH(600);
        MATCH(1200);
        MATCH(1800);
        MATCH(2400);
        MATCH(4800);
        MATCH(9600);
        MATCH(19200);
        MATCH(38400);
        MATCH(57600);
        MATCH(115200);
    }
    return -1;
}

gpiovuart_device* gpiovuart_open(const char* dev, int baud) {
    int fd = open(dev, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        perror("open");
        return NULL;
    }
    fcntl(fd, F_SETFL, 0);

    //Set the serial attributes
    struct termios options;
    if (tcgetattr(fd, &options) < 0) {
        perror ("tcgetattr");
        return NULL;
    }
    baud = _map_baud_to_posix(baud);
    if (baud < 0) {
        fprintf(stderr, "gpiovuart_open: Cannot use non-standard baud rate\n");
        return NULL;
    }
    cfsetispeed(&options, baud);
    cfsetospeed(&options, baud);
    options.c_cflag |= (CLOCAL | CREAD);
    //Set 8 N 1
    options.c_cflag &= ~CSIZE;
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag |= CS8;
    options.c_cflag &= ~CRTSCTS;
    //Raw input
    // Could have used cfmakeraw, but we want maximum portablity
    options.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR |
                        ICRNL | IXON);
    options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    //Row output
    options.c_oflag &= ~OPOST;
    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 100;
    if (tcsetattr(fd, TCSANOW, &options) != 0) {
        perror("tcsetattr");
        return NULL;
    }

    gpiovuart_device* d = malloc(sizeof(gpiovuart_device));
    d->fd = fd;
    return d;
}

void gpiovuart_close(gpiovuart_device* device) {
    close(device->fd);
    free(device);
}


