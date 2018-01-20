/******************************************************************************
 * Copyright 2018 Douglas V Johnson
 * Licensed under the BSD License. See LICENSE for details
 *****************************************************************************/

typedef struct gpiovuart_read_val_t {
    int status;
    char byte;
} gpiovuart_read_val;

//Platform Specific in _posix or _win
gpiovuart_read_val gpiovuart_read_byte(gpiovuart_device* device);
int gpiovuart_write_byte(gpiovuart_device* device, char byte);
gpiovuart_device* gpiovuart_open(const char* dev, int baud);
void gpiovuart_close(gpiovuart_device* device);
