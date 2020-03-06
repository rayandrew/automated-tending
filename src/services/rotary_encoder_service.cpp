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

#include "services/rotary_encoder_service.h"

#include "utils/simple_worker.h"

namespace emmerich::service {

RotaryEncoderServiceImpl::RotaryEncoderServiceImpl(
    Config*               config,
    Logger*               logger,
    State*                state,
    device::AnalogDevice* analogDevice)
    : _config(std::move(config)),
      _logger(std::move(logger)),
      _state(std::move(state)),
      _analogDevice(std::move(analogDevice)),
      _rotaryEncoderPin(
          (*_config)["devices"]["rotation"]["encoder"].as<int>()) {
  _logger->debug("RotaryEncoderService is initialized!");
  setupServiceThread();
}

RotaryEncoderServiceImpl::~RotaryEncoderServiceImpl() {
  // this is needed for closing the thread
  _rotaryEncoderThread->quit();
  _rotaryEncoderThread->wait();
}

void RotaryEncoderServiceImpl::setupServiceThread() {
  auto* worker = new simple_worker(make_simple_worker([this]() {
    while (_isRunning) {
      _state->setDegree(readRotaryDegree());
      QThread::msleep(200);
    }
  }));

  worker->moveToThread(_rotaryEncoderThread.get());
  connect(_rotaryEncoderThread.get(), &QThread::started, worker,
          &worker_object::run);
  connect(worker, &worker_object::finished, _rotaryEncoderThread.get(),
          &QThread::quit);
  connect(worker, &worker_object::finished, this,
          &RotaryEncoderServiceImpl::onFinished);
  connect(worker, &worker_object::finished, worker,
          &worker_object::deleteLater);
  connect(_rotaryEncoderThread.get(), &QThread::finished,
          _rotaryEncoderThread.get(), &QThread::deleteLater);
}

void RotaryEncoderServiceImpl::run() {
  _isRunning = true;
  _rotaryEncoderThread->start();
}

void RotaryEncoderServiceImpl::execute() {
  onStart();
  run();
}

void RotaryEncoderServiceImpl::stop() {
  _isRunning = false;
}

void RotaryEncoderServiceImpl::onStart() {
  _logger->debug("RotaryEncoderService is starting");
}

void RotaryEncoderServiceImpl::onFinished() {
  _logger->debug("RotaryEncoderService is finished");
}

fruit::Component<fruit::Annotated<RotaryEncoderService, Service>>
getRotaryEncoderServiceComponent() {
  return fruit::createComponent()
      .bind<fruit::Annotated<RotaryEncoderService, Service>,
            RotaryEncoderServiceImpl>()
      .install(getConfigComponent)
      .install(getLoggerComponent)
      .install(getStateComponent)
      .install(device::getPCF8591DeviceComponent);
}
}  // namespace emmerich::service
