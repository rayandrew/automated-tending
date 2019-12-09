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

#include "devices/analog/PCF8591.h"

namespace emmerich::device {
PCF8591Impl::PCF8591Impl(Logger* logger)
    : AnalogDevice(0x48, 1, 0), _logger(logger) {
  _logger->debug(
      "PCF8591Device with address {}, bus {}, and flags {} is initialized",
      _address, _bus, _flags);
}

PCF8591Impl::~PCF8591Impl() {}

int PCF8591Impl::read(unsigned char pin) {
  unsigned char command[1];
  command[0] = 0x40 | (pin & 0x03);
  writeDevice(reinterpret_cast<char*>(&command), 1);
  return readByte();
}

int PCF8591Impl::write(unsigned char pin, unsigned int val) {
  unsigned char command[2];
  command[0] = 0x40 | (pin & 0x03);
  command[1] = val;
  writeDevice(reinterpret_cast<char*>(&command), 2);
  return readByte();
}

fruit::Component<fruit::Annotated<PCF8591, AnalogDevice>>
getPCF8591DeviceComponent() {
  return fruit::createComponent()
      .bind<fruit::Annotated<PCF8591, AnalogDevice>, PCF8591Impl>()
      .install(getLoggerComponent);
}
}  // namespace emmerich::device
