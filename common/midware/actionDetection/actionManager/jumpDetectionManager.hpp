#include <string>
#include <vector>
#include <list>
#include <map>
#include <chrono> 
#include <math.h>
#include "actionData_generated.h"
#include "midwareComponent/midwareComponent.hpp"
#include "jumpDetectionManagerData.hpp"

namespace actionjump {

    class jump: public Midware::MidwareComponent {
        private:
            std::shared_ptr<vector<float>> jumpActionPoseData;

            float jumpHeight = 0;
            float onTheGround = 0;
            float trunkVelocity = 0;
            float incrementCount = 0;

            flatbuffers::Offset<ActionData::Jump> flatbuffersOffset;

        public:
            jump();
            ~ jump();
            bool process(const OsInput::InputMessage*, flatbuffers::FlatBufferBuilder&);
            void writeToFlatbuffers(ActionData::ActionBuilder&);
            void calculateCurrent(const PoseData::Pose* data);
            void calculateTrunk();
    };

    jump::jump(): MidwareComponent("jump")  {
        jumpActionPoseData = make_shared<vector<float>>(10);
    }

    jump::~jump() {}

    bool jump::process(const OsInput::InputMessage* data, flatbuffers::FlatBufferBuilder& builder) {
        if (data->type() == OsInput::MessageType::MessageType_Pose) {
            const PoseData::Pose* pose = data->pose();
            calculateCurrent(pose);
            calculateTrunk();

            flatbuffersOffset = ActionData::CreateJump(builder,
                                                        onTheGround,
                                                        trunkVelocity);
        }

        return true;
    }

    void jump::writeToFlatbuffers(ActionData::ActionBuilder& actionBuilder) {
        actionBuilder.add_jump(flatbuffersOffset);
    }

    void jump::calculateCurrent(const PoseData::Pose* data) {
        jumpActionPoseData->at(trunkCenter) = (data->keypoints()->Get(11)->y() + data->keypoints()->Get(12)->y() + data->keypoints()->Get(23)->y() + data->keypoints()->Get(24)->y())/4;
        jumpActionPoseData->at(lastTrunkCenterMean) = jumpActionPoseData->at(trunkCenterMean);
        jumpActionPoseData->at(trunkCenterMean) = jumpActionPoseData->at(trunkCenter) * 0.2 + jumpActionPoseData->at(trunkCenterMean) * 0.8;
    }

    void jump::calculateTrunk() {
        float increment = - (jumpActionPoseData->at(trunkCenterMean) - jumpActionPoseData->at(lastTrunkCenterMean));
        if(onTheGround == 0) {
            if(increment > 0) {
                jumpHeight = jumpHeight + increment;
                // threshold to be tested
                if(incrementCount > 5 && jumpHeight > 0.1) {
                    onTheGround = 1;
                    trunkVelocity = increment;
                }
                else {
                    incrementCount = incrementCount + 1;
                }
            }
            else {
                incrementCount = 0;
                jumpHeight = 0;
            }
        }
        if(onTheGround == 1) {
            if(increment < 0) {
                onTheGround = 0;
            }
        }
    }
}