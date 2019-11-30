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

#ifndef INPUT_DEVICE_H_
#define INPUT_DEVICE_H_

#pragma once

#include <exception>
#include <iostream>
#include <sstream>

#include <fmt/format.h>
#include <fruit/fruit.h>

#include "devices/device.h"
#include "logger.h"

namespace emmerich::device {
class InputDevice {
 public:
  InputDevice() = default;
  virtual ~InputDevice() = default;

  virtual inline device_output getStatus() const = 0;
  virtual bool                 isActive() const = 0;
};

class InputDeviceImpl : public InputDevice {
 private:
  std::unique_ptr<Device> _device;
  Logger*                 _logger;

 public:
  INJECT(InputDeviceImpl(ASSISTED(int) pin,
                         Logger*       logger,
                         DeviceFactory deviceFactory));
  virtual ~InputDeviceImpl() = default;

  virtual inline device_output getStatus() const override {
    return _device->read();
  }

  virtual inline bool isActive() const override {
    return getOutputModeBool(_device->read());
  }
};

using InputDeviceFactory = std::function<std::unique_ptr<InputDevice>(int)>;

fruit::Component<InputDeviceFactory> getInputDeviceComponent();
}  // namespace emmerich::device

#endif
