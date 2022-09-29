#include "midwareComponent/midwareComponent.hpp"
#include "actionData_generated.h"
#include "actionDetectionManager.hpp"
#include "actionDetection/jumpDetection.hpp"
#include "actionDetection/squatDetection.hpp"

namespace actionDetection {

    class actionDetectionComponent: public Midware::MidwareComponent {
        private:
            actionjump::jump jumpInstance;
            actionsquat::squat squatInstance;
            actionDetectionManager managerInstance;
        
        public:
            actionDetectionComponent();
            ~actionDetectionComponent();
            bool process(const Input::InputMessage*, flatbuffers::FlatBufferBuilder&);
            void writeToFlatbuffers(ActionData::ActionBuilder&);
    };

    actionDetectionComponent::actionDetectionComponent(): MidwareComponent("action_detection") {}
    actionDetectionComponent::~actionDetectionComponent() {}

    bool actionDetectionComponent::process(const Input::InputMessage* data, flatbuffers::FlatBufferBuilder& builder) {
        if (data->type() == Input::MessageType::MessageType_Pose) {
            managerInstance.process(data, builder);
            jumpInstance.process(data, builder);
            squatInstance.process(data, builder);
            return true;
        }
        if (data->type() == Input::MessageType::MessageType_ApplicationControl) {
            managerInstance.process(data, builder);
        }
        
        return false;
    }

    void actionDetectionComponent::writeToFlatbuffers(ActionData::ActionBuilder& actionBuilder) {
        managerInstance.writeToFlatbuffers(actionBuilder);
        jumpInstance.writeToFlatbuffers(actionBuilder);
        squatInstance.writeToFlatbuffers(actionBuilder);
    }
}