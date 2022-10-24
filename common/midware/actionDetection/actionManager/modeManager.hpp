#ifndef MODE_manager
#define MODE_manager

#include "inputMessage_generated.h"
#include "euroFilter.hpp"

namespace actionDetection {

    class modeManager {
        private:
            double avgFootCenter = 0.5;
            float frameCount = 0;
            double frequency = 60, mincutoff = 0.85, beta = 1.2, dcutoff = 1.0;
            std::shared_ptr<detection::OneEuroFilter> avgFootCenterFilter;
            int modeShiftBit = 0;
        public:
            modeManager();
            ~modeManager();
            void process(const OsInput::InputMessage* data);
            int getModeShiftBit();

    };

    modeManager::modeManager() {
        avgFootCenterFilter = make_shared<detection::OneEuroFilter>(frequency, mincutoff, beta, dcutoff);
    };

    modeManager::~modeManager() {};

    void modeManager::process(const OsInput::InputMessage* data) {
        double timestamp = frameCount/frequency;
        frameCount = frameCount + 1;
        const PoseData::Pose* pose = data->pose();
        double currentFootCenter = double((pose->keypoints()->Get(31)->x() + pose->keypoints()->Get(32)->x())/2);
        avgFootCenter = avgFootCenterFilter->filter(currentFootCenter, timestamp);
        static int modeShiftCount = 0;
        if(abs(currentFootCenter - avgFootCenter) > 0.02) {
            modeShiftCount++;
            if(modeShiftCount > 5) {
                modeShiftBit = 1;
                modeShiftCount = 0;
            }
        }
        else {
            modeShiftCount = 0;
            modeShiftBit = 0;
        }
    };

    int modeManager::getModeShiftBit() {
        return modeShiftBit;
    }

}

#endif