#include "midwareComponent/midwareComponent.hpp"
#include "actionData_generated.h"
#include "actionDetection/walkDetection.hpp"
#include "actionDetection/jumpDetection.hpp"
#include "actionDetection/squatDetection.hpp"

namespace actionDetection {

    class actionDetectionComponent: public Midware::MidwareComponent {
        private:
            actionwalk::walk walkInstance;
            actionjump::jump jumpInstance;
            actionsquat::squat squatInstance;
        
        public:
            actionDetectionComponent();
            ~actionDetectionComponent();
            bool process(const Input::InputMessage*, flatbuffers::FlatBufferBuilder&);
            void writeToFlatbuffers(actionData::ActionBuilder&);
    };

    actionDetectionComponent::actionDetectionComponent(): MidwareComponent("action_detection") {}
    actionDetectionComponent::~actionDetectionComponent() {}

    bool actionDetectionComponent::process(const Input::InputMessage* data, flatbuffers::FlatBufferBuilder& builder) {
        if (data->type() == Input::MessageType::MessageType_Pose) {
            walkInstance.process(data, builder);
            jumpInstance.process(data, builder);
            squatInstance.process(data, builder);
            return true;
        }
        if (data->type() == Input::MessageType::MessageType_ApplicationControl) {
            const ApplicationControl::Control* control = data->control(); 
            if (control->featureId()->str() == "action_detection") {
                walkInstance.process(data, builder);
            }
        }
        
        return false;
    }

    void actionDetectionComponent::writeToFlatbuffers(actionData::ActionBuilder& actionBuilder) {
        walkInstance.writeToFlatbuffers(actionBuilder);
        jumpInstance.writeToFlatbuffers(actionBuilder);
        squatInstance.writeToFlatbuffers(actionBuilder);
    }
}