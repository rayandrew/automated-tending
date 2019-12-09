/*
 * Licensed under the MIT License <http: //opensource.org/licenses/MIT>.
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2019 Ray Andrew
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#ifdef MOCK_GPIO

#define PI_OFF 0
#define PI_ON 1

#define PI_CLEAR 0
#define PI_SET 1

#define PI_LOW 0
#define PI_HIGH 1

#define PI_INPUT 0
#define PI_OUTPUT 1
#define PI_ALT0 4
#define PI_ALT1 5
#define PI_ALT2 6
#define PI_ALT3 7
#define PI_ALT4 3
#define PI_ALT5 2

// General
int  gpioInitialise(void);
void gpioTerminate(void);

// Digital
int gpioSetMode(int gpio, int mode);
int gpioRead(int gpio);
int gpioWrite(int gpio, int level);

// SPI
int i2cOpen(unsigned i2cBus, unsigned i2cAddr, unsigned i2cFlags);
int i2cClose(unsigned handle);

int i2cWriteDevice(unsigned handle, char* buf, unsigned count);
int i2cReadDevice(unsigned handle, char* buf, unsigned count);

int i2cWriteByte(unsigned handle, unsigned bVal);
int i2cReadByte(unsigned handle);

#else

#include <pigpio.h>

#endif
