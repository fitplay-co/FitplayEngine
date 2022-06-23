#include <string>
#include <vector>
#include <list>
#include <chrono> 
#include <math.h>
#include "actionData_generated.h"

namespace actionwalk {
    class walk {
        private:
            float current_lhip = 0;
            float current_rhip = 0;
            float current_lknee = 0;
            float current_rknee = 0;
            float current_lfoot = 0;
            float current_rfoot = 0;
            float current_thigh = 0;

            float current_lhip_mean = 0;
            float current_rhip_mean = 0;
            float current_lknee_mean = 0;
            float current_rknee_mean = 0;
            float current_lfoot_mean = 0;
            float current_rfoot_mean = 0;
            float current_lhip_vmean = 0;
            float current_rhip_vmean = 0;
            float current_thigh_mean = 0;
            float current_leg_mean = 0;
            float current_height_mean = 0;

            float current_left = 0;
            float current_right = 0;

            float amp_threshold = 0.02;
            float angle_threshold = 10;
            bool monitor_process = false;
            float fpm = 0;
            std::list<long long> times;
            float fpmStopCount = 0;
            float frameShiftFilterCount = 0;
            float fpmStopCount2 = 0;
            float frameShiftFilterCount2 = 0;

            float test;

        public:
            walk();
            ~ walk();
            void process(float walk_data[], float move_data[], const PoseData::Pose* data);
            void calculate_current_frame(const PoseData::Pose* data);
            void calculate_current_mean();
            void calculate_current_left();
            void calculate_current_right();
            void shiftDirectionStepPerMinutes();
            float calVecAngle(const PoseData::Pose* data, int num1, int num2, int num3, int config);
            float distanceFinder(const PoseData::Pose* data, int num1, int num2);
    };

    walk::walk() {}
    walk::~walk() {}
    void walk::process(float walk_data[], float move_data[], const PoseData::Pose* data) {
        calculate_current_frame(data);
        calculate_current_mean();
        calculate_current_left();
        calculate_current_right();
        // current_left;
        // current_right;
        // current_lhip_mean;
        // current_rhip_mean;
        // fpm;

        walk_data[0] = current_left;
        walk_data[1] = fpm;
        walk_data[2] = current_leg_mean;
    }

    void walk::calculate_current_frame(const PoseData::Pose* data) {
        current_lhip = calVecAngle(data, 12, 24, 26, 1);
        current_rhip = calVecAngle(data, 11, 23, 25, 1);
        current_lknee = calVecAngle(data, 24, 26, 28, 0);
        current_rknee = calVecAngle(data, 23, 25, 27, 0);
        current_lfoot = data->keypoints3D()->Get(31)->y();
        current_rfoot = data->keypoints3D()->Get(32)->y();
        float left_height = data->keypoints3D()->Get(23)->y() - data->keypoints3D()->Get(25)->y();
        float right_height = data->keypoints3D()->Get(24)->y() - data->keypoints3D()->Get(26)->y();
        current_thigh = std::max(left_height, right_height);
    }

    void walk::calculate_current_mean() {
        float last_lhip_mean = current_lhip_mean;
        float last_rhip_mean = current_rhip_mean;
        current_lhip_mean = current_lhip_mean * 0.9 + current_lhip * 0.1;
        current_rhip_mean = current_rhip_mean * 0.9 + current_rhip * 0.1;
        current_lknee_mean = current_lknee_mean * 0.9 + current_lknee * 0.1;
        current_rknee_mean = current_rknee_mean * 0.9 + current_rknee * 0.1;
        current_lfoot_mean = current_lfoot_mean * 0.9 + current_lfoot * 0.1;
        current_rfoot_mean = current_rfoot_mean * 0.9 + current_rfoot * 0.1;
        current_lhip_vmean = current_lhip_vmean * 0.9 + (current_lhip_mean - last_lhip_mean) * 0.1;
        current_rhip_vmean = current_rhip_vmean * 0.9 + (current_rhip_mean - last_rhip_mean) * 0.1;
        current_thigh_mean = current_thigh_mean * 0.9 + current_thigh * 0.1;
        current_leg_mean = current_thigh_mean * 2.004;
        current_height_mean = current_leg_mean / 0.245;
    }

    void walk::calculate_current_left() {
        // for left leg
        // using lhip, lhip mean, lhip velocity, lhip vmean, lfoot, lfoot mean
        // threshold to be tested
        test = current_lknee - current_lknee_mean;
        if(current_left == 0) {
            if(current_lknee - current_lknee_mean < -angle_threshold) {
                if(frameShiftFilterCount > 2) {
                    current_left = 1;
                    frameShiftFilterCount = 0;
                } else {
                    frameShiftFilterCount = frameShiftFilterCount + 1;
                }
            } else {
                frameShiftFilterCount = 0;
            }
        }
        if(current_left == 1) {
            if(current_lknee - current_lknee_mean > angle_threshold) {
                if(frameShiftFilterCount > 2) {
                    current_left = -1;
                    frameShiftFilterCount = 0;
                } else {
                    frameShiftFilterCount = frameShiftFilterCount + 1;
                }
            } else {
                frameShiftFilterCount = 0;
            }
        }
        if(current_left == -1) {
            if(abs(current_lknee - current_lknee_mean) < angle_threshold) {
                if(frameShiftFilterCount > 2) {
                    current_left = 0;
                    frameShiftFilterCount = 0;
                } else {
                    frameShiftFilterCount = frameShiftFilterCount + 1;
                }
            } else {
                frameShiftFilterCount = 0;
            }
        }
        if(current_left != 0) {
            if(abs(current_lknee - current_lknee_mean) < angle_threshold) {
                fpmStopCount = fpmStopCount + 1;
                if(fpmStopCount > 30) {
                    current_left = 0;
                    fpm = 0;
                }
            } else {
            fpmStopCount = 0;
            }
        }
    }

    void walk::calculate_current_right() {
        // for right leg
        // using lhip, lhip mean, lhip velocity, lhip vmean, lfoot, lfoot mean
        // threshold to be tested
        if(current_right == 0) {
            if(current_rhip - current_rhip_mean > amp_threshold || current_rfoot - current_rfoot_mean > amp_threshold) {
                if(frameShiftFilterCount2 > 2) {
                    current_right = 1;
                    frameShiftFilterCount2 = 0;
                } else {
                    frameShiftFilterCount2 = frameShiftFilterCount2 + 1;
                }
            } else {
                frameShiftFilterCount2 = 0;
            }
        }
        if(current_right == 1) {
            if(current_rhip_vmean < amp_threshold) {
                if(frameShiftFilterCount2 > 2) {
                    current_right = 2;
                    shiftDirectionStepPerMinutes();
                    frameShiftFilterCount2 = 0;
                } else {
                    frameShiftFilterCount2 = frameShiftFilterCount2 + 1;
                }
            } else {
                frameShiftFilterCount2 = 0;
            }
        }
        if(current_right == 2) {
            if(current_rhip - current_rhip_mean < -amp_threshold || current_rfoot - current_rfoot_mean < -amp_threshold) {
                if(frameShiftFilterCount2 > 2) {
                    current_right = 3;
                    frameShiftFilterCount2 = 0;
                } else {
                    frameShiftFilterCount2 = frameShiftFilterCount2 + 1;
                }
            } else {
                frameShiftFilterCount2 = 0;
            }
        }
        if(current_right == 3) {
            if(current_rhip_vmean < amp_threshold || abs(current_rfoot - current_rfoot_mean) < amp_threshold) {
                if(frameShiftFilterCount2 > 2) {
                    current_right = 0;
                    frameShiftFilterCount2 = 0;
                } else {
                    frameShiftFilterCount2 = frameShiftFilterCount2 + 1;
                }
            } else {
                frameShiftFilterCount2 = 0;
            }
        }
        if(current_right != 0) {
            if(abs(current_rhip - current_rhip_mean) < amp_threshold) {
                fpmStopCount2 = fpmStopCount2 + 1;
                if(fpmStopCount2 > 30) {
                    current_right = 0;
                    fpm = 0;
                }
            } else {
            fpmStopCount2 = 0;
            }
        }
    }

    void walk::shiftDirectionStepPerMinutes() {
        long long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        while (times.size() > 0 && times.front() <= now - 5000) {
            times.pop_front();
        }
        times.push_back(now);
        fpm = times.size() * 12;
    }

    float walk::calVecAngle(const PoseData::Pose* data, int num1, int num2, int num3, int config) {
        float vec[3][3];
        int angle_points[3] = {num1, num2, num3};
        for (int i = 0; i < 3; i++) {
            vec[i][0] = data->keypoints()->Get(angle_points[i])->x();
            vec[i][1] = data->keypoints()->Get(angle_points[i])->y();
            vec[i][2] = data->keypoints()->Get(angle_points[i])->z();
        }
        float Lba = sqrt(pow((vec[0][0]-vec[1][0]),2) + pow((vec[0][1]-vec[1][1]),2) + pow((vec[0][2]-vec[1][2]),2));
        float Lbc = sqrt(pow((vec[2][0]-vec[1][0]),2) + pow((vec[2][1]-vec[1][1]),2) + pow((vec[2][2]-vec[1][2]),2));
        float Labc = (vec[0][0]-vec[1][0])*(vec[2][0]-vec[1][0]) + (vec[0][1]-vec[1][1])*(vec[2][1]-vec[1][1]) + (vec[0][2]-vec[1][2])*(vec[2][2]-vec[1][2]);
        if(config == 1) {
            float Lba = sqrt(1);
            float Labc = (vec[2][1]-vec[1][1]);
        }
        if(config == 2) {
            float Lbc = sqrt(1);
            float Labc = (vec[0][2]-vec[1][2]);
        }
        float cosAngle = Labc/(Lba*Lbc);
        float angle = acos(cosAngle)*360/2/acos(-1);
        return angle;
    }

    float walk::distanceFinder(const PoseData::Pose* data, int num1, int num2) {
        float vec[2][3];
        int dis_points[2] = {num1, num2};
        for (int i = 0; i < 2; i++) {
            vec[i][0] = data->keypoints()->Get(dis_points[i])->x();
            vec[i][1] = data->keypoints()->Get(dis_points[i])->y();
            vec[i][2] = data->keypoints()->Get(dis_points[i])->z();
        }
        return sqrt(pow(vec[0][0]-vec[1][0],2) + pow(vec[0][1]-vec[1][1],2) + pow(vec[0][2]-vec[1][2],2));
    }
}