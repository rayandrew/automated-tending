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

#include "services/watering_service.h"

namespace emmerich::service {
WateringServiceImpl::WateringServiceImpl(
    Config*                            config,
    Logger*                            logger,
    State*                             state,
    device::DigitalOutputDeviceFactory digitalOutputDeviceFactory,
    mechanism::MovementFactory         movementMechanismFactory)
    : _logger(std::move(logger)),
      _state(std::move(state)),
      _pumpRelay(digitalOutputDeviceFactory(
          (*config)["devices"]["watering"]["pump"].as<int>())),
      _movementMechanism(movementMechanismFactory()) {
  setupMovementMechanism();
}

WateringServiceImpl::~WateringServiceImpl() {
  _movementThread->quit();
}

void WateringServiceImpl::setupMovementMechanism() {
  _movementMechanism->moveToThread(_movementThread.get());

  connect(_movementMechanism.get(), SIGNAL(started()), _movementThread.get(),
          SLOT(start()));
  connect(_movementThread.get(), &QThread::started, _movementMechanism.get(),
          &mechanism::Movement::followPaths);

  connect(_movementMechanism.get(), &mechanism::Movement::stopped, this,
          &WateringServiceImpl::onStopped);
  connect(_movementMechanism.get(), &mechanism::Movement::finished, this,
          &WateringServiceImpl::onFinished);

  connect(_movementMechanism.get(), &mechanism::Movement::finished,
          _movementThread.get(), &QThread::quit);
  connect(_movementMechanism.get(), &mechanism::Movement::stopped,
          _movementThread.get(), &QThread::quit);
}

void WateringServiceImpl::run() {
  _movementMechanism->start();
}

void WateringServiceImpl::onStart() {
  _logger->debug("Watering Service is starting");
  QThread::msleep(100);
  _logger->debug("Turning on the pump motor");
  _pumpRelay->on();
  QThread::msleep(50);
}

void WateringServiceImpl::onStopped() {
  _logger->debug("Turning off the pump motor");
  _pumpRelay->off();
  QThread::msleep(100);
  _logger->debug("Watering Service is stopped");
}

void WateringServiceImpl::onFinished() {
  // set state back to idle
  _logger->debug("Turning off the pump motor");
  _pumpRelay->off();
  QThread::msleep(100);
  _state->setMachineState(task_state::IDLE);
  _state->save();
  _logger->debug("Watering Service is finished");
}

void WateringServiceImpl::execute() {
  onStart();
  run();
}

void WateringServiceImpl::stop() {
  _movementMechanism->stop();
}

fruit::Component<fruit::Annotated<WateringService, Service>>
getWateringServiceComponent() {
  return fruit::createComponent()
      .bind<fruit::Annotated<WateringService, Service>, WateringServiceImpl>()
      .install(getConfigComponent)
      .install(getStateComponent)
      .install(getLoggerComponent)
      .install(device::getDigitalOutputDeviceComponent)
      .install(mechanism::getMovementMechanismComponent);
}
}  // namespace emmerich::service
