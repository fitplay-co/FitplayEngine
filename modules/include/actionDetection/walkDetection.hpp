#include <string>
#include <vector>
#include <list>
#include <chrono> 
#include "flatbuffer/actionData_generated.h"

namespace actionwalk {
    class walk {
        private:
            float current_distance_mean = 0;
            float current_speed_mean = 0;
            float current_frame_distance = 0;
            float current_leg = 0;
            float speed_mean = 0;
            float amp_threshold = 0.02;
            bool monitor_process = false;
            float fpm = 0;
            std::list<long long> times;
            float fpmStopCount = 0;
            float frameShiftFilterCount = 0;
        public:
            walk();
            ~ walk();
            //flatbuffers::Offset<actionData::Walk> process(flatbuffers::FlatBufferBuilder build_data, PoseData::Pose* data);
            void process(float walk_data[],const PoseData::Pose* data);
            void calculate_current_frame_distance(const PoseData::Pose* data);
            void calculate_current_distance_mean();
            void calculate_current_direction();
            void shiftDirectionStepPerMinutes();
    };

    walk::walk() {}
    walk::~walk() {}
    void walk::process(float walk_data[], const PoseData::Pose* data) {
        calculate_current_frame_distance(data);
        calculate_current_distance_mean();
        calculate_current_direction();
        walk_data[0] = current_leg;
        walk_data[1] = fpm;
        walk_data[2] = current_speed_mean;
        // auto pose = actionData::CreateWalk(build_data, current_leg, fpm, current_speed_mean);
        // build_data.Finish(pose);
        // return pose;

        // flatbuffers::FlatBufferBuilder build_data(1024);
        // PoseData::PoseBuilder build_pose(build_data);
        // auto point1 = PoseData::CreatePoint(build_data, 1, 1, 1, 1, build_data.CreateString("nose"));
        // auto point2 = PoseData::CreatePoint(build_data, 1, 1, 1, 1, build_data.CreateString("arm"));
        // auto point3 = PoseData::CreatePoint(build_data, 1, 1, 1, 1, build_data.CreateString("leg"));
        // std::vector<flatbuffers::Offset<PoseData::Point>> nodeVector;
        // nodeVector.push_back(point1);
        // nodeVector.push_back(point2);
        // nodeVector.push_back(point3);
        // auto keypoint = build_data.CreateVector(nodeVector);
        // build_pose.add_keypoints(keypoint);
        // build_pose.Finish();

        // flatbuffers::FlatBufferBuilder build_data(1024);
        // auto point1 = PoseData::CreatePoint(build_data, 1, 1, 1, 1, build_data.CreateString("nose"));
        // auto point2 = PoseData::CreatePoint(build_data, 1, 1, 1, 1, build_data.CreateString("arm"));
        // auto point3 = PoseData::CreatePoint(build_data, 1, 1, 1, 1, build_data.CreateString("leg"));
        // std::vector<flatbuffers::Offset<PoseData::Point>> nodeVector;
        // nodeVector.push_back(point1);
        // nodeVector.push_back(point2);
        // nodeVector.push_back(point3);
        // auto keypoint = build_data.CreateVector(nodeVector);
        // auto keypoint3d = build_data.CreateVector(nodeVector);
        // auto pose = PoseData::CreatePose(build_data, keypoint, keypoint3d);
        // build_data.Finish(pose);
    }

    void walk::calculate_current_frame_distance(const PoseData::Pose* data) {
        current_distance_mean = 0;
        float num1 = data->keypoints()->Get(26)->y();
        float num2 = data->keypoints()->Get(25)->y();
        float num3 = data->keypoints()->Get(28)->y();
        float num4 = data->keypoints()->Get(27)->y();
        float num5 = data->keypoints()->Get(32)->y();
        float num6 = data->keypoints()->Get(31)->y();
        float weighted_sum = num1 - num2 + num3 - num4 + num5 - num6;
        current_distance_mean = weighted_sum;
    }

    void walk::calculate_current_distance_mean() {
        current_distance_mean = current_distance_mean * 0.9 + current_frame_distance * 0.1;
    }

    void walk::calculate_current_direction() {
        current_speed_mean = current_speed_mean * 0.9  + abs(current_frame_distance - current_distance_mean) * 0.1;
        if(current_frame_distance - current_distance_mean > amp_threshold) {
            if(current_leg != 1) {
                if(frameShiftFilterCount > 2) {
                    shiftDirectionStepPerMinutes();
                    current_leg = 1;
                    frameShiftFilterCount = 0;
                } else {
                    frameShiftFilterCount = frameShiftFilterCount + 1;
                }
            } else {
                frameShiftFilterCount = 0;
            }        
        } 
        if(current_frame_distance - current_distance_mean < -amp_threshold) {
            if(current_leg != -1) {
                if(frameShiftFilterCount > 2) {
                    shiftDirectionStepPerMinutes();
                    current_leg = -1;
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
                current_leg = 0;
                fpm = 0;
            }
        } else {
            fpmStopCount = 0;
        }
    }

    void walk::shiftDirectionStepPerMinutes() {
        long long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        while (times.size() > 0 && times.front() <= now - 5000) {
            times.pop_front();
        }
        times.push_back(now);
        fpm = times.size() * 20;
    }
}