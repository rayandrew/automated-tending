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
#include <algorithm>
#include <cmath>
#include <functional>
#include <queue>

#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <QString>
#include <QThread>

#include <fruit/fruit.h>

#include "config.h"
#include "logger.h"
#include "state.h"

#include "general_config.h"

#include "utils/csv.h"
#include "utils/signal_merge.h"
#include "utils/worker.h"

#include "devices/input_device.h"
#include "devices/output_device.h"
#include "devices/stepper.h"

namespace emmerich::mechanisms {
class Movement : public Worker {
  Q_OBJECT

 protected:
  std::queue<Point> _paths;
  useconds_t        _delay = 500;

 public:
  Movement() = default;
  virtual ~Movement() = default;

  inline virtual const Movement& setStepDelay(useconds_t delay) {
    _delay = delay;
    return *this;
  }

  virtual const Movement& loadPathsFromFile(
      const std::string& filename = PROJECT_MOVEMENT_TEMPLATE_FILE) {
    io::CSVReader<2> in(filename);
    in.read_header(io::ignore_extra_column, "x", "y");

    int x, y;
    while (in.read_row(x, y)) {
      const Point point = {x, y};
      _paths.push(point);
    }

    return *this;
  }

  virtual const Movement& clearPaths() {
    std::queue<Point> empty;
    std::swap(_paths, empty);
    return *this;
  }

  virtual const Movement& setPaths(const std::queue<Point>& paths) {
    _paths = paths;
    return *this;
  }

  inline virtual const Movement& setPaths(const std::vector<Point> paths) {
    clearPaths();

    for (Point point : paths) {
      _paths.push(point);
    }

    return *this;
  }
};

class MovementImpl : public Movement {
  Q_OBJECT

 private:  // injected state
  Config*                                     _config;
  State*                                      _state;
  Logger*                                     _logger;
  const std::unique_ptr<device::Stepper>      _stepperX;
  const std::unique_ptr<device::Stepper>      _stepperY;
  const std::unique_ptr<device::InputDevice>  _limitSwitch;
  const std::unique_ptr<device::OutputDevice> _sleepDevice;
  const int                                   _xStepPerCm;
  const int                                   _yStepPerCm;
  bool                                        _isLimitSwitchTriggered = false;
  bool                                        _homing = false;

 private:  // internal state
  int  _currentX = 0;
  int  _currentY = 0;
  int  _progress = 0;
  bool _moveTogether = false;

 private:
  static inline int cmToSteps(int cm, int stepPerCm) { return cm * stepPerCm; }

  static inline int stepsToCm(int steps, int stepPerCm) {
    return steps / stepPerCm;
  }

  static inline bool isStepperRunning(bool limitSwitchStatus,
                                      bool homingStatus) {
    return !limitSwitchStatus || homingStatus;
  }

  void reset();
  void homing();
  void move(int x, int y);
  void move(const Point& point);

 public:
  INJECT(MovementImpl(Config*                     config,
                      State*                      state,
                      Logger*                     logger,
                      device::InputDeviceFactory  inputDeviceFactory,
                      device::OutputDeviceFactory outputDeviceFactory,
                      device::StepperFactory      stepperFactory));

  virtual ~MovementImpl();

 public slots:
  virtual void start() override;
  virtual void run() override;
  virtual void stop() override;
};

fruit::Component<Movement> getMovementComponent();
}  // namespace emmerich::mechanisms

#endif
