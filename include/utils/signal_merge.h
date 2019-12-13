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

#ifndef MERGE_SIGNALS_H
#define MERGE_SIGNALS_H

#include <functional>

#include <QMetaObject>
#include <QMutex>
#include <QObject>
#include <QSet>

namespace emmerich {
typedef QMetaObject::Connection Connection;
typedef QPair<QObject*, int>    ObjectMethod;

class BaseSignalMerge : public QObject {
  Q_OBJECT

 protected:
  QSet<ObjectMethod> m_signals, m_pendingSignals;

 protected:
  void       registerSignal(QObject* obj, const char* method);
  inline int findIndexOfMethod(QObject* obj, const char* method) {
    return obj->metaObject()->indexOfMethod(
        QMetaObject::normalizedSignature(method + 1));
  }

 public:
  virtual Connection connect(QObject*           sender,
                             const char*        signal,
                             Qt::ConnectionType type = Qt::AutoConnection) = 0;
  virtual void       disconnect(QObject* sender, const char* signal) = 0;
};

class SignalMerge : public BaseSignalMerge {
  Q_OBJECT

 protected slots:
  void merge();

 signals:
  void merged();

 public:
  void               clear();
  virtual Connection connect(
      QObject*           sender,
      const char*        signal,
      Qt::ConnectionType type = Qt::AutoConnection) override;
  virtual void disconnect(QObject* sender, const char* signal) override;
};

class SignalMergeFloat : public BaseSignalMerge {
  Q_OBJECT

  using ValueCallback = std::function<const float&(float value)>;

 private:
  float         _value = 0.0;
  QMutex        _mutex;
  ValueCallback _callback = [](float value) { return value; };

 protected slots:
  void merge(float value);

 signals:
  void merged(float combinedValue);

 public:
  SignalMergeFloat() {}
  SignalMergeFloat(ValueCallback callback) : _callback(callback) {}
  void setCallback(ValueCallback callback) { _callback = callback; }
  void clear();
  virtual Connection connect(
      QObject*           sender,
      const char*        signal,
      Qt::ConnectionType type = Qt::AutoConnection) override;
  virtual void disconnect(QObject* sender, const char* signal) override;
};
}  // namespace emmerich

#endif
