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

#ifndef MECHANISM_ROTATION_H_
#define MECHANISM_ROTATION_H_

#include <QObject>

#include <fruit/fruit.h>

#include "common.h"

#include "config.h"
#include "logger.h"
#include "state.h"

#include "utils/worker.h"

#include "devices/analog/PCF8591.h"
#include "devices/analog/device.h"

#include "devices/digital/output.h"
#include "devices/digital/pwm.h"

#include "utils/math.h"
#include "utils/pid.h"

namespace emmerich::mechanism {
class Rotation : public Worker {
  Q_OBJECT

 public:
  Rotation() = default;
  virtual ~Rotation() = default;
  virtual void run() = 0;
  virtual void homing() = 0;
};

class RotationImpl : public Rotation {
  Q_OBJECT

 private:  // injected state
  Config*                                            _config;
  State*                                             _state;
  Logger*                                            _logger;
  device::AnalogDevice*                              _analogDevice;
  const int                                          _rotaryEncoderPin;
  const std::unique_ptr<device::PWMDevice>           _motor;
  const std::unique_ptr<device::DigitalOutputDevice> _motorComplementer;
  // PID*                                               _pid;

 private:
  inline float readRotaryDegree() {
    return math::convertToDeg(_analogDevice->read(_rotaryEncoderPin));
  }
  void reset();

 public:
  INJECT(RotationImpl(
      Config* config,
      State*  state,
      Logger* logger,
      ANNOTATED(device::PCF8591, device::AnalogDevice*) analogDevice,
      device::PWMDeviceFactory           pwmDeviceFactory,
      device::DigitalOutputDeviceFactory digitalOutputDeviceFactory));

  virtual ~RotationImpl() override;
  virtual void homing() override;

 public slots:
  virtual void run() override;
  virtual void start() override;
  virtual void finish() override;
  virtual void stop() override;
};

using RotationFactory = std::function<std::unique_ptr<Rotation>()>;

fruit::Component<RotationFactory> getRotationMechanismComponent();
}  // namespace emmerich::mechanism

#endif
