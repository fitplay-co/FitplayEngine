#ifndef FITPLAY_testfittingStandPoseGravity
#define FITPLAY_testfittingStandPoseGravity

#include <iostream>
#include <vector>
#include <numeric>
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"
#include "testUtils.hpp"
#include "testUtilsMocLandmark.hpp"

using namespace std;
using namespace glm;
using namespace fitplay;

void testFittingGravityStandPose (fitplay::fitting fitInstance, bool detailPrint = false) {
    landmarks landmarkData(19);
    fitplay::FittingLandmark landmarkFittingInstance;
    fitplay::FittingFk fittingInstance;

    //calculate mean error score

    landmarkData = readLandmarks(1);
    cout << "===  starting testFittingBoneLengthBiasCalculationErrorCheck ===" << endl ;
    landmarkFittingInstance.handcraftFittingLandmarkRead(landmarkData, fittingInstance.jointPoints);
    landmarkFittingInstance.handcraftFittingRound();
    landmarkFittingInstance.beforeFittingError = landmarkFittingInstance.summarizeError();
    landmarkFittingInstance.jointGredientsUpdate();
    landmarkFittingInstance.handcraftFittingRound();
    landmarkFittingInstance.currentFittingError = landmarkFittingInstance.summarizeError();

    if(detailPrint == true){
        cout << "before error: " << landmarkFittingInstance.beforeFittingError << endl;
        cout << "after error: " << landmarkFittingInstance.currentFittingError<< endl;
    }
}

#endif
