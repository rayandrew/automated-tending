#include <fruit/fruit.h>

#include "config.h"
#include "logger.h"

#include "mechanisms/_helper/movement.h"
#include "mechanisms/_helper/speedy_stepper.h"

namespace emmerich::mechanism::helper {
enum class movement_algorithm {
  SPEEDY_STEPPER,
  LINEAR_SPEED_GEN_STEPPER_MOTOR
};

inline const std::string getMovementAlgorithmString(
    const movement_algorithm& algo) {
  switch (algo) {
    case movement_algorithm::SPEEDY_STEPPER:
      return "speedy_stepper";
    case movement_algorithm::LINEAR_SPEED_GEN_STEPPER_MOTOR:
      return "linear_speed_gen_stepper_motor";
    default:
      return "null";
  }
}

class MovementFactory {
 private:
  const Logger* _logger;

 private:
  movement_algorithm        _algorithm;
  std::unique_ptr<Movement> _movementAlgorithm;

 public:
  INJECT(MovementFactory(Config* _config, Logger* _logger));
  inline void setAlgorithm(movement_algorithm algorithm) {
    _logger->debug("Setting algorithm to {}",
                   getMovementAlgorithmString(algorithm));
  }
  inline Movement* getInstance() { return _movementAlgorithm.get(); }
};

template <typename... Args>
fruit::Component<std::function<std::unique_ptr<Movement>(Args... args)>>
getMovementAlgorithmComponent(
    defined_algorithm algorithm = defined_algorithm::SPEEDY_STEPPER) {
  return algorithm::getSpeedyStepperMovementAlgorithmComponent;
  // if (algorithm == defined_algorithm::SPEEDY_STEPPER) {
  //   return
  // } else {
  // }
}
}  // namespace emmerich::mechanism::helper
