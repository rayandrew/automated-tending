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

#ifndef PRECOMPILED_H_
#define PRECOMPILED_H_

#ifndef DISABLE_PRECOMPILED

// 1. stl
#include <unistd.h>
#include <chrono>
#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>

// 2. vendors
// 2.1. fruit
#include <fruit/fruit.h>

// 2.2. Pigpio
#ifndef MOCK_GPIO
#include <pigpio.h>
#endif

// 2.3. FMT
#include <fmt/format.h>

// 2.4. Spdlog
#include <spdlog/async.h>
#include <spdlog/spdlog.h>

//// 2.4.1 sinks
#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

// 2.5. Yaml
#include <yaml-cpp/yaml.h>

// 2.6. QT
#include <QApplication>
#include <QMainWindow>
#include <QMetaObject>
#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <QSet>
#include <QString>
#include <QThread>
#include <QWidget>
#include <QtWidgets>

//// 2.6.1 Widgets
#include <QColor>
#include <QComboBox>
#include <QLCDNumber>
#include <QLabel>
#include <QPalette>
#include <QProgressBar>
#include <QPushButton>

// 3. internal
//// 3.1. global
#include "app.h"
#include "common.h"
#include "config.h"
#include "dispatcher.h"
#include "gpio.h"
#include "logger.h"
#include "state.h"
#include "ui_main_window.h"

// 3.2. devices
//// 3.2.1. analog
#include "devices/analog/PCF8591.h"
#include "devices/analog/device.h"

//// 3.2.2. digital
#include "devices/digital/device.h"
#include "devices/digital/input.h"
#include "devices/digital/output.h"
#include "devices/digital/pwm.h"
#include "devices/digital/stepper.h"

// 3.3. mechanisms
#include "mechanisms/movement.h"
#include "mechanisms/rotation.h"

//// 3.3.1. helpers
#include "mechanisms/_helper/movement.h"
#include "mechanisms/_helper/speedy_stepper.h"

// 3.2 services
#include "services/reset_service.h"
#include "services/rotary_encoder_service.h"
#include "services/service.h"
#include "services/tending_service.h"
#include "services/watering_service.h"

// 3.4. utils
#include "utils/qspdlog.h"
#include "utils/signal_merge.h"
#include "utils/simple_worker.h"
#include "utils/worker.h"

#endif

#endif
