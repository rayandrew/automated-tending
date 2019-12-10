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
      _xStepPerCm(ceil(
          (*config)["devices"]["movement"]["x"]["step_per_cm"].as<float>())),
      _yStepPerCm(ceil(
          (*config)["devices"]["movement"]["y"]["step_per_cm"].as<float>())),
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

  loadPathsFromFile(_edgePaths, PROJECT_MOVEMENT_EDGE_FILE);
  loadPathsFromFile(_zigzagPaths, PROJECT_MOVEMENT_ZIGZAG_FILE);
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

  _isLimitSwitchEdgeTriggered = _limitSwitchEdge->isActive();

  while (_running && (step <= maxStep) && !_isLimitSwitchEdgeTriggered) {
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
