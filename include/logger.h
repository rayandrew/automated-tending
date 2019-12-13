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

#ifndef LOGGER_H_
#define LOGGER_H_

#include <fmt/format.h>
#include <fruit/fruit.h>

#include <spdlog/spdlog.h>

namespace emmerich {
class Logger {
 protected:
  std::shared_ptr<spdlog::logger> _logger;
  const std::string               _name;

 public:
  Logger() = default;
  Logger(const std::string& name) : _name(std::move(name)) {}
  virtual ~Logger() = default;

  inline const std::shared_ptr<spdlog::logger>& getLogger() const {
    return _logger;
  }

  inline spdlog::level::level_enum level() const { return _logger->level(); }

  inline void set_level(const spdlog::level::level_enum& log_level) {
    _logger->set_level(log_level);
  }

  template <typename... Args>
  inline void trace(fmt::basic_string_view<char> fmt, const Args&... args) {
    _logger->trace(fmt, args...);
  }

  template <typename... Args>
  inline void debug(fmt::basic_string_view<char> fmt, const Args&... args) {
    _logger->debug(fmt, args...);
  }

  template <typename... Args>
  inline void info(fmt::basic_string_view<char> fmt, const Args&... args) {
    _logger->info(fmt, args...);
  }

  template <typename... Args>
  inline void warn(fmt::basic_string_view<char> fmt, const Args&... args) {
    _logger->warn(fmt, args...);
  }

  template <typename... Args>
  inline void error(fmt::basic_string_view<char> fmt, const Args&... args) {
    _logger->error(fmt, args...);
  }

  template <typename... Args>
  void critical(fmt::basic_string_view<char> fmt, const Args&... args) {
    _logger->critical(fmt, args...);
  }
};

fruit::Component<Logger> getLoggerComponent();
}  // namespace emmerich

#endif
