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

#ifndef MECHANISM_MOVEMENT_H_
#define MECHANISM_MOVEMENT_H_

#include <unistd.h>
#include <chrono>
#include <cmath>
#include <fstream>
#include <queue>
#include <sstream>

#include <QMutex>
#include <QObject>
#include <QThread>

#include <fruit/fruit.h>

#include "config.h"
#include "logger.h"
#include "state.h"

#include "utils/worker.h"

#include "devices/digital/input.h"
#include "devices/digital/output.h"
#include "devices/digital/stepper.h"

#include "mechanisms/_helper/movement.h"

namespace emmerich::mechanism {
class Movement : public Worker {
  Q_OBJECT

 protected:
  std::queue<Point> _edgePaths;
  std::queue<Point> _zigzagPaths;
  useconds_t        _delay = 500;

 protected:
  static inline void loadPathsFromFile(std::queue<Point>& queue,
                                       const std::string& filename) {
    std::ifstream file(filename, std::ifstream::in);
    std::string   line;

    while (file.is_open() && getline(file, line)) {
      std::istringstream iss(line);
      std::string        tempX, tempY;

      iss >> tempX;
      iss >> tempY;

      const Point point{stoi(tempX), stoi(tempY)};

      queue.push(point);
    }

    file.close();
  }

 signals:
  void edgeFinished();

 public:
  Movement() = default;
  virtual ~Movement() = default;
  virtual void followPaths() = 0;
  virtual void homing() = 0;

  inline virtual const Movement& setStepDelay(useconds_t delay) {
    _delay = delay;
    return *this;
  }

  virtual const Movement& clearPaths(std::queue<Point>& paths) {
    std::queue<Point> empty;
    std::swap(paths, empty);
    return *this;
  }
};

enum AXIS_MOVEMENT_STATE { X_ONLY, Y_ONLY, BOTH };

class MovementImpl : public Movement {
  Q_OBJECT

 private:  // injected state
  Config*                                            _config;
  State*                                             _state;
  Logger*                                            _logger;
  const std::unique_ptr<device::Stepper>             _stepperX;
  const std::unique_ptr<device::Stepper>             _stepperY;
  const std::unique_ptr<device::DigitalInputDevice>  _limitSwitchHomeX;
  const std::unique_ptr<device::DigitalInputDevice>  _limitSwitchHomeY;
  const std::unique_ptr<device::DigitalInputDevice>  _limitSwitchEdge;
  const std::unique_ptr<device::DigitalInputDevice>  _limitSwitchBinDetection;
  const std::unique_ptr<device::DigitalOutputDevice> _sleepDevice;

 private:  // internal state
  bool                              _isLimitSwitchEdgeTriggered = false;
  std::unique_ptr<helper::Movement> _xMovement;
  std::unique_ptr<helper::Movement> _yMovement;

 private:
  static inline long mmToSteps(long mm, int stepPerMm) {
    return mm * stepPerMm;
  }

  static inline long stepsToMm(long steps, long stepPerMm) {
    return steps / stepPerMm;
  }

  static inline bool isHome(bool limitSwitchHomeX, bool limitSwitchHomeY) {
    return limitSwitchHomeX && limitSwitchHomeY;
  }

  void reset();
  void processPaths(const std::queue<Point>& paths);
  void move(int x, int y);
  void move(const Point& point);

 public:
  INJECT(MovementImpl(
      Config*                            config,
      State*                             state,
      Logger*                            logger,
      device::DigitalInputDeviceFactory  digitalInputDeviceFactory,
      device::DigitalOutputDeviceFactory digitalOutputDeviceFactory,
      device::StepperFactory             stepperFactory));

  virtual ~MovementImpl() = default;
  virtual void homing() override;
  virtual void followPaths() override;

 public slots:
  virtual void start() override;
  virtual void finish() override;
  virtual void stop() override;
};

using MovementFactory = std::function<std::unique_ptr<Movement>()>;

fruit::Component<MovementFactory> getMovementMechanismComponent();
}  // namespace emmerich::mechanism

#endif
