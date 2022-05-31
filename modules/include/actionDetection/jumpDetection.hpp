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
}