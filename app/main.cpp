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

#include <stdlib.h>
#include <cassert>
#include <iostream>

#include <QApplication>

#include <fruit/fruit.h>
#include <spdlog/spdlog.h>

#include "app.h"
#include "logger.h"

int main(int argc, char** argv) {
  qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  spdlog::init_thread_pool(8192, 1);

  fruit::Injector<emmerich::AppFactory> injector(emmerich::getAppComponent);
  emmerich::AppFactory                  appFactory(injector);

  std::unique_ptr<emmerich::App> app = appFactory(argc, argv);

  int exit_code = app->run();
  spdlog::drop_all();
  spdlog::shutdown();
  return exit_code;
}
