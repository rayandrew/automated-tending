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

#ifndef FINGER_MOVEMENT_H_
#define FINGER_MOVEMENT_H_

#pragma once

#include <unistd.h>
#include <cmath>
#include <functional>

#include <QObject>
#include <QString>
#include <QThread>

#include <QMutex>
#include <QMutexLocker>

#include <fruit/fruit.h>

#include "config.h"
#include "logger.h"
#include "state.h"

#include "utils/signal_merge.h"
#include "utils/worker.h"

#include "devices/stepper.h"

namespace emmerich::mechanisms {

class Movement : public QObject {
  Q_OBJECT

 public:
  Movement() = default;
  virtual ~Movement() = default;
  virtual const Movement& goTo(int x, int y) = 0;
  virtual void            moveX(int x, useconds_t step_delay) = 0;
  virtual void            moveY(int y, useconds_t step_delay) = 0;

 public slots:
  virtual void sendProgress(float progress) = 0;
  virtual void run() = 0;
  virtual void finish() = 0;

 signals:
  void progress(int progress);
  void finished();
};

class MovementImpl : public Movement {
  Q_OBJECT

 private:  // injected state
  Config*                                _config;
  State*                                 _state;
  Logger*                                _logger;
  const std::unique_ptr<device::Stepper> _stepperX;
  const std::unique_ptr<device::Stepper> _stepperY;
  const float                            _xStepToCm;
  const float                            _yStepToCm;

 private:  // internal state
  const std::unique_ptr<QMutex>           _mutex;
  const std::unique_ptr<QThread>          _stepperXThread;
  const std::unique_ptr<QThread>          _stepperYThread;
  const std::unique_ptr<SignalMerge>      _signalMergeWorkersFinished;
  const std::unique_ptr<SignalMergeFloat> _signalMergeWorkersProgress;
  int                                     _x = 0;
  int                                     _y = 0;
  int                                     _progress = 0;
  bool                                    _moveTogether = false;

 private:
  static inline int roundStepToCm(int step, float stepToCm) {
    float stepInCm = ceil(step * stepToCm);
    return (int)stepInCm;
  }

  inline void reset() {
    QMutexLocker locker(_mutex.get());
    _progress = 0;
    _logger->info("Finger Movement state resetted!");
  }

 public:
  INJECT(MovementImpl(Config*                config,
                      State*                 state,
                      Logger*                logger,
                      device::StepperFactory stepperFactory));

  virtual ~MovementImpl();

  inline virtual const Movement& goTo(int x, int y) override {
    QMutexLocker locker(_mutex.get());
    _x = x;
    _y = y;
    return *this;
  }

  virtual void moveX(int x, useconds_t step_delay = 5000) override;
  virtual void moveY(int y, useconds_t step_delay = 5000) override;

 public slots:
  virtual void sendProgress(float progress) override;
  virtual void run() override;
  virtual void finish() override;
};

fruit::Component<Movement> getMovementComponent();
}  // namespace emmerich::mechanisms

#endif
