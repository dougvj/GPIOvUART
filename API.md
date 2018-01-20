# GPIOvUART API v0.2 (Jan 2018)


---

## gpiovuart_open()

```c
gpiovuart_device* gpiovuart_open(const char* dev, int baud);
```

### Description

Creates a GPIOvUART device context for subsequent calls

### Parameters

Takes the name or path of a UART device in the first parameter followed by the 
baudrate in the second parameter. The example arduino firmware sets a baud of
9600 by default.

### Return

A pointer to the device context on success, NULL on failure

---

## gpiovuart_close()

```c
void gpiovuart_close(gpiovuart_device* dev);
```

### Description

Destroys a GPIOvUART device context.

### Parameters

A pointer to the device context. 

---
## gpiovuart_set_mode()

```c
int gpiovuart_set_mode(gpiovuart_device* dev, int pin, int mode);
```

### Description

Sets the input/output mode of a device pin.

### Parameters

- `dev` GPIOvUART Device Context
- `pin` PIN number to configure
- `mode` One of the following pin modes: `GPIOVUART_OUTPUT`, `GPIOVUART_INPUT`, `GPIOVUART_INPUT_PULLUP`

### Return

`GPIOVUART_SUCCESS` on success, one of several negative [[error codes]] on failure. 

---

## gpiovuart_write()

```c
int gpiovuart_write(gpiovuart_device* dev, int pin, int level);
```

### Description

Writes a high or low value to an output pin

### Parameters

- `dev` GPIOvUART Device Context
- `pin` PIN number to configure
- `level` Indicator of 0 for low, non- zero for high 

### Return

`GPIOVUART_SUCCESS` on success, A negative [error code](Error Codes) on failure. 

---

## gpiovuart_read()

```c
int gpiovuart_read(gpiovuart_device* dev, int pin);
```

### Description

Reads a high or low value from an input pin

### Parameters

- `dev` GPIOvUART Device Context
- `pin` PIN number to read

### Return

A non-negative integer corresponing to a high or low on success, A negative [error code](Error Codes) on failure. 

---

## gpiovuart_read_analog()

```c
int gpiovuart_read_analog(gpiovuart_device* dev, int pin);
```

### Description

Reads an ADC value on an input pin

### Parameters
- `dev` GPIOvUART Device Context
- `pin` PIN number to read

### Return
A non- negative integer corresponing to an ADC conversion, A negative [error code](Error Codes) on failure. 
---

## gpiovuart_get_lib_version()

```c
gpiovuart_version gpiovuart_get_lib_version();
```
### Description

Reads the version number of the GPIOvUART library that is currently used

### Return

The following typdef'd struct members `major` and `minor corresponding to the major and minor version number of the software

```c
typedef struct gpiovuart_version_t {
    int major, minor;
} gpiovuart_version;
```
---

## gpiovuart_get_version()

```c
gpiovuart_version gpiovuart_get_version(gpiovuart_device* dev);
```

### Description

Reads the version number of the GPIOvUART firmware running on the remote device

### Parameters

- `dev` GPIOvUART Device Context

### Return

The following typdef'd struct members `major` and `minor corresponding to the major and minor version number of the software

```c
typedef struct gpiovuart_version_t {
    int major, minor;
} gpiovuart_version;
```


---
## gpiovuart_get_version()

```c
long long gpiovuart_get_millis(gpiovuart_device* dev);
```
### Description

Returns the number of milliseconds that has elapsed since the remote device was powered on. This number may wrap (on Arduinos it's around 50 days)

### Parameters

- `dev` GPIOvUART Device Context

### Return

A large integer to handle possibly larger numbers than 50 days worth of microseconds on other firmwares or devices in the future

---

## gpiuovuart_get_err_string_()
```c
const char* gpiovuart_get_err_string(int err);
```

### Description

Returns a human readable error string corresponding to the error code given

### Parameters

- `err` An error code from among those described at the [error codes] section

---

## Error Codes

Some functions return error codes on success or failure. They are described here:

```c
#define GPIOVUART_SUCCESS       0x0
```
Indicates an operation was carried out successfully
```c
#define GPIOVUART_ERR_BADCMD   -0x1 
```
Indicates that the device recieved a bad command 
```c
#define GPIOVUART_ERR_TIMEOUT  -0x2
```
Indicates a device timeout while waiting for a response
```c
#define GPIOVUART_ERR_BADVAL   -0x3
```
Indicates that the device returned unexpected or unknown data
```c
#define GPIOVUART_ERR_DEVICE   -0x5
```
Indicates that there was an error in the local UART device
```c
#define GPIOVUART_ERR_BADPARAM -0x6
```
Indicates that a bad parameter was passed to a library call
