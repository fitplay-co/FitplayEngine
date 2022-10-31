#ifndef WALK_detection_manager
#define WALK_detection_manager

#include <vector>
#include "actionData_generated.h"
#include "midwareComponent/midwareComponent.hpp"
#include "euroFilter.hpp"
#include "walkDetectionManagerData.hpp"
#include "actionManagerUtility.hpp"

// #define useKneeAsDetectionPoint
#define walkDetectionThresholdTravel 0.005
#define walkDetectionThresholdStand 0.01
#ifdef useKneeAsDetectionPoint
    #define walkDetectionThresholdTravel 0.004
    #define walkDetectionThresholdStand 0.008
#endif

namespace actionwalk {
    class walk: public Midware::MidwareComponent {
        private:
            float walkDetectionThreshold;
            // calculated data
            std::shared_ptr<vector<float>> walkActionPoseData;
            std::shared_ptr<vector<float>> walkActionTimeData;
            // one euro filter parameters
            double frequency = 60, mincutoff = 0.85, beta = 1.2, dcutoff = 1.0;
            float euroFilterFrameCount = 0;
            std::shared_ptr<detection::OneEuroFilter> leftWalkEuroFilter;
            std::shared_ptr<detection::OneEuroFilter> rightWalkEuroFilter;
            // output API data
            float currentLeftWalkStatus = 0;
            float currentLeftStepRate = 0;
            float currentLeftStepLen = 0;
            float currentRightWalkStatus = 0;
            float currentRightStepRate = 0;
            float currentRightStepLen = 0;
            float currentStepRateBeta = 0;
            float currentStepLenBeta = 0;
            float currentVelocityBeta = 0;
            float currentVelocityBetaThreshold = 0;

            float configPlayerHeight = 0;
            float fpmStopCount = 0;

            flatbuffers::Offset<ActionData::Walk> flatbuffersOffset;
            vector<float> walkOffset;
        public:
            walk();
            ~ walk();
            bool process(const OsInput::InputMessage*, flatbuffers::FlatBufferBuilder&, float mode);
            void writeToFlatbuffers(ActionData::ActionBuilder&);
            void setPlayerHeight(float);
            vector<float> getWalkOffset() {return walkOffset; }
            void calculateFrame(const PoseData::Pose* data);
            void calculateMean();
            void calculateLeft();
            void calculateRight();
            void calculateStepLen();
            void calculateStepRate();
    };

    walk::walk(): MidwareComponent("walk") {
        walkActionPoseData = make_shared<vector<float>>(30);
        walkActionTimeData = make_shared<vector<float>>(10);
        leftWalkEuroFilter = make_shared<detection::OneEuroFilter>(frequency, mincutoff, beta, dcutoff);
        rightWalkEuroFilter = make_shared<detection::OneEuroFilter>(frequency, mincutoff, beta, dcutoff);
        walkActionTimeData->at(timeLock) = 0;
    }

    walk::~walk() {}

    bool walk::process(const OsInput::InputMessage* data, flatbuffers::FlatBufferBuilder& builder, float mode) {
        if (data->type() == OsInput::MessageType::MessageType_Pose) {
            const PoseData::Pose* pose = data->pose();

            if(mode == 1) walkDetectionThreshold = walkDetectionThresholdTravel;
            else if(mode == 0) walkDetectionThreshold = walkDetectionThresholdStand;

            calculateFrame(pose);
            calculateMean();
            calculateLeft();
            calculateRight();
            calculateStepLen();
            calculateStepRate();

            if(currentLeftWalkStatus == 0 && walkActionPoseData->at(preLeftWalkStatus) == 1) currentLeftWalkStatus = -1;
            if(currentRightWalkStatus == 0 && walkActionPoseData->at(preRightWalkStatus) == 1) currentRightWalkStatus = -1;

            walkOffset = { currentLeftWalkStatus, currentRightWalkStatus, currentLeftStepRate, currentRightStepRate,
                        walkActionPoseData->at(currentLeftHipAngMean), walkActionPoseData->at(currentRightHipAngMean), 
                        currentLeftStepLen, currentRightStepLen, 0, 0, 0, 
                        currentStepRateBeta, currentStepLenBeta, currentVelocityBeta, currentVelocityBetaThreshold, 0, 0};
        }
        return true;
    }

    void walk::writeToFlatbuffers(ActionData::ActionBuilder& actionBuilder) {
        actionBuilder.add_walk(flatbuffersOffset);
    }

    void walk::setPlayerHeight(float height) {
        configPlayerHeight = height * 0.01;
    }

    void walk::calculateFrame(const PoseData::Pose* data) {
        walkActionPoseData->at(preLeftWalkPose) = walkActionPoseData->at(currentLeftWalkPose);
        walkActionPoseData->at(preRightWalkPose) = walkActionPoseData->at(currentRightWalkPose);
        walkActionPoseData->at(currentLeftWalkPose) = data->keypoints()->Get(27)->y() + data->keypoints()->Get(31)->y();
        walkActionPoseData->at(currentRightWalkPose) = data->keypoints()->Get(28)->y() + data->keypoints()->Get(32)->y();
        #ifdef useKneeAsDetectionPoint
            walkActionPoseData->at(currentLeftWalkPose) = data->keypoints()->Get(27)->y() + data->keypoints()->Get(31)->y() + data->keypoints()->Get(25)->y();
            walkActionPoseData->at(currentRightWalkPose) = data->keypoints()->Get(28)->y() + data->keypoints()->Get(32)->y() + data->keypoints()->Get(26)->y();
        #endif
        walkActionPoseData->at(currentLeftHipAng) = actionManager::calVecAngle(data, 11, 23, 25, 1);
        walkActionPoseData->at(currentRightHipAng) = actionManager::calVecAngle(data, 12, 24, 26, 1);
        float leftThighHeight = data->keypoints3D()->Get(23)->y() - data->keypoints3D()->Get(25)->y();
        float rightThighHeight = data->keypoints3D()->Get(24)->y() - data->keypoints3D()->Get(26)->y();
        walkActionPoseData->at(currentThighHeight) = std::max(leftThighHeight, rightThighHeight);
        walkActionPoseData->at(currentLeftLegHeight) = data->keypoints3D()->Get(27)->y() - data->keypoints3D()->Get(23)->y();
        walkActionPoseData->at(currentRightLegHeight) = data->keypoints3D()->Get(28)->y() - data->keypoints3D()->Get(24)->y();
    }

    void walk::calculateMean() {
        // euro filter
        double timestamp = euroFilterFrameCount/frequency;
        euroFilterFrameCount = euroFilterFrameCount + 1;
        double leftWalkPoseFiltered = leftWalkEuroFilter->filter(double(walkActionPoseData->at(currentLeftWalkPose)), timestamp);
        double rightWalkPoseFiltered = rightWalkEuroFilter->filter(double(walkActionPoseData->at(currentRightWalkPose)), timestamp);
        walkActionPoseData->at(preLeftWalkPoseMean) = walkActionPoseData->at(currentLeftWalkPoseMean);
        walkActionPoseData->at(currentLeftWalkPoseMean) = float(leftWalkPoseFiltered);
        walkActionPoseData->at(preRightWalkPoseMean) = walkActionPoseData->at(currentRightWalkPoseMean);
        walkActionPoseData->at(currentRightWalkPoseMean) = float(rightWalkPoseFiltered);

        // low pass filter
        // walkActionPoseData->at(currentLeftFootMean) = walkActionPoseData->at(currentLeftFootMean) * 0.85 + walkActionPoseData->at(currentLeftFoot) * 0.15;
        // walkActionPoseData->at(currentRightFootMean) = walkActionPoseData->at(currentRightFootMean) * 0.85 + walkActionPoseData->at(currentRightFoot) * 0.15;
        walkActionPoseData->at(currentLeftHipAngMean) = walkActionPoseData->at(currentLeftHipAngMean) * 0.8 + walkActionPoseData->at(currentLeftHipAng) * 0.2;
        walkActionPoseData->at(currentRightHipAngMean) = walkActionPoseData->at(currentRightHipAngMean) * 0.8 + walkActionPoseData->at(currentRightHipAng) * 0.2;
        walkActionPoseData->at(currentThighHeightMean) = (walkActionPoseData->at(currentThighHeightMean) * 0.9 + walkActionPoseData->at(currentThighHeight) * 0.1) * 0.79 + 0.15;
        walkActionPoseData->at(currentLegHeightMean) = walkActionPoseData->at(currentThighHeightMean) * 2.004;
        walkActionPoseData->at(currentHeightMean) = walkActionPoseData->at(currentThighHeightMean) / 0.245;
    }

    void walk::calculateLeft() {
        walkActionPoseData->at(preLeftWalkStatus) = currentLeftWalkStatus;
        static int leftFrameShiftFilterCount = 0;
        // for left leg
        // check leg length for leg hanging case
        if(currentLeftWalkStatus == 2) {
            if(walkActionPoseData->at(currentRightLegHeight) - walkActionPoseData->at(currentLeftLegHeight) < 0.1) {
                currentLeftWalkStatus = 0;
            }
        }
        else {
            // foot reaches its peak and going down
            if(walkActionPoseData->at(currentLeftWalkPose) - walkActionPoseData->at(preLeftWalkPoseMean) > walkDetectionThreshold) {
                if(currentLeftWalkStatus != -1) {
                    if(leftFrameShiftFilterCount > 3 && currentLeftWalkStatus == 1) {
                        currentLeftWalkStatus = -1;
                        walkActionTimeData->at(timeLeftDown) = actionManager::militime(); 
                        walkActionTimeData->at(timeLeftWindow) = float((walkActionTimeData->at(timeLeftDown) - walkActionTimeData->at(timeLeftUp)))/1000;
                    }
                    else { leftFrameShiftFilterCount = leftFrameShiftFilterCount + 1; }
                }
                else { leftFrameShiftFilterCount = 0; }
            }
            // foot going up
            if(walkActionPoseData->at(currentLeftWalkPose) - walkActionPoseData->at(preLeftWalkPoseMean) < -walkDetectionThreshold) {
                if(currentLeftWalkStatus != 1) {
                    if(leftFrameShiftFilterCount > 3 && (currentLeftWalkStatus == 0 || currentLeftWalkStatus == -1)) {
                        if(walkActionTimeData->at(timeLock) == 1){
                            walkActionTimeData->at(timeBeta) = actionManager::militime();
                            walkActionTimeData->at(timeLock) = 0;
                        }
                        else {
                            walkActionTimeData->at(timeLock) = 1;
                            walkActionTimeData->at(timeAlpha) = actionManager::militime();
                        }
                        currentLeftWalkStatus = 1;
                        walkActionTimeData->at(timeLeftUp) = actionManager::militime();
                    }
                    else { leftFrameShiftFilterCount = leftFrameShiftFilterCount + 1; }
                }
                else { leftFrameShiftFilterCount = 0; }
            }
            // leg still
            if((-walkDetectionThreshold < (walkActionPoseData->at(currentLeftWalkPose) - walkActionPoseData->at(preLeftWalkPoseMean))) && ((walkActionPoseData->at(currentLeftWalkPose) - walkActionPoseData->at(preLeftWalkPoseMean) < walkDetectionThreshold))) {
                if(currentLeftWalkStatus != 0 && currentLeftWalkStatus != 2) {
                    if(leftFrameShiftFilterCount > 6) {
                        // threshold to be tested
                        if(walkActionPoseData->at(currentRightLegHeight) - walkActionPoseData->at(currentLeftLegHeight) > 0.1 && currentLeftWalkStatus == 1) {
                            currentLeftWalkStatus = 2;
                        }
                        else {
                            walkActionPoseData->at(preLeftWalkStatus) = currentLeftWalkStatus; 
                            currentLeftWalkStatus = 0;
                        }
                    }
                    else { leftFrameShiftFilterCount = leftFrameShiftFilterCount + 1; }
                }
                else { leftFrameShiftFilterCount = 0; }
            }
        }
    }

    void walk::calculateRight() {
        walkActionPoseData->at(preRightWalkStatus) = currentRightWalkStatus;
        static int rightFrameShiftFilterCount = 0;
        // for right leg
        // check leg length for leg hanging case
        if(currentRightWalkStatus == 2) {
            if(walkActionPoseData->at(currentLeftLegHeight) - walkActionPoseData->at(currentRightLegHeight) < 0.1) {
                currentRightWalkStatus = 0;
            }
        }
        else {
            // foot reaches its peak and going down
            if(walkActionPoseData->at(currentRightWalkPose) - walkActionPoseData->at(preRightWalkPoseMean) > walkDetectionThreshold) {
                if(currentRightWalkStatus != -1) {
                    if(rightFrameShiftFilterCount > 3 && currentRightWalkStatus == 1) {
                        currentRightWalkStatus = -1;
                        walkActionTimeData->at(timeRightDown) = actionManager::militime();
                        walkActionTimeData->at(timeRightWindow) = float((walkActionTimeData->at(timeRightDown) - walkActionTimeData->at(timeRightUp)))/1000;
                    }
                    else { rightFrameShiftFilterCount = rightFrameShiftFilterCount + 1; }
                }
                else { rightFrameShiftFilterCount = 0; }
            }
            // foot going up
            if(walkActionPoseData->at(currentRightWalkPose) - walkActionPoseData->at(preRightWalkPoseMean) < -walkDetectionThreshold) {
                if(currentRightWalkStatus != 1) {
                    if(rightFrameShiftFilterCount > 3 && (currentRightWalkStatus == 0 || currentRightWalkStatus == -1)) {
                        if(walkActionTimeData->at(timeLock) == 1){
                            walkActionTimeData->at(timeBeta) = actionManager::militime();
                            walkActionTimeData->at(timeLock) = 0;
                        }
                        else {
                            walkActionTimeData->at(timeLock) = 1;
                            walkActionTimeData->at(timeAlpha) = actionManager::militime();
                        }
                        currentRightWalkStatus = 1;
                        walkActionTimeData->at(timeRightUp) = actionManager::militime();
                    }
                    else { rightFrameShiftFilterCount = rightFrameShiftFilterCount + 1; }
                }
                else { rightFrameShiftFilterCount = 0; }
            }
            // leg still
            if((-walkDetectionThreshold < (walkActionPoseData->at(currentRightWalkPose) - walkActionPoseData->at(preRightWalkPoseMean))) && ((walkActionPoseData->at(currentRightWalkPose) - walkActionPoseData->at(preRightWalkPoseMean) < walkDetectionThreshold))) {
                if(currentRightWalkStatus != 0 && currentRightWalkStatus != 2) {
                    if(rightFrameShiftFilterCount > 6) {
                        // threshold to be tested
                        if(walkActionPoseData->at(currentLeftLegHeight) - walkActionPoseData->at(currentRightLegHeight) > 0.1 && currentRightWalkStatus == 1) {
                            currentRightWalkStatus = 2;
                        }
                        else {
                            walkActionPoseData->at(preRightWalkStatus) = currentRightWalkStatus; 
                            currentRightWalkStatus = 0;
                        }
                    }
                    else { rightFrameShiftFilterCount = rightFrameShiftFilterCount + 1; }
                }
                else { rightFrameShiftFilterCount = 0; }
            }
        }
    }

    void walk::calculateStepLen() {
        float maxSL = (configPlayerHeight==0)? walkActionPoseData->at(currentHeightMean) * 1.2 : configPlayerHeight * 1.2;//configHeight 是 身高？
        float leftFlexion = (180 - walkActionPoseData->at(currentLeftHipAngMean)) > 90 ? 90 : (180 - walkActionPoseData->at(currentLeftHipAngMean));
        float rightFlexion = (180 - walkActionPoseData->at(currentRightHipAngMean)) > 90 ? 90 : (180 - walkActionPoseData->at(currentRightHipAngMean));
        if(walkActionPoseData->at(preLeftWalkStatus) == 1 && currentLeftWalkStatus == -1) {
            currentLeftStepLen = (maxSL / pow(90,(float)1/3)) * pow(leftFlexion,(float)1/3);
            currentLeftStepLen = maxSL * sin(leftFlexion * acos(-1) / 180);
            currentStepLenBeta = currentLeftStepLen;
        }
        if(walkActionPoseData->at(preRightWalkStatus) == 1 && currentRightWalkStatus == -1) {
            currentRightStepLen = (maxSL / pow(90,(float)1/3)) * pow(rightFlexion,(float)1/3);
            currentRightStepLen = maxSL * sin(rightFlexion * acos(-1) / 180);
            currentStepLenBeta = currentRightStepLen;
        }
        // if(currentLeftStatus == 0 || currentLeftStatus == 2) { currentLeftStepLength = 0; }
        // if(currentRightStatus == 0 || currentRightStatus == 2) { currentRightStepLength = 0; }
    }

    void walk::calculateStepRate() {
        if((currentLeftWalkStatus == 0 || currentLeftWalkStatus == 2) && (currentRightWalkStatus == 0 || currentRightWalkStatus == 2)) {
            fpmStopCount = fpmStopCount + 1;
            if(fpmStopCount > 15) {
                walkActionTimeData->at(timeAlpha) = 0;
                walkActionTimeData->at(timeBeta) = 0;
                walkActionTimeData->at(timeLock) = 0;
                currentStepRateBeta = 0;
                walkActionTimeData->at(timeLeftWindow) = 0;
                walkActionTimeData->at(timeRightWindow) = 0;
            }
        }
        else { fpmStopCount = 0; }
        if (walkActionTimeData->at(timeLeftWindow) != 0) currentLeftStepRate = 0.35 / walkActionTimeData->at(timeLeftWindow); 
        else { currentLeftStepRate = 0; }
        if (walkActionTimeData->at(timeRightWindow) != 0) currentRightStepRate = 0.35 / walkActionTimeData->at(timeRightWindow);
        else { currentRightStepRate = 0; }

        float preStepRate = currentStepRateBeta;
        currentStepRateBeta = currentLeftStepRate > currentRightStepRate ? currentLeftStepRate : currentRightStepRate;

        if(walkActionTimeData->at(timeAlpha)!=0&&walkActionTimeData->at(timeBeta)!=0){
            currentStepRateBeta = 1 / (float(abs(walkActionTimeData->at(timeAlpha) - walkActionTimeData->at(timeBeta)))/1000);
        }
        if(currentStepRateBeta > 10) currentStepRateBeta = 10;
        // currentVelocity = currentVelocity * 0.8 + (currentStepRate * currentStepLength) * 0.2;
        // if(currentVelocity > 10) currentVelocity = 10;
        // if(currentVelocity < 0.01) currentVelocity = 0;
        currentVelocityBeta = currentStepRateBeta * currentStepLenBeta;
        if(currentLeftWalkStatus == 0 && currentRightWalkStatus == 0) {
            currentVelocityBeta = 0;
        }
        currentVelocityBetaThreshold = (configPlayerHeight==0)? sqrt(2.401 * walkActionPoseData->at(currentHeightMean)) : sqrt(2.401 * configPlayerHeight);
    }
}

#endif