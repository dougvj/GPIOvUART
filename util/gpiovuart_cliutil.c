
/******************************************************************************
 * Copyright 2018 Douglas V Johnson
 * Licensed under the BSD License. See LICENSE for details
 *****************************************************************************/
#include <gpiovuart.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define VERSION_MAJOR 0
#define VERSION_MINOR 2

int baud = 9600;
static gpiovuart_device* dev = NULL;


static void printUsage(char* msg) {
    fprintf(stderr, "%s\n", msg);
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "\tgpiovuart -v\n");
    fprintf(stderr, "\t\t-Get gpio utility version\n");
    fprintf(stderr, "\tgpiovuart -B <baud> -D <device>\n");
    fprintf(stderr, "\t\t-Optional baud setting with device.\n"
                    "\t\t When not specified, 9600 assumed.\n");
    fprintf(stderr, "\tgpiovuart -D <device> mode        <pin>\n"
                    "\t                        (output/input/input-pullup)\n");
    fprintf(stderr, "\t\t-Set pin mode for pin\n");
    fprintf(stderr, "\tgpiovuart -D <device> read        <pin>\n");
    fprintf(stderr, "\t\t-Digital read on pin\n");
    fprintf(stderr, "\tgpiovuart -D <device> write       <pin> <val>\n");
    fprintf(stderr, "\t\t-Digital write on pin. Val can be 1 or 0\n");
    fprintf(stderr, "\tgpiovuart -D <device> analog-read <pin>\n");
    fprintf(stderr, "\t\t-Analog read on pin\n");
    fprintf(stderr, "\tgpiovuart -D <device> version\n");
    fprintf(stderr, "\t\t-Get device firmware version\n");
    fprintf(stderr, "\tgpiovuart -D <device> millis\n");
    fprintf(stderr, "\t\t-Get device uptime in millis (50 day rollover)\n");
    fprintf(stderr, "\nCommands may be strung together, eg:\n");
    fprintf(stderr, "\tgpiovuart -D <device> mode 2 output write 2 1\n");
    fprintf(stderr, "\t\tSets pin 2 to output mode and sets it high\n");
    exit(1);
}

#define IS_FLAG(s) (s[0] == '-')


static int processFlag(int i, int argc, char** argv) {
    char* param = argv[i++];
    switch(param[1]) {
        case 'B':
            if (argc < i || IS_FLAG(argv[i]))
                printUsage("You must specify a baud rate after -B");
            if (dev)
                printUsage("You must specify baud rate before device");
            baud = atoi(argv[i++]);
            break;
        case 'D':
            if (argc < i || IS_FLAG(argv[i]))
                printUsage("You must specify a device after -D");
            dev = gpiovuart_open(argv[i++], baud);
            if (!dev) {
                fprintf(stderr, "Unable to open device at %s\n", argv[i-1]);
                exit(1);
            }
            break;
        case 'v':
            fprintf(stderr, "GPIOvUART Command Line Utility v%u.%u\n", 
                                                  VERSION_MAJOR, VERSION_MINOR);
            gpiovuart_version ver = gpiovuart_get_lib_version();
            fprintf(stderr, "\t libgpiovuart v%u.%u\n", ver.major, ver.minor);
            exit(0);
            break;
    }
    return i;
}


static void printErr(int err) {
    fprintf(stderr, "Error: %s\n", gpiovuart_get_err_string(err));
    exit(1);
}

static char* getParam(int i, int argc, char** argv, char* cmd, char* name) {
    if (argc < i || IS_FLAG(argv[i])) {
        fprintf(stderr, "Expected a '%s' parameter after '%s' command\n", 
                                                                    name, cmd);
        exit(1);
    }
    return argv[i];
}

static int getBinParam(int i, int argc, char** argv, char* cmd, char* name) {
    char* param = getParam(i, argc, argv, cmd, name);
    if (strlen(param) != 1 || !(param[0] == '1' || param[0] == '0')) {
        fprintf(stderr, "Expected binary digit for '%s' after '%s' command, "
                        "not '%s'\n", name, cmd, param);
        exit(1);
    }
    return atoi(param);
}

static int getPinParam(int i, int argc, char** argv, char* cmd, char* name) {
    char* param = getParam(i, argc, argv, cmd, name);
    int val = atoi(param);
    if (val == 0) {
        fprintf(stderr, "'%s' after '%s' command must be a positive number, "
                        "not '%s', %i\n", name, cmd, param, i);
        exit(1);
    }
    return val;
}

int checkError(int val) {
    if (val < 0) {
        printErr(val);
    }
    return val;
}

#define MATCH(str, match) (strcmp(str, match) == 0)

static int processCommand(int i, int argc, char** argv) {
    char* cmd = argv[i++];
    //TODO convert to hashmap eventually for faster cmd processing
    if      (MATCH(cmd, "mode")) {
        int pin = getPinParam(i++, argc, argv, "write", "pin");
        char* mode = getParam(i++, argc, argv, "mode", 
                                               "output/input/input-pullup");
        int gpiovuart_mode = 1;
        if (MATCH(mode, "output")) {
            gpiovuart_mode = GPIOVUART_OUTPUT;
        }
        else if (MATCH(mode, "input")) {
            gpiovuart_mode = GPIOVUART_INPUT;
        }
        else if (MATCH(mode, "input-pullup")) {
            gpiovuart_mode = GPIOVUART_INPUT_PULLUP;
        }
        else {
            fprintf(stderr, "Unknown mode %s after 'mode' command. "
                            "Try one of output/input/input-pullup\n", mode);
            exit(1);
        }
        checkError(gpiovuart_set_mode(dev, pin, gpiovuart_mode));
    }
    else if (MATCH(cmd, "read")) {
        int pin = getPinParam(i++, argc, argv, "read", "pin");
        int r = checkError(gpiovuart_read(dev, pin));
        printf("%i\n", r);
    }
    else if (MATCH(cmd, "write")) {
        int pin = getPinParam(i++, argc, argv, "write", "pin");
        int val = getBinParam(i++, argc, argv, "write", "val");
        checkError(gpiovuart_write(dev, pin, val));
    }
    else if (MATCH(cmd, "analog-read")) {
        int pin = getPinParam(i++, argc, argv, "analog-read", "pin");
        int r = checkError(gpiovuart_read_analog(dev, pin));
        printf("%i\n", r);
    }
    else if (MATCH(cmd, "version")) {
        gpiovuart_version ver = gpiovuart_get_version(dev);
        checkError(ver.major);
        checkError(ver.minor);
        printf("%i.%i\n", ver.major, ver.minor);
    }
    else if (MATCH(cmd, "millis")) {
        long long millis = gpiovuart_get_millis(dev);
        if (millis < 0)
            printErr(millis);
        printf("%lli\n", millis);
    }
    else if (MATCH(cmd, "testcomm")) {
        srand(time(NULL));
        for (int i = 0; i < 10; i++) {
            int random = rand() & 0xFFFF;
            int ret = gpiovuart_echoback(dev, random);
            if (ret < 0) {
                printErr(ret);
            }
            if (ret != random) {
                fprintf(stderr, "Communication error: sent %x recv %x\n", 
                                                                random, ret);
                exit(1);
            }
        }
        fprintf(stderr, "Communication OK\n");
    }
    else {
        fprintf(stderr, "Unknown command: %s\n", cmd);
        exit(1);
    }
    return i;
}

int main(int argc, char** argv) {
    int process_count = 0;
    if (argc < 2) {
        printUsage("");
    }
    for (int i = 1; i < argc;) {
        if (IS_FLAG(argv[i]))
            i=processFlag(i, argc, argv);
        else {
            if (!dev) {
                printUsage("No device specified");
            }
            process_count++;
            i=processCommand(i, argc, argv);
        }
    }
    if (!process_count) {
        printUsage("No commands specified");
    }
    return 0;
}
