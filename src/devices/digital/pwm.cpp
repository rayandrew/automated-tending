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

#include "precompiled.h"

#include "devices/digital/pwm.h"

namespace emmerich::device {
PWMDeviceImpl::PWMDeviceImpl(int                  pin,
                             Logger*              logger,
                             DigitalDeviceFactory digitalDeviceFactory)
    : _device(digitalDeviceFactory(pin, device_mode::PWM)),
      _logger(std::move(logger)) {
  _logger->debug("PWMDevice with pin {} is initialized!", pin);
}

void PWMDeviceImpl::setPWMDutyCycle(unsigned int dutyCycle) {
  _device->setPWMDutyCycle(dutyCycle);
}

int PWMDeviceImpl::getPWMDutyCycle() {
  return _device->getPWMDutyCycle();
}

void PWMDeviceImpl::setPWMRange(unsigned int range) {
  _device->setPWMRange(range);
}

int PWMDeviceImpl::getPWMRange() {
  return _device->getPWMRange();
}

int PWMDeviceImpl::getPWMRealRange() {
  return _device->getPWMRealRange();
}

void PWMDeviceImpl::setPWMFrequency(unsigned int frequency) {
  _device->setPWMFrequency(frequency);
}

int PWMDeviceImpl::getPWMFrequency() {
  return _device->getPWMFrequency();
}

void PWMDeviceImpl::setPWMHardware(unsigned int frequency,
                                   unsigned int dutyCycle) {
  _device->setPWMHardware(frequency, dutyCycle);
}

fruit::Component<PWMDeviceFactory> getPWMDeviceComponent() {
  return fruit::createComponent()
      .bind<PWMDevice, PWMDeviceImpl>()
      .install(getLoggerComponent)
      .install(getDigitalDeviceComponent);
  ;
}
}  // namespace emmerich::device
