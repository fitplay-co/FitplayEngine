#ifndef MOCK_manager
#define MOCK_manager

#include "../actionDetectionData.hpp"
#include "inputMessage_generated.h"
#include "actionData_generated.h"

namespace actionDetection {

    // record latest walk api cycle with high confidence
    class mockManager {
        private:
            int walkConfidence;

        public:
            mockManager();
            ~mockManager();
            void process(const Input::InputMessage* data);
            void recordWalkCycle(flatbuffers::Offset<actionData::Walk>, int);
            void calculateWalkConfidence(const PoseData::Pose* data);
    };

    mockManager::mockManager() {}
    mockManager::~mockManager() {}
    
    void mockManager::process(const Input::InputMessage* data) {
        if (data->type() == Input::MessageType::MessageType_Pose) {

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
    }
}

#endif