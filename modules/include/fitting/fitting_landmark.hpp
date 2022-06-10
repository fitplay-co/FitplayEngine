/**
 * @brief fitting error calculator and providing error in 
 * default bone layout  
 */
#ifndef FITPLAY_fitting_error
#define FITPLAY_fitting_error

#include <string>
#include <vector>
#include "json.hpp"
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"

#include "fitting.hpp"

using namespace std;

namespace fitplay {

constexpr int errorDataDims = 4;
constexpr int fittingRoundMaxium = 1;
constexpr float fittingErrorThreshold = 10.0f;


struct LandmarkErrorData {
    float errorScore = 0.0f;
    int fromPointIndex = 0;
    vec3 fromPointPosition = vec3(0,0,0);
    vec3 fromPointHandcraftGredients; //gredients by handcraft
    vec3 toPointPosition = vec3(0,0,0);
    int toPointIndex = 0;
    vec3 toPointHandcraftGredients = vec3(0,0,0);
};

struct BodyErrorData {
    LandmarkErrorData landmarkErrorData[jointPointSize + 1];
    float errorScore = 0.0f;
};

/**
 * @brief 
 * 
 * fitplay = human body fitting * play + human body fitness & play
 * 
 */
class FittingLandmark {
private:
    /* cached past frame joint points */
    fitting currentJointPoints;

public:
    std::vector<vec3> cachedLandmarkData;
    std::vector<vec3> currentRawLandmarkData;
    std::vector<vec3> currentFitLandmarkData;
    
    float beforeFittingError;
    float currentFittingError;

    BodyErrorData errorDataList[errorDataDims];
    bool frameCached = false;

    FittingLandmark ();
    ~FittingLandmark ();
    /**
     * @brief smooth error with previous frame
     */
    void smoothError (BodyErrorData & errorData); 
    //smoothness point wise 
    
    void boneDirectionError (BodyErrorData & errorData);
    //human like point wise for bone direction error 

    void boneLengthError (BodyErrorData & errorData);
    void jointBoneLengthCalculate(int index, int fromPoint, int toPoint, BodyErrorData & errorData);
    //human like bone length should punish to unusual human bone length 
    
    void humanBodyGravityError (BodyErrorData & errorData);
    //physical punish unusual human body gravity activities and make foot on ground 

    void jointGredientsUpdate ();
    //update joints with summarized gredients
    float summarizeError();
    //summarize Error 

    void handcraftFitting (const landmarks & fittingLandmark, const fitting & jointFitting);
    void handcraftFittingRound ();
    //hand craft fitting to joints to make use of joint directions 
};

void FittingLandmark::handcraftFitting(const landmarks & fittingLandmark,const fitting & jointFitting) {
    currentJointPoints = jointFitting;
    cachedLandmarkData = currentFitLandmarkData;
    currentRawLandmarkData = readLandmarkData(fittingLandmark);
    //inititalize 
    currentFitLandmarkData = currentRawLandmarkData;
    //calculate initial score 
    handcraftFittingRound();
    // summarizeError();

    // for(int i =0 ; i < fittingRoundMaxium; i ++) {
    //     jointGredientsUpdate();
    //     summarizeError();
    //     if(currentFittingError < fittingErrorThreshold) {
    //         break;
    //     } else {
    //         handcraftFittingRound();
    //     }
    // }

    //cached fram after one round 
    frameCached = true;
}

void FittingLandmark::handcraftFittingRound() {
    // smoothError(errorDataList[0]);
    boneDirectionError(errorDataList[1]);
    boneLengthError(errorDataList[2]);
    humanBodyGravityError(errorDataList[3]);
}

void FittingLandmark::smoothError(BodyErrorData & errorData) {
    //for each point should punish to previous frame location 
    if(!frameCached) {
        return;
    }  
    for(int i = 0; i < jointPointSize + 1; i++) {
        vec3 residual = currentFitLandmarkData[i] - cachedLandmarkData[i];
        errorData.landmarkErrorData[i].errorScore = glm::length(residual);
        errorData.landmarkErrorData[i].fromPointIndex = i;
        errorData.landmarkErrorData[i].fromPointHandcraftGredients = - residual;
        errorData.landmarkErrorData[i].fromPointIndex = -1;
    }
}

void FittingLandmark::boneDirectionError(BodyErrorData & errorData) {
    //do nothing untill we have rgbd
}

void FittingLandmark::boneLengthError(BodyErrorData & errorData) {
    //do nothing 
    //1.calculate bone length  
    //2.compare with bone length configuration
    //3.enlarge bone length gradient by z aixs expanding
    jointBoneLengthCalculate(2, NECK, L_SHOULDER, errorData);
    jointBoneLengthCalculate(3, NECK, R_SHOULDER, errorData);
    jointBoneLengthCalculate(4, L_SHOULDER, L_ARM, errorData);
    jointBoneLengthCalculate(5, R_SHOULDER, R_ARM, errorData);
    jointBoneLengthCalculate(6, L_ARM, L_WRIST, errorData);
    jointBoneLengthCalculate(7, R_ARM, R_WRIST, errorData);
    jointBoneLengthCalculate(12, L_HIP, L_KNEE, errorData);
    jointBoneLengthCalculate(13, R_HIP, R_KNEE, errorData);
    jointBoneLengthCalculate(14, L_KNEE, L_ANKLE, errorData);
    jointBoneLengthCalculate(15, R_KNEE, R_ANKLE, errorData);
}

void FittingLandmark::jointBoneLengthCalculate(int index, int fromPoint, int toPoint, BodyErrorData & errorData) {
    float targetBoneLength = currentJointPoints.jointPoints[0].boneLength;
    errorData.landmarkErrorData[index].fromPointPosition  = currentFitLandmarkData[fromPoint];
    errorData.landmarkErrorData[index].toPointPosition  = currentFitLandmarkData[toPoint];
    float currentBoneLength = glm::length(errorData.landmarkErrorData[index].fromPointPosition - errorData.landmarkErrorData[index].toPointPosition );
    errorData.landmarkErrorData[index].errorScore = abs(currentBoneLength - targetBoneLength);
    if(currentBoneLength > targetBoneLength) {
        //longer than expected lim down z residual
    } else {
        //short than expected enlarge up z residual
    }
    errorData.landmarkErrorData[index].fromPointIndex = fromPoint;
    errorData.landmarkErrorData[index].toPointIndex = toPoint;

}

void FittingLandmark::humanBodyGravityError(BodyErrorData & errorData) {
    //1.calculate each joint gravity model
    //2.total body rotation with gradient as rotation 
}

void FittingLandmark::jointGredientsUpdate() {
    //current do noting
} 

float FittingLandmark::summarizeError(){
    //current do noting
    return 0.0f;
}

FittingLandmark::FittingLandmark(){}
FittingLandmark::~FittingLandmark(){}
}
#endif