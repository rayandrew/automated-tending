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
MovementImpl::MovementImpl(Config*                    config,
                           State*                     state,
                           Logger*                    logger,
                           device::StepperFactory     stepperFactory,
                           device::LimitSwitchFactory limitSwitchFactory)
    : _config(std::move(config)),
      _state(std::move(state)),
      _logger(std::move(logger)),
      _xStepPerCm(
          (*config)["devices"]["movement"]["x"]["step_per_cm"].as<float>()),
      _yStepPerCm(
          (*config)["devices"]["movement"]["y"]["step_per_cm"].as<float>()),
      _stepperX(stepperFactory(
          (*config)["devices"]["movement"]["x"]["step_pin"].as<int>(),
          (*config)["devices"]["movement"]["x"]["direction_pin"].as<int>())),
      _stepperY(stepperFactory(
          (*config)["devices"]["movement"]["y"]["step_pin"].as<int>(),
          (*config)["devices"]["movement"]["y"]["direction_pin"].as<int>())),
      _limitSwitch(limitSwitchFactory(
          (*config)["devices"]["movement"]["limit_switch_pin"].as<int>())),
      _mutex(std::make_unique<QMutex>()) {
  setupStepperX();
  setupStepperY();
}

MovementImpl::~MovementImpl() {}

void MovementImpl::setupStepperX() {}

void MovementImpl::setupStepperY() {}

void MovementImpl::sendProgress(float currentProgress) {
  emit progress(round(currentProgress));
}

const Movement& MovementImpl::goTo(const Point& point) {
  QMutexLocker locker(_mutex.get());
  _currentX = point.x;
  _currentY = point.y;
  return *this;
}

void MovementImpl::run() {
  std::queue<Point> tempPaths = _paths;

  while (!tempPaths.empty()) {
    const Point point = tempPaths.front();

    int diffX = point.x - _state->getX();
    int diffY = point.y - _state->getY();

    if (diffX >= 0) {
      _stepperX->setDirection(device::stepper_direction::FORWARD);
    } else {
      _stepperX->setDirection(device::stepper_direction::BACKWARD);
    }

    if (diffY >= 0) {
      _stepperY->setDirection(device::stepper_direction::FORWARD);
    } else {
      _stepperY->setDirection(device::stepper_direction::BACKWARD);
    }

    int xStep = cmToSteps(diffX, _xStepPerCm);
    int yStep = cmToSteps(diffY, _yStepPerCm);

    int maxStep = std::max(xStep, yStep);

    int step = 1;

    while ((step <= maxStep) && _limitSwitch->getStatusBool()) {
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

      sendProgress(float(step) / float(maxStep));
      ++step;
    }

    _state->setX(point.x);
    _state->setY(point.y);

    tempPaths.pop();
  }

  finish();
}

void MovementImpl::reset() {
  // QMutexLocker locker(_mutex.get());
  clearPaths();
  _logger->info("Finger Movement state resetted!");
}

void MovementImpl::finish() {
  emit finished();
  reset();
}

fruit::Component<Movement> getMovementComponent() {
  return fruit::createComponent()
      .bind<Movement, MovementImpl>()
      .install(getConfigComponent)
      .install(getStateComponent)
      .install(getLoggerComponent)
      .install(device::getStepperComponent)
      .install(device::getLimitSwitchComponent);
}
}  // namespace emmerich::mechanisms
