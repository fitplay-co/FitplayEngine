#include "../actionDetectionData.hpp"

namespace actionDetection {

    class testManager {
        private:

        public:
            testManager();
            ~testManager();
            void process(const Input::InputMessage* data);
    };

    testManager::testManager() {}
    testManager::~testManager() {}
    
    void testManager::process(const Input::InputMessage* data) {
        struct subscribeAngle currentHipAngle = {};
    }
}