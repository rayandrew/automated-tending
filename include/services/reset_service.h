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

#ifndef RESET_SERVICE_H_
#define RESET_SERVICE_H_

#include <fruit/fruit.h>

#include <QObject>
#include <QThread>

#include "logger.h"
#include "state.h"

#include "services/service.h"

#include "mechanisms/movement.h"

namespace emmerich::service {
struct ResetService {};

class ResetServiceImpl : public Service {
  Q_OBJECT

 private:
  Logger*                                    _logger;
  State*                                     _state;
  const std::unique_ptr<mechanism::Movement> _movementMechanism;
  const std::unique_ptr<QThread> _serviceThread = std::make_unique<QThread>();

 protected:
  virtual void run() override;

 protected slots:
  virtual void onStart() override;
  virtual void onStopped() override;
  virtual void onFinished() override;

 public:
  INJECT(ResetServiceImpl(Logger*                    logger,
                          State*                     state,
                          mechanism::MovementFactory movementMechanismFactory));
  virtual ~ResetServiceImpl();

 public slots:
  virtual void execute() override;
  virtual void stop() override;
};

fruit::Component<fruit::Annotated<ResetService, Service>>
getResetServiceComponent();
}  // namespace emmerich::service

#endif