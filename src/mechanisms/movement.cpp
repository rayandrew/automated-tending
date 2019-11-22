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
MovementImpl::MovementImpl(Config*                config,
                           State*                 state,
                           Logger*                logger,
                           device::StepperFactory stepperFactory)
    : _config(std::move(config)),
      _state(std::move(state)),
      _logger(std::move(logger)),
      _xStepToCm(
          (*config)["devices"]["movement"]["x"]["step_to_cm"].as<float>()),
      _yStepToCm(
          (*config)["devices"]["movement"]["y"]["step_to_cm"].as<float>()),
      _stepperX(std::move(stepperFactory(
          (*config)["devices"]["movement"]["x"]["step_pin"].as<int>(),
          (*config)["devices"]["movement"]["x"]["direction_pin"].as<int>()))),
      _stepperY(std::move(stepperFactory(
          (*config)["devices"]["movement"]["y"]["step_pin"].as<int>(),
          (*config)["devices"]["movement"]["y"]["direction_pin"].as<int>()))),
      _mutex(std::make_unique<QMutex>()),
      _stepperXThread(std::make_unique<QThread>()),
      _stepperYThread(std::make_unique<QThread>()),
      _signalMergeWorkersFinished(std::make_unique<SignalMerge>()),
      _signalMergeWorkersProgress(std::make_unique<SignalMergeFloat>()) {
  _stepperX->moveToThread(_stepperXThread.get());
  _stepperY->moveToThread(_stepperYThread.get());

  connect(_stepperXThread.get(), &QThread::started, _stepperX.get(),
          &device::Stepper::run);
  connect(_stepperX.get(), &device::Stepper::finished, _stepperXThread.get(),
          &QThread::quit);
  connect(_stepperX.get(), &device::Stepper::finished, this, [this]() {
    _signalMergeWorkersProgress->disconnect(_stepperX.get(),
                                            SIGNAL(progress(float)));
    _moveTogether = false;
  });

  connect(_stepperYThread.get(), &QThread::started, _stepperY.get(),
          &device::Stepper::run);
  connect(_stepperY.get(), &device::Stepper::finished, _stepperYThread.get(),
          &QThread::quit);
  connect(_stepperY.get(), &device::Stepper::finished, this, [this]() {
    _signalMergeWorkersProgress->disconnect(_stepperY.get(),
                                            SIGNAL(progress(float)));
    _moveTogether = false;
  });

  _signalMergeWorkersProgress->setCallback([this](float value) {
    QMutexLocker locker(_mutex.get());
    return value * (_moveTogether ? 50 : 100);
  });

  connect(_signalMergeWorkersProgress.get(), &SignalMergeFloat::merged, this,
          &Movement::sendProgress);

  connect(_signalMergeWorkersFinished.get(), &SignalMerge::merged, this,
          &Movement::finish);
}

MovementImpl::~MovementImpl() {
  _stepperXThread->wait();
  _stepperYThread->wait();

  _stepperXThread->quit();
  _stepperYThread->quit();
};

void MovementImpl::sendProgress(float currentProgress) {
  emit progress(round(currentProgress));
}

void MovementImpl::run() {
  bool moveX = _state->getX() != _x;
  bool moveY = _state->getY() != _y;

  // [NOTE!] @rayandrews: hacky solution to send one signal into progress bar
  _moveTogether = moveX && moveY;

  if (moveX) {
    _stepperX->setStep(_x);
    _signalMergeWorkersProgress->connect(_stepperX.get(),
                                         SIGNAL(progress(float)));
    _signalMergeWorkersFinished->connect(_stepperX.get(), SIGNAL(finished()));
    _stepperXThread->start();
  }

  if (moveY) {
    _stepperY->setStep(_y);
    _signalMergeWorkersProgress->connect(_stepperY.get(),
                                         SIGNAL(progress(float)));
    _signalMergeWorkersFinished->connect(_stepperY.get(), SIGNAL(finished()));
    _stepperYThread->start();
  }
}

void MovementImpl::finish() {
  _signalMergeWorkersProgress->clear();
  _signalMergeWorkersFinished->clear();
  _state->setX(_x);
  _state->setY(_y);
  reset();
  emit finished();
}

void MovementImpl::moveX(int x, useconds_t step_delay) {
  if (_state->getX() != x) {
    // _stepperX->step(roundStepToCm(y, _yStepToCm));
    // _state->setX(y);

    auto* work = new worker(make_worker([this, x]() {
      _logger->info("Stepping");
      _stepperX->step(x);
    }));

    auto on_finish_callback = [this, x]() {
      _state->setX(x);
      _logger->info("Finished!");
    };

    auto on_error_callback = [this](const QString& err_msg) {
      _logger->error(err_msg.toStdString());
    };

    start_worker(this, work, on_finish_callback, on_error_callback);
  }
}

void MovementImpl::moveY(int y, useconds_t step_delay) {
  if (_state->getY() != y) {
    // _stepperX->step(roundStepToCm(y, _yStepToCm));
    // _state->setX(y);

    auto* work = new worker(make_worker([this, y]() {
      _logger->info("Stepping");
      _stepperY->step(y);
    }));

    auto on_finish_callback = [this, y]() {
      _state->setY(y);
      _logger->info("Finished!");
    };

    auto on_error_callback = [this](const QString& err_msg) {
      _logger->error(err_msg.toStdString());
    };

    start_worker(this, work, on_finish_callback, on_error_callback);
  }
}

fruit::Component<Movement> getMovementComponent() {
  return fruit::createComponent()
      .bind<Movement, MovementImpl>()
      .install(getConfigComponent)
      .install(getStateComponent)
      .install(getLoggerComponent)
      .install(device::getStepperComponent);
}
}  // namespace emmerich::mechanisms
