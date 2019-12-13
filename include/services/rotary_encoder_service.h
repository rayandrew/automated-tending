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

#ifndef ROTARY_ENCODER_SERVICE_H_
#define ROTARY_ENCODER_SERVICE_H_

#include <fruit/fruit.h>

#include <QThread>

#include "config.h"
#include "logger.h"
#include "state.h"

#include "services/service.h"

#include "devices/analog/PCF8591.h"
#include "devices/analog/device.h"

namespace emmerich::service {
struct RotaryEncoderService {};

class RotaryEncoderServiceImpl : public Service {
  Q_OBJECT

 private:
  Config*               _config;
  State*                _state;
  Logger*               _logger;
  device::AnalogDevice* _analogDevice;

  bool                           _isRunning = false;
  const std::unique_ptr<QThread> _rotaryEncoderThread =
      std::make_unique<QThread>();

 private:
  static inline float mapUnsignedCharToDegree(int value) {
    return value * 180 / UCHAR_MAX;
  }

  inline float readRotaryDegree() {
    return mapUnsignedCharToDegree(_analogDevice->read(
        (*_config)["devices"]["rotation"]["encoder"].as<unsigned char>()));
  }

  void setupServiceThread();

 public:
  INJECT(RotaryEncoderServiceImpl(Config* config,
                                  Logger* logger,
                                  State*  state,
                                  ANNOTATED(device::PCF8591,
                                            device::AnalogDevice*)
                                      analogDevice));
  virtual ~RotaryEncoderServiceImpl();

 protected:
  virtual void run() override;

 protected slots:
  virtual void onStart() override;
  virtual void onFinished() override;

 public slots:
  virtual void execute() override;
  virtual void stop() override;
};

fruit::Component<fruit::Annotated<RotaryEncoderService, Service>>
getRotaryEncoderServiceComponent();
}  // namespace emmerich::service

#endif
