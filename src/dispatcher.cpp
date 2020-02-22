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

#include "dispatcher.h"

namespace emmerich {
DispatcherImpl::DispatcherImpl(
    State*                            state,
    Logger*                           logger,
    fruit::Provider<service::Service> resetServiceProvider,
    fruit::Provider<service::Service> rotaryEncoderServiceProvider,
    fruit::Provider<service::Service> tendingServiceProvider,
    fruit::Provider<service::Service> wateringServiceProvider)
    : _state(std::move(state)),
      _logger(std::move(logger)),
      _resetServiceProvider(std::move(resetServiceProvider)),
      _rotaryEncoderServiceProvider(std::move(rotaryEncoderServiceProvider)),
      _tendingServiceProvider(std::move(tendingServiceProvider)),
      _wateringServiceProvider(std::move(wateringServiceProvider)) {
  _rotaryEncoderServiceProvider.get()->execute();
}

DispatcherImpl::~DispatcherImpl() {
  _rotaryEncoderServiceProvider.get()->stop();
}

void DispatcherImpl::handleTask(const task_state& task) {
  switch (task) {
    case task_state::WATERING:
      _wateringServiceProvider.get()->execute();
      break;
    case task_state::TENDING:
      _tendingServiceProvider.get()->execute();
      break;
    case task_state::RESET:
      _resetServiceProvider.get()->execute();
      break;
    case task_state::STOP:
      if (_lastTask == task_state::RESET) {
        _resetServiceProvider.get()->stop();
      } else if (_lastTask == task_state::TENDING) {
        _tendingServiceProvider.get()->stop();
      } else if (_lastTask == task_state::WATERING) {
        _wateringServiceProvider.get()->stop();
      }
      break;
    default:
      // noop
      break;
  }
  _lastTask = task;
}

fruit::Component<Dispatcher> getDispatcherComponent() {
  return fruit::createComponent()
      .bind<Dispatcher, DispatcherImpl>()
      .install(getStateComponent)
      .install(getLoggerComponent)
      .install(mechanism::getMovementMechanismComponent)
      .install(service::getResetServiceComponent)
      .install(service::getRotaryEncoderServiceComponent)
      .install(service::getTendingServiceComponent)
      .install(service::getWateringServiceComponent);
}
}  // namespace emmerich
