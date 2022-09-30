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
            vector<vector<float> > walkCycleLeft;
            vector<vector<float> > tempWalkCycleLeft;

            vector<vector<float> > walkCycleRight;
            vector<vector<float> > tempWalkCycleRight;
            bool cycleLeftFlag = true;
            bool cycleRightFlag = true;
            vector<int> cycleHelp{0,0};//辅助变量 存储 上一个数据 的左脚踝 右脚踝 legLeft legRight
            vector<int> lowConfidenceHelp{0,0};//辅助变量 记录所有对 confidence足够好的 上一个数据 为了给 confidence 从低到高这个循环的数据取舍
            vector<int> countHelp{0,0,0,0};// 四个数分别是 walkCycle 左1 左-1 右1 右-1 的个数 walkcycle里的四个数据
            vector<int> countTempHelp{0,0,0,0};//tempWalkCycle里的数据
            // -xyf end

        public:
            mockManager();
            ~mockManager();
            void process(const OsInput::InputMessage* data, vector<float>&);
            void recordWalkCycle(vector<float>, int, int);
            void mockWalkCycle(vector<float>&);
            void calculateWalkConfidence(const PoseData::Pose* data);
    };

    mockManager::mockManager() {}
    mockManager::~mockManager() {}
    
    void mockManager::process(const OsInput::InputMessage* data, vector<float>& walkOffset) {
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
        if(mock == 0) {
            if(walkConfidence != 0 ) {
                mock = 1;
                mockInit = true;
            }
        }
        if(mock == 1) {
            if(walkConfidence == 0) {
                shiftCount++;
                if(shiftCount > 20) {
                    mock = 0;
                }
                else {
                    if(walkCycleLeft.size() && walkCycleRight.size()){
                        mockWalkCycle(walkOffset);
                    }
                    
                }
            }
            else {
                shiftCount = 0;
                if(walkCycleLeft.size() && walkCycleRight.size()){
                    mockWalkCycle(walkOffset);
                }
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
                mockLeftCount = 0-1;//这里都补充了-1是因为出了这个判断后有个自增。这个-1是补偿。
                break;
            case 1:
                mockLeftCount = 0-1;
                break;
            case -1:
                mockLeftCount = countHelp.at(0)-1;
                break;            
            default:
                break;
            }

            switch (right)
            {
            case 0:
                mockRightCount = 0-1;
                break;
            case 1:
                mockRightCount = 0-1;
                break;
            case -1:
                mockRightCount = countHelp.at(2)-1;                                
                break;
            }
            
            mockInit = false;
        }
        
        mockLeftCount++;
        mockRightCount++;
        int leftCycleSerial = mockLeftCount % walkCycleLeft.size();
        int rightCycleSerial = mockRightCount % walkCycleRight.size();
        walkOffset.at(0) = walkCycleLeft.at(leftCycleSerial).at(0);
        walkOffset.at(1) = walkCycleRight.at(rightCycleSerial).at(1);
        walkOffset.at(2) = walkCycleLeft.at(leftCycleSerial).at(2);
        walkOffset.at(3) = walkCycleRight.at(rightCycleSerial).at(3);
        walkOffset.at(4) = walkCycleLeft.at(leftCycleSerial).at(4);
        walkOffset.at(5) = walkCycleRight.at(rightCycleSerial).at(5);
        walkOffset.at(6) = walkCycleLeft.at(leftCycleSerial).at(6);
        walkOffset.at(7) = walkCycleRight.at(rightCycleSerial).at(7);
        walkOffset.at(8) = walkCycleLeft.at(leftCycleSerial).at(8);
        walkOffset.at(9) = walkCycleRight.at(rightCycleSerial).at(9);

        walkOffset.at(11) = (walkCycleLeft.at(leftCycleSerial).at(11) + walkCycleRight.at(rightCycleSerial).at(11))/2;
        walkOffset.at(12) = (walkCycleLeft.at(leftCycleSerial).at(12) + walkCycleRight.at(rightCycleSerial).at(12))/2;
        walkOffset.at(13) = (walkCycleLeft.at(leftCycleSerial).at(13) + walkCycleRight.at(rightCycleSerial).at(13))/2;
    }

    void mockManager::recordWalkCycle(vector<float>  walkData, int legLeft, int legRight ){
        //左脚踝
        if(walkConfidence == 0){//说明一个正常的数据来了
            if((legLeft == 0 &&  tempWalkCycleLeft.size() && cycleHelp.at(0) == -1) || (legLeft == 1 &&  tempWalkCycleLeft.size() && cycleHelp.at(0) == -1) ) //说明一个循环到了 分两种情况
            {
                if(walkCycleLeft.size()){
                        // 说明 已经有一个有效的循环保存在walkCycle，先清空再把temp数据替换
                        walkCycleLeft.clear();
                        walkCycleLeft.assign(tempWalkCycleLeft.begin(),tempWalkCycleLeft.end());
                        countHelp.at(0) = countTempHelp.at(0);
                        countHelp.at(1) = countTempHelp.at(1);
                }
                else{
                    //walkCycle是空的 直接把temp数据加入
                    walkCycleLeft.assign(tempWalkCycleLeft.begin(),tempWalkCycleLeft.end());
                    countHelp.at(0) = countTempHelp.at(0);
                    countHelp.at(1) = countTempHelp.at(1);                    
                }
                //temp数据已经保存了 temp变量清空
                tempWalkCycleLeft.clear();
                countTempHelp.at(0) = 0;
                countTempHelp.at(1) = 0;
                

                if(legLeft == 1) { //如果是直接从上一个循环切换到新循环 新循环数据要保存
                    tempWalkCycleLeft.push_back(walkData);
                    countTempHelp.at(0)++;
                    cycleHelp.at(0) = legLeft;
                    lowConfidenceHelp.at(0) = legLeft;
                }

            }
            else if(legLeft == 0){
                //静止 数据跳过不需要保存 但是这个要保存
                lowConfidenceHelp.at(0) = legLeft;
                
            }
            else if(!cycleLeftFlag && lowConfidenceHelp.at(0) !=1 && legLeft == 1 ){
                cycleLeftFlag = true;
                tempWalkCycleLeft.push_back(walkData);
                countTempHelp.at(0)++;
                cycleHelp.at(0) = legLeft;
                lowConfidenceHelp.at(0) = legLeft;

            }
            else if(cycleLeftFlag){
                //正常的保存一个循环所需要的数据
                //目前 2的数据跳过不处理
                if(legLeft != 2){
                    tempWalkCycleLeft.push_back(walkData);
                    cycleHelp.at(0) = legLeft;
                    lowConfidenceHelp.at(0) = legLeft;
                    if(legLeft == 1){
                        countTempHelp.at(0)++;

                    } else if(legLeft == -1){
                        countTempHelp.at(1)++;
                    }
                }
            }
            else{
                lowConfidenceHelp.at(0) = legLeft;
            }

        }
        else{
            //非正常数据来了 目前无处理，请德涛补充。
            tempWalkCycleLeft.clear();
            cycleLeftFlag = false;
            countTempHelp.at(0) = 0;
            countTempHelp.at(1) = 0;
        }
        //右脚踝
         if(walkConfidence == 0){//说明一个正常的数据来了
            if((legRight == 0 &&  tempWalkCycleRight.size() && cycleHelp.at(1) == -1) || (legRight == 1 &&  tempWalkCycleRight.size() && cycleHelp.at(1) == -1) ) //说明一个循环到了 分两种情况
            {
                if(walkCycleRight.size()){
                        // 说明 已经有一个有效的循环保存在walkCycle，先清空再把temp数据替换
                        walkCycleRight.clear();
                        walkCycleRight.assign(tempWalkCycleRight.begin(),tempWalkCycleRight.end());
                        countHelp.at(2) = countTempHelp.at(2);
                        countHelp.at(3) = countTempHelp.at(3);                
                
                }
                else{
                    //walkCycle是空的 直接把temp数据加入
                    walkCycleRight.assign(tempWalkCycleRight.begin(),tempWalkCycleRight.end());
                    countHelp.at(2) = countTempHelp.at(2);
                    countHelp.at(3) = countTempHelp.at(3);                   
                
                }
                //temp数据已经保存了 temp变量清空
                tempWalkCycleRight.clear();
                countTempHelp.at(2) = 0;
                countTempHelp.at(3) = 0;
                if(legRight == 1) { //如果是直接从上一个循环切换到新循环 新循环数据要保存
                    tempWalkCycleRight.push_back(walkData);
                    countTempHelp.at(2)++;
                    cycleHelp.at(1) = legRight;
                    lowConfidenceHelp.at(1) = legRight;
                }

            }
            else if(legRight == 0){
                //静止 数据跳过不需要保存
                lowConfidenceHelp.at(1) = legRight;
                
            }
            else if(!cycleRightFlag && lowConfidenceHelp.at(1) !=1 && legRight ==1){
                cycleRightFlag = true;
                tempWalkCycleRight.push_back(walkData);
                countTempHelp.at(2)++;
                cycleHelp.at(1) = legRight;
                lowConfidenceHelp.at(1) = legRight;
            }
            else if(cycleRightFlag){
                //正常的保存一个循环所需要的数据
                if(legRight != 2){
                    tempWalkCycleRight.push_back(walkData);
                    cycleHelp.at(1) = legRight;
                    lowConfidenceHelp.at(1) = legRight;
                    if(legRight == 1){
                        countTempHelp.at(2)++;

                    } else if(legRight == -1){
                        countTempHelp.at(3)++;
                    }
                }
            }
            else{
                lowConfidenceHelp.at(1) = legRight;
            }

        }
        else {
            //非正常数据来了 目前无处理，请德涛补充。
            tempWalkCycleRight.clear();
            cycleRightFlag = false;
            countTempHelp.at(2) = 0;
            countTempHelp.at(3) = 0;
        }
}
}

#endif