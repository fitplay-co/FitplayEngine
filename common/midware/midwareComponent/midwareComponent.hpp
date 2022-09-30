#ifndef Midware_MidwareComponent
#define Midware_MidwareComponent

#include "inputMessage_generated.h"

using namespace std;

namespace Midware {
    class MidwareComponent {
        protected:
            string name;
            int subscriberCount = 0; //订阅此功能的客户端数目
            void increaseSubscriberCount();
            void decreaseSubscriberCount();
        public:
            MidwareComponent(string);
            ~MidwareComponent();
            bool handleInput(const OsInput::InputMessage*, flatbuffers::FlatBufferBuilder&);
            virtual bool process(const OsInput::InputMessage*, flatbuffers::FlatBufferBuilder&);
            virtual void writeToFlatbuffers(ActionData::ActionBuilder&);
    };

    MidwareComponent::MidwareComponent(string name) {
        this->name = name;
    }
    MidwareComponent::~MidwareComponent() {}

    void MidwareComponent::increaseSubscriberCount() {
        ++subscriberCount;
    }

    void MidwareComponent::decreaseSubscriberCount() {
        if (subscriberCount > 0) {
            --subscriberCount;
        }
    }

    bool MidwareComponent::process(const OsInput::InputMessage* data, flatbuffers::FlatBufferBuilder& builder) {
        return false;
    }

    bool MidwareComponent::handleInput(const OsInput::InputMessage* data, flatbuffers::FlatBufferBuilder& builder) {
        if (data->type() == OsInput::MessageType::MessageType_Pose) {
            return subscriberCount > 0 ? this->process(data, builder) : false;
            // return this->process(data, builder);
        } else if (data->type() == OsInput::MessageType::MessageType_ApplicationControl) {
            const ApplicationControl::Control* control = data->control(); 
            if (control->featureId()->str() != name) {
                return false;
            }
            if (control->action()->str() == "subscribe") {
                increaseSubscriberCount();
            } else if (control->action()->str() == "release") {
                decreaseSubscriberCount();
            } else {
                this->process(data, builder);
            }
        }
        return false;
    }

    void MidwareComponent::writeToFlatbuffers(ActionData::ActionBuilder& actionBuilder) {
        
    }
}
#endif