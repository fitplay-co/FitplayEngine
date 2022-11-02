#ifndef GENERAL_calculator
#define GENERAL_calculator

#include <string>
#include <vector>
#include <math.h>
#include "generalData.hpp"
#include "actionData_generated.h"
#include "midwareComponent/midwareComponent.hpp"

namespace generalDetection {

    class generalCalculator: public Midware::MidwareComponent {

        private:
            flatbuffers::Offset<ActionData::General> flatbuffersOffset;
            float confidence;

        public:
            generalCalculator();
            ~ generalCalculator();
            bool process(const OsInput::InputMessage*, flatbuffers::FlatBufferBuilder&);
            void writeToFlatbuffers(ActionData::ActionBuilder&);
            void calculateConfidence(const PoseData::Pose* data);
    };

    generalCalculator::generalCalculator(): MidwareComponent("general_detection") {}

    generalCalculator::~generalCalculator() {}

    bool generalCalculator::process(const OsInput::InputMessage* data, flatbuffers::FlatBufferBuilder& builder) {
        if (data->type() == OsInput::MessageType::MessageType_Pose) {
            const PoseData::Pose* pose = data->pose();
            calculateConfidence(pose);
            flatbuffersOffset = ActionData::CreateGeneral(builder,
                                                        confidence);
        }
        return true;
    }

    void generalCalculator::writeToFlatbuffers(ActionData::ActionBuilder& actionBuilder) {
        actionBuilder.add_general(flatbuffersOffset);
    }

    void generalCalculator::calculateConfidence(const PoseData::Pose* data) {
        confidence = 0;
        float trustCount = 0;
        int arrLen = sizeof(generalKeypoints) / sizeof(generalKeypoints[0]);
        for(int i = 0; i < arrLen; i++) {
            if(data->keypoints()->Get(generalKeypoints[i])->score() > 0.02) {
                trustCount++;
            };
        }
        if(trustCount > 8) confidence = 1;
    }
}

#endif