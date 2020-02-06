#ifndef MECHANISM_HELPER_MOVEMENT_H_
#define MECHANISM_HELPER_MOVEMENT_H_

#include <chrono>
#include <cmath>
#include <iostream>

#include <QThread>

#include "devices/digital/stepper.h"

namespace emmerich::mechanism::helper {

class Movement {
 private:
  const device::Stepper*                             _stepper;
  bool                                               _startMove;
  long                                               _targetPosition = 0;
  long                                               _currentPosition = 0;
  long                                               _decelerationDistance = 0;
  int                                                _directionScaler = 1;
  float                                              _currentStepPeriod = 0.0;
  float                                              _desiredStepPeriod = 0.0;
  float                                              _acceleration = 0.0;
  float                                              _speed = 0.0;
  float                                              _stepsPerMm = 0.0;
  float                                              _stepsPerCm = 0.0;
  std::chrono::time_point<std::chrono::system_clock> _rampLastStepTime;
  float _rampInitialStepPeriod = 0.0;
  float _rampNextStepPeriod = 0.0;
  long  _highestPosValue = 0.0;

 private:
  static inline long mmToSteps(long mm, int stepPerMm) {
    return mm * stepPerMm;
  }

  static inline long stepsToMm(long steps, long stepPerMm) {
    return steps / stepPerMm;
  }

 public:
  Movement(const device::Stepper* stepper,
           float                  speed,
           float                  acceleration,
           float                  stepsPerMm);
  ~Movement() = default;
  void setupRelativeMoveInSteps(long distanceInSteps);
  void setupRelativeMoveInMillimeters(float distanceInMillimeters);
  void setupMoveInSteps(long absolutePositionToMoveInSteps);
  void setupMoveInMillimeters(float absolutePositionInMillimeters);
  bool processMovement();

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

  inline double getPercentage() {
    if (isMotionCompleted())
      return 100.0;

    const long absTargetPosition = std::abs(_targetPosition);
    const long absCurrentPosition = std::abs(_currentPosition);

    if (_highestPosValue == absTargetPosition) {
      return round(absCurrentPosition * 100 / absTargetPosition);
    } else {
      return round((1.0 - (static_cast<float>(absCurrentPosition) /
                           static_cast<float>(_highestPosValue))) *
                   100);
    }
  }
};
}  // namespace emmerich::mechanism::helper

#endif
