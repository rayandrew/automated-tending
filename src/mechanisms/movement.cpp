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

#include "mechanisms/movement.h"

#include <cmath>

#include "general_config.h"

namespace emmerich::mechanism {
MovementImpl::MovementImpl(
    Config*                            config,
    State*                             state,
    Logger*                            logger,
    device::DigitalInputDeviceFactory  digitalInputDeviceFactory,
    device::DigitalOutputDeviceFactory digitalOutputDeviceFactory,
    device::StepperFactory             stepperFactory)
    : _config(std::move(config)),
      _state(std::move(state)),
      _logger(std::move(logger)),
      _sleepDevice(digitalOutputDeviceFactory(
          (*config)["devices"]["movement"]["sleep_pin"].as<int>())),
      _stepperX(stepperFactory(
          (*config)["devices"]["movement"]["x"]["step_pin"].as<int>(),
          (*config)["devices"]["movement"]["x"]["direction_pin"].as<int>())),
      _stepperY(stepperFactory(
          (*config)["devices"]["movement"]["y"]["step_pin"].as<int>(),
          (*config)["devices"]["movement"]["y"]["direction_pin"].as<int>())),
      _limitSwitchHomeX(digitalInputDeviceFactory(
          (*config)["devices"]["movement"]["limit_switch_pin"]["home_x"]
              .as<int>())),
      _limitSwitchHomeY(digitalInputDeviceFactory(
          (*config)["devices"]["movement"]["limit_switch_pin"]["home_y"]
              .as<int>())),
      _limitSwitchEdge(digitalInputDeviceFactory(
          (*config)["devices"]["movement"]["limit_switch_pin"]["edge"]
              .as<int>())),
      _limitSwitchBinDetection(digitalInputDeviceFactory(
          (*config)["devices"]["movement"]["limit_switch_pin"]["bin_detection"]
              .as<int>())) {
  _logger->debug("Movement mechanism is initialized!");
  _sleepDevice->setActiveState(false);
  _stepperX->setReverseDirection(
      (*config)["devices"]["movement"]["x"]["reversed"].as<bool>());
  _stepperY->setReverseDirection(
      (*config)["devices"]["movement"]["y"]["reversed"].as<bool>());

  XMovementState.speed = (*config)["devices"]["movement"]["speed"].as<float>();
  XMovementState.acceleration =
      (*config)["devices"]["movement"]["acceleration"].as<float>();
  XMovementState.stepPerMm =
      ceil((*config)["devices"]["movement"]["x"]["step_per_mm"].as<float>());

  YMovementState.speed = (*config)["devices"]["movement"]["speed"].as<float>();
  YMovementState.acceleration =
      (*config)["devices"]["movement"]["acceleration"].as<float>();
  YMovementState.stepPerMm =
      ceil((*config)["devices"]["movement"]["y"]["step_per_mm"].as<float>());

  loadPathsFromFile(_edgePaths, PROJECT_MOVEMENT_EDGE_FILE);
  loadPathsFromFile(_zigzagPaths, PROJECT_MOVEMENT_ZIGZAG_FILE);
}

bool MovementImpl::motionComplete(const MovementState& movementState) {
  return movementState.currentPosition == movementState.targetPosition;
}

void MovementImpl::setupMoveInMillimeters(
    const device::Stepper* stepper,
    MovementState&         movementState,
    float                  absolutePositionToMoveToInMillimeters) {
  setupMoveInSteps(stepper, movementState,
                   (long)round(absolutePositionToMoveToInMillimeters *
                               movementState.stepPerMm));
}

void MovementImpl::setupMoveInSteps(const device::Stepper* stepper,
                                    MovementState&         movementState,
                                    long absolutePositionToMoveInSteps) {
  long distanceToTravel;

  movementState.targetPosition = absolutePositionToMoveInSteps;

  movementState.rampInitialStepPeriod =
      1000000.0 / sqrt(2.0 * movementState.acceleration);

  movementState.desiredStepPeriod = 1000000.0 / movementState.speed;

  movementState.decelerationDistance =
      (long)round((movementState.speed * movementState.speed) /
                  (2.0 * movementState.acceleration));

  distanceToTravel =
      movementState.targetPosition - movementState.currentPosition;
  if (distanceToTravel < 0) {
    distanceToTravel = -distanceToTravel;
    movementState.directionScaler = -1;
    stepper->setDirection(device::stepper_direction::FORWARD);
    // digitalWrite(directionPin, HIGH);
  } else {
    movementState.directionScaler = 1;
    stepper->setDirection(device::stepper_direction::BACKWARD);
    // digitalWrite(directionPin, LOW);
  }

  //
  // check if travel distance is too short to accelerate up to the desired
  // velocity
  //
  if (distanceToTravel <= (movementState.decelerationDistance * 2L))
    movementState.decelerationDistance = (distanceToTravel / 2L);

  //
  // start the acceleration ramp at the beginning
  //
  movementState.rampNextStepPeriod = movementState.rampInitialStepPeriod;
  movementState.acceleration = movementState.acceleration / 1E12;
  movementState.startMove = true;
}

bool MovementImpl::processMovement(const device::Stepper* stepper,
                                   MovementState&         movementState) {
  std::chrono::time_point<std::chrono::system_clock> currentTime;
  unsigned long                                      periodSinceLastStep;
  long                                               distanceToTargetInSteps;

  //
  // check if already at the target position
  //
  if (movementState.currentPosition == movementState.targetPosition)
    return true;

  // check if this is the first call to start this new move
  if (movementState.startMove) {
    movementState.rampLastStepTime = std::chrono::system_clock::now();
    movementState.startMove = false;
  }

  //
  // determine how much time has elapsed since the last step (Note 1: this
  // method works even if the time has wrapped. Note 2: all variables must be
  // unsigned)
  //
  currentTime = std::chrono::system_clock::now();
  periodSinceLastStep = std::chrono::duration_cast<std::chrono::microseconds>(
                            movementState.rampLastStepTime - currentTime)
                            .count();

  //
  // if it is not time for the next step, return
  //
  if (periodSinceLastStep < (unsigned long)movementState.rampNextStepPeriod)
    return false;

  //
  // determine the distance from the current position to the target
  //
  distanceToTargetInSteps =
      movementState.targetPosition - movementState.currentPosition;
  // _logger->debug("Distance to target in steps {} {} {}",
  //                distanceToTargetInSteps, movementState.targetPosition,
  //                movementState.currentPosition);
  if (distanceToTargetInSteps < 0)
    distanceToTargetInSteps = -distanceToTargetInSteps;

  //
  // test if it is time to start decelerating, if so change from accelerating to
  // decelerating
  //
  if (distanceToTargetInSteps == movementState.decelerationDistance)
    movementState.acceleration = -movementState.acceleration;

  //
  // execute the step on the rising edge
  //
  // digitalWrite(stepPin, HIGH);
  stepper->pulseHigh();

  //
  // delay set to almost nothing because there is so much code between rising
  // and falling edges
  QThread::msleep(2);
  // delayMicroseconds(2);

  //
  // update the current position and speed
  //
  movementState.currentPosition += movementState.directionScaler;
  movementState.currentStepPeriod = movementState.rampNextStepPeriod;

  //
  // compute the period for the next step
  // StepPeriodInUS = LastStepPeriodInUS *
  //   (1 - AccelerationInStepsPerUSPerUS * LastStepPeriodInUS^2)
  //
  movementState.rampNextStepPeriod =
      movementState.rampNextStepPeriod *
      (1.0 - movementState.acceleration * movementState.rampNextStepPeriod *
                 movementState.rampNextStepPeriod);

  //
  // return the step line high
  //
  stepper->pulseLow();
  // digitalWrite(stepPin, LOW);

  //
  // clip the speed so that it does not accelerate beyond the desired velocity
  //
  if (movementState.rampNextStepPeriod < movementState.desiredStepPeriod)
    movementState.rampNextStepPeriod = movementState.desiredStepPeriod;

  //
  // update the acceleration ramp
  //
  movementState.rampLastStepTime = currentTime;

  //
  // check if move has reached its final target position, return true if all
  // done
  //
  if (movementState.currentPosition == movementState.targetPosition) {
    movementState.currentStepPeriod = 0.0;
    return true;
  }

  return false;
}

void MovementImpl::move(const Point& point) {
  move(point.x, point.y);
}

void MovementImpl::move(int x, int y) {
  setupMoveInMillimeters(_stepperX.get(), XMovementState, (float)x * 10);
  setupMoveInMillimeters(_stepperY.get(), YMovementState, (float)y * 10);

  _isLimitSwitchEdgeTriggered = _limitSwitchEdge->isActive();

  while (_running && !_isLimitSwitchEdgeTriggered &&
         !(motionComplete(XMovementState) && motionComplete(YMovementState))) {
    if (!XMovementState.motionComplete())
      processMovement(_stepperX.get(), XMovementState);
    if (!YMovementState.motionComplete())
      processMovement(_stepperY.get(), YMovementState);

    if ((XMovementState.currentPosition <= XMovementState.targetPosition) &&
        ((XMovementState.currentPosition % (XMovementState.stepPerMm * 10)) ==
         0)) {
      _state->setX(XMovementState.currentPosition /
                   (XMovementState.stepPerMm * 10));
    }

    if ((YMovementState.currentPosition <= YMovementState.targetPosition) &&
        ((YMovementState.currentPosition % (YMovementState.stepPerMm * 10)) ==
         0)) {
      _state->setY(YMovementState.currentPosition /
                   (YMovementState.stepPerMm * 10));
    }

    double percentage = round((XMovementState.getPercentage() / 2) +
                              (YMovementState.getPercentage() / 2));

    _state->setProgress(percentage);
    _isLimitSwitchEdgeTriggered = _limitSwitchEdge->isActive();
  }

  if (_isLimitSwitchEdgeTriggered) {
    _logger->debug("Is edge limit switch triggered {}",
                   _isLimitSwitchEdgeTriggered);
  }

  if (_running && !_isLimitSwitchEdgeTriggered) {
    _state->setX(x);
    _state->setY(y);
  }
}

// void MovementImpl::move(int x, int y) {
//   int diffX = x - _state->getX();
//   int diffY = y - _state->getY();

//   _stepperX->setDirection(diffX >= 0 ? device::stepper_direction::FORWARD
//                                      : device::stepper_direction::BACKWARD);
//   _stepperY->setDirection(diffY >= 0 ? device::stepper_direction::FORWARD
//                                      : device::stepper_direction::BACKWARD);

//   int xStep = mmToSteps(abs(diffX), XMovementState.stepPerMm);
//   int yStep = mmToSteps(abs(diffY), YMovementState.stepPerMm);
//   int maxStep = std::max(xStep, yStep);

//   _logger->debug("Moving with x step: {} y step: {} maxStep: {}", xStep,
//   yStep,
//                  maxStep);

//   int step = 1;

//   _isLimitSwitchEdgeTriggered = _limitSwitchEdge->isActive();

//   while (_running && (step <= maxStep) && !_isLimitSwitchEdgeTriggered) {
//     if (step <= XMovementState.stepPerMm)
//       _stepperX->pulseHigh();

//     if (step <= YMovementState.stepPerMm)
//       _stepperY->pulseHigh();

//     QThread::usleep(_delay);

//     if (step <= xStep)
//       _stepperX->pulseLow();

//     if (step <= yStep)
//       _stepperY->pulseLow();

//     QThread::usleep(_delay);

//     if (step <= xStep && ((step % XMovementState.stepPerMm) == 0)) {
//       int currentX = _state->getX();
//       int newX = diffX >= 0 ? currentX + 1 : currentX - 1;
//       _state->setX(newX);
//     }

//     if (step <= yStep && ((step % YMovementState.stepPerMm) == 0)) {
//       int currentY = _state->getY();
//       int newY = diffY >= 0 ? currentY + 1 : currentY - 1;
//       _state->setY(newY);
//     }

//     _state->setProgress(round(float(step) / float(maxStep) * 100));
//     ++step;

//     _isLimitSwitchEdgeTriggered = _limitSwitchEdge->isActive();
//   }

//   if (_isLimitSwitchEdgeTriggered) {
//     _logger->debug("Is edge limit switch triggered {}",
//                    _isLimitSwitchEdgeTriggered);
//   }

//   if (_running && !_isLimitSwitchEdgeTriggered) {
//     _state->setX(x);
//     _state->setY(y);
//   }
// }

void MovementImpl::processPaths(const std::queue<Point>& paths) {
  std::queue<Point> tempPaths = paths;

  _isLimitSwitchEdgeTriggered = _limitSwitchEdge->isActive();
  while (_running && !_isLimitSwitchEdgeTriggered && !tempPaths.empty()) {
    const Point point = tempPaths.front();
    _logger->debug("Moving to x: {} y: {}", point.x, point.y);
    move(point);
    QThread::usleep(_delay);
    tempPaths.pop();
  }
}

void MovementImpl::followPaths() {
  _logger->debug("Start moving the stepper according to edge paths");
  processPaths(_edgePaths);

  if (_running)
    emit edgeFinished();

  _logger->debug("Start moving the stepper according to zigzag paths");
  processPaths(_zigzagPaths);

  if (_running)
    finish();
}

void MovementImpl::homing() {
  _stepperX->setDirection(device::stepper_direction::BACKWARD);
  _stepperY->setDirection(device::stepper_direction::BACKWARD);
  QThread::msleep(100);

  while (_running && _limitSwitchBinDetection->isActive()) {
    _logger->info(
        "Retry homing every 2s because of bin is still inside the frame");
    QThread::sleep(2);
  }

  while (_running && !isHome(_limitSwitchHomeX->isActive(),
                             _limitSwitchHomeY->isActive())) {
    _stepperX->pulseHigh();
    _stepperY->pulseHigh();
    QThread::usleep(_delay);

    _stepperX->pulseLow();
    _stepperY->pulseLow();
    QThread::usleep(_delay);
  }

  _state->setX(0);
  _state->setY(0);

  if (_running)
    finish();
}

void MovementImpl::start() {
  _sleepDevice->off();
  QThread::msleep(100);
  Worker::start();
}

void MovementImpl::reset() {
  _sleepDevice->on();
  _logger->info("Finger Movement state resetted!");
}

void MovementImpl::finish() {
  reset();
  QThread::msleep(100);
  Worker::finish();
}

void MovementImpl::stop() {
  reset();
  QThread::msleep(100);
  Worker::stop();
}

fruit::Component<MovementFactory> getMovementMechanismComponent() {
  return fruit::createComponent()
      .bind<Movement, MovementImpl>()
      .install(getConfigComponent)
      .install(getStateComponent)
      .install(getLoggerComponent)
      .install(device::getDigitalInputDeviceComponent)
      .install(device::getDigitalOutputDeviceComponent)
      .install(device::getStepperComponent);
}
}  // namespace emmerich::mechanism
