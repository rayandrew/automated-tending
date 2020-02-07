#ifndef MECHANISM_HELPER_SPEEDY_STEPPER_H_
#define MECHANISM_HELPER_SPEEDY_STEPPER_H_

#include <chrono>
#include <cmath>
#include <iostream>

#include <fruit/fruit.h>

#include <QThread>

#include "mechanisms/_helper/movement.h"

namespace emmerich::mechanism::helper::algorithm {
class SpeedyStepper : public Movement {
 private:
  bool                                               _startMove;
  long                                               _decelerationDistance = 0;
  int                                                _directionScaler = 1;
  float                                              _currentStepPeriod = 0.0;
  float                                              _desiredStepPeriod = 0.0;
  float                                              _acceleration = 0.0;
  float                                              _accelerationInUs = 0.0;
  float                                              _speed = 0.0;
  std::chrono::time_point<std::chrono::steady_clock> _rampLastStepTime;
  float _rampInitialStepPeriod = 0.0;
  float _rampNextStepPeriod = 0.0;
  long  _highestPosValue = 0.0;

 public:
  INJECT(SpeedyStepper(ASSISTED(const device::Stepper*) stepper,
                       ASSISTED(float) stepsPerMm,
                       ASSISTED(float) speed,
                       ASSISTED(float) acceleration));
  virtual ~SpeedyStepper() override = default;
  virtual void setupMove(unsigned long steps, unsigned long time = 0) override;
  virtual long processMovement() override;
  virtual double getPercentage() const override;
};

using SpeedyStepperFactory = std::function<
    std::unique_ptr<Movement>(const device::Stepper*, float, float, float)>;

fruit::Component<SpeedyStepperFactory>
getSpeedyStepperMovementAlgorithmComponent();
}  // namespace emmerich::mechanism::helper::algorithm

#endif
