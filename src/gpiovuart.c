/******************************************************************************
 * Copyright 2018 Douglas V Johnson
 * Licensed under the BSD License. See LICENSE for details
 *****************************************************************************/
#include "gpiovuart.h"
#include "gpiovuart_backend.h"

#define VERSION_MAJOR 0
#define VERSION_MINOR 2

typedef unsigned char byte;

//Standard commands with pin parameters
#define CMD_READ                 0x0
#define CMD_READ_ANALOG          0x1
#define CMD_WRITE_LOW            0x2
#define CMD_WRITE_HIGH           0x3
#define CMD_SET_OUTPUT           0x4
#define CMD_SET_INPUT            0x5
#define CMD_SET_INPUT_PULLUP     0x6

//Extension command, with an extended command parameter
#define CMD_EXTENSION            0x7


//Extended commands
#define EXTCMD_GET_VERSION       0x0
#define EXTCMD_GET_MILLIS        0x1
#define EXTCMD_ECHO_BACK         0x2
//Reserved to never be used because it's the same as bad command
#define EXTCMD_RESERVED          0x1F

//Simple command struct 
typedef struct command_t {
    byte cmd;
    union {
        byte pin;
        byte cmd_ext;
    };
} command;

// Convert command to byte
static byte cmd_to_byte(command c) {
    byte b;
    b = c.cmd << 5;
    b |= c.pin & 0x1F;
    return b;
}

// Convert byte back into command
static command byte_to_cmd(byte b) {
    command c;
    c.cmd = b >>5;
    c.pin = b & 0x1F;
    return c;
}

// Send a simple command byte and insures that we recieve the same
// Byte back as an acknowledge
static int sendCommand(gpiovuart_device *dev, command c) {
    byte b = cmd_to_byte(c);
    int err;
    if ((err = gpiovuart_write_byte(dev, b)) < 0) {
        return err;
    }
    gpiovuart_read_val val = gpiovuart_read_byte(dev);
    // See if reading from the serial was succesful and if not return that
    if (val.status != GPIOVUART_SUCCESS) {
        return val.status;
    }
    // Otherwise we need to see if the dev acknowledged our command.
    // If they don't, then they either indicate that it was a bad command
    // (Which could be caused by a version mismatch) or they sent an unrelated
    // value
    else if (b != val.byte) {
        if (b == GPIOVUART_ERR_BADCMD)
            return GPIOVUART_ERR_BADCMD;
        return GPIOVUART_ERR_BADVAL;
    }
    return GPIOVUART_SUCCESS;
}

int gpiovuart_set_mode(gpiovuart_device* dev, int pin, int mode) {
    command c;
    switch (mode) {
        case GPIOVUART_OUTPUT: {
            c.cmd = CMD_SET_OUTPUT;
            break;
        }
        case GPIOVUART_INPUT: {
            c.cmd = CMD_SET_INPUT;
            break;
        }
        case GPIOVUART_INPUT_PULLUP: {
            c.cmd = CMD_SET_INPUT_PULLUP;
            break;
        }
        default: {
            return GPIOVUART_ERR_BADPARAM; 
        }
    }
    c.pin = pin;
    return sendCommand(dev, c);
}


int gpiovuart_write(gpiovuart_device* dev, int pin, int level) {
    command c;
    if (level)
        c.cmd = CMD_WRITE_HIGH;
    else
        c.cmd = CMD_WRITE_LOW;
    c.pin = pin;
    return sendCommand(dev, c);
}


int gpiovuart_read(gpiovuart_device* dev, int pin) {
    command c;
    c.cmd = CMD_READ;
    c.pin = pin;
    int err;
    if ((err = sendCommand(dev, c)) < 0) {
        return err;
    } 
    //Now we read back the 
    gpiovuart_read_val val = gpiovuart_read_byte(dev);
    if (val.status != GPIOVUART_SUCCESS) {
        return val.status;
    }
    //Contains our read value
    return val.byte;
}

//Simple wrapper to read several bytes into an int
static int read_multiple(gpiovuart_device* dev, int count) {
    int retval = 0;
    for (int i = 0; i < count; i++) {
        gpiovuart_read_val val;
        val = gpiovuart_read_byte(dev);
        if (val.status != GPIOVUART_SUCCESS)
            return val.status;
        retval |= (val.byte) << (8 * i);
    }
    return retval;
}

int gpiovuart_read_analog(gpiovuart_device* dev, int pin) {
    command c;
    c.cmd = CMD_READ_ANALOG;
    c.pin = pin;
    int err;
    if ((err = sendCommand(dev, c)) < 0) {
        return err;
    }
    return read_multiple(dev, 2);
}

gpiovuart_version gpiovuart_get_version(gpiovuart_device* dev) {
    command c;
    c.cmd = CMD_EXTENSION;
    c.cmd_ext = EXTCMD_GET_VERSION;
    int err;
    if ((err = sendCommand(dev, c)) < 0) { 
        return (gpiovuart_version){err, err};
    }
    int major = read_multiple(dev, 2);
    int minor = read_multiple(dev, 2);
    return (gpiovuart_version){major, minor};

}

gpiovuart_version gpiovuart_get_lib_version() {
    return (gpiovuart_version){VERSION_MAJOR, VERSION_MINOR};
}

long long gpiovuart_get_millis(gpiovuart_device* dev) {
    command c;
    c.cmd = CMD_EXTENSION;
    c.cmd_ext = EXTCMD_GET_MILLIS;
    int err;
    if ((err = sendCommand(dev, c)) < 0) { 
        return err;
    }
    return read_multiple(dev, 4);
}

int gpiovuart_echoback(gpiovuart_device* dev, int echo) {
    command c;
    c.cmd = CMD_EXTENSION;
    c.cmd_ext = EXTCMD_ECHO_BACK;
    int err;
    if ((err = sendCommand(dev, c)) < 0) {
        return err;
    }
    int ret = 0;
    for (int i = 0; i < 2; i++) {
        char b = (echo >> (i * 8)) & 0xFF;
        int err;
        if ((err = gpiovuart_write_byte(dev, b) ) < 0) {
            return err;
        }
        gpiovuart_read_val val = gpiovuart_read_byte(dev);
        if (val.status < 0) {
            return val.status;
        }
        ret |= val.byte << (8 * i); 
    }
    return ret; 
}

static const char ERR_STRINGS[][128] = {
    "Device does not recognize command",
    "Device response has timed out",
    "Device returned invalid data",
    "Local UART device has gone away",
    "Bad parameter to library call"
};

const char* gpiovuart_get_err_string(int err) {
    err = ~err; //Maps -1 to 0, -2 to 1, etc
    if (err < 5) {
        return ERR_STRINGS[err];
    }  
    return "Unknown Error Code";
}



