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

#include "utils/simple_worker.h"

#include <QThread>

namespace emmerich {
void start_simple_worker(QObject*                     parent,
                         worker_object*               thread_worker,
                         const worker_callback&       on_finish,
                         const worker_error_callback& on_error) {
  auto* worker_thread = new QThread;
  thread_worker->moveToThread(worker_thread);
  QObject::connect(thread_worker, &worker_object::error, parent, on_error);
  QObject::connect(worker_thread, &QThread::started, thread_worker,
                   &worker_object::run);
  QObject::connect(thread_worker, &worker_object::finished, worker_thread,
                   &QThread::quit);
  QObject::connect(thread_worker, &worker_object::finished, parent, on_finish);
  QObject::connect(thread_worker, &worker_object::finished, thread_worker,
                   &worker_object::deleteLater);
  QObject::connect(worker_thread, &QThread::finished, worker_thread,
                   &QThread::deleteLater);
  worker_thread->start();
}
}  // namespace emmerich
