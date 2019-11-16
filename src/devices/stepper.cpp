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

#include "devices/stepper.h"

namespace emmerich::device {
class StepperImpl : public Stepper {
 private:
  std::unique_ptr<Device> _step_device;
  std::unique_ptr<Device> _direction_device;
  std::unique_ptr<Logger> _logger;

 public:
  INJECT(StepperImpl(ASSISTED(int) step_pin,
                     ASSISTED(int) direction_pin,
                     DeviceFactory deviceFactory,
                     LoggerFactory loggerFactory))
      : Stepper(step_pin, direction_pin),
        _step_device(deviceFactory(step_pin, device_mode::OUTPUT)),
        _direction_device(deviceFactory(direction_pin, device_mode::OUTPUT)),
        _logger(loggerFactory(
            fmt::format("Stepper(Step#{})(Dir#{})", step_pin, direction_pin))) {
    _logger->debug("Stepper with step pin {} and direction pin {} initialized!",
                   step_pin, direction_pin);
  }

  virtual ~StepperImpl() = default;

  virtual void step(int n, useconds_t step_delay = 5000) override {
    _logger->debug("Initiate step count : {} with step_delay : {} microseconds",
                   n, step_delay);

    for (int i = 0; i <= n; i++) {
      _step_device->write(device_output::LOW);
      usleep(step_delay);
      _step_device->write(device_output::HIGH);
      usleep(step_delay);
    }
  }

  virtual Stepper& set_direction(
      const stepper_direction& step_direction) override {
    _logger->debug("Initiate step count : {}",
                   getStepperDirectionString(step_direction));
    _direction_device->write(device_output::HIGH);
    return *this;
  }
};

fruit::Component<StepperFactory> getStepperComponent() {
  return fruit::createComponent()
      .bind<Stepper, StepperImpl>()
      .install(getLoggerComponent)
      .install(getDeviceComponent);
  ;
}
}  // namespace emmerich::device
