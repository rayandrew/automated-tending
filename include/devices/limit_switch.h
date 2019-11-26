/*
 * Licensed under the MIT License <http://opensource.org/licenses/MIT>.
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

#ifndef DEVICE_LIMIT_SWITCH_H_
#define DEVICE_LIMIT_SWITCH_H_

#pragma once

#include <unistd.h>

#include <QObject>
#include <QString>
#include <QThread>

#include <fmt/format.h>
#include <fruit/fruit.h>

#include "devices/device.h"
#include "logger.h"

namespace emmerich::device {
class LimitSwitch {
 protected:
  const int _pin;

 public:
  LimitSwitch(int pin) : _pin(pin) {}
  virtual ~LimitSwitch() = default;
  virtual const device_output getStatus() const = 0;
  virtual bool                getStatusBool() const = 0;
};

class LimitSwitchImpl : public LimitSwitch {
 private:
  std::unique_ptr<Device> _limit_switch_device;
  Logger*                 _logger;

 public:
  INJECT(LimitSwitchImpl(ASSISTED(int) pin,
                         Logger*       logger,
                         DeviceFactory deviceFactory));

  virtual ~LimitSwitchImpl() = default;

  virtual inline const device_output getStatus() const override {
    return _limit_switch_device->read();
  }

  virtual inline bool getStatusBool() const override {
    return getOutputModeBool(_limit_switch_device->read());
  }
};

using LimitSwitchFactory = std::function<std::unique_ptr<LimitSwitch>(int)>;

fruit::Component<LimitSwitchFactory> getLimitSwitchComponent();
}  // namespace emmerich::device

#endif
