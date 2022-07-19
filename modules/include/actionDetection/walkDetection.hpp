#include <string>
#include <vector>
#include <list>
#include <map>
#include <chrono> 
#include <math.h>
#include "actionData_generated.h"

namespace actionwalk {

    typedef flatbuffers::Offset<actionData::Walk> flat;

    class walk {
        private:
            map<std::string, float> frameData;
            map<std::string, float> meanData;
            map<std::string, float> timeData;

            float currentLeft = 0;
            float currentRight = 0;
            float stepRateLeft = 0;
            float stepRateRight = 0;
            float stepLenLeft = 0;
            float stepLenRight = 0;

            float fpmStopCount = 0;
            float frameShiftFilterCount = 0;
            float fpmStopCount2 = 0;
            float frameShiftFilterCount2 = 0;
            float fpmStopCount3 = 0;

            float threshold = 0.05;

        public:
            walk();
            ~ walk();
            void process(const PoseData::Pose* data);
            void calculateFrame(const PoseData::Pose* data);
            void calculateMean();
            void calculateLeft();
            void calculateRight();
            void calculateStepLen();
            void checkStepRate();
            flat writeFlatBuffer(flatbuffers::FlatBufferBuilder& resultBuilder);
            int militime();
            float calVecAngle(const PoseData::Pose* data, int num1, int num2, int num3, int config);
            float distanceFinder(const PoseData::Pose* data, int num1, int num2);
    };

    walk::walk() {}

    walk::~walk() {}

    void walk::process(const PoseData::Pose* data) {
        calculateFrame(data);
        calculateMean();
        calculateLeft();
        calculateRight();
        calculateStepLen();
        checkStepRate();
    }

    flat walk::writeFlatBuffer(flatbuffers::FlatBufferBuilder& resultBuilder) {
        return actionData::CreateWalk(resultBuilder, 
            currentLeft,
            frameData["leftFoot"] - meanData["leftFoot"],
            stepRateLeft,
            stepRateRight,
            meanData["leftHip"],
            meanData["rightHip"],
            stepLenLeft,
            stepLenRight);
    }

    void walk::calculateFrame(const PoseData::Pose* data) {
        frameData["rightFoot"] = data->keypoints()->Get(28)->y() + data->keypoints()->Get(32)->y();
        frameData["leftFoot"] = data->keypoints()->Get(27)->y() + data->keypoints()->Get(31)->y();
        frameData["rightHip"] = calVecAngle(data, 12, 24, 26, 1);
        frameData["leftHip"] = calVecAngle(data, 11, 23, 25, 1);
        float leftThighHeight = data->keypoints3D()->Get(23)->y() - data->keypoints3D()->Get(25)->y();
        float rightThighHeight = data->keypoints3D()->Get(24)->y() - data->keypoints3D()->Get(26)->y();
        frameData["thigh"] = std::max(leftThighHeight, rightThighHeight);
        frameData["leftLeg"] = data->keypoints3D()->Get(27)->y() - data->keypoints3D()->Get(23)->y();
        frameData["rightLeg"] = data->keypoints3D()->Get(28)->y() - data->keypoints3D()->Get(24)->y();
    }

    void walk::calculateMean() {
        meanData["leftFoot"] = meanData["leftFoot"] * 0.85 + frameData["leftFoot"] * 0.15;
        meanData["rightFoot"] = meanData["rightFoot"] * 0.85 + frameData["rightFoot"] * 0.15;
        meanData["leftHip"] = meanData["leftHip"] * 0.8 + frameData["leftHip"] * 0.2;
        meanData["rightHip"] = meanData["rightHip"] * 0.8 + frameData["rightHip"] * 0.2;
        meanData["thigh"] = (meanData["thigh"] * 0.9 + frameData["thigh"] * 0.1) * 0.79 + 0.15;
        meanData["leg"] = meanData["thigh"] * 2.004;
        meanData["height"] = meanData["thigh"] / 0.245;
    }

    void walk::calculateLeft() {
        // for left leg
        // leg down
        // if(currentLeft == 2) {
        //     if(frameData["rightLeg"] - frameData["leftLeg"] > 0.1) {
        //         currentLeft == 2;
        //     }
        // }
        if(frameData["leftFoot"] - meanData["leftFoot"] > 0.01) {
            if(currentLeft != -1) {
                if(frameShiftFilterCount > 3 && (currentLeft == 1 || currentLeft == 2)) {
                    currentLeft = -1;
                    timeData["tEndLeft"] = militime();
                    timeData["tWindowLeft"] = float((timeData["tEndLeft"] - timeData["tStartLeft"]))/1000;
                }
                else { frameShiftFilterCount = frameShiftFilterCount + 1; }
            }
            else { frameShiftFilterCount = 0; }
        }
        // leg up testing
        if(frameData["leftFoot"] - meanData["leftFoot"] < -0.01) {
            if(currentLeft != 1) {
                if(frameShiftFilterCount > 3 && (currentLeft == 0 || currentLeft == -1)) {
                    currentLeft = 1;
                    timeData["tStartLeft"] = militime();
                }
                else { frameShiftFilterCount = frameShiftFilterCount + 1; }
            }
            else { frameShiftFilterCount = 0; }
        }
        // leg still
        if((-0.01 < (frameData["leftFoot"] - meanData["leftFoot"])) && ((frameData["leftFoot"] - meanData["leftFoot"]) < 0.01)) {
            if(currentLeft != 0 && currentLeft != 2) {
                if(frameShiftFilterCount > 6) {
                    // threshold to be tested
                    if(frameData["rightLeg"] - frameData["leftLeg"] > 0.1 && currentLeft == 1) {
                        currentLeft = 2;
                    }
                    else { currentLeft = 0; }
                }
                else { frameShiftFilterCount = frameShiftFilterCount + 1; }
            }
            else { frameShiftFilterCount = 0; }
        }
        // if(currentLeft != 0) {
        //     if(abs(frameData["leftFoot"] - meanData["leftFoot"]) < threshold) {
        //         fpmStopCount = fpmStopCount + 1;
        //         if(fpmStopCount > 15) {
        //             currentLeft = 0;
        //         }
        //     }
        //     else { fpmStopCount = 0; }
        // }
    }

    void walk::calculateRight() {
        // for right leg
        if(frameData["rightFoot"] - meanData["rightFoot"] > threshold) {
            if(currentRight != -1) {
                if(frameShiftFilterCount2 > 3 && currentRight == 1) {
                    currentRight = -1;
                    timeData["tEndRight"] = militime();
                    timeData["tWindowRight"] = float((timeData["tEndRight"] - timeData["tStartRight"]))/1000;
                }
                else { frameShiftFilterCount2 = frameShiftFilterCount2 + 1; }
            }
            else { frameShiftFilterCount2 = 0; }
        }
        if(frameData["rightFoot"] - meanData["rightFoot"] < -threshold) {
            if(currentRight != 1) {
                if(frameShiftFilterCount2 > 3) {
                    currentRight = 1;
                    timeData["tStartRight"] = militime();
                }
                else { frameShiftFilterCount2 = frameShiftFilterCount2 + 1; }
            }
            else { frameShiftFilterCount2 = 0; }
        }
        if(abs(frameData["rightFoot"] - meanData["rightFoot"]) < threshold) {
            if(currentRight != 0) {
                if(frameShiftFilterCount2 > 6) {
                    currentRight = 0;
                }
                else { frameShiftFilterCount2 = frameShiftFilterCount2 + 1; }
            }
            else { frameShiftFilterCount2 = 0; }
        }
        if(currentRight != 0) {
            if(abs(frameData["rightFoot"] - meanData["rightFoot"]) < threshold) {
                fpmStopCount2 = fpmStopCount2 + 1;
                if(fpmStopCount2 > 15) {
                    currentRight = 0;
                }
            }
            else { fpmStopCount2 = 0; }
        }
    }

    void walk::calculateStepLen() {
        float maxSL = frameData["height"] * 1.2;
        float leftFlexion = (180 - meanData["leftHip"]) > 90 ? 90 : (180 - meanData["leftHip"]);
        float rightFlexion = (180 - meanData["rightHip"]) > 90 ? 90 : (180 - meanData["rightHip"]);
        stepLenLeft = (maxSL / pow(90,(1/3))) * pow(leftFlexion,(1/3));
        stepLenRight = (maxSL / pow(90,(1/3))) * pow(rightFlexion,(1/3));
    }

    void walk::checkStepRate() {
        if((currentLeft == 0 || currentLeft == 2) && (currentRight == 0 || currentLeft == 2)) {
            fpmStopCount3 = fpmStopCount3 + 1;
            if(fpmStopCount3 > 15) {
                timeData["tWindowLeft"] = 0;
                timeData["tWindowRight"] = 0;
            }
        }
        else { fpmStopCount3 = 0; }
        if (timeData["tWindowLeft"] != 0) stepRateLeft = 0.6 / timeData["tWindowLeft"]; 
        else { stepRateLeft = 0; }
        if (timeData["tWindowRight"] != 0) stepRateRight = 0.6 / timeData["tWindowRight"];
        else { stepRateRight = 0; }
    }

    int walk::militime() {
        auto now = chrono::high_resolution_clock::now();
        auto timeMillis = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();
        int mili = (int)timeMillis;
        return mili;
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