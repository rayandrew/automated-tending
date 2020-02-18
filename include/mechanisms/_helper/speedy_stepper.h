/*
 * Licensed under the MIT License <http://opensource.org/licenses/MIT>.
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2014 Stanley Reifel & Co.
 * Modified by Ray Andrew <raydreww@gmail.com>
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

#ifndef MECHANISM_HELPER_SPEEDY_STEPPER_H_
#define MECHANISM_HELPER_SPEEDY_STEPPER_H_

#include <chrono>
#include <cmath>

// vendors
#include <fruit/fruit.h>

#include <QThread>

#include "mechanisms/_helper/movement.h"

namespace emmerich::mechanism::helper::algorithm {
class SpeedyStepper : public Movement {
 private:
  bool                                               _startMove;
  long                                               _decelerationDistance = 0;
  int                                                _directionScaler = 1;
  float                                              _currentStepPeriod = 0.0;
  float                                              _desiredStepPeriod = 0.0;
  float                                              _acceleration = 0.0;
  float                                              _accelerationInUs = 0.0;
  float                                              _speed = 0.0;
  std::chrono::time_point<std::chrono::steady_clock> _rampLastStepTime;
  float _rampInitialStepPeriod = 0.0;
  float _rampNextStepPeriod = 0.0;
  long  _highestPosValue = 0.0;

 public:
  INJECT(SpeedyStepper(ASSISTED(const device::Stepper*) stepper,
                       ASSISTED(float) stepsPerMm,
                       ASSISTED(float) speed,
                       ASSISTED(float) acceleration));
  virtual ~SpeedyStepper() override = default;
  virtual void   setupMove(long steps, long time = 0) override;
  virtual long   processMovement() override;
  virtual double getPercentage() const override;
};

using SpeedyStepperFactory = std::function<
    std::unique_ptr<Movement>(const device::Stepper*, float, float, float)>;

fruit::Component<SpeedyStepperFactory>
getSpeedyStepperMovementAlgorithmComponent();
}  // namespace emmerich::mechanism::helper::algorithm

#endif
