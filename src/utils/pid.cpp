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

#include "precompiled.h"

#include "utils/pid.h"

NAMESPACE_BEGIN

namespace pid::impl {
// PID::PID(double                   kp,
//          double                   ki,
//          double                   kd,
//          const pid::proportional& proportional,
//          const pid::direction&    direction) {
//   setTuning(kp, ki, kd, proportional, direction);
//   initialize();
// }

_PID::_PID(double                   kp,
           double                   ki,
           double                   kd,
           const pid::proportional& proportional,
           const pid::direction&    direction) {
  setTuning(kp, ki, kd, proportional, direction);
  initialize();
}

_PID::_PID() {
  _lastTime = std::chrono::high_resolution_clock::now();
}

bool _PID::compute() {
  // sanity check
  if (_mode != mode::AUTOMATIC)
    return false;

  pid_time now = std::chrono::high_resolution_clock::now();

  auto pid_duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(now - _lastTime);
  double duration = pid_duration.count();

  if (duration >= _samplingTime) {
    double input = _input;
    double error = _setPoint - _lastInput;
    double dInput = input - _lastInput;
    _outSum += (_ki * error);

    if (_proportional == proportional::MEASUREMENT) {
      _outSum -= (_kp * dInput);
    }

    _outSum = constrain(_outSum);

    _output = 0;
    if (_proportional == proportional::ERROR) {
      _output = _kp * error;
    }

    _output += _outSum - (_kd * dInput);

    _output = constrain(_output);
    _lastInput = input;
    _lastTime = now;
    return true;
  }

  return false;
}

void _PID::initialize() {
  double samplingTimeInSec = _samplingTime / 1000;
  _ki = _ki * samplingTimeInSec;
  _kd = _kd / samplingTimeInSec;

  if (_direction == direction::BACKWARD) {
    _kp = (0 - _kp);
    _ki = (0 - _ki);
    _kd = (0 - _kd);
  }
}

void _PID::setTuning(double              kp,
                     double              ki,
                     double              kd,
                     const proportional& __proportional,
                     const direction&    __direction) {
  if (kp < 0 || ki < 0 || kd < 0)
    return;

  // set proportional
  _proportional = __proportional;

  // set constants
  _kp = kp;
  _ki = ki;
  _kd = kd;

  _dispKp = kp;
  _dispKi = ki;
  _dispKd = kd;

  // set direction
  _direction = __direction;

  initialize();
}

void _PID::setMode(const mode& __mode) {
  mode _temp_mode = _mode;
  _mode = __mode;
  if ((_temp_mode == mode::MANUAL) && (__mode == mode::AUTOMATIC)) {
    _outSum = constrain(_output);
    _lastInput = _input;
  }
}

void _PID::setDirection(const direction& __direction) {
  if ((_mode == mode::AUTOMATIC) && (_direction != __direction)) {
    // reverse constant values
    _kp = 0 - _kp;
    _ki = 0 - _ki;
    _kd = 0 - _kd;
  }
  _direction = __direction;
}

void _PID::setSamplingTime(double samplingTime) {
  if (samplingTime > 0) {
    double ratio = samplingTime / _samplingTime;
    _ki *= ratio;
    _kd /= ratio;
    _samplingTime = samplingTime;
  }
}

void _PID::setOutputLimit(double lowerBound, double upperBound) {
  // sanity checks
  if (lowerBound >= upperBound)
    return;

  _outMin = lowerBound;
  _outMax = upperBound;

  if (_mode == mode::AUTOMATIC) {
    _output = constrain(_output);
    _outSum = constrain(_outSum);
  }
}
}  // namespace pid::impl

NAMESPACE_END
