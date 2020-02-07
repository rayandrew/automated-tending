#include "mechanisms/_helper/movement.h"

#include <utility>

namespace emmerich::mechanism::helper {
Movement::Movement(const device::Stepper* stepper, float stepsPerMm)
    : _stepper(std::move(stepper)), _stepsPerMm(stepsPerMm) {
  _stepsPerCm = _stepsPerMm * 10.0;
}
}  // namespace emmerich::mechanism::helper
