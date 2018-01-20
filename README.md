# GPIOvUART
## General Purpose Input Output via Universal Asynchronous Receiver/Tranmitter

GPIOvUART is A simple C library and command line utility to configure, read, and write GPIOs over UART, such as for an arduino connected to a bluetooth devices like the HC 05.

### Dependencies

The code currently has an arduino implementation on the device side, and POSIX client on the software side, however it was designed with portability in mind and is easily usable on other platforms and devices. 

### Compiling

#### Arduino

The arduino code is located in ./firmware/arduino/gpiovuart/gpiovuart.ino. Use
the arduino IDE or tools to compile and load onto an arduino device.

#### Client

Build the library by entering ./src and running `make` 
Built the cli utility by entering ./util and running `make`

Currently the code expects a POSIX environment with a compiler that supports `-std=gnu99`

### Using the Library

See [API Reference](API.md)


### Using the command line utility
```
 Usage:
        gpiovuart -v
                -Get gpio utility version
        gpiovuart -B <baud> -D <device>
                -Optional baud setting with device.
                 When not specified, 9600 assumed.
        gpiovuart -D <device> mode        <pin>
                                (output/input/input-pullup)
                -Set pin mode for pin
        gpiovuart -D <device> read        <pin>
                -Digital read on pin
        gpiovuart -D <device> write       <pin> <val>
                -Digital write on pin. Val can be 1 or 0
        gpiovuart -D <device> analog-read <pin>
                -Analog read on pin
        gpiovuart -D <device> version
                -Get device firmware version
        gpiovuart -D <device> millis
                -Get device uptime in millis (50 day rollover)

Commands may be strung together, eg:
        gpiovuart -D <device> mode 2 output write 2 1
                Sets pin 2 to output mode and sets it high
```
### Example Use Case
#### Wirelessly Controlling Arduino GPIOs from a Raspberry Pi 3 over Bluetooth
1. Program the Arduino with the GPIOvUART Firmware
2. Connect the Arduino to a Bluetooth UART device such as the [HC_05](https://www.itead.cc/wiki/Serial_Port_Bluetooth_Module_(Master/Slave))
3. Pair with the HC_O5 Module on a Raspberry Pi with `bluetoothctl`
4. Enable bluetooth Serial Port Protocol and bind an rfcomm device to the HC_05, Where XX:XX:X:XX:XX:XX Corresponds to the HC_05's Device ID
```
sdptool add sp
rfcomm bind hci0 XX:XX:XX:XX:XX:XX
```
5. Use GPIOvUART over /dev/rfcomm0 to control the arduino's input/outputs. Occasionally another process binds to the device thinking it's a modem. You can use `lsof | grep /dev/rfcomm0` to determine which. 


### License

Released under the BSD License

See [License](LICENSE)

