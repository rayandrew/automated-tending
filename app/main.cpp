/*
 * Licensed under the MIT License <http: //opensource.org/licenses/MIT>.
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

#ifdef ENABLE_DOCTEST_IN_LIBRARY
#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest.h>
#endif

#include <cassert>
#include <iostream>
#include <stdlib.h>

#include <boost/di.hpp>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include "app.h"
#include "config.h"
#include "example.h"
#include "logger.h"

// #define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
// #define SPDLOG_TRACE_ON

/*
 * Simple main program that demontrates how access
 * CMake definitions (here the version number) from source code.
 */
int
main()
{
  // spdlog::info("Test");
  const auto injector = boost::di::make_injector(
    boost::di::bind<std::string>.named(tending::LoggerName).to("Automated Tending"));

  injector.create<tending::App>();

  // SPDLOG_DEBUG("Some trace message with param {}", "test");

  // spdlog::info("Automated Tending Project v{}.{}.{}.{}",
  //              PROJECT_VERSION_MAJOR,
  //              PROJECT_VERSION_MINOR,
  //              PROJECT_VERSION_PATCH,
  //              PROJECT_VERSION_TWEAK);
  // std::system("cat ../LICENSE");

  // Bring in the dummy class from the example source,
  // just to show that it is accessible from main.cpp.
  // Dummy d = Dummy();
  // return d.doSomething() ? 0 : -1;

  return 0;
}