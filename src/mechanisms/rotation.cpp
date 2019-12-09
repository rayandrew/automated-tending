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

#include "mechanisms/rotation.h"

namespace emmerich::mechanism {
RotationImpl::RotationImpl(
    Config*                            config,
    State*                             state,
    Logger*                            logger,
    device::AnalogDevice*              analogDevice,
    device::DigitalOutputDeviceFactory digitalOutputDeviceFactory)
    : _config(std::move(config)),
      _state(std::move(state)),
      _logger(std::move(logger)),
      _analogDevice(std::move(analogDevice)),
      _motor(digitalOutputDeviceFactory(
          (*config)["devices"]["rotation"]["motor"].as<int>())) {
  _logger->debug("Rotation mechanism is initialized!");
}

RotationImpl::~RotationImpl() {}

void RotationImpl::run() {
  while (_running) {
    _motor->on();
    _state->setDegree(readRotaryDegree());
  }

  if (_running)
    finish();
}

void RotationImpl::homing() {
  if (_running)
    finish();
}

void RotationImpl::reset() {
  _motor->off();
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
      .install(device::getPCF8591DeviceComponent);
}
}  // namespace emmerich::mechanism
