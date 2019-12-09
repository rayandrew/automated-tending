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

#ifndef DIGITAL_INPUT_DEVICE_H_
#define DIGITAL_INPUT_DEVICE_H_

#include <exception>
#include <iostream>
#include <sstream>

#include <fmt/format.h>
#include <fruit/fruit.h>

#include "devices/digital/device.h"
#include "logger.h"

namespace emmerich::device {
class DigitalInputDevice {
 public:
  DigitalInputDevice() = default;
  virtual ~DigitalInputDevice() = default;

  virtual void                 setActiveState(bool activeState) = 0;
  virtual inline device_output getStatus() const = 0;
  virtual bool                 isActive() const = 0;
};

class DigitalInputDeviceImpl : public DigitalInputDevice {
 private:
  std::unique_ptr<DigitalDevice> _device;
  Logger*                        _logger;
  bool                           _activeState = true;

 public:
  INJECT(DigitalInputDeviceImpl(ASSISTED(int) pin,
                                Logger*              logger,
                                DigitalDeviceFactory digitalDeviceFactory));
  virtual ~DigitalInputDeviceImpl() = default;

  virtual inline void setActiveState(bool activeState) override {
    _activeState = activeState;
  };

  virtual inline device_output getStatus() const override {
    device_output output = _device->read();
    if (!_activeState) {
      output = inverseOutput(output);
    }
    return output;
  }

  virtual inline bool isActive() const override {
    return getOutputModeBool(getStatus());
  }
};

using DigitalInputDeviceFactory =
    std::function<std::unique_ptr<DigitalInputDevice>(int)>;

fruit::Component<DigitalInputDeviceFactory> getDigitalInputDeviceComponent();
}  // namespace emmerich::device

#endif
