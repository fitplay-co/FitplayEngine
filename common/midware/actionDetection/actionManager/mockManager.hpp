#ifndef MOCK_manager
#define MOCK_manager

#include "../actionDetectionData.hpp"
#include "inputMessage_generated.h"
#include "actionData_generated.h"

namespace actionDetection {

    // record latest walk api cycle with high confidence
    class mockManager {
        private:

            int mock = 0;
            bool mockInit = false;
            int mockLeftCount = 0;
            int mockRightCount = 0;
            int walkConfidence = 0;// 0 是 不遮挡 ？ 数据正常
            // -xyf start
            vector<vector<float> *> walkCycleLeft;
            vector<vector<float> *> tempWalkCycleLeft;

            vector<vector<float> *> walkCycleRight;
            vector<vector<float> *> tempWalkCycleRight;
            
            // -xyf end

        public:
            mockManager();
            ~mockManager();
            void process(const Input::InputMessage* data, vector<float>&);
            void recordWalkCycle(vector<float>, int, int);
            void mockWalkCycle(vector<float>&);
            void calculateWalkConfidence(const PoseData::Pose* data);
    };

    mockManager::mockManager() {}
    mockManager::~mockManager() {}
    
    void mockManager::process(const Input::InputMessage* data, vector<float>& walkOffset) {
        // walkOffset = { currentLeftStatus, currentRightStatus, currentLeftStepRate, currentRightStepRate,
        //             meanData->at(currentLeftHipAngMean), meanData->at(currentRightHipAngMean), currentLeftStepLength,
        //             currentRightStepLength, currentLeftProgress, currentRightProgress, currentTurnAng, currentStepRate,
        //             currentStepLength, currentVelocity, currentVelocityThreshold, currentRealTimeLeftStatus, currentRealTimeRightStatus};
        const PoseData::Pose* pose = data->pose();
        calculateWalkConfidence(pose);
        recordWalkCycle(walkOffset, walkOffset.at(0), walkOffset.at(1));
        // high confidence : 0
        // low confidence : -1
        static int shiftCount = 0;
        if(walkConfidence == 0 && walkOffset.at(0) == 0 && walkOffset.at(1) == 0) { 
            shiftCount = 0;
            mock = 0;
            mockInit = false;
            return ;
        }
        if(mock = 0) {
            if(walkConfidence != 0 ) {
                mock = 1;
                mockInit = true;
            }
        }
        if(mock = 1) {
            if(walkConfidence == 0) {
                shiftCount++;
                if(shiftCount > 20) {
                    mock = 0;
                }
                else {
                    mockWalkCycle(walkOffset);
                }
            }
            else {
                shiftCount = 0;
                mockWalkCycle(walkOffset);
            }
        }
    }

    void mockManager::calculateWalkConfidence(const PoseData::Pose* data) {
        int leftConfidence, rightConfidence;
        // left foot
        if(data->keypoints()->Get(27)->score()>0.7 && data->keypoints()->Get(31)->score()>0.7) leftConfidence = 1;
        else leftConfidence = 0;
        // right foot
        if(data->keypoints()->Get(28)->score()>0.7 && data->keypoints()->Get(32)->score()>0.7) rightConfidence = 1;
        else rightConfidence = 0;

        if(leftConfidence && rightConfidence) walkConfidence = 0;
        else walkConfidence = -1;
    }

    void mockManager::mockWalkCycle(vector<float>& walkOffset) {
        // walkOffset = { currentLeftStatus, currentRightStatus, currentLeftStepRate, currentRightStepRate,
        //             meanData->at(currentLeftHipAngMean), meanData->at(currentRightHipAngMean), currentLeftStepLength,
        //             currentRightStepLength, currentLeftProgress, currentRightProgress, currentTurnAng, currentStepRate,
        //             currentStepLength, currentVelocity, currentVelocityThreshold, currentRealTimeLeftStatus, currentRealTimeRightStatus};
        int left = walkOffset.at(0);
        int right = walkOffset.at(1);
        if(mockInit) {
            switch (left)
            {
            case 0:
                mockLeftCount = ;
                break;
            
            default:
                break;
            }

            switch (right)
            {
            case 0:
                mockRightCount = ;
                break;
            }
            
            mockInit = false;
        }
        
        mockLeftCount++;
        mockRightCount++;
        int leftCycleSerial = mockLeftCount % walkCycleLeft.size();
        int rightCycleSerial = mockRightCount % walkCycleRight.size();
        walkOffset.at(0) = walkCycleLeft.at(mockLeftCount)->at(0);
        walkOffset.at(1) = walkCycleRight.at(mockRightCount)->at(1);
        walkOffset.at(2) = walkCycleLeft.at(mockLeftCount)->at(2);
        walkOffset.at(3) = walkCycleRight.at(mockRightCount)->at(3);
        walkOffset.at(4) = walkCycleLeft.at(mockLeftCount)->at(4);
        walkOffset.at(5) = walkCycleRight.at(mockRightCount)->at(5);
        walkOffset.at(6) = walkCycleLeft.at(mockLeftCount)->at(6);
        walkOffset.at(7) = walkCycleRight.at(mockRightCount)->at(7);
        walkOffset.at(8) = walkCycleLeft.at(mockLeftCount)->at(8);
        walkOffset.at(9) = walkCycleRight.at(mockRightCount)->at(9);

        walkOffset.at(11) = (walkCycleLeft.at(mockLeftCount)->at(11) + walkCycleRight.at(mockRightCount)->at(11))/2;
        walkOffset.at(12) = (walkCycleLeft.at(mockLeftCount)->at(12) + walkCycleRight.at(mockRightCount)->at(12))/2;
        walkOffset.at(13) = (walkCycleLeft.at(mockLeftCount)->at(13) + walkCycleRight.at(mockRightCount)->at(13))/2;
    }

    void mockManager::recordWalkCycle(vector<float> walkData, int legLeft, int legRight ){
        //左脚踝
        if(walkConfidence == 0){//说明一个正常的数据来了
            if((legLeft == 0 &&  tempWalkCycleLeft.size() && tempWalkCycleLeft.back() == -1) || (legLeft == 1 &&  tempWalkCycleLeft.size() && tempWalkCycleLeft.back() == -1) ) //说明一个循环到了 分两种情况
            {
                if(walkCycleLeft.size()){
                        // 说明 已经有一个有效的循环保存在walkCycle，先清空再把temp数据替换
                        walkCycleLeft.clear();
                        walkCycleLeft.assign(tempWalkCycleLeft.begin(),tempWalkCycleLeft.end());
                }
                else{
                    //walkCycle是空的 直接把temp数据加入
                    walkCycleLeft.assign(tempWalkCycleLeft.begin(),tempWalkCycleLeft.end());
                }
                //temp数据已经保存了 temp变量清空
                tempWalkCycleLeft.clear();
                if(legLeft == 1) { //如果是直接从上一个循环切换到新循环 新循环数据要保存
                    tempWalkCycleLeft.push_back(walkData);
                }

            }
            else if(legLeft == 0){
                //静止 数据跳过不需要保存
                
            }
            else{
                //正常的保存一个循环所需要的数据
                tempWalkCycleLeft.push_back(walkData);
            }

        }
        else{
            //非正常数据来了 目前无处理，请德涛补充。
        }
        //右脚踝
         if(walkConfidence == 0){//说明一个正常的数据来了
            if((legRight == 0 &&  tempWalkCycleRight.size() && tempWalkCycleRight.back() == -1) || (legRight == 1 &&  tempWalkCycleRight.size() && tempWalkCycleRight.back() == -1) ) //说明一个循环到了 分两种情况
            {
                if(walkCycleRight.size()){
                        // 说明 已经有一个有效的循环保存在walkCycle，先清空再把temp数据替换
                        walkCycleRight.clear();
                        walkCycleRight.assign(tempWalkCycleRight.begin(),tempWalkCycleRight.end());
                }
                else{
                    //walkCycle是空的 直接把temp数据加入
                    walkCycleRight.assign(tempWalkCycleRight.begin(),tempWalkCycleRight.end());
                }
                //temp数据已经保存了 temp变量清空
                tempWalkCycleRight.clear();
                if(legRight == 1) { //如果是直接从上一个循环切换到新循环 新循环数据要保存
                    tempWalkCycleRight.push_back(walkData);
                }

            }
            else if(legRight == 0){
                //静止 数据跳过不需要保存
                
            }
            else{
                //正常的保存一个循环所需要的数据
                tempWalkCycleRight.push_back(walkData);
            }

        }
        else{
            //非正常数据来了 目前无处理，请德涛补充。
        }
}
}

#endif