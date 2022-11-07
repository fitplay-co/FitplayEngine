#ifndef ACTION_detection_data
#define ACTION_detection_data

namespace actionDetection {

    enum pose_landmark {
        head = 0,

        right_shoulder = 12,
        right_elbow = 14,
        right_wrist = 16,
        right_hand = 18,

        left_shoulder = 11,
        left_elbow = 13,
        left_wrist = 15,
        left_hand = 17,

        right_hip = 24,
        right_knee = 26,
        right_ankle = 28,
        right_toe = 30,

        left_hip = 23,
        left_knee = 25,
        left_ankle = 27,
        left_toe = 29
    };

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
        float shiftCount1;
        float shiftCount2;
        float stopCount;
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