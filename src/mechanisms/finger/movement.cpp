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

#include "mechanisms/finger/movement.h"

namespace emmerich::mechanisms::finger {
class FingerMovementImpl : public FingerMovement {
 private:
  Config*                          _config;
  std::unique_ptr<device::Stepper> _stepperX;
  std::unique_ptr<device::Stepper> _stepperY;

 public:
  INJECT(FingerMovementImpl(Config*                config,
                            device::StepperFactory stepperFactory))
      : _config(config) {
    _stepperX = stepperFactory(
        (*config)["finger"]["movement"]["x"]["step_pin"].as<int>(),
        (*config)["finger"]["movement"]["x"]["direction_pin"].as<int>());
    _stepperY = stepperFactory(
        (*config)["finger"]["movement"]["y"]["step_pin"].as<int>(),
        (*config)["finger"]["movement"]["y"]["direction_pin"].as<int>());
  }

  virtual ~FingerMovementImpl() = default;

  virtual void moveX(int x) override {
    _stepperX->step(x);
  }

  virtual void moveY(int y) override {
    _stepperX->step(y);
  }
};

fruit::Component<FingerMovement> getFingerMovementComponent() {
  return fruit::createComponent()
      .bind<FingerMovement, FingerMovementImpl>()
      .install(getConfigComponent)
      .install(device::getStepperComponent);
}
}  // namespace emmerich::mechanisms::finger