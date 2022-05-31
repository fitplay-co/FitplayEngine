#include <string>
#include <vector>
#include <list>
#include <chrono> 
#include "flatbuffer/actionData_generated.h"

namespace actionjump {
    class jump {
        private:
            float current_distance_mean = 0;
            float current_speed_mean = 0;
            float current_frame_distance = 0;
            float current_jump = 0;
            float current_height = 0;
            float speed_mean = 0;
            float amp_threshold = 0.25;
            bool monitor_process = false;
            float fpm = 0;
            std::list<long long> times;
            float fpmStopCount = 0;
            float frameShiftFilterCount = 0;
        public:
            jump();
            ~ jump();
            //flatbuffers::Offset<actionData::Walk> process(flatbuffers::FlatBufferBuilder build_data, PoseData::Pose* data);
            void process(float jump_data[], PoseData::Pose* data);
            void calculate_current_frame_distance(PoseData::Pose* data);
            void calculate_current_distance_mean();
            void calculate_current_direction();
            void shiftDirectionStepPerMinutes();

    };
    jump::jump() {}
    jump::~jump() {}
    void jump::process(float jump_data[], PoseData::Pose* data) {
        calculate_current_frame_distance(data);
        calculate_current_distance_mean();
        calculate_current_direction();
        jump_data[0] = current_jump;
        jump_data[1] = current_height;
    }
    void jump::calculate_current_frame_distance(PoseData::Pose* data) {
        current_distance_mean = 0;
        float num1 = data->keypoints()->Get(23)->y();
        float num2 = data->keypoints()->Get(24)->y();
        float num3 = data->keypoints()->Get(25)->y();
        float num4 = data->keypoints()->Get(26)->y();
        float num5 = data->keypoints()->Get(27)->y();
        float num6 = data->keypoints()->Get(28)->y();
        float weighted_sum = num1 + num2 + num3 + num4 + num5 + num6;
        current_distance_mean = weighted_sum;
    }
    void jump::calculate_current_distance_mean() {
        current_distance_mean = current_distance_mean * 0.8 + current_frame_distance * 0.2;
    }
    void jump::calculate_current_direction() {
        current_speed_mean = current_speed_mean * 0.9  + abs(current_frame_distance - current_distance_mean) * 0.1;
        if(current_frame_distance - current_distance_mean > amp_threshold) {
            if(current_jump != 0) {
                if(frameShiftFilterCount > 2) {
                    shiftDirectionStepPerMinutes();
                    current_jump = 0;
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
                    shiftDirectionStepPerMinutes();
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
                fpm = 0;
            }
        } else {
            fpmStopCount = 0;
        }
        current_height = current_jump == 1? current_speed_mean:0;
    }
    void jump::shiftDirectionStepPerMinutes() {
        long long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        while (times.size() > 0 && times.front() <= now - 5000) {
            times.pop_front();
        }
        times.push_back(now);
        fpm = times.size() * 20;
    }
}