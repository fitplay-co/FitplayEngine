#include <string>
#include <vector>
#include <list>
#include <map>
#include <chrono> 
#include <math.h>
#include "actionData_generated.h"

namespace actionjump {
    class jump {
        private:
            bool init = false;
            map<std::string, float> frameData;
            float leftOff = 0;
            float rightOff = 0;
            float frameShiftCount = 0;
            float frameShiftCount2 = 0;

            float current_distance_mean = 0;
            float current_speed_mean = 0;
            float current_frame_distance = 0;
            float current_jump = 0;
            float current_height = 0;
            float speed_mean = 0;
            float amp_threshold = 0.25;
            float fpmStopCount = 0;
            float frameShiftFilterCount = 0;
            float last_distance_mean = 0;
            float current_height_mean = 0;
            float height = 0;
        public:
            jump();
            ~ jump();
            void process(const PoseData::Pose* data);
            void calculate_current_frame_distance(const PoseData::Pose* data);
            void calculate_current_distance_mean();
            void calculate_current_direction();
            flatbuffers::Offset<actionData::Jump> writeFlatBuffer(flatbuffers::FlatBufferBuilder& resultBuilder);

    };
    jump::jump() {}
    jump::~jump() {}
    void jump::process(const PoseData::Pose* data) {
        calculate_current_frame_distance(data);
        calculate_current_distance_mean();
        calculate_current_direction();
    }
    flatbuffers::Offset<actionData::Jump> jump::writeFlatBuffer(flatbuffers::FlatBufferBuilder& resultBuilder) {
        return actionData::CreateJump(resultBuilder, 
            leftOff,
            height);
    }
    void jump::calculate_current_frame_distance(const PoseData::Pose* data) {
        if(init == false) {
            init = true;
            frameData["preLeftFoot"] = 0;
            frameData["preRightFoot"] = 0;
        }
        frameData["currentLeftDis"] = (data->keypoints()->Get(27)->y() - frameData["preLeftFoot"]) * 0.2 + frameData["preLeftFoot"] * 0.8;
        frameData["currentRightDis"] = (data->keypoints()->Get(28)->y() - frameData["preRightFoot"]) * 0.2 + frameData["preRightFoot"] * 0.8;
        frameData["preLeftFoot"] = frameData["currentLeftDis"];
        frameData["preRightFoot"] = frameData["currentRightDis"];
        float num1 = data->keypoints()->Get(23)->y();
        float num2 = data->keypoints()->Get(24)->y();
        float num3 = data->keypoints()->Get(25)->y();
        float num4 = data->keypoints()->Get(26)->y();
        float num5 = data->keypoints()->Get(27)->y();
        float num6 = data->keypoints()->Get(28)->y();
        float weighted_sum = num1 + num2 + num3 + num4 + num5 + num6;
        current_frame_distance = weighted_sum;
    }
    void jump::calculate_current_distance_mean() {
        last_distance_mean = current_distance_mean;
        current_distance_mean = current_distance_mean * 0.8 + current_frame_distance * 0.2;
    }
    void jump::calculate_current_direction() {
        if(frameData["currentLeftDis"] > 0.005 && frameData["currentRightDis"] > 0.005) {
            if(leftOff != 1) {
                if(frameShiftCount > 5) {
                    leftOff = 1;
                    frameShiftCount = 0;
                } else {
                    frameShiftCount = frameShiftCount + 1;
                }
            } else { frameShiftCount = 0; }
        }
        current_speed_mean = current_speed_mean * 0.9  + abs(current_frame_distance - current_distance_mean) * 0.1;
        if(current_frame_distance - current_distance_mean > amp_threshold) {
            if(current_jump != -1) {
                if(frameShiftFilterCount > 2) {
                    current_jump = -1;
                    frameShiftFilterCount = 0;
                } else {
                    frameShiftFilterCount = frameShiftFilterCount + 1;
                }
            } else {
                frameShiftFilterCount = 0;
            }        
        } 
        if(current_frame_distance - current_distance_mean < -amp_threshold) {
            if(current_jump != 1) {
                if(frameShiftFilterCount > 2) {
                    current_jump = 1;
                    frameShiftFilterCount = 0;
                } else {
                    frameShiftFilterCount = frameShiftFilterCount + 1;
                }
            } else {
                frameShiftFilterCount = 0;
            }
        } 
        if(abs(current_frame_distance - current_distance_mean) < amp_threshold) {
            fpmStopCount = fpmStopCount + 1;
            if(fpmStopCount > 30) {
                current_jump = 0;
                leftOff = 0;
            }
        } else {
            fpmStopCount = 0;
        }
        current_height = current_distance_mean - last_distance_mean;
        current_height_mean = current_height_mean * 0.5 + current_height * 0.5;
        height = -(fabs(current_height_mean)>0.03? current_height_mean:0);
    }
}