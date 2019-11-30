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

#ifndef DEVICE_STEPPER_H_
#define DEVICE_STEPPER_H_

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
enum class stepper_direction {
  FORWARD,
  BACKWARD,
};

inline const std::string getStepperDirectionString(
    const stepper_direction& step_direction) {
  if (step_direction == stepper_direction::FORWARD) {
    return "forward";
  } else {
    return "backward";
  }
}

class Stepper {
 protected:
  const int _step_pin;
  const int _direction_pin;

 public:
  Stepper(int step_pin, int direction_pin)
      : _step_pin(step_pin), _direction_pin(direction_pin) {}
  virtual ~Stepper() = default;
  virtual const Stepper& setDirection(
      const stepper_direction& step_direction) const = 0;
  virtual void setReverseDirection(bool reverseDirection) = 0;
  virtual void pulseHigh() = 0;
  virtual void pulseLow() = 0;
};

class StepperImpl : public Stepper {
 private:
  std::unique_ptr<Device> _step_device;
  std::unique_ptr<Device> _direction_device;
  Logger*                 _logger;
  bool                    _reverseDirection = false;

 public:
  INJECT(StepperImpl(ASSISTED(int) step_pin,
                     ASSISTED(int) direction_pin,
                     Logger*       logger,
                     DeviceFactory deviceFactory));

  virtual ~StepperImpl() = default;

  virtual const Stepper& setDirection(
      const stepper_direction& step_direction) const override;
  virtual void setReverseDirection(bool reverseDirection) override;

  virtual void pulseHigh() override;
  virtual void pulseLow() override;
};

using StepperFactory = std::function<std::unique_ptr<Stepper>(int, int)>;

fruit::Component<StepperFactory> getStepperComponent();
}  // namespace emmerich::device

#endif
