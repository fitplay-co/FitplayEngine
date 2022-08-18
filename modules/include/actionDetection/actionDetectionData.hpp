#ifndef ACTION_detection_data
#define ACTION_detection_data

namespace actionDetection {

    struct subscribeAngle {
        int num1;
        int num2;
        int num3;
        int config;
    };

    struct subscribeDistance {
        int num1;
        int num2;
        int config;
    };

    // filterType 1 == low-pass
    // filterType 2 == euro-filter
    // filterType 3 == raw
    struct featureVelocity {
        float target;
        int filterType;
        float threshold1;
        float threshold2;
    };

    struct featureVelocityData {
        float currentTarget;
        float preTarget;
        float currentTargetMean;
        float preTargetMean;
        float preStatus;
        float frameShiftCount;
    };

}

#endif