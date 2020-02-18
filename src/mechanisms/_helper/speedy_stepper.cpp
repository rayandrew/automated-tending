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

#include "precompiled.h"

#include "mechanisms/_helper/speedy_stepper.h"

namespace emmerich::mechanism::helper::algorithm {
SpeedyStepper::SpeedyStepper(const device::Stepper* stepper,
                             float                  stepsPerMm,
                             float                  speed,
                             float                  acceleration)
    : Movement(stepper, stepsPerMm),
      _speed(speed),
      _acceleration(acceleration) {}

void SpeedyStepper::setupMove(long target, long time) {
  long distanceToTravel;

  _targetPosition = target;

  _highestPosValue = std::abs(_targetPosition);
  if (std::abs(_currentPosition) > std::abs(_targetPosition)) {
    _highestPosValue = std::abs(_currentPosition);
  }

  _rampInitialStepPeriod = 1000000.0 / sqrt(2.0 * _acceleration);

  _desiredStepPeriod = 1000000.0 / _speed;

  _decelerationDistance =
      (long)round((_speed * _speed) / (2.0 * _acceleration));

  distanceToTravel = _targetPosition - _currentPosition;

  if (distanceToTravel < 0) {
    distanceToTravel = -distanceToTravel;
    _directionScaler = -1;
    _stepper->setDirection(device::stepper_direction::FORWARD);
  } else {
    _directionScaler = 1;
    _stepper->setDirection(device::stepper_direction::BACKWARD);
  }

  // check if travel distance is too short to
  // accelerate up to the desired velocity
  if (distanceToTravel <= (_decelerationDistance * 2L))
    _decelerationDistance = (distanceToTravel / 2L);

  // start the acceleration ramp at the beginning
  _rampNextStepPeriod = _rampInitialStepPeriod;
  _accelerationInUs = _acceleration / 1E12;
  _startMove = true;
}

long SpeedyStepper::processMovement() {
  std::chrono::time_point<std::chrono::steady_clock> currentTime;
  unsigned long                                      periodSinceLastStep;
  long                                               distanceToTargetInSteps;

  // check if already at the target position
  if (_currentPosition == _targetPosition)
    return true;

  // check if this is the first call to start this new move
  if (_startMove) {
    _rampLastStepTime = std::chrono::steady_clock::now();
    _startMove = false;
  }

  // determine how much time has elapsed since the last step (Note 1: this
  // method works even if the time has wrapped. Note 2: all variables must be
  // unsigned)
  currentTime = std::chrono::steady_clock::now();
  periodSinceLastStep = std::chrono::duration_cast<std::chrono::microseconds>(
                            currentTime - _rampLastStepTime)
                            .count();

  // if it is not time for the next step, return
  if (periodSinceLastStep < static_cast<unsigned long>(_rampNextStepPeriod))
    return false;

  // determine the distance from the current position to the target
  distanceToTargetInSteps = _targetPosition - _currentPosition;
  if (distanceToTargetInSteps < 0)
    distanceToTargetInSteps = -distanceToTargetInSteps;

  // test if it is time to start decelerating, if so change from accelerating to
  // decelerating
  if (distanceToTargetInSteps == _decelerationDistance)
    _accelerationInUs = -_accelerationInUs;

  // execute the step on the rising edge
  _stepper->pulseHigh();

  // delay set to almost nothing because there is so much code between rising
  // and falling edges
  QThread::usleep(2);

  // update the current position and speed
  _currentPosition += _directionScaler;
  _currentStepPeriod = _rampNextStepPeriod;

  // compute the period for the next step
  // StepPeriodInUS = LastStepPeriodInUS *
  //   (1 - AccelerationInStepsPerUSPerUS * LastStepPeriodInUS^2)
  _rampNextStepPeriod =
      _rampNextStepPeriod *
      (1.0 - _accelerationInUs * _rampNextStepPeriod * _rampNextStepPeriod);

  // return the step line high
  _stepper->pulseLow();

  // clip the speed so that it does not accelerate beyond the desired velocity
  if (_rampNextStepPeriod < _desiredStepPeriod)
    _rampNextStepPeriod = _desiredStepPeriod;

  // update the acceleration ramp
  _rampLastStepTime = currentTime;

  // check if move has reached its final target position, return true if all
  // done
  if (_currentPosition == _targetPosition) {
    _currentStepPeriod = 0.0;
    return true;
  }

  return false;
}

double SpeedyStepper::getPercentage() const {
  if (isMotionCompleted())
    return 100.0;

  const long absTargetPosition = std::abs(_targetPosition);
  const long absCurrentPosition = std::abs(_currentPosition);

  if (_highestPosValue == absTargetPosition) {
    return round(absCurrentPosition * 100 / absTargetPosition);
  } else {
    return round((1.0 - (static_cast<float>(absCurrentPosition) /
                         static_cast<float>(_highestPosValue))) *
                 100);
  }
}

fruit::Component<SpeedyStepperFactory>
getSpeedyStepperMovementAlgorithmComponent() {
  return fruit::createComponent().bind<Movement, SpeedyStepper>();
}
}  // namespace emmerich::mechanism::helper::algorithm
