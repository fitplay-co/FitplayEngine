#include <string>
#include <vector>
#include <list>
#include <map>
#include <chrono> 
#include <math.h>
#include "actionData_generated.h"
#include "midwareComponent/midwareComponent.hpp"
#include "euroFilter.hpp"
#include "walkData.hpp"

namespace actionwalk {
    class walk: public Midware::MidwareComponent {
        private:
            // calculated data
            vector<float> *frameData;
            vector<float> *meanData;
            vector<float> *progressData;
            vector<float> *timeData;
            vector<int> *timeData2;

            // for one euro filter
            float frameCount = 0;
            double frequency = 60, mincutoff = 0.85, beta = 1.2, dcutoff = 1.0;
            detection::OneEuroFilter *leftFootFilter;
            detection::OneEuroFilter *rightFootFilter;

            // output API data
            float currentLeftStatus = 0;
            float currentRightStatus = 0;
            float currentLeftStepRate = 0;
            float currentRightStepRate = 0;
            float currentLeftStepLength = 0;
            float currentRightStepLength = 0;
            float currentLeftProgress = 0;
            float currentRightProgress = 0;
            float currentTurnAng = 0;
            // velocity data
            // version 2
            float currentStepRate = 0;
            float currentStepLength = 0;
            float currentVelocity = 0;

            float frameShiftFilterCount = 0;
            float frameShiftFilterCount2 = 0;
            float frameShiftFilterCount3 = 0;
            float frameShiftFilterCount4 = 0;
            float frameShiftFilterCount5 = 0;
            float frameShiftFilterCount6 = 0;
            float frameShiftFilterCount7 = 0;
            float frameShiftFilterCount8 = 0;
            float fpmStopCount = 0;

            float currentRealTimeLeftStatus = 0;

            flatbuffers::Offset<actionData::Walk> flatbuffersOffset;
        public:
            walk();
            ~ walk();
            bool process(const Input::InputMessage*, flatbuffers::FlatBufferBuilder&);
            void writeToFlatbuffers(actionData::ActionBuilder&);
            void calculateFrame(const PoseData::Pose* data);
            void calculateMean();
            void calculateLeft();
            void calculateRight();
            void calculateStepLength();
            void calculateStepRate();
            void calculateProgress();
            void calculateRealTime();
            void calculateTurn(const PoseData::Pose* data);
            int militime();
            float calVecAngle(const PoseData::Pose* data, int num1, int num2, int num3, int config);
            float distanceFinder(const PoseData::Pose* data, int num1, int num2);
    };

    walk::walk(): MidwareComponent("walk") {
        frameData = new vector<float>(10, 0);
        meanData = new vector<float>(10, 0);
        progressData = new vector<float>(10, 0);
        progressData->at(maxLeftDistance) = 30;
        progressData->at(maxRightDistance) = 30;
        timeData = new vector<float>(10, 0);
        timeData2 = new vector<int>(10, 0);
        leftFootFilter = new detection::OneEuroFilter(frequency, mincutoff, beta, dcutoff);
        rightFootFilter = new detection::OneEuroFilter(frequency, mincutoff, beta, dcutoff);
        timeData2->at(timeLock) = 0;
    }

    walk::~walk() {}

    bool walk::process(const Input::InputMessage* data, flatbuffers::FlatBufferBuilder& builder) {
        if (data->type() == Input::MessageType::MessageType_Pose) {
            const PoseData::Pose* pose = data->pose();

            calculateFrame(pose);
            calculateMean();
            calculateLeft();
            calculateRight();
            calculateStepLength();
            calculateStepRate();
            calculateProgress();
            calculateTurn(pose);
            calculateRealTime();

            flatbuffersOffset = actionData::CreateWalk(builder, 
                                                        currentRealTimeLeftStatus,
                                                        progressData->at(totalLeftDistance),
                                                        progressData->at(maxLeftDistance),
                                                        progressData->at(currentProgressLeftStatus),
                                                        frameData->at(currentLeftFoot) - frameData->at(preLeftFoot),
                                                        frameData->at(currentLeftFoot),
                                                        meanData->at(currentRightHipAngMean),
                                                        currentLeftStepLength,
                                                        currentRightStepLength,
                                                        currentLeftProgress,
                                                        currentRightProgress,
                                                        currentTurnAng,
                                                        currentStepRate,
                                                        currentVelocity);
        }
        return true;
    }

    void walk::writeToFlatbuffers(actionData::ActionBuilder& actionBuilder) {
        actionBuilder.add_walk(flatbuffersOffset);
    }

    void walk::calculateFrame(const PoseData::Pose* data) {
        frameData->at(preLeftFoot) = frameData->at(currentLeftFoot);
        frameData->at(preRightFoot) = frameData->at(currentRightFoot);
        frameData->at(currentLeftFoot) = data->keypoints()->Get(27)->y() + data->keypoints()->Get(31)->y();
        frameData->at(currentRightFoot) = data->keypoints()->Get(28)->y() + data->keypoints()->Get(32)->y();
        frameData->at(currentLeftHipAng) = calVecAngle(data, 11, 23, 25, 1);
        frameData->at(currentRightHipAng) = calVecAngle(data, 12, 24, 26, 1);
        float leftThighHeight = data->keypoints3D()->Get(23)->y() - data->keypoints3D()->Get(25)->y();
        float rightThighHeight = data->keypoints3D()->Get(24)->y() - data->keypoints3D()->Get(26)->y();
        frameData->at(currentThighHeight) = std::max(leftThighHeight, rightThighHeight);
        frameData->at(currentLeftLegHeight) = data->keypoints3D()->Get(27)->y() - data->keypoints3D()->Get(23)->y();
        frameData->at(currentRightLegHeight) = data->keypoints3D()->Get(28)->y() - data->keypoints3D()->Get(24)->y();
    }

    void walk::calculateMean() {
        // euro filter
        double timestamp = frameCount/frequency;
        frameCount = frameCount + 1;
        double leftFootFiltered = leftFootFilter->filter(double(frameData->at(currentLeftFoot)), timestamp);
        double rightFootFiltered = rightFootFilter->filter(frameData->at(currentRightFoot), timestamp);
        meanData->at(preLeftFootMean) = meanData->at(currentLeftFootMean);
        meanData->at(currentLeftFootMean) = float(leftFootFiltered);
        meanData->at(preRightFootMean) = meanData->at(currentRightFootMean);
        meanData->at(currentRightFootMean) = float(rightFootFiltered);

        // low pass filter
        // meanData->at(currentLeftFootMean) = meanData->at(currentLeftFootMean) * 0.85 + frameData->at(currentLeftFoot) * 0.15;
        // meanData->at(currentRightFootMean) = meanData->at(currentRightFootMean) * 0.85 + frameData->at(currentRightFoot) * 0.15;
        meanData->at(currentLeftHipAngMean) = meanData->at(currentLeftHipAngMean) * 0.8 + frameData->at(currentLeftHipAng) * 0.2;
        meanData->at(currentRightHipAngMean) = meanData->at(currentRightHipAngMean) * 0.8 + frameData->at(currentRightHipAng) * 0.2;
        meanData->at(currentThighHeightMean) = (meanData->at(currentThighHeightMean) * 0.9 + frameData->at(currentThighHeight) * 0.1) * 0.79 + 0.15;
        meanData->at(currentLegHeightMean) = meanData->at(currentThighHeightMean) * 2.004;
        meanData->at(currentHeightMean) = meanData->at(currentThighHeightMean) / 0.245;
    }

    void walk::calculateLeft() {
        progressData->at(preLeftStatus) = currentLeftStatus;
        // for left leg
        // check leg length for leg hanging case
        if(currentLeftStatus == 2) {
            if(frameData->at(currentRightLegHeight) - frameData->at(currentLeftLegHeight) < 0.1) {
                currentLeftStatus = 0;
            }
        }
        else {
            // foot reaches its peak and going down
            if(frameData->at(currentLeftFoot) - meanData->at(preLeftFootMean) > 0.01) {
                if(currentLeftStatus != -1) {
                    if(frameShiftFilterCount > 3 && currentLeftStatus == 1) {
                        currentLeftStatus = -1;
                        timeData->at(timeLeftDown) = militime();
                        timeData->at(timeLeftWindow) = float((timeData->at(timeLeftDown) - timeData->at(timeLeftUp)))/1000;
                    }
                    else { frameShiftFilterCount = frameShiftFilterCount + 1; }
                }
                else { frameShiftFilterCount = 0; }
            }
            // foot going up
            if(frameData->at(currentLeftFoot) - meanData->at(preLeftFootMean) < -0.01) {
                if(currentLeftStatus != 1) {
                    if(frameShiftFilterCount > 3 && (currentLeftStatus == 0 || currentLeftStatus == -1)) {
                        if(timeData2->at(timeLock) == 1){
                            timeData2->at(timeBeta) = militime();
                            timeData2->at(timeLock) = 0;
                        }
                        else {
                            timeData2->at(timeLock) = 1;
                            timeData2->at(timeAlpha) = militime();
                        }
                        currentLeftStatus = 1;
                        timeData->at(timeLeftUp) = militime();
                    }
                    else { frameShiftFilterCount = frameShiftFilterCount + 1; }
                }
                else { frameShiftFilterCount = 0; }
            }
            // leg still
            if((-0.01 < (frameData->at(currentLeftFoot) - meanData->at(preLeftFootMean))) && ((frameData->at(currentLeftFoot) - meanData->at(preLeftFootMean) < 0.01))) {
                if(currentLeftStatus != 0 && currentLeftStatus != 2) {
                    if(frameShiftFilterCount > 6) {
                        // threshold to be tested
                        if(frameData->at(currentRightLegHeight) - frameData->at(currentLeftLegHeight) > 0.1 && currentLeftStatus == 1) {
                            currentLeftStatus = 2;
                        }
                        else {
                            progressData->at(preLeftStatus) = currentLeftStatus; 
                            currentLeftStatus = 0;
                        }
                    }
                    else { frameShiftFilterCount = frameShiftFilterCount + 1; }
                }
                else { frameShiftFilterCount = 0; }
            }
        }
    }

    void walk::calculateRight() {
        progressData->at(preRightStatus) = currentRightStatus;
        // for right leg
        // check leg length for leg hanging case
        if(currentRightStatus == 2) {
            if(frameData->at(currentLeftLegHeight) - frameData->at(currentRightLegHeight) < 0.1) {
                currentRightStatus = 0;
            }
        }
        else {
            // foot reaches its peak and going down
            if(frameData->at(currentRightFoot) - meanData->at(preRightFootMean) > 0.01) {
                if(currentRightStatus != -1) {
                    if(frameShiftFilterCount2 > 3 && currentRightStatus == 1) {
                        currentRightStatus = -1;
                        timeData->at(timeRightDown) = militime();
                        timeData->at(timeRightWindow) = float((timeData->at(timeRightDown) - timeData->at(timeRightUp)))/1000;
                    }
                    else { frameShiftFilterCount2 = frameShiftFilterCount2 + 1; }
                }
                else { frameShiftFilterCount2 = 0; }
            }
            // foot going up
            if(frameData->at(currentRightFoot) - meanData->at(preRightFootMean) < -0.01) {
                if(currentRightStatus != 1) {
                    if(frameShiftFilterCount2 > 3 && (currentRightStatus == 0 || currentRightStatus == -1)) {
                        if(timeData2->at(timeLock) == 1){
                            timeData2->at(timeBeta) = militime();
                            timeData2->at(timeLock) = 0;
                        }
                        else {
                            timeData2->at(timeLock) = 1;
                            timeData2->at(timeAlpha) = militime();
                        }
                        currentRightStatus = 1;
                        timeData->at(timeRightUp) = militime();
                    }
                    else { frameShiftFilterCount2 = frameShiftFilterCount2 + 1; }
                }
                else { frameShiftFilterCount2 = 0; }
            }
            // leg still
            if((-0.01 < (frameData->at(currentRightFoot) - meanData->at(preRightFootMean))) && ((frameData->at(currentRightFoot) - meanData->at(preRightFootMean) < 0.01))) {
                if(currentRightStatus != 0 && currentRightStatus != 2) {
                    if(frameShiftFilterCount2 > 6) {
                        // threshold to be tested
                        if(frameData->at(currentLeftLegHeight) - frameData->at(currentRightLegHeight) > 0.1 && currentLeftStatus == 1) {
                            currentRightStatus = 2;
                        }
                        else {
                            progressData->at(preRightStatus) = currentRightStatus; 
                            currentRightStatus = 0;
                        }
                    }
                    else { frameShiftFilterCount2 = frameShiftFilterCount2 + 1; }
                }
                else { frameShiftFilterCount2 = 0; }
            }
        }
    }

    void walk::calculateStepLength() {
        float maxSL = meanData->at(currentHeightMean) * 1.2;
        float leftFlexion = (180 - meanData->at(currentLeftHipAngMean)) > 90 ? 90 : (180 - meanData->at(currentLeftHipAngMean));
        float rightFlexion = (180 - meanData->at(currentRightHipAngMean)) > 90 ? 90 : (180 - meanData->at(currentRightHipAngMean));
        if(progressData->at(preLeftStatus) == 1 && currentLeftStatus == -1) {
            currentLeftStepLength = (maxSL / pow(90,(float)1/3)) * pow(leftFlexion,(float)1/3);
            currentStepLength = currentLeftStepLength;
        }
        if(progressData->at(preRightStatus) == 1 && currentRightStatus == -1) {
            currentRightStepLength = (maxSL / pow(90,(float)1/3)) * pow(rightFlexion,(float)1/3);
            currentStepLength = currentRightStepLength;
        }
        // if(currentLeftStatus == 0 || currentLeftStatus == 2) { currentLeftStepLength = 0; }
        // if(currentRightStatus == 0 || currentRightStatus == 2) { currentRightStepLength = 0; }
    }

    void walk::calculateStepRate() {
        if((currentLeftStatus == 0 || currentLeftStatus == 2) && (currentRightStatus == 0 || currentRightStatus == 2)) {
            fpmStopCount = fpmStopCount + 1;
            if(fpmStopCount > 15) {
                timeData2->at(timeAlpha) = 0;
                timeData2->at(timeBeta) = 0;
                timeData2->at(timeLock) = 0;
                currentStepRate = 0;
                timeData->at(timeLeftWindow) = 0;
                timeData->at(timeRightWindow) = 0;
            }
        }
        else { fpmStopCount = 0; }
        if (timeData->at(timeLeftWindow) != 0) currentLeftStepRate = 0.35 / timeData->at(timeLeftWindow); 
        else { currentLeftStepRate = 0; }
        if (timeData->at(timeRightWindow) != 0) currentRightStepRate = 0.35 / timeData->at(timeRightWindow);
        else { currentRightStepRate = 0; }

        float preStepRate = currentStepRate;
        currentStepRate = currentLeftStepRate > currentRightStepRate ? currentLeftStepRate : currentRightStepRate;

        if(timeData2->at(timeAlpha)!=0&&timeData2->at(timeBeta)!=0){
            currentStepRate = 1 / (float(abs(timeData2->at(timeAlpha) - timeData2->at(timeBeta)))/1000);
        }
        // currentVelocity = currentVelocity * 0.8 + (currentStepRate * currentStepLength) * 0.2;
        // if(currentVelocity > 10) currentVelocity = 10;
        // if(currentVelocity < 0.01) currentVelocity = 0;
        currentVelocity = currentStepRate * currentStepLength;
    }

    void walk::calculateProgress() {
        // left progress
        // if(progressData->at(currentProgressLeftStatus) == 2) {
        //     if(frameData->at(currentLeftFoot) - frameData->at(preLeftFoot) < 0.001) {
        //         progressData->at(currentProgressLeftStatus) = 0;
        //         progressData->at(totalLeftDistance) = 0;
        //         currentLeftProgress = 1;
        //     }
        // }
        if(progressData->at(preLeftStatus) == 0 && currentLeftStatus == 0) { 
            float increment = - (frameData->at(currentLeftFoot) - frameData->at(preLeftFoot));
            progressData->at(totalLeftDistance) = progressData->at(totalLeftDistance) + increment;
            currentLeftProgress = progressData->at(totalLeftDistance) / progressData->at(maxLeftDistance) * 0.5;
            currentLeftProgress = currentLeftProgress < 0.05 ? 0 : currentLeftProgress;
            // calculate realtime status
            if(currentLeftProgress > 0) {
                if(frameShiftFilterCount3 > 2){
                    progressData->at(realLeftStatus) = 1;
                }
                else{
                    frameShiftFilterCount3++;
                }
            }
        }
        if(progressData->at(currentProgressLeftStatus) == 0) {
            if(currentLeftStatus == 1) {
                float increment = - (frameData->at(currentLeftFoot) - frameData->at(preLeftFoot));
                if(increment < 0) {
                    progressData->at(maxLeftDistance) = progressData->at(totalLeftDistance);
                    progressData->at(totalLeftDistance) = 0;
                    progressData->at(currentProgressLeftStatus) = -1;
                    currentLeftProgress = 0.5;
                    progressData->at(realLeftStatus) = -1;
                }
                else {
                    progressData->at(totalLeftDistance) = progressData->at(totalLeftDistance) + increment;
                    currentLeftProgress = progressData->at(totalLeftDistance) / progressData->at(maxLeftDistance) * 0.5;
                    progressData->at(realLeftStatus) = 1;
                    currentLeftProgress = currentLeftProgress > 0.5 ? 0.5 : currentLeftProgress;
                }
            }
        }
        if(progressData->at(currentProgressLeftStatus) == -1) {
            float increment = (frameData->at(currentLeftFoot) - frameData->at(preLeftFoot));
            progressData->at(totalLeftDistance) = progressData->at(totalLeftDistance) + increment;
            currentLeftProgress = progressData->at(totalLeftDistance) / progressData->at(maxLeftDistance) * 0.5 + 0.5;
            currentLeftProgress = currentLeftProgress > 1 ? 1 : currentLeftProgress;
            if(currentLeftProgress > 0.8) {
                if(frameShiftFilterCount4 > 5){
                    progressData->at(realLeftStatus) = 0;
                    // progressData->at(currentProgressLeftStatus) = 2;
                    // progressData->at(totalLeftDistance) = 0;
                }
                else{
                    frameShiftFilterCount4++;
                }
            }
        }
        if(progressData->at(preLeftStatus) == -1 && currentLeftStatus != progressData->at(preLeftStatus)) {
            progressData->at(currentProgressLeftStatus) = 0;
            progressData->at(totalLeftDistance) = 0;
            currentLeftProgress = 1;
        }
    }

    void walk::calculateRealTime() {
        float increment = - (frameData->at(currentLeftFoot) - frameData->at(preLeftFoot));
        if(currentRealTimeLeftStatus == 0) {
            if(increment > 0.006) {
                if(frameShiftFilterCount5 > 5) {
                    currentRealTimeLeftStatus = 1;
                }
                else {
                    frameShiftFilterCount5++;
                }
            }
        }
        else if(currentRealTimeLeftStatus == 1) {
            if(increment < - 0) currentRealTimeLeftStatus = -1;
            // if(increment < - 0) {
            //     if(frameShiftFilterCount6 > 3) {
            //         currentRealTimeLeftStatus = -1;
            //     }
            //     else {
            //         frameShiftFilterCount6++;
            //     }
            // }
        }
        else if(currentRealTimeLeftStatus == -1) {
            if(abs(increment) <  0.001) {
                if(frameShiftFilterCount7 > 7) {
                    currentRealTimeLeftStatus = 0;
                }
                else {
                    frameShiftFilterCount7++;
                }
            }
        }
        // if(abs(increment) <  0.001) {
        //     if(frameShiftFilterCount8 > 10) {
        //         currentRealTimeLeftStatus = 0;
        //     }
        //     else {
        //         frameShiftFilterCount8++;
        //     }
        // }
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
        currentTurnAng = acos(cosr)*180/acos(-1);
        if(data->keypoints3D()->Get(12)->z() < data->keypoints3D()->Get(11)->z()) currentTurnAng = - currentTurnAng;
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