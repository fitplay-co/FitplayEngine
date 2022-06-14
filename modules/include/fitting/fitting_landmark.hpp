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
#include "fitting_data.hpp"
#include "fitting.hpp"

using namespace std;

namespace fitplay {

/**
 * @brief 
 * 
 * fitplay = human body fitting * play + human body fitness & play
 * 
 */
class FittingLandmark {
private:
    /* cached past frame joint points */
    std::vector<jointPoint> currentJointPoints;

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

    //update through each joint
    void jointGredientsUpdate ();
    //update through kinamacic chain
    void updateToKenamaticChain(int index, vec3 updateVec);

    //update joints with summarized gredients
    float summarizeError();
    //summarize Error 

    void handcraftFitting (const landmarks & fittingLandmark, const vector<jointPoint> & joints);
    void handcraftFittingLandmarkRead (const landmarks & fittingLandmark,const vector<jointPoint> & joints);
    void handcraftFittingRound ();
    //hand craft fitting to joints to make use of joint directions 
};

void FittingLandmark::handcraftFitting(const landmarks & fittingLandmark,const vector<jointPoint> & joints) {
    //read from landmark
    handcraftFittingLandmarkRead(fittingLandmark, joints);
    //calculate initial score 
    handcraftFittingRound();
    beforeFittingError = summarizeError();
    // jointGredientsUpdate();
    
    //summarize new error , will skip when real time fitting
    // handcraftFittingRound();
    currentFittingError = summarizeError();

    //cached fram after one round 
    frameCached = true;
}

void FittingLandmark::handcraftFittingLandmarkRead(const landmarks & fittingLandmark,const vector<jointPoint> & joints) {
    if(!frameCached) {
        for(int i=0; i< jointPointSize + 1; i++){
            currentJointPoints.push_back(joints[i]);
        }
    }
    cachedLandmarkData = currentFitLandmarkData;
    currentRawLandmarkData = readLandmarkData(fittingLandmark);
    //inititalize 
    currentFitLandmarkData = currentRawLandmarkData;
}

void FittingLandmark::handcraftFittingRound() {
    //fitting weight set at first round
    if(!frameCached) {
        errorDataList[0].errorWeight = 0.0f;
        errorDataList[1].errorWeight = 0.0f;
        errorDataList[2].errorWeight = 1.0f;
        errorDataList[3].errorWeight = 0.0f;
    }

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
        errorData.landmarkErrorData[i].toPointHandcraftSolution = - residual;
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
    errorData.landmarkErrorData[index].fromPointIndex = fromPoint;
    errorData.landmarkErrorData[index].toPointIndex = toPoint;
    errorData.landmarkErrorData[index].fromPointPosition  = currentFitLandmarkData[fromPoint];
    errorData.landmarkErrorData[index].toPointPosition  = currentFitLandmarkData[toPoint];

    float currentBoneLength = glm::length(errorData.landmarkErrorData[index].toPointPosition - errorData.landmarkErrorData[index].fromPointPosition);
    //update statistic bias 
    float boneLengthTotalResudual = currentBoneLength - currentJointPoints[index].boneLength;
    //by frames correct statistic bias
    errorData.landmarkErrorData[index].statisticBias = errorData.landmarkErrorData[index].statisticBias * 0.99 + boneLengthTotalResudual * 0.01;
    //update bone length with bias and predefined bone length; later may set bone length correction range or other facilities
    float targetBoneLength = currentJointPoints[index].boneLength + errorData.landmarkErrorData[index].statisticBias;
    
    errorData.landmarkErrorData[index].errorScore = abs(currentBoneLength - targetBoneLength);
    float dx = errorData.landmarkErrorData[index].toPointPosition.x - errorData.landmarkErrorData[index].fromPointPosition.x;   
    float dy = errorData.landmarkErrorData[index].toPointPosition.y - errorData.landmarkErrorData[index].fromPointPosition.y;
    float dz = errorData.landmarkErrorData[index].toPointPosition.z - errorData.landmarkErrorData[index].fromPointPosition.z;

    //longer than expected lim down z residual 
    //targetBoneLength^2 = x^2 + y^2 + ( solz + dz )^2 
    //solz as min for each z 
    float solz;
    float SuqareError = targetBoneLength * targetBoneLength -  dx*dx - dy*dy;

    if(SuqareError > 0) {
        float unsinedTargetZ = sqrt(SuqareError);
        if( dz> 0 ) {
            solz = unsinedTargetZ - dz;
        } else {
            solz = - unsinedTargetZ - dz;
        }
    } else {
        solz = 0.0f;
    }
    errorData.landmarkErrorData[index].toPointHandcraftSolution = vec3(0.0f, 0.0f, solz);
}

void FittingLandmark::humanBodyGravityError(BodyErrorData & errorData) {
    //1.calculate each joint gravity model
    //2.total body rotation with gradient as rotation 
}

void FittingLandmark::updateToKenamaticChain(int index, vec3 updateVec) {
    currentFitLandmarkData[index] += updateVec;
    for(int i = 0; i < 3; i++) {
        int nextIndex = kinamaticChainNext[index][i];
        if(nextIndex == NO_NEXT) {
            break;
        } else {
            updateToKenamaticChain(nextIndex, updateVec);
        }
    }
}

void FittingLandmark::jointGredientsUpdate() {
    //current do noting
    for(int i = 0; i < errorDataDims ; i ++) {
        for(int j =0 ; j< jointPointSize + 1; j++) {
            int updateIndex = errorDataList[i].landmarkErrorData[j].toPointIndex;
            vec3 updateVec = errorDataList[i].landmarkErrorData[j].toPointHandcraftSolution;
            float weight = errorDataList[i].errorWeight;
            updateToKenamaticChain(updateIndex, updateVec * weight);
        }
    }
} 

float FittingLandmark::summarizeError(){
    float sumError = 0.0f;
    for(int i = 0; i < errorDataDims ; i ++) {
        for(int j = 0; j < jointPointSize + 1 ; j++) {
            sumError += errorDataList[i].landmarkErrorData[j].errorScore;
        }
    }
    return sumError;
}

FittingLandmark::FittingLandmark(){}

FittingLandmark::~FittingLandmark(){}
}
#endif