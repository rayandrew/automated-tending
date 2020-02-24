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

#include "devices/digital/device.h"

#include "gpio.h"

namespace emmerich::device {
DigitalDeviceImpl::DigitalDeviceImpl(int pin, const device_mode& mode)
    : DigitalDevice(pin, mode) {
  setMode(mode);
}

void DigitalDeviceImpl::setMode(const device_mode& mode) {
  _mode = mode;
  int res =
      gpioSetMode(_pin, mode == device_mode::INPUT ? PI_INPUT : PI_OUTPUT);
  if (res != PI_OK) {
    throw DigitalDeviceException(
        "[DigitalDevice::setMode] Cannot set device mode to ", _pin, _mode,
        res);
  }
}

void DigitalDeviceImpl::write(const device_output& output) {
  if (_mode != device_mode::OUTPUT) {
    throw DigitalOutputDeviceException(
        "[DigitalDevice::write] ONLY DEVICE_MODE 'OUTPUT' that can send output "
        "to "
        "pin",
        _pin, _mode, output, PI_BAD_GPIO);
  }

  int res;
  switch (output) {
    case device_output::LOW:
      res = gpioWrite(_pin, PI_LOW);
      break;
    case device_output::HIGH:
      res = gpioWrite(_pin, PI_HIGH);
      break;
    default:
      throw DigitalOutputDeviceException(
          "[DigitalDevice::write] Device output should not reach here", _pin,
          _mode, output, PI_BAD_LEVEL);
  }

  if (res != PI_OK) {
    throw DigitalOutputDeviceException(
        "[DigitalDevice::write] failed to write digital output value", _pin,
        _mode, output, res);
  }
}

device_output DigitalDeviceImpl::read() const {
  if (_mode != device_mode::INPUT) {
    throw DigitalDeviceException(
        "[DigitalDevice::read] ONLY DEVICE_MODE "
        "'INPUT' that can get value of digital input from the pin",
        _pin, _mode, PI_BAD_GPIO);
  }

  int res = gpioRead(_pin);

  if (res == PI_BAD_GPIO) {
    throw DigitalDeviceException(
        "[DigitalDevice::read] failed to read value of digital input", _pin,
        _mode, res);
  }

  return res == 0 ? device_output::LOW : device_output::HIGH;
}

void DigitalDeviceImpl::setPWMDutyCycle(unsigned int dutyCycle) {
  if (_mode != device_mode::PWM) {
    throw PWMOutputDeviceException(
        "[DigitalDevice::setPWMDutyCycle] ONLY DEVICE_MODE "
        "'PWM' that can set value of pwm duty cycle",
        _pin, _mode, dutyCycle, PI_BAD_GPIO);
  }

  int res = gpioPWM(_pin, dutyCycle);

  if (res != PI_OK) {
    throw PWMOutputDeviceException(
        "[DigitalDevice::setPWMDutyCycle] failed to "
        "set value of pwm duty cycle",
        _pin, _mode, dutyCycle, res);
  }
}

int DigitalDeviceImpl::getPWMDutyCycle() {
  if (_mode != device_mode::PWM) {
    throw DigitalDeviceException(
        "[DigitalDevice::getPWMDutyCycle] ONLY DEVICE_MODE "
        "'PWM' that can get value of pwm duty cycle",
        _pin, _mode, PI_BAD_GPIO);
  }

  int res = gpioGetPWMdutycycle(_pin);

  if (res == PI_BAD_USER_GPIO || res == PI_NOT_PWM_GPIO) {
    throw DigitalDeviceException(
        "[DigitalDevice::getPWMDutyCycle] failed to "
        "get value of pwm duty cycle",
        _pin, _mode, res);
  }

  return res;
}

void DigitalDeviceImpl::setPWMRange(unsigned int range) {
  if (_mode != device_mode::PWM) {
    throw PWMOutputDeviceException(
        "[DigitalDevice::setPWMRange] ONLY DEVICE_MODE "
        "'PWM' that can set value of pwm range",
        _pin, _mode, range, PI_BAD_GPIO);
  }

  int res = gpioSetPWMrange(_pin, range);

  if (res == PI_BAD_USER_GPIO || res == PI_BAD_DUTYRANGE) {
    throw PWMOutputDeviceException(
        "[DigitalDevice::setPWMRange] failed to set value of pwm range", _pin,
        _mode, range, res);
  }
}

int DigitalDeviceImpl::getPWMRange() {
  if (_mode != device_mode::PWM) {
    throw DigitalDeviceException(
        "[DigitalDevice::getPWMRange] ONLY DEVICE_MODE "
        "'PWM' that can get value of pwm range",
        _pin, _mode, PI_BAD_GPIO);
  }

  int res = gpioGetPWMrange(_pin);

  if (res == PI_BAD_USER_GPIO) {
    throw DigitalDeviceException(
        "[DigitalDevice::getPWMRange] failed to get value of pwm range", _pin,
        _mode, res);
  }

  return res;
}

int DigitalDeviceImpl::getPWMRealRange() {
  if (_mode != device_mode::PWM) {
    throw DigitalDeviceException(
        "[DigitalDevice::getPWMRealRange] ONLY DEVICE_MODE "
        "'PWM' that can get value of pwm real range",
        _pin, _mode, PI_BAD_GPIO);
  }

  int res = gpioGetPWMrealRange(_pin);

  if (res == PI_BAD_USER_GPIO) {
    throw DigitalDeviceException(
        "[DigitalDevice::getPWMRealRange] failed to "
        "get value of pwm real range",
        _pin, _mode, res);
  }

  return res;
}

void DigitalDeviceImpl::setPWMFrequency(unsigned int frequency) {
  if (_mode != device_mode::PWM) {
    throw PWMOutputDeviceException(
        "[DigitalDevice::setPWMFrequency] ONLY DEVICE_MODE "
        "'PWM' that can set value of pwm frequency",
        _pin, _mode, frequency, PI_BAD_GPIO);
  }

  int res = gpioSetPWMfrequency(_pin, frequency);

  if (res == PI_BAD_USER_GPIO) {
    throw PWMOutputDeviceException(
        "[DigitalDevice::setPWMFrequency] failed to "
        "set value of pwm frequency",
        _pin, _mode, frequency, PI_BAD_GPIO);
  }
}

int DigitalDeviceImpl::getPWMFrequency() {
  if (_mode == device_mode::INPUT) {
    throw DigitalDeviceException(
        "[DigitalDevice::getPWMFrequency] ONLY DEVICE_MODE "
        "'PWM' that can get value of pwm frequency",
        _pin, _mode, PI_BAD_GPIO);
  }

  int res = gpioGetPWMfrequency(_pin);

  if (res == PI_BAD_USER_GPIO) {
    throw DigitalDeviceException(
        "[DigitalDevice::getPWMFrequency] failed to get value of pwm frequency",
        _pin, _mode, res);
  }

  return res;
}

void DigitalDeviceImpl::setPWMHardware(unsigned int frequency,
                                       unsigned int dutyCycle) {
  if (_mode != device_mode::PWM) {
    throw DigitalDeviceException(
        "[DigitalDevice::setPWMHardware] ONLY DEVICE_MODE "
        "'PWM' that can set value of pwm hardware",
        _pin, _mode, PI_BAD_GPIO);
  }

  int res = gpioHardwarePWM(_pin, frequency, dutyCycle);

  if (res == PI_BAD_USER_GPIO) {
    std::stringstream ss;
    ss << "[DigitalDevice::setPWMFrequency] cannot set pwm freq : ";
    ss << frequency << " ";
    ss << "with duty cycle : ";
    ss << dutyCycle;
    throw PWMOutputDeviceException(ss.str(), _pin, _mode, dutyCycle, res);
  }
}

fruit::Component<DigitalDeviceFactory> getDigitalDeviceComponent() {
  return fruit::createComponent().bind<DigitalDevice, DigitalDeviceImpl>();
}
}  // namespace emmerich::device
