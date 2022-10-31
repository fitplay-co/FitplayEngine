namespace actionwalk {

    // walk action pose data
    static constexpr int preLeftWalkPose = 0;
    static constexpr int preRightWalkPose = 1;
    static constexpr int currentLeftWalkPose = 2;
    static constexpr int currentRightWalkPose = 3; 
    static constexpr int currentLeftHipAng = 4; 
    static constexpr int currentRightHipAng = 5;
    static constexpr int currentThighHeight = 6;
    static constexpr int currentLeftLegHeight = 7;
    static constexpr int currentRightLegHeight = 8;
    static constexpr int preLeftWalkPoseMean = 11;
    static constexpr int preRightWalkPoseMean = 12;
    static constexpr int currentLeftWalkPoseMean = 13;
    static constexpr int currentRightWalkPoseMean = 14;
    static constexpr int currentLeftHipAngMean = 15; 
    static constexpr int currentRightHipAngMean = 16;
    static constexpr int currentThighHeightMean = 17;
    static constexpr int currentLegHeightMean = 18;
    static constexpr int currentHeightMean = 19;
    static constexpr int preLeftWalkStatus = 20;
    static constexpr int preRightWalkStatus = 21;

    // walk action time data
    static constexpr int timeLeftDown = 0;
    static constexpr int timeLeftUp = 1;
    static constexpr int timeLeftWindow = 2;
    static constexpr int timeRightDown = 3;
    static constexpr int timeRightUp = 4;
    static constexpr int timeRightWindow = 5;
    static constexpr int timeLock = 6;
    static constexpr int timeAlpha = 7;
    static constexpr int timeBeta = 8;
}