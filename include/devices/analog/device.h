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

#ifndef ANALOG_DEVICE_H_
#define ANALOG_DEVICE_H_

#include <iostream>

#include <fmt/format.h>
#include <fruit/fruit.h>

#include "logger.h"

#include "gpio.h"

namespace emmerich::device {
/** @brief Analog SPI Interface

    This is the wrapper of SPI ADC devices.

    @author Ray Andrew
    @date December 2019
*/
class AnalogDevice {
 protected:
  const unsigned char _address;
  const unsigned char _bus;
  const unsigned char _flags;
  int                 _handle;

 protected:
  AnalogDevice(unsigned char address, unsigned char bus, unsigned char flags);
  virtual ~AnalogDevice();

  virtual int writeDevice(char* buf, unsigned count);
  virtual int readDevice(char* buf, unsigned count);

  virtual int writeByte(unsigned val);
  virtual int readByte();

 public:
  virtual int read(unsigned char pin) = 0;
  virtual int write(unsigned char pin, unsigned int val) = 0;
};
}  // namespace emmerich::device

#endif
