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

#include "dispatcher.h"

namespace emmerich {
DispatcherImpl::DispatcherImpl(
    State*                             state,
    Logger*                            logger,
    fruit::Provider<services::Service> tendingServiceProvider,
    fruit::Provider<services::Service> resetServiceProvider)
    : _state(std::move(state)),
      _logger(std::move(logger)),
      _tendingServiceProvider(std::move(tendingServiceProvider)),
      _resetServiceProvider(std::move(resetServiceProvider)) {}

DispatcherImpl::~DispatcherImpl() {}

void DispatcherImpl::handleTask(const task_state& task) {
  std::cout << "Dispatcher Thread " << this->thread() << std::endl;

  switch (task) {
    case task_state::WATERING:
      // noop
      break;
    case task_state::TENDING:
      _tendingServiceProvider.get()->execute();
      break;
    case task_state::RESET:
      _resetServiceProvider.get()->execute();
      break;
    case task_state::IDLE:
      // noop
      break;
    default:
      _resetServiceProvider.get()->stop();
      _tendingServiceProvider.get()->stop();
      break;
  }
}

fruit::Component<Dispatcher> getDispatcherComponent() {
  return fruit::createComponent()
      .bind<Dispatcher, DispatcherImpl>()
      .install(getStateComponent)
      .install(getLoggerComponent)
      .install(mechanisms::getMovementMechanismComponent)
      .install(services::getTendingServiceComponent)
      .install(services::getResetServiceComponent);
}
}  // namespace emmerich
