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

#ifndef TENDING_SERVICE_H_
#define TENDING_SERVICE_H_

#include <fruit/fruit.h>

#include <QObject>
#include <QThread>

#include "logger.h"
#include "state.h"

#include "services/service.h"

#include "mechanisms/movement.h"
#include "mechanisms/rotation.h"

#include "utils/signal_merge.h"

namespace emmerich::service {
struct TendingService {};

class TendingServiceImpl : public Service {
  Q_OBJECT

 private:
  Logger* _logger;
  State*  _state;

  const std::unique_ptr<mechanism::Movement> _movementMechanism;
  const std::unique_ptr<mechanism::Rotation> _rotationMechanism;

  const std::unique_ptr<QThread> _movementThread = std::make_unique<QThread>();
  const std::unique_ptr<QThread> _rotationThread = std::make_unique<QThread>();

  const std::unique_ptr<SignalMerge> _signalMergeFinished =
      std::make_unique<SignalMerge>();
  const std::unique_ptr<SignalMerge> _signalMergeStopped =
      std::make_unique<SignalMerge>();

 private:
  void setupMovementMechanism();
  void setupRotationMechanism();

 protected:
  virtual void run() override;

 protected slots:
  virtual void onStart() override;
  virtual void onStopped() override;
  virtual void onFinished() override;

 public:
  INJECT(
      TendingServiceImpl(Logger*                    logger,
                         State*                     state,
                         mechanism::MovementFactory movementMechanismFactory,
                         mechanism::RotationFactory rotationMechanismFactory));
  virtual ~TendingServiceImpl();

 public slots:
  virtual void execute() override;
  virtual void stop() override;
};

fruit::Component<fruit::Annotated<TendingService, Service>>
getTendingServiceComponent();
}  // namespace emmerich::service

#endif
