/*
 * Copyright (c) 2014 Reinstate Monica.
 * Modified by Ray Andrew <raydreww@gmail.com>
 *
 * Taken from this link : https://stackoverflow.com/a/21142877
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

#include "utils/signal_merge.h"

#include <QMutexLocker>

namespace emmerich {
/**
 * 0. Base signal merge
 */
void BaseSignalMerge::registerSignal(QObject* obj, const char* method) {
  int index = findIndexOfMethod(obj, method);
  if (index < 0)
    return;
  m_signals.insert(ObjectMethod(obj, index));
}

/**
 * 1. Signal Merge
 */

void SignalMerge::disconnect(QObject* obj, const char* method) {
  int index = findIndexOfMethod(obj, method);
  if (index < 0)
    return;

  m_signals.remove(ObjectMethod(obj, index));

  QMetaObject::disconnect(obj, index, this,
                          staticMetaObject.indexOfSlot("merge()"));
}

void SignalMerge::merge() {
  if (m_pendingSignals.isEmpty())
    m_pendingSignals = m_signals;
  m_pendingSignals.remove(ObjectMethod(sender(), senderSignalIndex()));
  if (m_pendingSignals.isEmpty())
    emit merged();
}

void SignalMerge::clear() {
  foreach (ObjectMethod om, m_signals) {
    QMetaObject::disconnect(om.first, om.second, this,
                            staticMetaObject.indexOfSlot("merge()"));
  }
  m_signals.clear();
  m_pendingSignals.clear();
}

Connection SignalMerge::connect(QObject*           sender,
                                const char*        signal,
                                Qt::ConnectionType type) {
  Connection conn = QObject::connect(sender, signal, this, SLOT(merge()), type);
  if (conn)
    registerSignal(sender, signal);
  return conn;
}

/**
 * 2. Signal Merge Float
 */
void SignalMergeFloat::disconnect(QObject* obj, const char* method) {
  int index = findIndexOfMethod(obj, method);

  if (index < 0)
    return;
  m_signals.remove(ObjectMethod(obj, index));

  bool status = QMetaObject::disconnect(
      obj, index, this, staticMetaObject.indexOfSlot("merge(float)"));
}

void SignalMergeFloat::merge(float value) {
  QMutexLocker locker(&_mutex);

  if (m_pendingSignals.isEmpty()) {
    m_pendingSignals = m_signals;
    _value = 0;
  }

  m_pendingSignals.remove(ObjectMethod(sender(), senderSignalIndex()));
  _value += _callback(value);

  if (m_pendingSignals.isEmpty()) {
    emit merged(_value);
  }
}

void SignalMergeFloat::clear() {
  QMutexLocker locker(&_mutex);
  foreach (ObjectMethod om, m_signals) {
    QMetaObject::disconnect(om.first, om.second, this,
                            staticMetaObject.indexOfSlot("merge(float)"));
  }
  _value = 0;
  m_signals.clear();
  m_pendingSignals.clear();
}

Connection SignalMergeFloat::connect(QObject*           sender,
                                     const char*        signal,
                                     Qt::ConnectionType type) {
  Connection conn =
      QObject::connect(sender, signal, this, SLOT(merge(float)), type);
  if (conn)
    registerSignal(sender, signal);
  return conn;
}
}  // namespace emmerich
