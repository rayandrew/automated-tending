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

#include "precompiled.h"

#include "mechanisms/rotation.h"

#include <QThread>

namespace emmerich::mechanism {
RotationImpl::RotationImpl(
    Config*                            config,
    State*                             state,
    Logger*                            logger,
    device::AnalogDevice*              analogDevice,
    device::PWMDeviceFactory           pwmDeviceFactory,
    device::DigitalOutputDeviceFactory digitalOutputDeviceFactory)
    : _config(std::move(config)),
      _state(std::move(state)),
      _logger(std::move(logger)),
      _analogDevice(std::move(analogDevice)),
      _motor(
          pwmDeviceFactory((*config)["devices"]["rotation"]["pwm"].as<int>())),
      _motorComplementer(digitalOutputDeviceFactory(
          (*config)["devices"]["rotation"]["nonpwm"].as<int>())),
      _rotaryEncoderPin(
          (*_config)["devices"]["rotation"]["encoder"].as<int>()) {
  _logger->debug("Rotation mechanism is initialized!");
  _motorComplementer->off();
  // _pid = PIDBuilder<DefaultPID>::create()
  //            .setKP(0.1)
  //            .setKI(0.2)
  //            .setKD(0.3)
  //            .setSamplingTime(100.0)
  //            .setOutputLimit(0.0, 255.0)
  //            .initialize();
}

RotationImpl::~RotationImpl() {
  // delete _pid;
}

void RotationImpl::run() {
  // _motor->on();
  _motor->setPWMDutyCycle(255);

  while (_running) {
    QThread::msleep(200);
  }

  if (_running)
    finish();
}

void RotationImpl::homing() {
  // _pid->setSetPoint(0.0);
  float        deg = readRotaryDegree();
  unsigned int dutyCycle = 20;
  // unsigned char dutyCycle = math::map<unsigned char>();
  while (_running && (deg != 0.0)) {
    // dutyCycle =
    _motor->setPWMDutyCycle(dutyCycle);
    deg = readRotaryDegree();
  }

  if (_running)
    finish();
}

void RotationImpl::reset() {
  // _motor->off();
  _motorComplementer->off();
}

void RotationImpl::start() {
  Worker::start();
}

void RotationImpl::finish() {
  reset();
  Worker::finish();
}

void RotationImpl::stop() {
  reset();
  Worker::stop();
}

fruit::Component<RotationFactory> getRotationMechanismComponent() {
  return fruit::createComponent()
      .bind<Rotation, RotationImpl>()
      .install(getConfigComponent)
      .install(getStateComponent)
      .install(getLoggerComponent)
      .install(device::getDigitalOutputDeviceComponent)
      .install(device::getPWMDeviceComponent)
      .install(device::getPCF8591DeviceComponent);
}
}  // namespace emmerich::mechanism
