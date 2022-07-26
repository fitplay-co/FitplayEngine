#include "fitting.hpp"
#include "midwareComponent/midwareComponent.hpp"
#include "actionData_generated.h"

namespace fitplay {

    class fittingComponent: public Midware::MidwareComponent {
        private:
            fitplay::fitting fitInstance;
            fitplay::fitting mirrorFitInstance;
            flatbuffers::Offset<actionData::Fitting> flatbuffersOffset;
        
        public:
            fittingComponent();
            ~fittingComponent();
            bool process(const Input::InputMessage*, flatbuffers::FlatBufferBuilder&);
            void writeToFlatbuffers(actionData::ActionBuilder&);
    };

    fittingComponent::fittingComponent(): MidwareComponent("fitting") {
        fitInstance.mirror = false;
        mirrorFitInstance.mirror = true;
    }
    fittingComponent::~fittingComponent() {}

    bool fittingComponent::process(const Input::InputMessage* data, flatbuffers::FlatBufferBuilder& builder) {
        if (data->type() == Input::MessageType::MessageType_Pose) {
            const PoseData::Pose* pose = data->pose();
            fitInstance.process(pose);
            mirrorFitInstance.process(pose);
            auto p1 = fitInstance.writeFlatBuffer(builder);
            auto p2 = mirrorFitInstance.writeFlatBuffer(builder);
            auto pl1 = fitInstance.writeFlatBufferLocal(builder);
            auto pl2 = mirrorFitInstance.writeFlatBufferLocal(builder);
            flatbuffersOffset = actionData::CreateFitting(builder, p1, p2, pl1, pl2);
        }
        
        return true;
    }

    void fittingComponent::writeToFlatbuffers(actionData::ActionBuilder& actionBuilder) {
        actionBuilder.add_fitting(flatbuffersOffset);
    }
}