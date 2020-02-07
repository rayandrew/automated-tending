#ifndef MECHANISM_HELPER_MOVEMENT_H_
#define MECHANISM_HELPER_MOVEMENT_H_

#include <chrono>
#include <cmath>
#include <iostream>

#include <QThread>

#include "devices/digital/stepper.h"

namespace emmerich::mechanism::helper {
namespace movement {
enum class Mode { CONSTANT_SPEED, LINEAR_SPEED };
enum class State { STOPPED, ACCELERATING, CRUISING, DECELERATING };
}  // namespace movement

class Movement {
 protected:
  const device::Stepper* _stepper;
  movement::Mode         _mode = movement::Mode::CONSTANT_SPEED;
  movement::State        _state = movement::State::STOPPED;
  long                   _targetPosition = 0;
  long                   _currentPosition = 0;
  float                  _stepsPerMm = 0.0;
  float                  _stepsPerCm = 0.0;

 protected:
  static inline unsigned long mmToSteps(float mm, float stepPerMm) {
    return static_cast<unsigned long>(round(mm * stepPerMm));
  }
  static inline float stepsToMm(long steps, float stepPerMm) {
    return static_cast<float>(steps) / stepPerMm;
  }

 public:
  Movement(const device::Stepper* stepper, float stepsPerMm);
  virtual ~Movement() = default;

 public:
  virtual long processMovement() = 0;
  virtual void setupMove(unsigned long steps, unsigned long time = 0) = 0;
  inline virtual void setupMoveInMillimeters(float         mms,
                                             unsigned long time = 0) {
    setupMove(mmToSteps(mms, _stepsPerMm), time);
  }

  inline float getStepPerMm() const { return _stepsPerMm; }
  inline float getStepPerCm() const { return _stepsPerCm; }
  inline long  getCurrentPosition() const { return _currentPosition; }
  inline long  getCurrentPositionInMm() const {
    if (_stepsPerMm == 0.0)
      return 0;
    return _currentPosition / static_cast<long>(ceil(_stepsPerMm));
  }
  inline long getCurrentPositionInCm() const {
    if (_stepsPerCm == 0.0)
      return 0;
    return _currentPosition / static_cast<long>(_stepsPerCm);
  }
  inline long getTargetPosition() const { return _targetPosition; }
  inline bool isCompleted() const { return _state == movement::State::STOPPED; }
  inline bool isMotionCompleted() const {
    return _currentPosition == _targetPosition;
  }
  inline bool isMotionOngoing() const { return !isMotionCompleted(); }
  inline bool isStepsDivisibleByCm() const {
    return (_currentPosition % static_cast<long>(_stepsPerCm)) == 0;
  }
  inline bool isStepsDivisibleByMm() const {
    return (_currentPosition % static_cast<long>(ceil(_stepsPerMm))) == 0;
  }

  virtual double getPercentage() const = 0;
};
}  // namespace emmerich::mechanism::helper

#endif
