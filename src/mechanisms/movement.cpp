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

namespace emmerich::mechanisms {
MovementImpl::MovementImpl(Config*                     config,
                           State*                      state,
                           Logger*                     logger,
                           device::InputDeviceFactory  inputDeviceFactory,
                           device::OutputDeviceFactory outputDeviceFactory,
                           device::StepperFactory      stepperFactory)
    : _config(std::move(config)),
      _state(std::move(state)),
      _logger(std::move(logger)),
      _xStepPerCm(ceil(
          (*config)["devices"]["movement"]["x"]["step_per_cm"].as<float>())),
      _yStepPerCm(ceil(
          (*config)["devices"]["movement"]["y"]["step_per_cm"].as<float>())),
      _sleepDevice(outputDeviceFactory(
          (*config)["devices"]["movement"]["sleep_pin"].as<int>())),
      _stepperX(stepperFactory(
          (*config)["devices"]["movement"]["x"]["step_pin"].as<int>(),
          (*config)["devices"]["movement"]["x"]["direction_pin"].as<int>())),
      _stepperY(stepperFactory(
          (*config)["devices"]["movement"]["y"]["step_pin"].as<int>(),
          (*config)["devices"]["movement"]["y"]["direction_pin"].as<int>())),
      _limitSwitch(inputDeviceFactory(
          (*config)["devices"]["movement"]["limit_switch_pin"].as<int>())) {
  _sleepDevice->setActiveState(false);
  _stepperX->setReverseDirection(
      (*config)["devices"]["movement"]["x"]["reversed"].as<bool>());
  _stepperY->setReverseDirection(
      (*config)["devices"]["movement"]["y"]["reversed"].as<bool>());
  loadPathsFromFile();
}

MovementImpl::~MovementImpl() {}

void MovementImpl::move(const Point& point) {
  move(point.x, point.y);
}

void MovementImpl::move(int x, int y) {
  int diffX = x - _state->getX();
  int diffY = y - _state->getY();

  _stepperX->setDirection(diffX >= 0 ? device::stepper_direction::FORWARD
                                     : device::stepper_direction::BACKWARD);
  _stepperY->setDirection(diffY >= 0 ? device::stepper_direction::FORWARD
                                     : device::stepper_direction::BACKWARD);

  int xStep = cmToSteps(abs(diffX), _xStepPerCm);
  int yStep = cmToSteps(abs(diffY), _yStepPerCm);
  int maxStep = std::max(xStep, yStep);

  _logger->debug("Moving with x step: {} y step: {} maxStep: {}", xStep, yStep,
                 maxStep);

  int step = 1;

  while ((step <= maxStep) &&
         isStepperRunning(_isLimitSwitchTriggered, _homing)) {
    _isLimitSwitchTriggered = _limitSwitch->isActive();

    if (step <= xStep)
      _stepperX->pulseHigh();

    if (step <= yStep)
      _stepperY->pulseHigh();

    QThread::usleep(_delay);

    if (step <= xStep)
      _stepperX->pulseLow();

    if (step <= yStep)
      _stepperY->pulseLow();

    QThread::usleep(_delay);

    if (step <= xStep && ((step % _xStepPerCm) == 0)) {
      int currentX = _state->getX();
      int newX = diffX >= 0 ? currentX + 1 : currentX - 1;
      _state->setX(newX);
    }

    if (step <= yStep && ((step % _yStepPerCm) == 0)) {
      int currentY = _state->getY();
      int newY = diffY >= 0 ? currentY + 1 : currentY - 1;
      _state->setY(newY);
    }

    _state->setProgress(round(float(step) / float(maxStep) * 100));
    ++step;
  }

  if (_isLimitSwitchTriggered) {
    _logger->debug("Is limit switch triggered {}", _isLimitSwitchTriggered);
  }

  if (!_isLimitSwitchTriggered || _homing) {
    _state->setX(x);
    _state->setY(y);
  }
}

void MovementImpl::run() {
  std::queue<Point> tempPaths = _paths;

  _logger->debug("Start moving the stepper");

  _isLimitSwitchTriggered = _limitSwitch->isActive();
  while (!_isLimitSwitchTriggered && !tempPaths.empty()) {
    const Point point = tempPaths.front();
    _logger->debug("Moving to x: {} y: {}", point.x, point.y);
    move(point);
    QThread::usleep(_delay);
    tempPaths.pop();
  }

  if (_isLimitSwitchTriggered) {
    homing();
  }

  stop();
}

void MovementImpl::homing() {
  _homing = true;
  _stepperX->setDirection(device::stepper_direction::BACKWARD);
  _stepperY->setDirection(device::stepper_direction::BACKWARD);
  QThread::usleep(100);
  move(0, 0);
}

void MovementImpl::start() {
  _sleepDevice->off();
  QThread::usleep(100);
  Worker::start();
}

void MovementImpl::reset() {
  homing();
  _homing = false;
  _sleepDevice->on();
  _logger->info("Finger Movement state resetted!");
}

void MovementImpl::stop() {
  Worker::stop();
  reset();
}

fruit::Component<Movement> getMovementComponent() {
  return fruit::createComponent()
      .bind<Movement, MovementImpl>()
      .install(getConfigComponent)
      .install(getStateComponent)
      .install(getLoggerComponent)
      .install(device::getInputDeviceComponent)
      .install(device::getOutputDeviceComponent)
      .install(device::getStepperComponent);
}
}  // namespace emmerich::mechanisms
