
/******************************************************************************
 * Copyright 2018 Douglas V Johnson
 * Licensed under the BSD License. See LICENSE for details
 *****************************************************************************/
#ifndef GPIOVUART_H_
#define GPIOVUART_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gpiovuart_device_t gpiovuart_device;

#define GPIOVUART_OUTPUT       0x0
#define GPIOVUART_INPUT        0x1
#define GPIOVUART_INPUT_PULLUP 0x2

#define GPIOVUART_SUCCESS       0x0
// This particular error value is the only one shared with the firmware, so
// it's important. The others are more or less arbitrary.
#define GPIOVUART_ERR_BADCMD   -0x1
#define GPIOVUART_ERR_TIMEOUT  -0x2
#define GPIOVUART_ERR_BADVAL   -0x3
#define GPIOVUART_ERR_DEVICE   -0x5
#define GPIOVUART_ERR_BADPARAM -0x6

gpiovuart_device* gpiovuart_open(const char* dev, int baud);
void gpiovuart_close(gpiovuart_device* dev);
int gpiovuart_set_mode(gpiovuart_device* dev, int pin, int mode);
int gpiovuart_write(gpiovuart_device* dev, int pin, int level);
int gpiovuart_read(gpiovuart_device* dev, int pin);
int gpiovuart_read_analog(gpiovuart_device* dev, int pin);

typedef struct gpiovuart_version_t {
    int major, minor;
} gpiovuart_version;

gpiovuart_version gpiovuart_get_lib_version();
gpiovuart_version gpiovuart_get_version(gpiovuart_device* dev);

long long gpiovuart_get_millis(gpiovuart_device* dev);
int gpiovuart_echoback(gpiovuart_device* dev, int echo);

const char* gpiovuart_get_err_string(int err);

#ifdef __cplusplus
}
#endif

#endif
