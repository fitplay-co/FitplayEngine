#ifndef Midware_MidwareComponent
#define Midware_MidwareComponent

#include "inputMessage_generated.h"

using namespace std;

namespace Midware {
    class MidwareComponent {
        protected:
            string name;
            int subscriberCount = 0; //订阅此功能的客户端数目
            float* params = nullptr;
            int params_num = 0;
            void increaseSubscriberCount();
            void decreaseSubscriberCount();
        public:
            MidwareComponent(string);
            ~MidwareComponent();
            void setParameters(float* params, int num);
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

    void MidwareComponent::setParameters(float* params, int num){
        if(num == 0){
            if(this->params != nullptr){
                free(this->params);
            }
            goto end;
        }

        if(this->params_num != num){
            if(this->params != nullptr){
                free(this->params);
            }
            this->params = (float*)calloc(num, sizeof(float));
        }

        memcpy(this->params, params, num * sizeof(float));

end:
        this->params_num = num;

        cout << __FUNCTION__ << ": ";
        for(int i = 0; i < this->params_num; i++){
             cout << this->params[i] << " ";
        }
        cout << ", with num of " << this->params_num <<endl;
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