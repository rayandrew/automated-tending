/*
 * Licensed under the MIT License <http://opensource.org/licenses/MIT>.
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2017 Brett Beauregard <br3ttb@gmail.com> brettbeauregard.com
 * Modified by Ray Andrew <raydreww@gmail.com>
 *
 *  This file is distributed under the MIT license.
 *  See: https://github.com/br3ttb/Arduino-PID-Library
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

#ifndef PID_H_
#define PID_H_

#include <chrono>
#include <type_traits>

#include "common.h"

#include "utils/math.h"

NAMESPACE_BEGIN

// forward declaration
class PID;

namespace pid {
enum class mode { AUTOMATIC, MANUAL };
enum class direction { FORWARD, BACKWARD };
enum class proportional { MEASUREMENT, ERROR };

// forward declaration
namespace impl {
class _PID;
};
}  // namespace pid

using DefaultPID = pid::impl::_PID;

template <typename T = pid::impl::_PID,
          typename = std::enable_if<std::is_base_of<PID, T>::value, bool>>
class PIDBuilder {
 private:
  T* pid;

 private:
  PIDBuilder() { pid = new T(); }

 public:
  ~PIDBuilder() = default;
  static PIDBuilder create() { return PIDBuilder(); }

  PIDBuilder& setKP(double kp) {
    pid->setKP(kp);
    return *this;
  }
  PIDBuilder& setKI(double ki) {
    pid->setKI(ki);
    return *this;
  }
  PIDBuilder& setKD(double kd) {
    pid->setKD(kd);
    return *this;
  }
  PIDBuilder& setMode(const pid::mode& mode) {
    pid->setMode(mode);
    return *this;
  }
  PIDBuilder& setProportional(const pid::proportional& proportional) {
    pid->setProportional(proportional);
    return *this;
  }
  PIDBuilder& setDirection(const pid::direction& direction) {
    pid->setDirection(direction);
    return *this;
  }
  PIDBuilder& setSamplingTime(double samplingTime) {
    pid->setSamplingTime(samplingTime);
    return *this;
  }
  PIDBuilder& setTuning(
      double                   kp,
      double                   ki,
      double                   kd,
      const pid::proportional& proportional = pid::proportional::ERROR,
      const pid::direction&    direction = pid::direction::FORWARD) {
    pid->setTuning(kp, ki, kd, proportional, direction);
    return *this;
  }
  PIDBuilder& initialize() {
    pid->initialize();
    return *this;
  }
  PIDBuilder& setOutputLimit(double lowerBound, double upperBound) {
    pid->setOutputLimit(lowerBound, upperBound);
    return *this;
  }
  operator T*() { return pid; }
};

class PID {
 protected:
  using pid_time = std::chrono::time_point<std::chrono::high_resolution_clock>;
  // using pid_duration = std::chrono::duration<std::chrono::milliseconds>;

  pid::mode         _mode = pid::mode::AUTOMATIC;
  pid::proportional _proportional = pid::proportional::MEASUREMENT;
  pid::direction    _direction = pid::direction::FORWARD;

  // helpers
  pid_time _lastTime;
  double   _outMin = 0.0;
  double   _outMax = 255.0;
  double   _outSum;
  double   _lastInput;

  // user's input
  //// constants
  double _kp;  // proportional tuning parameter
  double _ki;  // integral tuning parameter
  double _kd;  // derivative tuning parameter

  double _dispKp;
  double _dispKi;
  double _dispKd;

  //// others
  unsigned long _samplingTime = 100;
  double        _input;
  double        _output;
  double        _setPoint;

  inline double constrain(double value) {
    return math::constrain<double>(value, _outMin, _outMax);
  }

 public:
  PID() = default;
  PID(double                   kp,
      double                   ki,
      double                   kd,
      const pid::proportional& proportional = pid::proportional::ERROR,
      const pid::direction&    direction = pid::direction::FORWARD) {}
  virtual ~PID() = default;

  // getter
  inline pid::mode         getMode() const { return _mode; }
  inline pid::proportional getProportional() const { return _proportional; }
  inline pid::direction    getDirection() const { return _direction; }
  inline unsigned long     getSamplingTime() const { return _samplingTime; }
  inline double            getKP() const { return _dispKp; }
  inline double            getKI() const { return _dispKi; }
  inline double            getKD() const { return _dispKd; }
  inline double            getInput() const { return _input; }
  inline double            getOutput() const { return _output; }
  inline double            getSetPoint() const { return _setPoint; }

  // setter
  inline virtual void setMode(const pid::mode& __mode) { _mode = __mode; }
  inline virtual void setProportional(const pid::proportional& proportional) {
    _proportional = proportional;
  }
  inline virtual void setDirection(const pid::direction& direction) {
    _direction = direction;
  }
  inline virtual void setSamplingTime(double samplingTime) {
    _samplingTime = samplingTime;
  }

  // you must call initialize() after using these 3 methods
  inline void setKP(double kp) { _kp = kp; }
  inline void setKI(double ki) { _ki = ki; }
  inline void setKD(double kd) { _kd = kd; }

  inline virtual void setTuning(
      double                   kp,
      double                   ki,
      double                   kd,
      const pid::proportional& proportional = pid::proportional::ERROR,
      const pid::direction&    direction = pid::direction::FORWARD) = 0;

  inline virtual void setInput(double input) { _input = input; }
  inline virtual void setOutput(double output) { _output = output; }
  inline virtual void setSetPoint(double setPoint) { _setPoint = setPoint; }

  virtual void setOutputLimit(double lowerBound, double upperBound) = 0;
  virtual bool compute() = 0;
  virtual void initialize() = 0;
};

namespace pid::impl {
class _PID : public PID {
 public:
  _PID();
  _PID(double                   kp,
       double                   ki,
       double                   kd,
       const pid::proportional& proportional = pid::proportional::ERROR,
       const pid::direction&    direction = pid::direction::FORWARD);
  virtual ~_PID() override = default;

 public:
  virtual void initialize() override;
  virtual bool compute() override;
  virtual void setTuning(
      double              kp,
      double              ki,
      double              kd,
      const proportional& __proportional = proportional::ERROR,
      const direction&    __direction = direction::FORWARD) override;
  virtual void setMode(const mode& __mode) override;
  virtual void setDirection(const direction& __direction) override;
  virtual void setSamplingTime(double samplingTime) override;
  virtual void setOutputLimit(double lowerBound, double upperBound) override;
};
}  // namespace pid::impl

NAMESPACE_END

#endif
