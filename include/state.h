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
#include <map>

#include <QObject>
#include <QString>

#include <QMutex>
#include <QMutexLocker>

#include <fmt/format.h>
#include <fruit/fruit.h>
#include <yaml-cpp/yaml.h>

#include "logger.h"

#pragma once

namespace emmerich {
struct Point {
  int x = 0;
  int y = 0;
};

class State : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY(State)

 protected:
  Point _coordinate;
  float _degree = 0.0;

 public:
  State() = default;
  virtual ~State() = default;

  inline float          getDegree() const { return _degree; }
  inline const Point&   getCoordinate() const { return _coordinate; }
  inline int            getX() const { return _coordinate.x; };
  inline int            getY() const { return _coordinate.y; };
  friend std::ostream&  operator<<(std::ostream& os, const State& state);
  friend YAML::Emitter& operator<<(YAML::Emitter& out, const State& state);

  virtual void load(const std::string& filename = "state.yaml");
  virtual void save(const std::string& filename = "state.yaml");

 public slots:
  virtual void setDegree(float degree) = 0;
  virtual void setX(int x) = 0;
  virtual void setY(int y) = 0;
  virtual void setCoordinate(const Point& coordinate) = 0;

 signals:
  void degreeHasChanged(const QString& new_degree);
  void xHasChanged(const QString& new_x);
  void yHasChanged(const QString& new_y);
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
  virtual void setDegree(float degree) override;
  virtual void setX(int x) override;
  virtual void setY(int y) override;
  virtual void setCoordinate(const Point& coordinate) override;
};

fruit::Component<State> getStateComponent();
}  // namespace emmerich

#endif