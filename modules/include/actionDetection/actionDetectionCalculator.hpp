#ifndef ACTION_detection_calculator
#define ACTION_detection_calculator

#include "actionData_generated.h"
#include "inputMessage_generated.h"
#include "actionDetectionData.hpp"
#include "euroFilter.hpp"

namespace actionDetection {

    class actionDetectionCalculator {
        private:
            vector<subscribeAngle> *subscribeAngleList;
            vector<subscribeDistance> *subscribeDistanceList;

            // store feature velocity data from manager for calculation
            vector<featureVelocity> *featureVelocityList;
            vector<featureVelocityData> *featureVelocityDataList;
            vector<float> *featureVelocityTargetList;

            vector<float> *subscribeAngleResult;
            vector<float> *subscribeDistanceResult;
            vector<float> *featureVelocityResult;

        public:
            actionDetectionCalculator();
            ~actionDetectionCalculator();
            bool process(const Input::InputMessage*);
            int addSubscribeAngle(subscribeAngle*);
            int addSubscribeDistance(subscribeDistance*);
            int addFeatureVelocity(featureVelocity*);
            vector<float>* getSubscribeAngle();
            vector<float>* getSubscribeDistance();
            vector<float>* getFeatureVelocity();
            void calculateSubscribe(const PoseData::Pose* data);
            void calculateFeature(const PoseData::Pose* data);

            void calculateFeatureTarget(const PoseData::Pose* data);
            float calculateVecAngle(const PoseData::Pose* data, int num1, int num2, int num3, int config);
            float calculateDistance(const PoseData::Pose* data, int num1, int num2, int config);
    };

    actionDetectionCalculator::actionDetectionCalculator() {
        subscribeAngleList = new vector<subscribeAngle>;
        subscribeDistanceList = new vector<subscribeDistance>;
        featureVelocityList = new vector<featureVelocity>;
        featureVelocityDataList = new vector<featureVelocityData>;
        featureVelocityTargetList = new vector<float>(10,0);

        subscribeAngleResult = new vector<float>;
        subscribeDistanceResult = new vector<float>;
        featureVelocityResult = new vector<float>;
    }
    
    actionDetectionCalculator::~actionDetectionCalculator() {}

    bool actionDetectionCalculator::process(const Input::InputMessage* data) {
        if (data->type() == Input::MessageType::MessageType_Pose) {
            const PoseData::Pose* pose = data->pose();
            calculateSubscribe(pose);
            calculateFeature(pose);
        }
        return true;
    }

    void actionDetectionCalculator::calculateSubscribe(const PoseData::Pose* data) {
        for(int i = 0; i < subscribeAngleList->size(); i++) {
            float ang = calculateVecAngle(data, subscribeAngleList->at(i).num1, subscribeAngleList->at(i).num2, 
                                        subscribeAngleList->at(i).num3, subscribeAngleList->at(i).config);
            subscribeAngleResult->at(i) = ang;                    
        }

        for(int i = 0; i < subscribeDistanceList->size(); i++) {
            float dis = calculateDistance(data, subscribeDistanceList->at(i).num1, subscribeDistanceList->at(i).num2,
                                        subscribeDistanceList->at(i).config);
            subscribeDistanceResult->at(i) = dis;
        }
    }

    void actionDetectionCalculator::calculateFeature(const PoseData::Pose* data) {
        calculateFeatureTarget(data);
        for(int i = 0; i < featureVelocityList->size(); i++) {

            float currentTarget = featureVelocityDataList->at(i).currentTarget;
            float preTarget = featureVelocityDataList->at(i).preTarget;
            float currentTargetMean = featureVelocityDataList->at(i).currentTargetMean;
            float preTargetMean = featureVelocityDataList->at(i).preTargetMean;
            float preStatus = featureVelocityDataList->at(i).preStatus;
            float frameShiftCount = featureVelocityDataList->at(i).frameShiftCount;

            float status = featureVelocityResult->at(i);

            float threshold1 = featureVelocityList->at(i).threshold1;
            float threshold2 = featureVelocityList->at(i).threshold2;
            float filterType = featureVelocityList->at(i).filterType;

            if(filterType == 1) {
                currentTargetMean = currentTargetMean * 0.8 + currentTarget * 0.2;
            }
            else if(filterType == 2) {
                // one-euro filter
            }

            preTarget = currentTarget;
            preTargetMean = currentTargetMean;
            preStatus = status;

            if(currentTarget - preTargetMean > threshold1) {
                if(status != -1) {
                    if(frameShiftCount > 3 && status == 1) {
                        status = -1;
                    }
                    else frameShiftCount = frameShiftCount + 1;
                }
                else frameShiftCount = 0;
            }

            else if(currentTarget - preTargetMean < threshold2) {
                if(status != 1) {
                    if(frameShiftCount > 3 && (status == -1 || status == 0)) {
                        status = 1;
                    }
                    else frameShiftCount = frameShiftCount + 1;
                }
                else frameShiftCount = 0;
            }

            else {
                if(status != 0) {
                    if(frameShiftCount > 5) {
                        status = 0;
                    }
                    else frameShiftCount = frameShiftCount + 1;
                }
                else frameShiftCount = 0;
            }

            featureVelocityDataList->at(i).currentTargetMean = currentTargetMean;
            featureVelocityDataList->at(i).preTargetMean = preTargetMean;
            featureVelocityDataList->at(i).preStatus = preStatus;
            featureVelocityDataList->at(i).frameShiftCount = frameShiftCount;

            featureVelocityResult->at(i) = status;
        }
    }

    vector<float>* actionDetectionCalculator::getSubscribeAngle() {
        return subscribeAngleResult;
    }

    vector<float>* actionDetectionCalculator::getSubscribeDistance() {
        return subscribeDistanceResult;
    }

    vector<float>* actionDetectionCalculator::getFeatureVelocity() {
        return featureVelocityResult;
    }

    int actionDetectionCalculator::addSubscribeAngle(subscribeAngle* newSubscribe) {
        for(int i = 0; i < subscribeAngleList->size(); i++) {
            if(subscribeAngleList->at(i).num1 != newSubscribe->num1) break;
            if(subscribeAngleList->at(i).num2 != newSubscribe->num2) break;
            if(subscribeAngleList->at(i).num3 != newSubscribe->num3) break;
            if(subscribeAngleList->at(i).config != newSubscribe->config) break;
            return i;
        }
        subscribeAngleList->push_back(*newSubscribe);
        subscribeAngleResult->push_back(0);
        return subscribeAngleList->size() - 1;
    }

    int actionDetectionCalculator::addSubscribeDistance(subscribeDistance* newSubscribe) {
        subscribeDistanceList->push_back(*newSubscribe);
        subscribeDistanceResult->push_back(0);
        return subscribeDistanceList->size() - 1;
    }

    int actionDetectionCalculator::addFeatureVelocity(featureVelocity* newfeature) {
        featureVelocityList->push_back(*newfeature);
        featureVelocityData velocityData = {};
        featureVelocityDataList->push_back(velocityData);
        featureVelocityResult->push_back(0);
        return subscribeDistanceList->size() - 1;
    }

    // provide customization for feature target
    void actionDetectionCalculator::calculateFeatureTarget(const PoseData::Pose* data) {
        featureVelocityTargetList->at(0) = data->keypoints()->Get(12)->y();
        for(int i = 0; i < featureVelocityDataList->size(); i++) {
            featureVelocityDataList->at(i).currentTarget = featureVelocityTargetList->at(featureVelocityList->at(i).target);
        }
    }

    float actionDetectionCalculator::calculateVecAngle(const PoseData::Pose* data, int num1, int num2, int num3, int config) {
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

    float actionDetectionCalculator::calculateDistance(const PoseData::Pose* data, int num1, int num2, int config) {
        float vec[2][3];
        int dis_points[2] = {num1, num2};
        for (int i = 0; i < 2; i++) {
            vec[i][0] = data->keypoints3D()->Get(dis_points[i])->x();
            vec[i][1] = data->keypoints3D()->Get(dis_points[i])->y();
            vec[i][2] = data->keypoints3D()->Get(dis_points[i])->z();
        }
        switch (config) {
            case 1:
                return vec[0][0]-vec[1][0];
            
            case 2:
                return vec[0][1]-vec[1][1];
            
            case 3:
                return vec[0][2]-vec[1][2];
            
            default:
                return sqrt(pow(vec[0][0]-vec[1][0],2) + pow(vec[0][1]-vec[1][1],2) + pow(vec[0][2]-vec[1][2],2));
        }
    }
}

#endif