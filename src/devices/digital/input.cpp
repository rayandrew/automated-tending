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

#include "devices/digital/input.h"

namespace emmerich::device {
DigitalInputDeviceImpl::DigitalInputDeviceImpl(
    int                  pin,
    Logger*              logger,
    DigitalDeviceFactory digitalDeviceFactory)
    : _device(digitalDeviceFactory(pin, device_mode::INPUT)),
      _logger(std::move(logger)) {
  _logger->debug("DigitalInputDevice with pin {} is initialized!", pin);
}

fruit::Component<DigitalInputDeviceFactory> getDigitalInputDeviceComponent() {
  return fruit::createComponent()
      .bind<DigitalInputDevice, DigitalInputDeviceImpl>()
      .install(getLoggerComponent)
      .install(getDigitalDeviceComponent);
}
}  // namespace emmerich::device
