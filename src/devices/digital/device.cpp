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

#include "devices/digital/device.h"

#include <sstream>
#include <string>

#include "gpio.h"

namespace emmerich::device {
DigitalDeviceException::DigitalDeviceException(Logger*              logger,
                                               const std::string&   message,
                                               const int            pin,
                                               const device_mode&   mode,
                                               const device_output& output)
    : _logger(std::move(logger)),
      _message(std::move(message)),
      _pin(std::move(pin)),
      _mode(std::move(mode)),
      _output(std::move(output)) {}

const char* DigitalDeviceException::what() const noexcept {
  std::stringstream temp;
  temp << "Digital Device Exception" << std::endl;
  temp << "Pin       : " << _pin << std::endl;
  temp << "Mode      : " << getDeviceModeString(_mode) << std::endl;
  temp << "Output    : " << getOutputModeString(_output) << std::endl;
  temp << "Message   : " << _message << std::endl;

  std::string message = temp.str();

  _logger->error(message);

  return message.c_str();
}

DigitalDeviceImpl::DigitalDeviceImpl(int                pin,
                                     const device_mode& mode,
                                     Logger*            logger)
    : DigitalDevice(pin, mode), _logger(std::move(logger)) {
  setMode(mode);
}

DigitalDevice& DigitalDeviceImpl::setMode(const device_mode& mode) {
  _mode = mode;
  gpioSetMode(_pin, mode == device_mode::INPUT ? PI_INPUT : PI_OUTPUT);
  return *this;
}

void DigitalDeviceImpl::write(const device_output& output) {
  if (_mode == device_mode::INPUT) {
    throw DigitalDeviceException(
        _logger, "DEVICE_MODE 'INPUT' cannot send output to pin", _pin, _mode,
        output);
  }

  switch (output) {
    case device_output::LOW:
      gpioWrite(_pin, PI_LOW);
      break;
    case device_output::HIGH:
      gpioWrite(_pin, PI_HIGH);
      break;
    default:
      throw DigitalDeviceException(
          _logger, "Device output should not reach here", _pin, _mode, output);
  }
}

device_output DigitalDeviceImpl::read() const {
  if (_mode == device_mode::OUTPUT) {
    throw DigitalDeviceException(
        _logger, "DEVICE_MODE 'OUTPUT' cannot get input from the pin", _pin,
        _mode, device_output::LOW);
  }

  int value = gpioRead(_pin);

  return value == 0 ? device_output::LOW : device_output::HIGH;
}

fruit::Component<DigitalDeviceFactory> getDigitalDeviceComponent() {
  return fruit::createComponent()
      .bind<DigitalDevice, DigitalDeviceImpl>()
      .install(getLoggerComponent);
}
}  // namespace emmerich::device
