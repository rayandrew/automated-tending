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

#ifndef SIMPLE_WORKER_H_
#define SIMPLE_WORKER_H_

#pragma once

#include <iostream>

#include <functional>
#include <tuple>

#include <QObject>
#include <QString>
#include <QThread>

namespace emmerich {
class worker_object : public QObject {
  Q_OBJECT

 public:
  inline worker_object() = default;
  inline ~worker_object() = default;

 public slots:
  inline virtual void run(){/*...*/};

 signals:
  void finished();
  void error(const QString& err_msg);
};

namespace helper {
template <int... Is>
struct index {};

template <int N, int... Is>
struct gen_seq : gen_seq<N - 1, N - 1, Is...> {};

template <int... Is>
struct gen_seq<0, Is...> : index<Is...> {};
}  // namespace helper

template <typename... Ts>
class simple_worker : public worker_object {
 public: /* Functions */
  template <typename Func, typename... Args>
  inline simple_worker(Func&& fn, Args&&... args)
      : fn_(std::forward<Func>(fn)),
        args_(std::forward<Args>(args)...) { /*...*/
  }

  inline ~simple_worker() = default;

  inline void run() override {
    func(args_);
    emit finished();
  }

 private: /* Functions */
  template <typename... Args, int... Is>
  inline void func(std::tuple<Args...>& tup, helper::index<Is...>) {
    fn_(std::get<Is>(tup)...);
  }

  template <typename... Args>
  inline void func(std::tuple<Args...>& tup) {
    func(tup, helper::gen_seq<static_cast<int>(sizeof...(Args))>{});
  }

 private: /* Class members */
  std::function<void(Ts...)> fn_;
  std::tuple<Ts...>          args_;
};

/**
 * @brief Helper function to create a worker by which specification of the
 * template arguments aren't necessary.
 */
template <typename Func, typename... Args>
simple_worker<Args...> make_worker(Func&& fn, Args&&... args) {
  return simple_worker<Args...>(std::forward<Func>(fn),
                                std::forward<Args>(args)...);
}

using worker_callback = std::function<void()>;
using worker_error_callback = std::function<void(const QString&)>;

void start_worker(QObject*                     parent,
                  worker_object*               work,
                  const worker_callback&       on_finish,
                  const worker_error_callback& on_error);
}  // namespace emmerich

#endif
