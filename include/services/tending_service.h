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

#ifndef MOVEMENT_SERVICE_H
#define MOVEMENT_SERVICE_H

#include <fruit/fruit.h>

#include <QObject>
#include <QThread>

#include "logger.h"
#include "state.h"

#include "services/service.h"

#include "mechanisms/movement.h"

namespace emmerich::services {
struct TendingService {};

class TendingServiceImpl : public Service {
  Q_OBJECT

 private:
  Logger*                                     _logger;
  State*                                      _state;
  const std::unique_ptr<mechanisms::Movement> _movementMechanism;
  const std::unique_ptr<QThread> _serviceThread = std::make_unique<QThread>();

 protected:
  virtual void run() override;

 protected slots:
  virtual void onStart() override;
  virtual void onStopped() override;
  virtual void onFinish() override;

 public:
  INJECT(
      TendingServiceImpl(Logger*                     logger,
                         State*                      state,
                         mechanisms::MovementFactory movementMechanismFactory));
  virtual ~TendingServiceImpl();

 public slots:
  virtual void execute() override;
  virtual void stop() override;
};

fruit::Component<fruit::Annotated<TendingService, Service>>
getTendingServiceComponent();
}  // namespace emmerich::services

#endif
