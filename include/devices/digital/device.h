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

#include <sstream>
#include <string>
#include <type_traits>

// vendors
#include <fruit/fruit.h>

namespace emmerich::device {
enum class device_mode {
  INPUT,
  OUTPUT,
  PWM,
};

inline const std::string getDeviceModeString(const device_mode& mode) {
  if (mode == device_mode::INPUT) {
    return "input";
  } else if (mode == device_mode::OUTPUT) {
    return "output";
  } else {
    return "pwm";
  }
}

enum class device_output {
  LOW,
  HIGH,
};

template <
    typename T,
    typename = std::enable_if<std::is_same<T, device_output>::value, bool>>
inline const std::string getOutputModeString(const device_output& output) {
  return output == device_output::LOW ? "low" : "high";
}

template <typename T,
          typename = std::enable_if<std::is_fundamental<T>::value, bool>>
inline const std::string getOutputModeString(const T output) {
  return std::to_string(output);
}

inline device_output inverseOutput(const device_output& output) {
  return output == device_output::LOW ? device_output::HIGH
                                      : device_output::LOW;
}

inline bool getOutputModeBool(const device_output& output) {
  return output == device_output::HIGH;
}

template <bool withOutput = false, typename T = device_output>
class DigitalDeviceException : public std::exception {
 private:
  const std::string _message;
  const int         _pin;
  const int         _errcode;
  const device_mode _mode;
  T                 _output;

 public:
  template <typename = std::enable_if<withOutput, bool>>
  DigitalDeviceException(const std::string& message,
                         const int          pin,
                         const device_mode& mode,
                         const T&           output,
                         const int          errcode)
      : _message(std::move(message)),
        _pin(pin),
        _mode(std::move(mode)),
        _output(std::move(output)),
        _errcode(errcode) {}

  template <typename = std::enable_if<!withOutput, bool>>
  DigitalDeviceException(const std::string& message,
                         const int          pin,
                         const device_mode& mode,
                         const int          errcode)
      : _message(std::move(message)),
        _pin(pin),
        _mode(std::move(mode)),
        _errcode(errcode) {}

  const char* what() const noexcept {
    std::stringstream temp;
    temp << "[DigitalDeviceException: {";
    temp << ", errcode : " << _errcode;
    temp << ", pin : " << _pin;
    temp << ", mode : " << getDeviceModeString(_mode);
    if (withOutput) {
      temp << ", expected : " << getOutputModeString<T>(_output);
    }
    temp << ", message : " << _message;
    temp << "}]" << std::endl;

    std::string message = temp.str();
    return message.c_str();
  }
};

typedef DigitalDeviceException<true>               DigitalOutputDeviceException;
typedef DigitalDeviceException<true, unsigned int> PWMOutputDeviceException;

class DigitalDevice {
 protected:
  const int   _pin;
  device_mode _mode;

 public:
  DigitalDevice(int pin) : _pin(pin) {}
  DigitalDevice(int pin, const device_mode& mode) : _pin(pin), _mode(mode) {}
  virtual ~DigitalDevice() = default;

  // Digital IO
  virtual void          setMode(const device_mode& mode) = 0;
  virtual void          write(const device_output& level) = 0;
  virtual device_output read() const = 0;
  inline int            getPin() const { return _pin; }
  inline device_mode    getMode() const { return _mode; }

  // PWM
  virtual void setPWMDutyCycle(unsigned int dutyCycle) = 0;
  virtual int  getPWMDutyCycle() = 0;

  virtual void setPWMRange(unsigned int range) = 0;
  virtual int  getPWMRange() = 0;

  virtual int getPWMRealRange() = 0;

  virtual void setPWMFrequency(unsigned int frequency) = 0;
  virtual int  getPWMFrequency() = 0;

  virtual void setPWMHardware(unsigned int frequency,
                              unsigned int dutyCycle) = 0;
};

class DigitalDeviceImpl : public DigitalDevice {
 public:
  INJECT(DigitalDeviceImpl(ASSISTED(int) pin,
                           ASSISTED(const device_mode&) mode));
  DigitalDeviceImpl(int pin) : DigitalDevice(pin) {}
  virtual ~DigitalDeviceImpl() = default;

  virtual void          setMode(const device_mode& mode) override;
  virtual void          write(const device_output& level) override;
  virtual device_output read() const override;

  // PWM
  virtual void setPWMDutyCycle(unsigned int dutyCycle) override;
  virtual int  getPWMDutyCycle() override;

  virtual void setPWMRange(unsigned int range) override;
  virtual int  getPWMRange() override;

  virtual int getPWMRealRange() override;

  virtual void setPWMFrequency(unsigned int frequency) override;
  virtual int  getPWMFrequency() override;

  virtual void setPWMHardware(unsigned int frequency,
                              unsigned int dutyCycle) override;
};

using DigitalDeviceFactory =
    std::function<std::unique_ptr<DigitalDevice>(int, const device_mode&)>;

fruit::Component<DigitalDeviceFactory> getDigitalDeviceComponent();
}  // namespace emmerich::device

#endif
