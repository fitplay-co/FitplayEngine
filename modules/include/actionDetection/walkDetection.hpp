#include <string>
#include <vector>
#include <list>
#include <map>
#include <chrono> 
#include <math.h>
#include "actionData_generated.h"
#include "euroFilter.hpp"
#include "walkData.hpp"

namespace actionwalk {

    typedef flatbuffers::Offset<actionData::Walk> flat;

    class walk {
        private:
            // calculated data
            map<int, float> frameData;
            map<int, float> meanData;
            map<int, float> timeData;
            map<int, int> timeData2;

            // for one euro filter
            float frameCount = 0;
            double frequency = 60, mincutoff = 0.85, beta = 1.2, dcutoff = 1.0;
            detection::OneEuroFilter *leftFootFilter;
            detection::OneEuroFilter *rightFootFilter;

            // output data
            float currentLeft = 0;
            float currentRight = 0;
            float stepRateLeft = 0;
            float stepRateRight = 0;
            float stepLenLeft = 0;
            float stepLenRight = 0;
            float leftProgress = 0;
            float rightProgress = 0;
            float turn = 0;

            // velocity data
            // version 2
            float stepRate = 0;
            float stepLen = 0;
            float velocity = 0;

            // progress info
            float preLeft = 0;
            float preRight = 0;
            float maxLeft = 30;
            float maxRight = 30;
            float lenLeft = 0;
            float lenRight = 0;
            float frameLeft = 0;
            float frameRight = 0;

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
            void calculateProgress();
            void calculateTurn(const PoseData::Pose* data);
            flat writeFlatBuffer(flatbuffers::FlatBufferBuilder& resultBuilder);
            int militime();
            float calVecAngle(const PoseData::Pose* data, int num1, int num2, int num3, int config);
            float distanceFinder(const PoseData::Pose* data, int num1, int num2);
    };

    walk::walk() {
        leftFootFilter = new detection::OneEuroFilter(frequency, mincutoff, beta, dcutoff);
        rightFootFilter = new detection::OneEuroFilter(frequency, mincutoff, beta, dcutoff);
        timeData2[tlock] = 0;
    }

    walk::~walk() {}

    void walk::process(const PoseData::Pose* data) {
        calculateFrame(data);
        calculateMean();
        calculateLeft();
        calculateRight();
        calculateStepLen();
        checkStepRate();
        calculateProgress();
        calculateTurn(data);

    }

    flat walk::writeFlatBuffer(flatbuffers::FlatBufferBuilder& resultBuilder) {
        return actionData::CreateWalk(resultBuilder, 
            currentLeft,
            currentRight,
            stepRateLeft,
            stepRateRight,
            meanData[leftHip],
            meanData[rightHip],
            stepLenLeft,
            stepLenRight,
            leftProgress,
            rightProgress,
            turn,
            stepRate,
            stepLen,
            velocity);
    }

    void walk::calculateFrame(const PoseData::Pose* data) {

        frameData[preLeftFoot] = frameData[leftFoot];
        frameData[preRightFoot] = frameData[rightFoot];
        frameData[rightFoot] = data->keypoints()->Get(28)->y() + data->keypoints()->Get(32)->y();
        frameData[leftFoot] = data->keypoints()->Get(27)->y() + data->keypoints()->Get(31)->y();
        frameData[rightHip] = calVecAngle(data, 12, 24, 26, 1);
        frameData[leftHip] = calVecAngle(data, 11, 23, 25, 1);
        float leftThighHeight = data->keypoints3D()->Get(23)->y() - data->keypoints3D()->Get(25)->y();
        float rightThighHeight = data->keypoints3D()->Get(24)->y() - data->keypoints3D()->Get(26)->y();
        frameData[thigh] = std::max(leftThighHeight, rightThighHeight);
        frameData[leftLeg] = data->keypoints3D()->Get(27)->y() - data->keypoints3D()->Get(23)->y();
        frameData[rightLeg] = data->keypoints3D()->Get(28)->y() - data->keypoints3D()->Get(24)->y();
    }

    void walk::calculateMean() {
        double timestamp = frameCount/frequency;
        frameCount = frameCount + 1;
        double leftFootFiltered = leftFootFilter->filter(frameData[leftFoot], timestamp);
        double rightFootFiltered = rightFootFilter->filter(frameData[rightFoot], timestamp);
        meanData[leftFoot] = meanData[currentLeftFoot];
        meanData[currentLeftFoot] = float(leftFootFiltered);
        meanData[rightFoot] = meanData[currentRightFoot];
        meanData[currentRightFoot] = float(rightFootFiltered);
        // meanData[leftFoot] = meanData[leftFoot] * 0.85 + frameData[leftFoot] * 0.15;
        // meanData[rightFoot] = meanData[rightFoot] * 0.85 + frameData[rightFoot] * 0.15;
        meanData[leftHip] = meanData[leftHip] * 0.8 + frameData[leftHip] * 0.2;
        meanData[rightHip] = meanData[rightHip] * 0.8 + frameData[rightHip] * 0.2;
        meanData[thigh] = (meanData[thigh] * 0.9 + frameData[thigh] * 0.1) * 0.79 + 0.15;
        meanData[leg] = meanData[thigh] * 2.004;
        meanData[height] = meanData[thigh] / 0.245;
    }

    void walk::calculateLeft() {
        preLeft = currentLeft;
        // for left leg
        if(currentLeft == 2) {
            if(frameData[rightLeg] - frameData[leftLeg] < 0.1) {
                currentLeft = 0;
            }
        }
        else {
            // leg down
            if(frameData[leftFoot] - meanData[leftFoot] > 0.01) {
                if(currentLeft != -1) {
                    if(frameShiftFilterCount > 3 && currentLeft == 1) {
                        currentLeft = -1;
                        timeData[tEndLeft] = militime();
                        timeData[tWindowLeft] = float((timeData[tEndLeft] - timeData[tStartLeft]))/1000;
                    }
                    else { frameShiftFilterCount = frameShiftFilterCount + 1; }
                }
                else { frameShiftFilterCount = 0; }
            }
            // leg up
            if(frameData[leftFoot] - meanData[leftFoot] < -0.01) {
                if(currentLeft != 1) {
                    if(frameShiftFilterCount > 3 && (currentLeft == 0 || currentLeft == -1)) {
                        if(timeData2[tlock] == 1){
                            timeData2[t2] = militime();
                            timeData2[tlock] = 0;
                        }
                        else {
                            timeData2[tlock] = 1;
                            timeData2[t1] = militime();
                        }
                        currentLeft = 1;
                        timeData[tStartLeft] = militime();
                    }
                    else { frameShiftFilterCount = frameShiftFilterCount + 1; }
                }
                else { frameShiftFilterCount = 0; }
            }
            // leg still
            if((-0.01 < (frameData[leftFoot] - meanData[leftFoot])) && ((frameData[leftFoot] - meanData[leftFoot]) < 0.01)) {
                if(currentLeft != 0 && currentLeft != 2) {
                    if(frameShiftFilterCount > 6) {
                        // threshold to be tested
                        if(frameData[rightLeg] - frameData[leftLeg] > 0.1 && currentLeft == 1) {
                            currentLeft = 2;
                        }
                        else {
                            preLeft = currentLeft; 
                            currentLeft = 0;
                        }
                    }
                    else { frameShiftFilterCount = frameShiftFilterCount + 1; }
                }
                else { frameShiftFilterCount = 0; }
            }
        }
    }

    void walk::calculateRight() {
        preRight = currentRight;
        // for right leg
        if(currentRight == 2) {
            if(frameData[leftLeg] - frameData[rightLeg] < 0.1) {
                currentRight = 0;
            }
        }
        else {
            // leg down
            if(frameData[rightFoot] - meanData[rightFoot] > 0.01) {
                if(currentRight != -1) {
                    if(frameShiftFilterCount2 > 3 && currentRight == 1) {
                        currentRight = -1;
                        timeData[tEndRight] = militime();
                        timeData[tWindowRight] = float((timeData[tEndRight] - timeData[tStartRight]))/1000;
                    }
                    else { frameShiftFilterCount2 = frameShiftFilterCount2 + 1; }
                }
                else { frameShiftFilterCount2 = 0; }
            }
            // leg up
            if(frameData[rightFoot] - meanData[rightFoot] < -0.01) {
                if(currentRight != 1) {
                    if(frameShiftFilterCount2 > 3 && (currentRight == 0 || currentRight == -1)) {
                        if(timeData2[tlock] == 1){
                            timeData2[t2] = militime();
                            timeData2[tlock] = 0;
                        }
                        else {
                            timeData2[tlock] = 1;
                            timeData2[t1] = militime();
                        }
                        currentRight = 1;
                        timeData[tStartRight] = militime();
                    }
                    else { frameShiftFilterCount2 = frameShiftFilterCount2 + 1; }
                }
                else { frameShiftFilterCount2 = 0; }
            }
            // leg still
            if((-0.01 < (frameData[rightFoot] - meanData[rightFoot])) && ((frameData[rightFoot] - meanData[rightFoot]) < 0.01)) {
                if(currentRight != 0 && currentRight != 2) {
                    if(frameShiftFilterCount2 > 6) {
                        // threshold to be tested
                        if(frameData[leftLeg] - frameData[rightLeg] > 0.1 && currentRight == 1) {
                            currentRight = 2;
                        }
                        else {
                            preRight = currentRight;
                            currentRight = 0; 
                        }
                    }
                    else { frameShiftFilterCount2 = frameShiftFilterCount2 + 1; }
                }
                else { frameShiftFilterCount2 = 0; }
            }
        }
    }

    void walk::calculateStepLen() {
        float maxSL = meanData[height] * 1.2;
        float leftFlexion = (180 - meanData[leftHip]) > 90 ? 90 : (180 - meanData[leftHip]);
        float rightFlexion = (180 - meanData[rightHip]) > 90 ? 90 : (180 - meanData[rightHip]);
        if(preLeft == 1 && currentLeft == -1) {
            stepLenLeft = (maxSL / pow(90,(float)1/3)) * pow(leftFlexion,(float)1/3);
            stepLen = stepLenLeft;
        }
        if(preRight == 1 && currentRight == -1) {
            stepLenRight = (maxSL / pow(90,(float)1/3)) * pow(rightFlexion,(float)1/3);
            stepLen = stepLenRight;
        }
        // if(currentLeft == 0 || currentLeft == 2) { stepLenLeft = 0; }
        // if(currentRight == 0 || currentRight == 2) { stepLenRight = 0; }
    }

    void walk::checkStepRate() {
        if((currentLeft == 0 || currentLeft == 2) && (currentRight == 0 || currentRight == 2)) {
            fpmStopCount3 = fpmStopCount3 + 1;
            if(fpmStopCount3 > 15) {
                timeData2[t1] = 0;
                timeData2[t2] = 0;
                timeData2[tlock] = 0;
                stepRate = 0;
                timeData[tWindowLeft] = 0;
                timeData[tWindowRight] = 0;
            }
        }
        else { fpmStopCount3 = 0; }
        if (timeData[tWindowLeft] != 0) stepRateLeft = 0.35 / timeData[tWindowLeft]; 
        else { stepRateLeft = 0; }
        if (timeData[tWindowRight] != 0) stepRateRight = 0.35 / timeData[tWindowRight];
        else { stepRateRight = 0; }

        float preStepRate = stepRate;
        stepRate = stepRateLeft > stepRateRight ? stepRateLeft : stepRateRight;

        if(timeData2[t1]!=0&&timeData2[t2]!=0){
            stepRate = 1 / (float(abs(timeData2[t1] - timeData2[t2]))/1000);
        }
        // stepRate = preStepRate * 0.8 + stepRate * 0.2;
        velocity = velocity * 0.8 + (stepRate * stepLen) * 0.2;
        if(velocity > 10) velocity = 10;
        if(velocity < 0.01) velocity = 0;
        // velocity = stepRate * stepLenLeft;
    }

    void walk::calculateProgress() {
        // left progress
        if(preLeft == 0 && currentLeft == 0) { 
            float increment = - (frameData[leftFoot] - frameData[preLeftFoot]);
            lenLeft = lenLeft + increment;
            leftProgress = lenLeft / maxLeft * 0.5;
            leftProgress = leftProgress < 0.05 ? 0 : leftProgress;
        }
        if(frameLeft == 0) {
            if(currentLeft == 1) {
                float increment = - (frameData[leftFoot] - frameData[preLeftFoot]);
                if(increment < 0) {
                    maxLeft = lenLeft;
                    lenLeft = 0;
                    frameLeft = -1;
                    leftProgress = 0.5;
                }
                else {
                    lenLeft = lenLeft + increment;
                    leftProgress = lenLeft / maxLeft * 0.5;
                    leftProgress = leftProgress > 0.5 ? 0.5 : leftProgress;
                }
            }
        }
        if(frameLeft == -1){
            float increment = (frameData[leftFoot] - frameData[preLeftFoot]);
            lenLeft = lenLeft + increment;
            leftProgress = lenLeft / maxLeft * 0.5 + 0.5;
            leftProgress = leftProgress > 1 ? 1 : leftProgress;
        }
        if(preLeft == -1 && currentLeft != preLeft) {
            frameLeft = 0;
            lenLeft = 0;
            leftProgress = 1;
        }

        // right progress
        if(preRight == 0 && currentRight == 0) { 
            float increment = - (frameData[rightFoot] - frameData[preRightFoot]);
            lenRight = lenRight + increment;
            rightProgress = lenRight / maxRight * 0.5;
            rightProgress = rightProgress < 0.05 ? 0 : rightProgress;
        }
        if(frameRight == 0) {
            if(currentRight == 1) {
                float increment = - (frameData[rightFoot] - frameData[preRightFoot]);
                if(increment < 0) {
                    maxRight = lenRight;
                    lenRight = 0;
                    frameRight = -1;
                    rightProgress = 0.5;
                }
                else {
                    lenRight = lenRight + increment;
                    rightProgress = lenRight / maxRight * 0.5;
                    rightProgress = rightProgress > 0.5 ? 0.5 : rightProgress;
                }
            }
        }
        if(frameRight == -1){
            float increment = (frameData[rightFoot] - frameData[preRightFoot]);
            lenRight = lenRight + increment;
            rightProgress = lenRight / maxRight * 0.5 + 0.5;
            rightProgress = rightProgress > 1 ? 1 : rightProgress;
        }
        if(preRight == -1 && currentRight != preRight) {
            frameRight = 0;
            lenRight = 0;
            rightProgress = 1;
        }

    }

    void walk::calculateTurn(const PoseData::Pose* data) {
        float a[2] = {data->keypoints3D()->Get(11)->x() - data->keypoints3D()->Get(12)->x()
                    ,data->keypoints3D()->Get(11)->z() - data->keypoints3D()->Get(12)->z()};
        // float b[2] = {data->keypoints3D()->Get(23)->x() - data->keypoints3D()->Get(24)->x()
        //             ,data->keypoints3D()->Get(23)->z() - data->keypoints3D()->Get(24)->z()};
        float b[2] = {1,0};
        float ab,a1,b1,cosr;
        ab = a[0]*b[0]+a[1]*b[1];
        a1 = sqrt(a[0]*a[0]+a[1]*a[1]);
        b1 = sqrt(b[0]*b[0]+b[1]*b[1]);
        cosr = ab/a1/b1;
        turn = acos(cosr)*180/acos(-1);
        if(data->keypoints3D()->Get(12)->z() < data->keypoints3D()->Get(11)->z()) turn = - turn;
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