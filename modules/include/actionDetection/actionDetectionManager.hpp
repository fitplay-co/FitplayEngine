#ifndef ACTION_detection_manager
#define ACTION_detection_manager

#include "actionManager/testManager.hpp"
#include "actionDetectionCalculator.hpp"

namespace actionDetection {

    class actionDetectionManager {
        private:
            testManager testInstance;
            actionDetectionCalculator calculatorInstance;
            flatbuffers::Offset<actionData::Stand> flatbuffersOffset;
            bool init = false;

        public:
            actionDetectionManager();
            ~actionDetectionManager();
            bool process(const Input::InputMessage* data, flatbuffers::FlatBufferBuilder& builder);
            void writeToFlatbuffers(actionData::ActionBuilder&);
    };

    actionDetectionManager::actionDetectionManager() {};
    actionDetectionManager::~actionDetectionManager() {};

    bool actionDetectionManager::process(const Input::InputMessage* data, flatbuffers::FlatBufferBuilder& builder) {
        if (data->type() == Input::MessageType::MessageType_Pose) {
            if(init == false) {
                struct subscribeAngle test = { 12,24,26,1 };
                struct subscribeDistance test2 = { 11,12,3 };
                struct featureVelocity test3 = { 0,1,0.01,-0.01 };
                int temp = calculatorInstance.addSubscribeAngle(&test);
                int temp2 = calculatorInstance.addSubscribeDistance(&test2);
                int temp3 = calculatorInstance.addFeatureVelocity(&test3);
                init = true;
            }
            calculatorInstance.process(data);
            float temp = calculatorInstance.getSubscribeAngle()->at(0);
            float temp2 = calculatorInstance.getSubscribeDistance()->at(0);
            float temp3 = calculatorInstance.getFeatureVelocity()->at(0);
            flatbuffersOffset = actionData::CreateStand(builder,
                                                            temp3);
            return true;
        }
        return false;
    }

    void actionDetectionManager::writeToFlatbuffers(actionData::ActionBuilder& actionBuilder) {
        actionBuilder.add_stand(flatbuffersOffset);
    }

}

#endif