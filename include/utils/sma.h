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

#ifndef SMA_H_
#define SMA_H_

#include <cstddef>
#include <string>
#include <type_traits>

NAMESPACE_BEGIN

namespace sma {
class SMAException : public std::exception {
 private:
  const std::string _message;

 public:
  SMAException(const std::string& message);
  const char* what() const noexcept;
};

// this class is taken from RosettaCode
// https://rosettacode.org/wiki/Averages/Simple_moving_average#C.2B.2B
// change to support reading from other thread
template <typename T,
          typename = typename std::enable_if<std::is_floating_point<T>::value,
                                             T>::type>
class SMA {
 private:
  unsigned int _period;
  T*           _window;  // Holds the values to calculate the average of.

  // Logically, head is before tail
  T* _head;  // Points at the oldest element we've stored.
  T* _tail;  // Points at the newest element we've stored.

  T _total;  // Cache the total so we don't sum everything each time.

 private:
  // Bumps the given pointer up by one.
  // Wraps to the start of the array if needed.
  inline void inc(T*& p) {
    if (++p >= _window + _period) {
      p = _window;
    }
  }

  // Returns how many numbers we have stored.
  std::ptrdiff_t size() const {
    if (_head == NULL)
      return 0;
    if (_head == _tail)
      return _period;
    return (_period + _tail - _head) % _period;
  }

  // Adds a value to the average, pushing one out if nescessary
  void add(T val) {
    // Special case: Initialization
    if (_head == NULL) {
      _head = _window;
      *_head = val;
      _tail = _head;
      inc(_tail);
      _total = val;
      return;
    }

    // Were we already full?
    if (_head == _tail) {
      // Fix total-cache
      _total -= *_head;
      // Make room
      inc(_head);
    }

    // Write the value in the next spot.
    *_tail = val;
    inc(_tail);

    // Update our total-cache
    _total += val;
  }

  // Returns the average of the last P elements added to this SMA.
  // If no elements have been added yet, returns 0.0
  T avg() const {
    ptrdiff_t size = this->size();
    if (size == 0) {
      return 0;  // No entries => 0 average
    }
    return _total /
           (double)size;  // Cast to double for floating point arithmetic
  }

 public:
  SMA(unsigned int period)
      : _period(period),
        _window(new double[period]),
        _head(NULL),
        _tail(NULL),
        _total(0.0) {
    if (_period < 1)
      throw SMAException("Period variable cannot be less than 1");
  }

  ~SMA() { delete[] _window; }

  inline operator double() const { return static_cast<double>(avg()); }
  inline operator float() const { return static_cast<float>(avg()); }

  template <
      typename B,
      typename = typename std::enable_if<std::is_arithmetic<B>::value, B>::type>
  inline void operator()(B val) {
    add(static_cast<T>(val));
  }
};

}  // namespace sma

NAMESPACE_END

#endif
