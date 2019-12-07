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

#include "services/tending_service.h"

namespace emmerich::services {

TendingServiceImpl::TendingServiceImpl(
    Logger*                     logger,
    State*                      state,
    mechanisms::MovementFactory movementMechanismFactory)
    : _logger(std::move(logger)),
      _state(std::move(state)),
      _movementMechanism(movementMechanismFactory()) {
  _movementMechanism->moveToThread(_serviceThread.get());

  connect(_movementMechanism.get(), SIGNAL(started()), _serviceThread.get(),
          SLOT(start()));
  connect(_serviceThread.get(), &QThread::started, _movementMechanism.get(),
          &mechanisms::Movement::followPaths);

  connect(_movementMechanism.get(), &mechanisms::Movement::finished,
          _serviceThread.get(), &QThread::quit, Qt::DirectConnection);
  connect(_movementMechanism.get(), &mechanisms::Movement::stopped,
          _serviceThread.get(), &QThread::quit, Qt::DirectConnection);

  connect(_movementMechanism.get(), &mechanisms::Movement::finished, this,
          &TendingServiceImpl::onFinish);
  connect(_movementMechanism.get(), &mechanisms::Movement::stopped, this,
          &TendingServiceImpl::onStopped);
}

TendingServiceImpl::~TendingServiceImpl() {
  _serviceThread->quit();
}

void TendingServiceImpl::run() {
  _movementMechanism->start();
}

void TendingServiceImpl::onStart() {
  _logger->debug("Finger movement service is starting");
  QThread::msleep(100);
}

void TendingServiceImpl::onStopped() {
  _logger->debug("Finger movement service is stopped!");
}

void TendingServiceImpl::onFinish() {
  // set state back to idle
  _state->setMachineState(task_state::IDLE);
  _logger->debug("Finger movement service is finished");
}

void TendingServiceImpl::execute() {
  onStart();
  run();
}

void TendingServiceImpl::stop() {
  _movementMechanism->stop();
  _logger->debug("Movement Service is stopped!");
}

fruit::Component<fruit::Annotated<TendingService, Service>>
getTendingServiceComponent() {
  return fruit::createComponent()
      .bind<fruit::Annotated<TendingService, Service>, TendingServiceImpl>()
      .install(getStateComponent)
      .install(getLoggerComponent)
      .install(mechanisms::getMovementMechanismComponent);
}
}  // namespace emmerich::services
