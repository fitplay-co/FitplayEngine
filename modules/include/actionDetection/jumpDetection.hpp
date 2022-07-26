#include <string>
#include <vector>
#include <list>
#include <map>
#include <chrono> 
#include <math.h>
#include "actionData_generated.h"
#include "midwareComponent/midwareComponent.hpp"

namespace actionjump {

    class jump: public Midware::MidwareComponent {
        private:
            bool init = false;
            map<std::string, float> frameData;
            map<std::string, float> timeData;

            float footOff = 0;
            float kneeOff = 0;
            float height = 0;
            float velocity = 0;

            float frameShiftCount = 0;
            float frameShiftCount2 = 0;
            bool timeLock = false;

            flatbuffers::Offset<actionData::Jump> flatbuffersOffset;

        public:
            jump();
            ~ jump();
            bool process(const Input::InputMessage*, flatbuffers::FlatBufferBuilder&);
            void writeToFlatbuffers(actionData::ActionBuilder&);
            void calculateCurrent(const PoseData::Pose* data);
            void calculateFoot();
            void calculateKnee();
            void calculateAnkle();
            void checkTimestamp();
            float militime();
    };

    jump::jump(): MidwareComponent("jump")  {}

    jump::~jump() {}

    bool jump::process(const Input::InputMessage* data, flatbuffers::FlatBufferBuilder& builder) {
        if (data->type() == Input::MessageType::MessageType_Pose) {
            const PoseData::Pose* pose = data->pose();
            calculateCurrent(pose);
            calculateFoot();
            calculateKnee();
            checkTimestamp();

            flatbuffersOffset = actionData::CreateJump(builder, 
                                                        kneeOff,
                                                        footOff,
                                                        height,
                                                        velocity);
        }

        return true;
    }

    void jump::writeToFlatbuffers(actionData::ActionBuilder& actionBuilder) {
        actionBuilder.add_jump(flatbuffersOffset);
    }

    void jump::calculateCurrent(const PoseData::Pose* data) {
        frameData["currentLeftKnee"] = data->keypoints()->Get(25)->y();
        frameData["currentLeftKneeMean"] = frameData["currentLeftKnee"] * 0.1 + frameData["currentLeftKneeMean"] * 0.9;
        frameData["currentRightKnee"] = data->keypoints()->Get(26)->y();
        frameData["currentRightKneeMean"] = frameData["currentRightKnee"] * 0.1 + frameData["currentRightKneeMean"] * 0.9;
        frameData["currentLeftFoot"] = data->keypoints()->Get(31)->y();
        frameData["currentLeftFootMean"] = frameData["currentLeftFoot"] * 0.1 + frameData["currentLeftFootMean"] * 0.9;
        frameData["currentRightFoot"] = data->keypoints()->Get(32)->y();
        frameData["currentRightFootMean"] = frameData["currentRightFoot"] * 0.1 + frameData["currentRightFootMean"] * 0.9;

        frameData["currentHip"] = (data->keypoints()->Get(23)->y() + data->keypoints()->Get(24)->y())/2;
        frameData["currentHipMean"] = frameData["currentHip"] * 0.1 + frameData["currentHipMean"] * 0.9;
    }

    void jump::calculateFoot() {
        if(frameData["currentLeftFoot"] - frameData["currentLeftFootMean"] < -0.035 && frameData["currentRightFoot"] - frameData["currentRightFootMean"] < -0.035) {
            if(footOff != 1) {
                if(frameShiftCount > 5) {
                    footOff = 1;
                    // timestamp
                    if(timeLock == true) {
                        timeData["end"] = militime();
                        timeData["window"] = (timeData["end"] - timeData["start"])/1000;
                        velocity = frameData["currentHip"] - frameData["currentHipMean"];
                        height = 0.063 / timeData["window"];
                        timeLock = false;
                    }
                    frameShiftCount = 0;
                } else {
                    frameShiftCount = frameShiftCount + 1;
                }
            } else { frameShiftCount = 0; }
        }
        if(frameData["currentLeftFoot"] - frameData["currentLeftFootMean"] > 0.05 || frameData["currentRightFoot"] - frameData["currentRightFootMean"] > 0.05) {
            if(footOff != 0) {
                if(frameShiftCount > 5) {
                    footOff = 0;
                    frameShiftCount = 0;
                } else {
                    frameShiftCount = frameShiftCount + 1;
                }
            } else { frameShiftCount = 0; }
        }
        if(abs(frameData["currentLeftFoot"] - frameData["currentLeftFootMean"]) < 0.04 || abs(frameData["currentRightFoot"] - frameData["currentRightFootMean"]) < 0.04) {
            if(footOff != 0) {
                if(frameShiftCount > 5) {
                    footOff = 0;
                    frameShiftCount = 0;
                } else {
                    frameShiftCount = frameShiftCount + 1;
                }
            } else { frameShiftCount = 0; }
        }
    }

    void jump::calculateKnee() {
        if(frameData["currentLeftKnee"] - frameData["currentLeftKneeMean"] < -0.04 && frameData["currentRightKnee"] - frameData["currentRightKneeMean"] < -0.04) {
            if(kneeOff != 1) {
                if(frameShiftCount2 > 5) {
                    kneeOff = 1;
                    if(timeLock == false) {
                        timeData["start"] = militime();
                        timeLock = true;
                    }
                    frameShiftCount2 = 0;
                } else {
                    frameShiftCount2 = frameShiftCount2 + 1;
                }
            } else { frameShiftCount2 = 0; }
        }
        if(frameData["currentLeftKnee"] - frameData["currentLeftKneeMean"] > 0.04 || frameData["currentRightKnee"] - frameData["currentRightKneeMean"] > 0.04) {
            if(kneeOff != 0) {
                if(frameShiftCount2 > 5) {
                    kneeOff = 0;
                    frameShiftCount2 = 0;
                } else {
                    frameShiftCount2 = frameShiftCount2 + 1;
                }
            } else { frameShiftCount2 = 0; }
        }
    }

    void jump::checkTimestamp() {
        if(kneeOff == 0 && footOff == 0) {
            timeLock = false;
            height = 0;
            velocity = 0;
            timeData["window"] = 0;
        }
    }

    float jump::militime() {
        auto now = chrono::high_resolution_clock::now();
        auto timeMillis = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();
        int mili = (int)timeMillis;
        return (float)mili;
    }
}