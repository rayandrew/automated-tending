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

#ifndef DIGITAL_DEVICE_H_
#define DIGITAL_DEVICE_H_

#include <fruit/fruit.h>

#include "logger.h"

namespace emmerich::device {
enum class device_mode {
  INPUT,
  OUTPUT,
};

inline const std::string getDeviceModeString(const device_mode& mode) {
  if (mode == device_mode::INPUT) {
    return "input";
  } else {
    return "output";
  }
}

enum class device_output {
  LOW,
  HIGH,
};

inline const std::string getOutputModeString(const device_output& output) {
  return output == device_output::LOW ? "low" : "high";
}

inline device_output inverseOutput(const device_output& output) {
  return output == device_output::LOW ? device_output::HIGH
                                      : device_output::LOW;
}

inline bool getOutputModeBool(const device_output& output) {
  return output == device_output::HIGH;
}

class DigitalDeviceException : public std::exception {
 private:
  Logger*             _logger;
  const std::string   _message;
  const int           _pin;
  const device_mode   _mode;
  const device_output _output;

 public:
  DigitalDeviceException(Logger*              logger,
                         const std::string&   message,
                         const int            pin,
                         const device_mode&   mode,
                         const device_output& output);
  const char* what() const noexcept;
};

class DigitalDevice {
 protected:
  const int   _pin;
  device_mode _mode;

 public:
  DigitalDevice(int pin) : _pin(pin) {}
  DigitalDevice(int pin, const device_mode& mode) : _pin(pin), _mode(mode) {}
  virtual ~DigitalDevice() = default;

  virtual DigitalDevice& setMode(const device_mode& mode) = 0;
  virtual void           write(const device_output& level) = 0;
  virtual device_output  read() const = 0;
  inline int             getPin() const { return _pin; }
  inline device_mode     getMode() const { return _mode; }
};

class DigitalDeviceImpl : public DigitalDevice {
 private:
  Logger* _logger;

 public:
  INJECT(DigitalDeviceImpl(ASSISTED(int) pin,
                           ASSISTED(const device_mode&) mode,
                           Logger* logger));
  DigitalDeviceImpl(int pin) : DigitalDevice(pin) {}
  virtual ~DigitalDeviceImpl() = default;

  virtual DigitalDevice& setMode(const device_mode& mode) override;
  virtual void           write(const device_output& level) override;
  virtual device_output  read() const override;
};

using DigitalDeviceFactory =
    std::function<std::unique_ptr<DigitalDevice>(int, const device_mode&)>;

fruit::Component<DigitalDeviceFactory> getDigitalDeviceComponent();
}  // namespace emmerich::device

#endif
