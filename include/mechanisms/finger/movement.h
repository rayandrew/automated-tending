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

#include <cmath>

#include <QObject>
#include <QThread>

#include <fruit/fruit.h>

#include "config.h"
#include "state.h"

#include "devices/stepper.h"

namespace emmerich::mechanisms::finger {
class FingerMovement : public QThread {
  Q_OBJECT

 public:
  FingerMovement(QObject* parent = nullptr) : QThread(parent){};
  virtual ~FingerMovement() = default;
  virtual void run() = 0;
  virtual void moveX(int x) = 0;
  virtual void moveY(int y) = 0;
};

class FingerMovementImpl : public FingerMovement {
  Q_OBJECT

 private:
  Config*                          _config;
  State*                           _state;
  std::unique_ptr<device::Stepper> _stepperX;
  std::unique_ptr<device::Stepper> _stepperY;
  const float                      _xStepToCm;
  const float                      _yStepToCm;

  static inline int roundStepToCm(int step, float stepToCm) {
    float stepInCm = ceil(step * stepToCm);
    // return int(step + 0.5 - (step < 0));
    return (int)stepInCm;
  }

 public:
  INJECT(FingerMovementImpl(ASSISTED(QObject*) parent,
                            Config*                config,
                            State*                 state,
                            device::StepperFactory stepperFactory));

  virtual ~FingerMovementImpl();
  virtual void run();
  virtual void moveX(int x);
  virtual void moveY(int y);
};

using FingerMovementFactory =
    std::function<std::unique_ptr<FingerMovement>(QObject*)>;

fruit::Component<FingerMovementFactory> getFingerMovementComponent();
}  // namespace emmerich::mechanisms::finger

#endif