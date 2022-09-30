#ifndef ACTION_detection_manager
#define ACTION_detection_manager

#include "actionManager/mockManager.hpp"
#include "actionDetectionCalculator.hpp"
#include "walkDetection.hpp"

namespace actionDetection {

    class actionDetectionManager {
        private:

            mockManager mockInstance;
            actionwalk::walk walkInstance;
            actionDetectionCalculator calculatorInstance;

            bool init = false;

            flatbuffers::Offset<ActionData::Stand> standFlatbuffersOffset;
            flatbuffers::Offset<ActionData::Walk> walkFlatbuffersOffset;

            // stand travel mode shift params
            // 0 : stand
            // 1 : travel
            float mode = 0;
            float currentLeftStatus = 0;
            float currentRightStatus = 0;
            int modeLeftShiftStatus = 0;
            int modeRightShiftStatus = 0;
            int modeShiftCount = 0;
            void resetModeStatusBit() {
                modeLeftShiftStatus = 0;
                modeRightShiftStatus = 0;
                modeShiftCount = 0;
            }

        public:
            actionDetectionManager();
            ~actionDetectionManager();
            void addActionDetectionCalculator();
            void calculateMode(flatbuffers::FlatBufferBuilder& builder);
            void calculateMock(const OsInput::InputMessage* data, flatbuffers::FlatBufferBuilder& builder);
            bool process(const OsInput::InputMessage* data, flatbuffers::FlatBufferBuilder& builder);
            void writeToFlatbuffers(ActionData::ActionBuilder&);
    };

    actionDetectionManager::actionDetectionManager() {};
    actionDetectionManager::~actionDetectionManager() {};

    bool actionDetectionManager::process(const OsInput::InputMessage* data, flatbuffers::FlatBufferBuilder& builder) {
        if (data->type() == OsInput::MessageType::MessageType_Pose) {

            if(init == false) {
                addActionDetectionCalculator();
                init = true;
            }

            calculatorInstance.process(data);
            walkInstance.process(data, builder);
            calculateMode(builder);
            calculateMock(data, builder);

            return true;
        }
        if (data->type() == OsInput::MessageType::MessageType_ApplicationControl) {
            const ApplicationControl::Control* control = data->control(); 
            if (control->featureId()->str() == "action_detection") {
                if (control->action()->str() == "set_player") {
                    walkInstance.setPlayer(control->data()->height());
                }
            }
        }
        return false;
    }

    void actionDetectionManager::addActionDetectionCalculator() {
        // subscribe angle
        struct subscribeAngle test = { 12,24,26,1 };
        calculatorInstance.addSubscribeAngle(&test);

        // subscribe distance
        struct subscribeDistance kneeDis = { 25,26,1 };
        struct subscribeDistance footDis = { 27,28,1 };
        struct subscribeDistance leftDis = { 25,27,1 };
        struct subscribeDistance rightDis = { 26,28,1 };
        struct subscribeDistance shoulderDis = { 11,12,1 };
        calculatorInstance.addSubscribeDistance(&kneeDis);
        calculatorInstance.addSubscribeDistance(&footDis);
        calculatorInstance.addSubscribeDistance(&leftDis);
        calculatorInstance.addSubscribeDistance(&rightDis);
        calculatorInstance.addSubscribeDistance(&shoulderDis);

        // feature velocity
        struct featureVelocity leftFootConstraint = { 1,1,0.04,-0.04,3,3,5 };
        struct featureVelocity rightFootConstraint = { 2,1,0.04,-0.04,3,3,5 };
        calculatorInstance.addFeatureVelocity(&leftFootConstraint);
        calculatorInstance.addFeatureVelocity(&rightFootConstraint);
    }

    void actionDetectionManager::calculateMode(flatbuffers::FlatBufferBuilder& builder) {
        static int modeShiftCount2 = 0;
        float temp = calculatorInstance.getSubscribeAngle()->at(0);
        float kneeDis = calculatorInstance.getSubscribeDistance()->at(0);
        float footDis = calculatorInstance.getSubscribeDistance()->at(1);
        float leftDis = calculatorInstance.getSubscribeDistance()->at(2);
        float rightDis = calculatorInstance.getSubscribeDistance()->at(3);
        float shoulderDis = calculatorInstance.getSubscribeDistance()->at(4);
        float leftFootConstraint = calculatorInstance.getFeatureVelocity()->at(0);
        float rightFootConstraint = calculatorInstance.getFeatureVelocity()->at(1);
        vector<float> walkOffset = walkInstance.getWalkOffset();
        float preLeftStatus = currentLeftStatus;
        float preRightStatus = currentRightStatus;
        shoulderDis = abs(shoulderDis);
        currentLeftStatus = walkOffset.at(0);
        currentRightStatus = walkOffset.at(1);

        if(mode == 0) {
            if(leftFootConstraint == 0 && rightFootConstraint == 0) {
                if(abs(kneeDis) < (shoulderDis + 0.15) && abs(footDis) < (shoulderDis + 0.1) && abs(leftDis) < 0.15 && abs(rightDis) < 0.15 ) {
                    switch (modeLeftShiftStatus)
                    {
                    case 0:
                        if(preLeftStatus != 1 && currentLeftStatus == 1) modeLeftShiftStatus = 1;
                        break;
                    case 1:
                        if(preLeftStatus == 1 && currentLeftStatus == -1) modeLeftShiftStatus = 2;
                        break;
                    default:
                        break;
                    }

                    switch (modeRightShiftStatus)
                    {
                    case 0:
                        if(preRightStatus != 1 && currentRightStatus == 1) modeRightShiftStatus = 1;
                        break;
                    case 1:
                        if(preRightStatus == 1 && currentRightStatus == -1) modeRightShiftStatus = 2;
                        break;
                    default:
                        break;
                    }

                    if(modeLeftShiftStatus == 2 && modeRightShiftStatus == 2) {
                        mode = 1;
                        resetModeStatusBit();
                    }
                    if(modeLeftShiftStatus == 2 && modeRightShiftStatus == 1) {
                        mode = 1;
                        resetModeStatusBit();
                    }
                    if(modeLeftShiftStatus == 1 && modeRightShiftStatus == 2) {
                        mode = 1;
                        resetModeStatusBit();
                    }
                    if(modeLeftShiftStatus == 1 || modeRightShiftStatus == 1) {
                        if(modeShiftCount > 80) {
                            resetModeStatusBit();
                        }
                        else modeShiftCount = modeShiftCount + 1;
                    }
                    else modeShiftCount = 0;
                }
                else {
                    resetModeStatusBit();
                }
            }
            else {
                resetModeStatusBit();
            }
        }
        else if(mode == 1) {
            if((currentLeftStatus == 0 && currentRightStatus == 0) || currentLeftStatus == 2 || currentRightStatus == 2) {
                if(modeShiftCount > 120) {
                    mode = 0;
                    modeShiftCount = 0;
                }
                else modeShiftCount = modeShiftCount + 1;
            }
            else modeShiftCount = 0;

            if(abs(footDis) > (shoulderDis + 0.15)) {
                mode = 0;
                resetModeStatusBit();
            }
        }
        standFlatbuffersOffset = ActionData::CreateStand(builder, mode);
    }

    void actionDetectionManager::calculateMock(const OsInput::InputMessage* data, flatbuffers::FlatBufferBuilder& builder) {
        vector<float> walkOffset = walkInstance.getWalkOffset();

         mockInstance.process(data, walkOffset);

        // walkOffset = { currentLeftStatus, currentRightStatus, currentLeftStepRate, currentRightStepRate,
        //             meanData->at(currentLeftHipAngMean), meanData->at(currentRightHipAngMean), currentLeftStepLength,
        //             currentRightStepLength, currentLeftProgress, currentRightProgress, currentTurnAng, currentStepRate,
        //             currentStepLength, currentVelocity, currentVelocityThreshold, currentRealTimeLeftStatus, currentRealTimeRightStatus};
        walkFlatbuffersOffset = ActionData::CreateWalk(builder, 
                                                    int(walkOffset.at(0)),
                                                    int(walkOffset.at(1)),
                                                    walkOffset.at(2),
                                                    walkOffset.at(3),
                                                    walkOffset.at(4),
                                                    walkOffset.at(5),
                                                    walkOffset.at(6),
                                                    walkOffset.at(7),
                                                    walkOffset.at(8),
                                                    walkOffset.at(9),
                                                    walkOffset.at(10),
                                                    walkOffset.at(11),
                                                    walkOffset.at(12),
                                                    walkOffset.at(13),
                                                    walkOffset.at(14),
                                                    walkOffset.at(15),
                                                    walkOffset.at(16));
    }

    void actionDetectionManager::writeToFlatbuffers(ActionData::ActionBuilder& actionBuilder) {
        actionBuilder.add_stand(standFlatbuffersOffset);
        actionBuilder.add_walk(walkFlatbuffersOffset);
        // walkInstance.writeToFlatbuffers(actionBuilder);
    }

}

#endif