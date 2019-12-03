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

#ifndef DISPATCHER_H_
#define DISPATCHER_H_

#include <QObject>

#include <fruit/fruit.h>

#include "logger.h"
#include "state.h"

#include "services/movement_service.h"
#include "services/reset_service.h"

namespace emmerich {

class Dispatcher : public QObject {
  Q_OBJECT

 public slots:
  virtual void handleTask(const task_state& task) = 0;
};

class DispatcherImpl : public Dispatcher {
  Q_OBJECT

 private:
  Logger*                            _logger;
  fruit::Provider<services::Service> _movementServiceProvider;
  fruit::Provider<services::Service> _resetServiceProvider;

 public:
  INJECT(DispatcherImpl(
      Logger* logger,
      ANNOTATED(services::MovementService, fruit::Provider<services::Service>)
          movementServiceProvider,
      ANNOTATED(services::ResetService, fruit::Provider<services::Service>)
          resetServiceProvider));

 public slots:
  virtual void handleTask(const task_state& task) override;
};

fruit::Component<Dispatcher> getDispatcherComponent();
}  // namespace emmerich

#endif
