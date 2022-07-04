#include <string>
#include <vector>
#include <list>
#include <chrono> 
#include <math.h>
#include "actionData_generated.h"

namespace actionwalk {
    class walk {
        private:
            float current_lfoot = 0;
            float current_rfoot = 0;
            float current_lhip = 0;
            float current_rhip = 0;
            float current_lhip_mean = 0;
            float current_rhip_mean = 0;
            float current_lfoot_mean = 0;
            float current_rfoot_mean = 0;

            float current_lfoot_height = 0;
            float current_rfoot_height = 0;
            float current_thigh = 0;
            float current_thigh_mean = 0;
            float current_leg_mean = 0;
            float current_height_mean = 0;

            float current_left = 0;
            float current_right = 0;

            float amp_threshold = 0.01;
            // float fpm = 0;
            // std::list<long long> times;

            // float stepCount = 0;
            bool tLock = false;
            int tStart = 0;
            int tStart2 = 0;
            int tEnd = 0;
            int tEnd2 = 0;
            float tWindow = 0;
            float tWindow2 = 0;
            float step_rate = 0;

            float fpmStopCount = 0;
            float frameShiftFilterCount = 0;
            float fpmStopCount2 = 0;
            float frameShiftFilterCount2 = 0;
            float fpmStopCount3 = 0;

            float test;

        public:
            walk();
            ~ walk();
            void process(float walk_data[], const PoseData::Pose* data);
            void calculate_current_frame(const PoseData::Pose* data);
            void calculate_current_mean();
            void calculate_current_left();
            void calculate_current_right();
            // void shiftDirectionStepPerMinutes();
            void calculateWindow();
            int militime();
            void checkCurrentStepRate();
            float calVecAngle(const PoseData::Pose* data, int num1, int num2, int num3, int config);
            float distanceFinder(const PoseData::Pose* data, int num1, int num2);
    };

    walk::walk() {}
    walk::~walk() {}
    void walk::process(float walk_data[], const PoseData::Pose* data) {
        calculate_current_frame(data);
        calculate_current_mean();
        calculate_current_left();
        calculate_current_right();
        checkCurrentStepRate();
        walk_data[0] = current_left;
        walk_data[1] = current_right;
        walk_data[2] = step_rate;
        walk_data[3] = current_lhip_mean;
        walk_data[4] = current_rhip_mean;
    }

    void walk::calculate_current_frame(const PoseData::Pose* data) {
        current_rfoot = data->keypoints()->Get(28)->y() + data->keypoints()->Get(30)->y() + data->keypoints()->Get(32)->y()
                        + data->keypoints()->Get(28)->x() + data->keypoints()->Get(30)->x() + data->keypoints()->Get(32)->x();
        current_lfoot = data->keypoints()->Get(27)->y() + data->keypoints()->Get(29)->y() + data->keypoints()->Get(31)->y()
                        + data->keypoints()->Get(27)->x() + data->keypoints()->Get(29)->x() + data->keypoints()->Get(31)->x();
        current_lfoot_height = data->keypoints3D()->Get(31)->y();
        current_rfoot_height = data->keypoints3D()->Get(32)->y();
        current_rhip = calVecAngle(data, 12, 24, 26, 1);
        current_lhip = calVecAngle(data, 11, 23, 25, 1);
        float left_height = data->keypoints3D()->Get(23)->y() - data->keypoints3D()->Get(25)->y();
        float right_height = data->keypoints3D()->Get(24)->y() - data->keypoints3D()->Get(26)->y();
        current_thigh = std::max(left_height, right_height);
    }

    void walk::calculate_current_mean() {
        current_lfoot_mean = current_lfoot_mean * 0.85 + current_lfoot * 0.15;
        current_rfoot_mean = current_rfoot_mean * 0.85 + current_rfoot * 0.15;
        current_lhip_mean = current_lhip_mean * 0.8 + current_lhip * 0.2;
        current_rhip_mean = current_rhip_mean * 0.8 + current_rhip * 0.2;
        current_thigh_mean = (current_thigh_mean * 0.9 + current_thigh * 0.1) * 0.79 + 0.15;
        current_leg_mean = current_thigh_mean * 2.004;
        current_height_mean = current_thigh_mean / 0.245;
    }

    void walk::calculate_current_left() {
        // for left leg
        // threshold to be tested
        if(current_lfoot - current_lfoot_mean > amp_threshold) {
            if(current_left != -1) {
                if(frameShiftFilterCount > 3 && current_left == 1) {
                    current_left = -1;
                    // tEnd = militime();
                    // tWindow = float((tEnd - tStart))/1000;
                    // calculateWindow();
                }
                else {
                    frameShiftFilterCount = frameShiftFilterCount + 1;
                }
            }
            else {
                frameShiftFilterCount = 0;
            }
        }
        if(current_lfoot - current_lfoot_mean < -amp_threshold) {
            if(current_left != 1) {
                if(frameShiftFilterCount > 3) {
                    current_left = 1;
                    // tStart = militime();
                    calculateWindow();
                    // shiftDirectionStepPerMinutes();
                }
                else {
                    frameShiftFilterCount = frameShiftFilterCount + 1;
                }
            }
            else {
                frameShiftFilterCount = 0;
            }
        }
        if(abs(current_lfoot - current_lfoot_mean) < amp_threshold) {
            if(current_left != 0) {
                if(frameShiftFilterCount > 6) {
                    current_left = 0;
                }
                else {
                    frameShiftFilterCount = frameShiftFilterCount + 1;
                }
            }
            else {
                frameShiftFilterCount = 0;
            }
        }
        if(current_left != 0) {
            if(abs(current_lfoot - current_lfoot_mean) < amp_threshold) {
                fpmStopCount = fpmStopCount + 1;
                if(fpmStopCount > 15) {
                    current_left = 0;
                }
            }
            else {
                fpmStopCount = 0;
            }
        }

    }

    void walk::calculate_current_right() {
        // for right leg
        // threshold to be tested
        if(current_rfoot - current_rfoot_mean > amp_threshold) {
            if(current_right != -1) {
                if(frameShiftFilterCount2 > 3 && current_right == 1) {
                    current_right = -1;
                    // tEnd2 = militime();
                    // tWindow2 = float((tEnd2 - tStart2))/1000;
                }
                else {
                    frameShiftFilterCount2 = frameShiftFilterCount2 + 1;
                }
            }
            else {
                frameShiftFilterCount2 = 0;
            }
        }
        if(current_rfoot - current_rfoot_mean < -amp_threshold) {
            if(current_right != 1) {
                if(frameShiftFilterCount2 > 3) {
                    current_right = 1;
                    // tStart2 = militime();
                    calculateWindow();
                    // shiftDirectionStepPerMinutes();
                }
                else {
                    frameShiftFilterCount2 = frameShiftFilterCount2 + 1;
                }
            }
            else {
                frameShiftFilterCount2 = 0;
            }
        }
        if(abs(current_rfoot - current_rfoot_mean) < amp_threshold) {
            if(current_right != 0) {
                if(frameShiftFilterCount2 > 6) {
                    current_right = 0;
                }
                else {
                    frameShiftFilterCount2 = frameShiftFilterCount2 + 1;
                }
            }
            else {
                frameShiftFilterCount2 = 0;
            }
        }
        if(current_right != 0) {
            if(abs(current_rfoot - current_rfoot_mean) < amp_threshold) {
                fpmStopCount2 = fpmStopCount2 + 1;
                if(fpmStopCount2 > 15) {
                    current_right = 0;
                }
            }
            else {
                fpmStopCount2 = 0;
            }
        }
    }

    void walk::checkCurrentStepRate() {
        if(current_left == 0 && current_right == 0) {
            fpmStopCount3 = fpmStopCount3 + 1;
            if(fpmStopCount3 > 15) {
                tWindow = 0;
                tWindow2 = 0;
                step_rate = 0;
                tLock = false;
            }
        }
        else {
            fpmStopCount3 = 0;
        }
        // if(tWindow!=0 || tWindow2!=0) {
        //     step_rate = 0.6 / std::max(tWindow, tWindow2);
        // }
        // else {
        //     step_rate = 0;
        // }
    }

    // void walk::shiftDirectionStepPerMinutes() {
    //     long long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    //     while (times.size() > 0 && times.front() <= now - 1000) {
    //         times.pop_front();
    //     }
    //     times.push_back(now);
    //     stepCount = times.size();
    //     if(stepCount < 2) step_rate = 0;
    //     fpm = times.size() * 60;
    // }

    int walk::militime() {
        auto now = chrono::high_resolution_clock::now();
        auto timeMillis = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();
        int mili = (int)timeMillis;
        return mili;
    }

    void walk::calculateWindow() {
        auto now = chrono::high_resolution_clock::now();
        auto timeMillis = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();
        int mili = (int)timeMillis;
        if(!tLock) {
            tStart = mili;
            tLock = true;
        }
        else {
            tLock = false;
            tEnd = mili;
            float tWindow = float((tEnd - tStart))/1000;
            step_rate = 0.7/tWindow;
            tLock = true;
            tStart = mili;
        }
    }

    float walk::calVecAngle(const PoseData::Pose* data, int num1, int num2, int num3, int config) {
        float vec[3][3];
        int angle_points[3] = {num1, num2, num3};
        for (int i = 0; i < 3; i++) {
            vec[i][0] = data->keypoints3D()->Get(angle_points[i])->x();
            vec[i][1] = data->keypoints3D()->Get(angle_points[i])->y();
            vec[i][2] = data->keypoints3D()->Get(angle_points[i])->z();
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
            vec[i][0] = data->keypoints3D()->Get(dis_points[i])->x();
            vec[i][1] = data->keypoints3D()->Get(dis_points[i])->y();
            vec[i][2] = data->keypoints3D()->Get(dis_points[i])->z();
        }
        return sqrt(pow(vec[0][0]-vec[1][0],2) + pow(vec[0][1]-vec[1][1],2) + pow(vec[0][2]-vec[1][2],2));
    }
}