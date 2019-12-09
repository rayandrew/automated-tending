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

#include "devices/digital/stepper.h"

namespace emmerich::device {
StepperImpl::StepperImpl(int                        step_pin,
                         int                        direction_pin,
                         Logger*                    logger,
                         DigitalOutputDeviceFactory digitalOutputDeviceFactory)
    : Stepper(step_pin, direction_pin),
      _step_device(digitalOutputDeviceFactory(step_pin)),
      _direction_device(digitalOutputDeviceFactory(direction_pin)),
      _logger(std::move(logger)) {
  _logger->debug(
      "StepperDevice with step pin {} and direction pin {} is initialized!",
      step_pin, direction_pin);
}

const Stepper& StepperImpl::setDirection(
    const stepper_direction& step_direction) const {
  _logger->debug("Set step direction : {}",
                 getStepperDirectionString(step_direction));
  _direction_device->setActiveState(!_reverseDirection);
  _direction_device->on();
  return *this;
}

void StepperImpl::setReverseDirection(bool reverseDirection) {
  _logger->debug("Set reverse direction for to {}", reverseDirection);
  _reverseDirection = reverseDirection;
}

void StepperImpl::pulseHigh() {
  _step_device->on();
}

void StepperImpl::pulseLow() {
  _step_device->off();
}

fruit::Component<StepperFactory> getStepperComponent() {
  return fruit::createComponent()
      .bind<Stepper, StepperImpl>()
      .install(getLoggerComponent)
      .install(getDigitalOutputDeviceComponent);
}
}  // namespace emmerich::device
