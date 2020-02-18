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

#include "devices/digital/output.h"

namespace emmerich::device {
DigitalOutputDeviceImpl::DigitalOutputDeviceImpl(
    int                  pin,
    Logger*              logger,
    DigitalDeviceFactory digitalDeviceFactory)
    : _device(digitalDeviceFactory(pin, device_mode::OUTPUT)),
      _logger(std::move(logger)) {
  _logger->debug("DigitalOutputDevice with pin {} is initialized!", pin);
}

void DigitalOutputDeviceImpl::setActiveState(bool activeState) {
  _activeState = activeState;
}

void DigitalOutputDeviceImpl::on() {
  _device->write(_activeState ? device_output::HIGH : device_output::LOW);
}

void DigitalOutputDeviceImpl::off() {
  _device->write(_activeState ? device_output::LOW : device_output::HIGH);
}

fruit::Component<DigitalOutputDeviceFactory> getDigitalOutputDeviceComponent() {
  return fruit::createComponent()
      .bind<DigitalOutputDevice, DigitalOutputDeviceImpl>()
      .install(getLoggerComponent)
      .install(getDigitalDeviceComponent);
}

}  // namespace emmerich::device
