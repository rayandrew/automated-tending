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

#include <QObject>
// #include <QString>

#include <fmt/format.h>
#include <fruit/fruit.h>

#include "logger.h"

#pragma once

namespace emmerich {
struct Coordinate {
  int x = 0;
  int y = 0;
};

class State : public QObject {
  Q_OBJECT

 protected:
  Coordinate _coordinate;

  //  protected:
  //   virtual void _setX(int x) { _coordinate.x = x; }
  //   virtual void _setY(int y) { _coordinate.y = y; }
  //   virtual void _setCoordinate(const Coordinate& coordinate) {
  //     _coordinate = coordinate;
  //   }

 public:
  State() = default;
  virtual ~State() = default;

  inline const Coordinate& getCoordinate() const { return _coordinate; }
  inline int               getX() { return _coordinate.x; };
  inline int               getY() { return _coordinate.y; };

  // singleton stuffs
  // State(State const&) = delete;
  // State(State&&) = delete;
  // State&               operator=(State const&) = delete;
  // State&               operator=(State&&) = delete;

 public slots:
  virtual void setX(int x) = 0;
  virtual void setY(int y) = 0;
  virtual void setCoordinate(const Coordinate& coordinate) = 0;

 signals:
  void xHasChanged(const QString& new_x);
  void yHasChanged(const QString& new_y);
};

class StateImpl : public State {
  Q_OBJECT

 private:
  std::unique_ptr<Logger> _logger;

 public:
  INJECT(StateImpl(LoggerFactory loggerFactory));

  virtual ~StateImpl() = default;

 public slots:
  virtual void setX(int x);
  virtual void setY(int y);
  virtual void setCoordinate(const Coordinate& coordinate);
};

fruit::Component<State> getStateComponent();
}  // namespace emmerich

#endif