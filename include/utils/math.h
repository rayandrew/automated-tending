/*
 * Licensed under the MIT License <http://opensource.org/licenses/MIT>.
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2019 by Ray Andrew
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

#ifndef MATH_H_
#define MATH_H_

#include <climits>

#include "common.h"

NAMESPACE_BEGIN

namespace math {
// taken from Arduino `map` function
// https://www.arduino.cc/reference/en/language/functions/math/map/
template <typename T>
static T map(T x, T in_min, T in_max, T out_min, T out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static float convertToDeg(unsigned char value, float mapDegree = MAX_DEGREE) {
  return math::map<float>(static_cast<float>(value), 0.0, mapDegree, 0.0,
                          static_cast<float>(UCHAR_MAX));
}

// taken from Arduino `constrain` function
// https://www.arduino.cc/reference/en/language/functions/math/constrain/
template <typename T>
static T constrain(const T& value, const T& lower, const T& upper) {
  T temp = value;
  if (value > upper)
    temp = upper;
  else if (value < lower)
    temp = lower;
  return temp;
}
}  // namespace math

NAMESPACE_END

#endif
