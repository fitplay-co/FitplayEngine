#ifndef MOCK_manager
#define MOCK_manager

#include "../actionDetectionData.hpp"
#include "inputMessage_generated.h"
#include "actionData_generated.h"

namespace actionDetection {

    // record latest walk api cycle with high confidence
    class mockManager {
        private:

            int walkConfidence;// 0 是 不遮挡 ？ 数据正常
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
            void recordWalkCycle(vector<float>*, int, int);
            void calculateWalkConfidence(const PoseData::Pose* data);
    };

    mockManager::mockManager() {}
    mockManager::~mockManager() {}
    
    void mockManager::process(const Input::InputMessage* data, vector<float>& walkOffset) {
        // int currentLeftStatus = walkOffset.at(0);
        // int currentRightStatus = walkOffset.at(1);
        recordWalkCycle(&walkOffset, walkOffset.at(0), walkOffset.at(1));
    }

    void mockManager::calculateWalkConfidence(const PoseData::Pose* data) {
        int leftConfidence, rightConfidence;
        // left foot
        if(data->keypoints()->Get(27)->score()>0.7 && data->keypoints()->Get(31)->score()>0.7) leftConfidence = 1;
        else leftConfidence = 0;
        // right foot
        if(data->keypoints()->Get(28)->score()>0.7 && data->keypoints()->Get(32)->score()>0.7) rightConfidence = 1;
        else rightConfidence = 0;
    }

    void mockManager::recordWalkCycle(vector<float> * walkData, int legLeft, int legRight ){
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