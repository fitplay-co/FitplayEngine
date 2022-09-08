#ifndef ACTION_detection_manager
#define ACTION_detection_manager

#include "actionManager/testManager.hpp"
#include "actionDetectionCalculator.hpp"
#include "walkDetection.hpp"

namespace actionDetection {

    class actionDetectionManager {
        private:

            testManager testInstance;
            actionwalk::walk walkInstance;
            actionDetectionCalculator calculatorInstance;

            // 0 : stand
            // 1 : travel
            float mode = 0;

            flatbuffers::Offset<actionData::Stand> flatbuffersOffset;

            bool init = false;

            // stand travel mode shift params
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
            bool process(const Input::InputMessage* data, flatbuffers::FlatBufferBuilder& builder);
            void writeToFlatbuffers(actionData::ActionBuilder&);
    };

    actionDetectionManager::actionDetectionManager() {};
    actionDetectionManager::~actionDetectionManager() {};

    bool actionDetectionManager::process(const Input::InputMessage* data, flatbuffers::FlatBufferBuilder& builder) {
        if (data->type() == Input::MessageType::MessageType_Pose) {
            static int modeShiftCount2 = 0;

            if(init == false) {

                struct subscribeAngle test = { 12,24,26,1 };
                struct subscribeDistance kneeDis = { 25,26,1 };
                struct subscribeDistance footDis = { 27,28,1 };
                struct subscribeDistance leftDis = { 25,27,1 };
                struct subscribeDistance rightDis = { 26,28,1 };
                struct subscribeDistance shoulderDis = { 11,12,1 };
                struct featureVelocity leftFootConstraint = { 1,1,0.04,-0.04,3,3,5 };
                struct featureVelocity rightFootConstraint = { 2,1,0.04,-0.04,3,3,5 };

                calculatorInstance.addSubscribeAngle(&test);
                calculatorInstance.addSubscribeDistance(&kneeDis);
                calculatorInstance.addSubscribeDistance(&footDis);
                calculatorInstance.addSubscribeDistance(&leftDis);
                calculatorInstance.addSubscribeDistance(&rightDis);
                calculatorInstance.addSubscribeDistance(&shoulderDis);
                calculatorInstance.addFeatureVelocity(&leftFootConstraint);
                calculatorInstance.addFeatureVelocity(&rightFootConstraint);

                init = true;
            }

            walkInstance.process(data, builder);
            calculatorInstance.process(data);

            float temp = calculatorInstance.getSubscribeAngle()->at(0);
            float kneeDis = calculatorInstance.getSubscribeDistance()->at(0);
            float footDis = calculatorInstance.getSubscribeDistance()->at(1);
            float leftDis = calculatorInstance.getSubscribeDistance()->at(2);
            float rightDis = calculatorInstance.getSubscribeDistance()->at(3);
            float shoulderDis = calculatorInstance.getSubscribeDistance()->at(4);
            float leftFootConstraint = calculatorInstance.getFeatureVelocity()->at(0);
            float rightFootConstraint = calculatorInstance.getFeatureVelocity()->at(1);

            float preLeftStatus = currentLeftStatus;
            float preRightStatus = currentRightStatus;
            shoulderDis = abs(shoulderDis);

            currentLeftStatus = walkInstance.getCurrentLeftStatus();
            currentRightStatus = walkInstance.getCurrentRightStatus();

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
                if(currentLeftStatus == 0 && currentRightStatus == 0) {
                    if(modeShiftCount > 20) {
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
            flatbuffersOffset = actionData::CreateStand(builder,
                                                            mode);
            return true;
        }
        if (data->type() == Input::MessageType::MessageType_ApplicationControl) {
            const ApplicationControl::Control* control = data->control(); 
            if (control->featureId()->str() == "action_detection") {
                if (control->action()->str() == "set_player") {
                    walkInstance.setPlayer(control->data()->height());
                }
            }
        }
        return false;
    }

    void actionDetectionManager::writeToFlatbuffers(actionData::ActionBuilder& actionBuilder) {
        actionBuilder.add_stand(flatbuffersOffset);
        walkInstance.writeToFlatbuffers(actionBuilder);
    }

}

#endif