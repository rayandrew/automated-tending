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

#include "precompiled.h"

#ifdef MOCK_GPIO

// General
int gpioInitialise(void) {
  return 0;
}

void gpioTerminate(void) {}

// Digital
int gpioSetMode(int gpio, int mode) {
  return 0;
}

int gpioRead(int gpio) {
  return 0;
}

int gpioWrite(int gpio, int level) {
  return 0;
}

// SPI
int i2cOpen(unsigned int i2cBus, unsigned int i2cAddr, unsigned int i2cFlags) {
  return 0;
}

int i2cClose(unsigned int handle) {
  return 0;
}

int i2cWriteDevice(unsigned int handle, char* buf, unsigned int count) {
  return 0;
}

int i2cReadDevice(unsigned int handle, char* buf, unsigned int count) {
  return 0;
}

int i2cWriteByte(unsigned int handle, unsigned int bVal) {
  return 0;
}

int i2cReadByte(unsigned int handle) {
  return 0;
}

// PWM

int gpioPWM(unsigned int user_gpio, unsigned int dutycycle) {
  return 0;
}

int gpioGetPWMdutycycle(unsigned int user_gpio) {
  return 0;
}

int gpioSetPWMrange(unsigned int user_gpio, unsigned int range) {
  return 0;
}

int gpioGetPWMrange(unsigned int user_gpio) {
  return 0;
}

int gpioGetPWMrealRange(unsigned int user_gpio) {
  return 0;
}

int gpioSetPWMfrequency(unsigned int user_gpio, unsigned int frequency) {
  return 0;
}

int gpioGetPWMfrequency(unsigned int user_gpio) {
  return 0;
}

int gpioHardwarePWM(unsigned int gpio,
                    unsigned int PWMfreq,
                    unsigned int PWMduty) {
  return 0;
}

#endif
