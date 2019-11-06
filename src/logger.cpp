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

#include "logger.h"

namespace emmerich {
Logger::Logger(std::string name) {
  _name = std::move(name);

  // Define sinks
  try {
    // auto console_sink =
    // std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    // console_sink->set_level(spdlog::level::warn);

    // auto rotating_sink = spdlog::rotating_logger_mt(
    //   _name,
    //   "logs/rotating.txt",
    //   1048576 * 5,
    //   3); // Create a file rotating logger with 5mb size max and 3 rotate
    //   files
    // rotating_sink->set_level(spdlog::level::trace);

    // _logger = std::make_shared<spdlog::logger>(
    //   _name, spdlog::sinks_init_list({ console_sink, rotating_sink }));
    // _logger->info("Hello from here!");

    auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    stdout_sink->set_level(spdlog::level::warn);

    auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        "logs/rotating.txt", 1024 * 1024 * 10, 3);
    rotating_sink->set_level(spdlog::level::trace);

    std::vector<spdlog::sink_ptr> sinks{stdout_sink, rotating_sink};

    _logger = std::make_shared<spdlog::logger>(_name, begin(sinks), end(sinks));

    _logger->trace("Logger has been initialized!");

    spdlog::register_logger(_logger);
    _logger->debug("Logger has been initialized!");
  } catch (const spdlog::spdlog_ex& ex) {
    std::cout << "Log initialization failed: " << ex.what() << std::endl;
  }
}
}  // namespace tending
