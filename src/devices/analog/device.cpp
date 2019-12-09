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

#include "devices/analog/device.h"

namespace emmerich::device {
AnalogDevice::AnalogDevice(unsigned char address,
                           unsigned char bus,
                           unsigned char flags)
    : _address(address), _bus(bus), _flags(flags) {
  _handle = i2cOpen(bus, address, flags);
}

AnalogDevice::~AnalogDevice() {
  i2cClose(_handle);
}

int AnalogDevice::writeDevice(char* buf, unsigned int count) {
  return i2cWriteDevice(_handle, buf, count);
}

int AnalogDevice::readDevice(char* buf, unsigned int count) {
  return i2cReadDevice(_handle, buf, count);
}

int AnalogDevice::writeByte(unsigned int val) {
  return i2cWriteByte(_handle, val);
}

int AnalogDevice::readByte() {
  return i2cReadByte(_handle);
}
}  // namespace emmerich::device
