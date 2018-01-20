/******************************************************************************
 * Copyright 2018 Douglas V Johnson
 * Licensed under the BSD License. See LICENSE for details
 *****************************************************************************/

void setup() {
    Serial.begin(9600);
}

#define VERSION_MAJOR 0
#define VERSION_MINOR 2

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

#define BAD_COMMAND              -0x1 //Same as 0xFF

//The command space and parameter are entirely in one byte. For now we don't 
//need more than that but in the future we can expand the space with optional
//parameters on cmd_ext
struct command {
    byte cmd;
    union {
        byte pin;
        byte cmd_ext;
    };
};

struct command byte_to_cmd(byte b) {
    command c;
    c.cmd = b >> 5;
    c.pin = b & 0x1F;
    return c;
}



void loop() {
}

void writeShort(unsigned short s) {
    Serial.write((byte) s & 0xF);
    Serial.write((byte) s >> 8);
}

void writeLong(unsigned long l) {
    Serial.write((byte) l & 0xF);
    Serial.write((byte) (l >>  8) & 0xF);
    Serial.write((byte) (l >> 16) & 0xF);
    Serial.write((byte) (l >> 24));
}


void serialEvent() {
    while (Serial.available()) {
        byte b = Serial.read();
        command c = byte_to_cmd(b);
        switch(c.cmd) {
            case CMD_READ: {
                byte r = digitalRead(c.pin);
                Serial.write(b);
                Serial.write(r);
                break;
            }
            case CMD_READ_ANALOG: {
                unsigned short r = analogRead(c.pin);
                Serial.write(b);
                writeShort(r);
                break;
            }
            case CMD_WRITE_LOW: {
                digitalWrite(c.pin, LOW);
                Serial.write(b);
                break;
            }
            case CMD_WRITE_HIGH: {
                digitalWrite(c.pin, HIGH);
                Serial.write(b);
                break;
            }
            case CMD_SET_OUTPUT: {
                pinMode(c.pin, OUTPUT);
                Serial.write(b);
                break;
            }
            case CMD_SET_INPUT: {
                pinMode(c.pin, INPUT);
                Serial.write(b);
                break;
            }
            case CMD_SET_INPUT_PULLUP: {
                pinMode(c.pin, INPUT_PULLUP);
                Serial.write(b);
                break;
            }
            case CMD_EXTENSION: {
                switch(c.cmd_ext) {
                    case EXTCMD_GET_VERSION: {
                        Serial.write(b);
                        writeShort(VERSION_MAJOR);
                        writeShort(VERSION_MINOR);
                        break;
                    }
                    case EXTCMD_GET_MILLIS: {
                        unsigned long m = millis();
                        Serial.write(b);
                        writeLong(m);
                        break;
                    }
                    case EXTCMD_ECHO_BACK: {
                        //Acknowledge
                        Serial.write(b);
                        //Then read 2 bytes and send them back
                        while(Serial.available() == 0);
                        b = Serial.read();
                        Serial.write(b);
                        while(Serial.available() == 0);
                        b = Serial.read();
                        Serial.write(b);
                        break;
                    }
                    default: {
                        Serial.write((byte)BAD_COMMAND);
                    }
                }
                break;
            }
        }
    }
}
