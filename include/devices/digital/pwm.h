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

#ifndef DIGITAL_PWM_DEVICE_H_
#define DIGITAL_PWM_DEVICE_H_

// vendors
#include <fruit/fruit.h>

#include "logger.h"

#include "devices/digital/device.h"

namespace emmerich::device {
class PWMDevice {
 public:
  PWMDevice() = default;
  virtual ~PWMDevice() = default;

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

class PWMDeviceImpl : public PWMDevice {
 private:
  std::unique_ptr<DigitalDevice> _device;
  Logger*                        _logger;

 public:
  INJECT(PWMDeviceImpl(ASSISTED(int) pin,
                       Logger*              logger,
                       DigitalDeviceFactory digitalDeviceFactory));
  virtual ~PWMDeviceImpl() override = default;

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

using PWMDeviceFactory = std::function<std::unique_ptr<PWMDevice>(int)>;

fruit::Component<PWMDeviceFactory> getPWMDeviceComponent();
}  // namespace emmerich::device

#endif
