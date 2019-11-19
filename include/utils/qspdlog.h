/*
 * Copyright (c) 2018 Stefan Broekman.
 * Modified by Ray Andrew <raydreww@gmail.com>
 *
 *  This file is distributed under the MIT license.
 *  See: https://stefanbroekman.nl
 *  See: https://github.com/Broekman/Qt5_template
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

#ifndef SPDLOG_QSPDLOG_HPP
#define SPDLOG_QSPDLOG_HPP

#pragma once

#include <memory>

#include <QString>
#include <QWidget>

#include <fmt/format.h>
#include <spdlog/async_logger.h>
#include <spdlog/sinks/base_sink.h>

namespace emmerich {
class QSpdlog : public QWidget, public spdlog::sinks::base_sink<std::mutex> {
  Q_OBJECT

 public:
  QSpdlog(QWidget* parent = nullptr) : QWidget(parent) {}
  virtual ~QSpdlog() = default;

 protected:
  virtual void sink_it_(const spdlog::details::log_msg& msg) override;
  virtual void flush_() override;

 signals:
  void newLogEntry(const QString& msg);
};
}  // namespace emmerich

#endif