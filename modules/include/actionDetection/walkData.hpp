namespace actionwalk {

    // frame data
    static constexpr int preLeftFoot = 0;
    static constexpr int preRightFoot = 1;
    static constexpr int currentLeftFoot = 2;
    static constexpr int currentRightFoot = 3; 
    static constexpr int currentLeftHipAng = 4; 
    static constexpr int currentRightHipAng = 5;
    static constexpr int currentThighHeight = 6;
    static constexpr int currentLeftLegHeight = 7;
    static constexpr int currentRightLegHeight = 8;

    // mean data
    static constexpr int preLeftFootMean = 0;
    static constexpr int preRightFootMean = 1;
    static constexpr int currentLeftFootMean = 2;
    static constexpr int currentRightFootMean = 3;
    static constexpr int currentLeftHipAngMean = 4; 
    static constexpr int currentRightHipAngMean = 5;
    static constexpr int currentThighHeightMean = 6;
    static constexpr int currentLegHeightMean = 7;
    static constexpr int currentHeightMean = 8;

    // progress data
    static constexpr int preLeftStatus = 0;
    static constexpr int totalLeftDistance = 1;
    static constexpr int maxLeftDistance = 2;
    static constexpr int currentProgressLeftStatus = 3;
    static constexpr int realLeftStatus = 7;
    static constexpr int preRightStatus = 4;
    static constexpr int totalRightDistance = 5;
    static constexpr int maxRightDistance = 6;
    static constexpr int currentProgressRightStatus = 7;

    // time data
    static constexpr int timeLeftDown = 0;
    static constexpr int timeLeftUp = 1;
    static constexpr int timeLeftWindow = 2;
    static constexpr int timeRightDown = 3;
    static constexpr int timeRightUp = 4;
    static constexpr int timeRightWindow = 5;

    // time data
    // version 2
    static constexpr int timeLock = 0;
    static constexpr int timeAlpha = 1;
    static constexpr int timeBeta = 2;
}