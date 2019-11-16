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
StateImpl::StateImpl(LoggerFactory loggerFactory)
    : _logger(loggerFactory("AppState")) {}

void StateImpl::setX(int x) {
  if (x != _coordinate.x) {
    _coordinate.x = x;
    emit xHasChanged(x);
    _logger->info(fmt::format("Coordinate X has changed into {}", x));
  }
};

void StateImpl::setY(int y) {
  if (y != _coordinate.y) {
    _coordinate.y = y;
    emit yHasChanged(y);
    _logger->info(fmt::format("Coordinate Y has changed into {}", y));
  }
};

void StateImpl::setCoordinate(const Coordinate& coordinate) {
  setX(coordinate.x);
  setY(coordinate.y);
};

fruit::Component<State> getStateComponent() {
  return fruit::createComponent().bind<State, StateImpl>().install(
      getLoggerComponent);
}
}  // namespace emmerich