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

#include "state.h"

namespace emmerich {
std::ostream& operator<<(std::ostream& os, const State& state) {
  os << fmt::format("[positioning: [x : {}, y: {}], degree: {}]", state.getX(),
                    state.getY(), state.getDegree());
  return os;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const State& state) {
  std::map<std::string, int> coordinate;
  coordinate["x"] = state.getX();
  coordinate["y"] = state.getY();

  out << YAML::BeginMap;
  out << YAML::Key << "positioning" << YAML::Value << coordinate;
  out << YAML::Key << "degree" << YAML::Value << state.getDegree();
  out << YAML::EndMap;
  return out;
}

void State::load(const std::string& filename) {
  YAML::Node configFile = YAML::LoadFile(filename);
  setDegree(configFile["degree"].as<float>());
  setX(configFile["positioning"]["x"].as<int>());
  setY(configFile["positioning"]["y"].as<int>());
}

void State::save(const std::string& filename) {
  YAML::Emitter out;
  out << *this;

  std::ofstream fout(filename);
  fout << out.c_str();
}

StateImpl::StateImpl(Logger* logger)
    : _logger(std::move(logger)),
      _mutex(std::move(std::make_unique<QMutex>())) {}

void StateImpl::setDegree(float degree) {
  QMutexLocker locker(_mutex.get());

  if (degree != _degree) {
    _degree = degree;
    emit degreeHasChanged(QString::number(degree));
    _logger->info("Degree has changed into {}", degree);
  }
}

void StateImpl::setProgress(int progress) {
  QMutexLocker locker(_mutex.get());

  if (_progress != progress && progress >= 0 && progress <= 100) {
    _progress = progress;
    emit progressHasChanged(progress);
    // _logger->debug("Progress has changed into {}%", progress);
  }
}

void StateImpl::setX(int x) {
  QMutexLocker locker(_mutex.get());

  if (x != _coordinate.x) {
    _coordinate.x = x;
    emit xHasChanged(QString::number(x));
    _logger->debug("Coordinate X has changed into {}", x);
  }
};

void StateImpl::setY(int y) {
  QMutexLocker locker(_mutex.get());
  if (y != _coordinate.y) {
    _coordinate.y = y;
    emit yHasChanged(QString::number(y));
    _logger->debug("Coordinate Y has changed into {}", y);
  }
};

void StateImpl::setCoordinate(const Point& coordinate) {
  setX(coordinate.x);
  setY(coordinate.y);
};

fruit::Component<State> getStateComponent() {
  return fruit::createComponent().bind<State, StateImpl>().install(
      getLoggerComponent);
}
}  // namespace emmerich
