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

#ifndef PCF8591_ANALOG_DEVICE_H_
#define PCF8591_ANALOG_DEVICE_H_

#include <fruit/fruit.h>

#include "logger.h"

#include "devices/analog/device.h"

namespace emmerich::device {
struct PCF8591 {};

class PCF8591Impl : public AnalogDevice {
 private:
  Logger* _logger;

 public:
  INJECT(PCF8591Impl(Logger* logger));
  virtual ~PCF8591Impl();

  virtual int read(unsigned char pin) override;
  virtual int write(unsigned char pin, unsigned int val) override;
};

fruit::Component<fruit::Annotated<PCF8591, AnalogDevice>>
getPCF8591DeviceComponent();
}  // namespace emmerich::device

#endif
