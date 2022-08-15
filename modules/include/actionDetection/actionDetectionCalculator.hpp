#include "actionData_generated.h"
#include "midwareComponent/midwareComponent.hpp"
#include "euroFilter.hpp"

namespace actionDetection {
    class actionDetectionCalculator {
        public:
            actionDetectionCalculator();
            ~actionDetectionCalculator();
            bool process(const Input::InputMessage*);
    };

    actionDetectionCalculator::actionDetectionCalculator() {}
    actionDetectionCalculator::~actionDetectionCalculator() {}

    bool actionDetectionCalculator::process(const Input::InputMessage* data) {
        
    }
}