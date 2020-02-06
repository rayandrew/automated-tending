#include "mechanisms/_helper/movement.h"

#include <utility>

namespace emmerich::mechanism::helper {
Movement::Movement(const device::Stepper* stepper,
                   float                  speed,
                   float                  acceleration,
                   float                  stepsPerMm)
    : _stepper(std::move(stepper)),
      _speed(speed),
      _acceleration(acceleration),
      _stepsPerMm(stepsPerMm) {
  _stepsPerCm = _stepsPerMm * 10.0;
}

void Movement::setupMoveInSteps(long absolutePos) {
  long distanceToTravel;

  _targetPosition = absolutePos;

  _highestPosValue = std::abs(_targetPosition);
  if (std::abs(_currentPosition) > std::abs(_targetPosition)) {
    _highestPosValue = std::abs(_currentPosition);
  }

  _rampInitialStepPeriod = 1000000.0 / sqrt(2.0 * _acceleration);

  _desiredStepPeriod = 1000000.0 / _speed;

  _decelerationDistance =
      (long)round((_speed * _speed) / (2.0 * _acceleration));

  distanceToTravel = _targetPosition - _currentPosition;

  if (distanceToTravel < 0) {
    distanceToTravel = -distanceToTravel;
    _directionScaler = -1;
    _stepper->setDirection(device::stepper_direction::FORWARD);
  } else {
    _directionScaler = 1;
    _stepper->setDirection(device::stepper_direction::BACKWARD);
  }

  // check if travel distance is too short to
  // accelerate up to the desired velocity
  if (distanceToTravel <= (_decelerationDistance * 2L))
    _decelerationDistance = (distanceToTravel / 2L);

  // start the acceleration ramp at the beginning
  _rampNextStepPeriod = _rampInitialStepPeriod;
  _acceleration = _acceleration / 1E12;
  _startMove = true;
}

void Movement::setupMoveInMillimeters(float absolutePositionInMillimeters) {
  setupMoveInSteps(
      static_cast<long>(round(absolutePositionInMillimeters * _stepsPerMm)));
}

void Movement::setupRelativeMoveInSteps(long distance) {
  setupMoveInSteps(_currentPosition + distance);
}

void Movement::setupRelativeMoveInMillimeters(float distanceInMillimeters) {
  setupRelativeMoveInSteps(
      static_cast<long>(round(distanceInMillimeters * _stepsPerMm)));
}

bool Movement::processMovement() {
  std::chrono::time_point<std::chrono::system_clock> currentTime;
  unsigned long                                      periodSinceLastStep;
  long                                               distanceToTargetInSteps;

  // check if already at the target position
  if (_currentPosition == _targetPosition)
    return true;

  // check if this is the first call to start this new move
  if (_startMove) {
    _rampLastStepTime = std::chrono::system_clock::now();
    _startMove = false;
  }

  // determine how much time has elapsed since the last step (Note 1: this
  // method works even if the time has wrapped. Note 2: all variables must be
  // unsigned)
  currentTime = std::chrono::system_clock::now();
  periodSinceLastStep = std::chrono::duration_cast<std::chrono::microseconds>(
                            _rampLastStepTime - currentTime)
                            .count();

  // if it is not time for the next step, return
  if (periodSinceLastStep < static_cast<unsigned long>(_rampNextStepPeriod))
    return false;

  // determine the distance from the current position to the target
  distanceToTargetInSteps = _targetPosition - _currentPosition;
  if (distanceToTargetInSteps < 0)
    distanceToTargetInSteps = -distanceToTargetInSteps;

  // test if it is time to start decelerating, if so change from accelerating to
  // decelerating
  if (distanceToTargetInSteps == _decelerationDistance)
    _acceleration = -_acceleration;

  // execute the step on the rising edge
  _stepper->pulseHigh();

  // delay set to almost nothing because there is so much code between rising
  // and falling edges
  QThread::msleep(2);

  // update the current position and speed
  _currentPosition += _directionScaler;
  _currentStepPeriod = _rampNextStepPeriod;

  // compute the period for the next step
  // StepPeriodInUS = LastStepPeriodInUS *
  //   (1 - AccelerationInStepsPerUSPerUS * LastStepPeriodInUS^2)
  _rampNextStepPeriod =
      _rampNextStepPeriod *
      (1.0 - _acceleration * _rampNextStepPeriod * _rampNextStepPeriod);

  // return the step line high
  _stepper->pulseLow();

  // clip the speed so that it does not accelerate beyond the desired velocity
  if (_rampNextStepPeriod < _desiredStepPeriod)
    _rampNextStepPeriod = _desiredStepPeriod;

  // update the acceleration ramp
  _rampLastStepTime = currentTime;

  // check if move has reached its final target position, return true if all
  // done
  if (_currentPosition == _targetPosition) {
    _currentStepPeriod = 0.0;
    return true;
  }

  return false;
}
}  // namespace emmerich::mechanism::helper
