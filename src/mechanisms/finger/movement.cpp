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
FingerMovementImpl::FingerMovementImpl(QObject*               parent,
<<<<<<< Updated upstream
=======
                                       int                    x,
                                       int                    y,
                                       Logger*                logger,
>>>>>>> Stashed changes
                                       Config*                config,
                                       State*                 state,
                                       device::StepperFactory stepperFactory)
    : FingerMovement(parent),
<<<<<<< Updated upstream
      _config(std::move(config)),
      _state(std::move(state)),
=======
      _config(config),
      _state(state),
      _logger(logger),
      _x(x),
      _y(y),
>>>>>>> Stashed changes
      _xStepToCm(
          (*config)["finger"]["movement"]["x"]["step_to_cm"].as<float>()),
      _yStepToCm(
          (*config)["finger"]["movement"]["y"]["step_to_cm"].as<float>()) {
  _stepperX = stepperFactory(
      (*config)["finger"]["movement"]["x"]["step_pin"].as<int>(),
      (*config)["finger"]["movement"]["x"]["direction_pin"].as<int>());
  _stepperY = stepperFactory(
      (*config)["finger"]["movement"]["y"]["step_pin"].as<int>(),
      (*config)["finger"]["movement"]["y"]["direction_pin"].as<int>());
<<<<<<< Updated upstream
=======
  _state->moveToThread(this);
  start();
>>>>>>> Stashed changes
}

FingerMovementImpl::~FingerMovementImpl() {
  quit();
  wait();
};

void FingerMovementImpl::run() {}

void FingerMovementImpl::moveX(int x) {
  _stepperX->step(roundStepToCm(x, _xStepToCm));
  _state->setX(x);
}

void FingerMovementImpl::moveY(int y) {
  _stepperX->step(roundStepToCm(y, _yStepToCm));
}

// class FingerMovementImpl : public FingerMovement {
//   Q_OBJECT

//  private:
//   Config*                          _config;
//   State*                           _state;
//   std::unique_ptr<device::Stepper> _stepperX;
//   std::unique_ptr<device::Stepper> _stepperY;
//   const float                      _xStepToCm;
//   const float                      _yStepToCm;

//   static int roundStepToCm(int step, float stepToCm) {
//     float stepInCm = ceil(step * stepToCm);
//     // return int(step + 0.5 - (step < 0));
//     return (int)stepInCm;
//   }

//  public:
//   INJECT(FingerMovementImpl(ASSISTED(QObject*) parent,
//                             Config*                config,
//                             State*                 state,
//                             device::StepperFactory stepperFactory))
//       : FingerMovement(parent),
//         _config(std::move(config)),
//         _state(std::move(state)),
//         _xStepToCm(
//             (*config)["finger"]["movement"]["x"]["step_to_cm"].as<float>()),
//         _yStepToCm(
//             (*config)["finger"]["movement"]["y"]["step_to_cm"].as<float>()) {
//     _stepperX = stepperFactory(
//         (*config)["finger"]["movement"]["x"]["step_pin"].as<int>(),
//         (*config)["finger"]["movement"]["x"]["direction_pin"].as<int>());
//     _stepperY = stepperFactory(
//         (*config)["finger"]["movement"]["y"]["step_pin"].as<int>(),
//         (*config)["finger"]["movement"]["y"]["direction_pin"].as<int>());
//   }

//   virtual ~FingerMovementImpl() override {
//     quit();
//     wait();
//   };

//   virtual void run() override {

//   }

//   virtual void moveX(int x) override {
//     _stepperX->step(roundStepToCm(x, _xStepToCm));
//     _state->setX(x);
//   }

//   virtual void moveY(int y) override {
//     _stepperX->step(roundStepToCm(y, _yStepToCm));
//   }
// };

fruit::Component<fruit::Required<Config, Logger, State>, FingerMovementFactory>
getFingerMovementComponent() {
  return fruit::createComponent()
      .bind<FingerMovement, FingerMovementImpl>()
      .install(device::getStepperComponent);
}
}  // namespace emmerich::mechanisms::finger