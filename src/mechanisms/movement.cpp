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

#include "precompiled.h"

#include "mechanisms/movement.h"

#include "common.h"

namespace emmerich::mechanism {
MovementImpl::MovementImpl(
    Config*                                 config,
    State*                                  state,
    Logger*                                 logger,
    device::DigitalInputDeviceFactory       digitalInputDeviceFactory,
    device::DigitalOutputDeviceFactory      digitalOutputDeviceFactory,
    device::StepperFactory                  stepperFactory,
    helper::algorithm::SpeedyStepperFactory movementAlgorithmFactory)
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
      _stepperZ(stepperFactory(
          (*config)["devices"]["movement"]["z"]["step_pin"].as<int>(),
          (*config)["devices"]["movement"]["z"]["direction_pin"].as<int>())),
      _limitSwitchHomeX(digitalInputDeviceFactory(
          (*config)["devices"]["movement"]["limit_switch_pin"]["home_x"]
              .as<int>())),
      _limitSwitchHomeY(digitalInputDeviceFactory(
          (*config)["devices"]["movement"]["limit_switch_pin"]["home_y"]
              .as<int>())),
      _limitSwitchHomeZ(digitalInputDeviceFactory(
          (*config)["devices"]["movement"]["limit_switch_pin"]["home_z"]
              .as<int>())),
      _limitSwitchEdge(digitalInputDeviceFactory(
          (*config)["devices"]["movement"]["limit_switch_pin"]["edge"]
              .as<int>())),
      _zHeight((*config)["devices"]["movement"]["z"]["height"].as<int>()) {
  _logger->debug("Movement mechanism is initialized!");
  _sleepDevice->setActiveState(false);
  _stepperX->setReverseDirection(
      (*config)["devices"]["movement"]["x"]["reversed"].as<bool>());
  _stepperY->setReverseDirection(
      (*config)["devices"]["movement"]["y"]["reversed"].as<bool>());
  _stepperZ->setReverseDirection(
      (*config)["devices"]["movement"]["z"]["reversed"].as<bool>());

  _xMovement = movementAlgorithmFactory(
      _stepperX.get(),
      ceil((*config)["devices"]["movement"]["x"]["step_per_mm"].as<float>()),
      (*config)["devices"]["movement"]["x"]["speed"].as<float>(),
      (*config)["devices"]["movement"]["x"]["acceleration"].as<float>());

  _yMovement = movementAlgorithmFactory(
      _stepperY.get(),
      ceil((*config)["devices"]["movement"]["y"]["step_per_mm"].as<float>()),
      (*config)["devices"]["movement"]["y"]["speed"].as<float>(),
      (*config)["devices"]["movement"]["y"]["acceleration"].as<float>());

  _zMovement = movementAlgorithmFactory(
      _stepperZ.get(),
      ceil((*config)["devices"]["movement"]["z"]["step_per_mm"].as<float>()),
      (*config)["devices"]["movement"]["z"]["speed"].as<float>(),
      (*config)["devices"]["movement"]["z"]["acceleration"].as<float>());

  loadPathsFromFile(_edgePaths, PROJECT_MOVEMENT_EDGE_FILE);
  loadPathsFromFile(_zigzagPaths, PROJECT_MOVEMENT_ZIGZAG_FILE);
}

void MovementImpl::move(const Point& point) {
  move(point.x, point.y, point.z);
}

void MovementImpl::move(int x, int y, int z) {
  _xMovement->setupMoveInMillimeters(static_cast<float>(x));
  _yMovement->setupMoveInMillimeters(static_cast<float>(y));
  _zMovement->setupMoveInMillimeters(static_cast<float>(z));

  _isLimitSwitchEdgeTriggered = _limitSwitchEdge->isActive();

  axis_state axisState = axis_state::NONE;
  if (_xMovement->isMotionOngoing()) {
    axisState = axisState | axis_state::X_ONLY;
  }

  if (_yMovement->isMotionOngoing()) {
    axisState = axisState | axis_state::Y_ONLY;
  }

  if (_zMovement->isMotionOngoing()) {
    axisState = axisState | axis_state::Z_ONLY;
  }

  while (_running && !_isLimitSwitchEdgeTriggered &&
         (_xMovement->isMotionOngoing() || _yMovement->isMotionOngoing() ||
          _zMovement->isMotionOngoing())) {
    if (_xMovement->isMotionOngoing())
      _xMovement->processMovement();

    if (_yMovement->isMotionOngoing())
      _yMovement->processMovement();

    if (_zMovement->isMotionOngoing())
      _zMovement->processMovement();

    if (_xMovement->isMotionOngoing() && _xMovement->isStepsDivisibleByCm()) {
      _state->setX(_xMovement->getCurrentPositionInCm());
    }

    if (_yMovement->isMotionOngoing() && _yMovement->isStepsDivisibleByCm()) {
      _state->setY(_yMovement->getCurrentPositionInCm());
    }

    if (_zMovement->isMotionOngoing() && _zMovement->isStepsDivisibleByCm()) {
      _state->setZ(_zMovement->getCurrentPositionInCm());
    }

    double percentage = getProgress(axisState, _xMovement.get(),
                                    _yMovement.get(), _zMovement.get());
    _state->setProgress(percentage);
    _isLimitSwitchEdgeTriggered = _limitSwitchEdge->isActive();
  }

  if (_isLimitSwitchEdgeTriggered) {
    _logger->debug("Is edge limit switch triggered {}",
                   _isLimitSwitchEdgeTriggered);
  }

  if (_running && !_isLimitSwitchEdgeTriggered) {
    _state->setX(mmToCm(x));
    _state->setY(mmToCm(y));
    _state->setZ(mmToCm(z));
  }
}

void MovementImpl::processPaths(const std::queue<Point>& paths) {
  std::queue<Point> tempPaths = paths;

  _isLimitSwitchEdgeTriggered = _limitSwitchEdge->isActive();
  while (_running && !_isLimitSwitchEdgeTriggered && !tempPaths.empty()) {
    const Point point = tempPaths.front();
    _logger->debug("Moving to x: {} y: {} z: {}", point.x, point.y, point.z);
    move(point);
    QThread::usleep(_delay);
    tempPaths.pop();
  }
}

void MovementImpl::followPaths() {
  _logger->debug("Lowering the finger");
  _stepperZ->setDirection(device::stepper_direction::FORWARD);
  move(0, 0, _zHeight);

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
  _stepperZ->setDirection(device::stepper_direction::BACKWARD);
  QThread::msleep(100);

  // while (_running) {
  //   _logger->info(
  //       "Retry homing every 2s because of bin is still inside the frame");
  //   QThread::sleep(2);
  // }

  _logger->info("Homing z axis");
  while (_running && !_limitSwitchHomeZ->isActive()) {
    _stepperZ->pulseHigh();
    QThread::usleep(_delay);
    _stepperZ->pulseLow();
    QThread::usleep(_delay);
  }

  _state->setZ(0);

  while (_running && !isHome(_limitSwitchHomeX->isActive(),
                             _limitSwitchHomeY->isActive())) {
    if (!_limitSwitchHomeX->isActive())
      _stepperX->pulseHigh();
    if (!_limitSwitchHomeY->isActive())
      _stepperY->pulseHigh();
    QThread::usleep(_delay);

    if (!_limitSwitchHomeX->isActive())
      _stepperX->pulseLow();
    if (!_limitSwitchHomeY->isActive())
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
      .install(device::getStepperComponent)
      .install(helper::algorithm::getSpeedyStepperMovementAlgorithmComponent);
}
}  // namespace emmerich::mechanism
