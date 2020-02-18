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

#ifndef STATE_H_
#define STATE_H_

#include <fstream>

#include <QObject>
#include <QString>

#include <QMutex>
#include <QMutexLocker>

#include <fmt/format.h>
#include <fruit/fruit.h>
#include <yaml-cpp/yaml.h>

#include "logger.h"

#include "general_config.h"

namespace emmerich {
struct Point;
enum class task_state;
}  // namespace emmerich

Q_DECLARE_METATYPE(emmerich::Point)
Q_DECLARE_METATYPE(emmerich::task_state)

namespace emmerich {
struct Point {
  int x = 0;
  int y = 0;
  int z = 0;
};

enum class task_state { IDLE, WATERING, TENDING, RESET, STOP };

const task_state INITIAL_STATE = task_state::IDLE;

inline const std::string getTaskStateString(const task_state& state) {
  switch (state) {
    case task_state::WATERING:
      return "watering";
    case task_state::TENDING:
      return "tending";
    case task_state::STOP:
      return "stop";
    case task_state::RESET:
      return "reset";
    case task_state::IDLE:
      return "idle";
    default:
      return "unk";
  }
}

class State : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY(State)

 protected:
  // Global
  int _progress = 0;

  // Movement
  Point      _coordinate;
  float      _degree = 0.0;
  task_state _machineState = task_state::IDLE;

 public:
  State() = default;
  virtual ~State() = default;

  // Global
  inline int               getProgress() const { return _progress; }
  inline const task_state& getMachineState() const { return _machineState; }

  // Movement
  inline float        getDegree() const { return _degree; }
  inline const Point& getCoordinate() const { return _coordinate; }
  inline int          getX() const { return _coordinate.x; }
  inline int          getY() const { return _coordinate.y; }
  inline int          getZ() const { return _coordinate.z; }

  // Formatter
  friend std::ostream&  operator<<(std::ostream& os, const State& state);
  friend YAML::Emitter& operator<<(YAML::Emitter& out, const State& state);

  virtual void load(const std::string& filename = PROJECT_STATE_FILE);
  virtual void save(const std::string& filename = PROJECT_STATE_FILE);

 public slots:
  // Global
  virtual void setProgress(int progress) = 0;
  virtual void setMachineState(const task_state& new_state) = 0;

  // Movement
  virtual void setDegree(float degree) = 0;
  virtual void setX(int x) = 0;
  virtual void setY(int y) = 0;
  virtual void setZ(int y) = 0;
  virtual void setCoordinate(const Point& coordinate) = 0;

 signals:
  // Global
  void progressHasChanged(int new_progress);
  void machineStateHasChanged(const task_state& new_machine_state);
  void machineStateStringHasChanged(const QString& new_machine_state);

  // Movement
  void degreeHasChanged(const QString& new_degree);
  void xHasChanged(const QString& new_x);
  void yHasChanged(const QString& new_y);
  void zHasChanged(const QString& new_z);
};

class StateImpl : public State {
  Q_OBJECT

 private:
  Logger*                       _logger;
  const std::unique_ptr<QMutex> _mutex;

 public:
  INJECT(StateImpl(Logger* logger));
  virtual ~StateImpl() = default;

 public slots:
  // Global
  virtual void setProgress(int progress) override;
  virtual void setMachineState(const task_state& state) override;

  // Movement
  virtual void setDegree(float degree) override;
  virtual void setX(int x) override;
  virtual void setY(int y) override;
  virtual void setZ(int z) override;
  virtual void setCoordinate(const Point& coordinate) override;
};

fruit::Component<State> getStateComponent();
}  // namespace emmerich

#endif
