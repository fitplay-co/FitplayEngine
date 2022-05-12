#include <emscripten.h>
#include <emscripten/bind.h>
#include <string>
#include <vector>
#include <list>
#include <chrono> 
#include "json.hpp"

// for convenience
using json = nlohmann::json;
using namespace emscripten;

class Jump {
    public:
    Jump(){}
    std::string process(const std::string &str) {
        this->calculate_current_frame_distance(str);
        this->calculate_current_distance_mean();
        this->calculate_current_direction();
        std::string res = this->add_json(str);
        return res;
    }

    void calculate_current_distance_mean() {
        this->current_distance_mean = this->current_distance_mean * 0.8 + this->current_frame_distance * 0.2;
    }

    void calculate_current_direction() {
        this->current_speed_mean = this->current_speed_mean * 0.9  + abs(this->current_frame_distance - this->current_distance_mean) * 0.1;
        if(this->current_frame_distance - this->current_distance_mean > this->amp_threshold) {
            if(this->current_jump != 0) {
                if(this->frameShiftFilterCount > 2) {
                    this->shiftDirectionStepPerMinutes();
                    this->current_jump = 0;
                    this->frameShiftFilterCount = 0;
                } else {
                    this->frameShiftFilterCount = this->frameShiftFilterCount + 1;
                }
            } else {
                this->frameShiftFilterCount = 0;
            }        
        } 
        if(this->current_frame_distance - this->current_distance_mean < -this->amp_threshold) {
            if(this->current_jump != 1) {
                if(this->frameShiftFilterCount > 2) {
                    this->shiftDirectionStepPerMinutes();
                    this->current_jump = 1;
                    this->frameShiftFilterCount = 0;
                } else {
                    this->frameShiftFilterCount = this->frameShiftFilterCount + 1;
                }
            } else {
                this->frameShiftFilterCount = 0;
            }
        } 
        if(abs(this->current_frame_distance - this->current_distance_mean) < this->amp_threshold) {
            this->fpmStopCount = this->fpmStopCount + 1;
            if(this->fpmStopCount > 30) {
                this->current_jump = 0;
                this->fpm = 0;
            }
        } else {
            this->fpmStopCount = 0;
        }
        this->current_height = this->current_jump == 1? this->current_speed_mean:0;
    }

    void shiftDirectionStepPerMinutes() {
        long long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        while (this->times.size() > 0 && this->times.front() <= now - 5000) {
            this->times.pop_front();
        }
        this->times.push_back(now);
        this->fpm = this->times.size() * 20;
    }

    void calculate_current_frame_distance(const std::string &str) {

        this->current_distance_mean = 0;
        auto j = json::parse(str);
        // get data from json
        float num1 = j["pose_landmark"]["keypoints"][23]["y"];
        float num2 = j["pose_landmark"]["keypoints"][24]["y"];
        float num3 = j["pose_landmark"]["keypoints"][25]["y"];
        float num4 = j["pose_landmark"]["keypoints"][26]["y"];
        float num5 = j["pose_landmark"]["keypoints"][27]["y"];
        float num6 = j["pose_landmark"]["keypoints"][28]["y"];
        float weighted_sum = num1 + num2 + num3 + num4 + num5 + num6;
        this->current_distance_mean = weighted_sum;
    }

    std::string add_json(const std::string &str) {

        auto j = json::parse(str);
        // get data from json
        j["action_detection"]["jump"]["up"] = this->current_jump;
        j["action_detection"]["jump"]["strength"] = this->current_height;
        std::string s = j.dump();
        return s;
    }


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
};

// Binding code
EMSCRIPTEN_BINDINGS(my_class_example) {
  class_<Jump>("Jump")
    .constructor<>()
    .function("process", &Jump::process)
    ;
}