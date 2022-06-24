#ifndef FITPLAY_testfittingrgbd 
#define FITPLAY_testfittingrgbd

#include <iostream>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"
#include "testUtils.hpp"
#include "testUtilsMocLandmark.hpp"

using namespace std;
using namespace glm;

void testFittingRGBDOcculusionFullSquence(fitplay::fitting fitInstance, bool detailPrint = false) {
    fitplay::landmarks landmarkData3D;
    fitplay::landmarks landmarkData2D;

    fitplay::FittingLandmark landmarkFittingInstance;
    fitplay::FittingFk fittingInstance;
    fitplay::FittingRGBD fittingRGBDInstance;

    cout << "===  starting testFittingRGBDOcculusionFullSquence ===" << endl;
    for(int index = 0; index < 500; index ++) {
        cout << "landmark index: " << index << endl;
        landmarkData3D = readLandmarks3D(index);
        landmarkData2D = readLandmarks2D(index);
    
        fittingRGBDInstance.updateRGBDOcculusion(landmarkData3D, landmarkData2D);
        
        if(detailPrint) {
            for(int i = 0; i < fitplay::jointPointSize; i++) {
                if(fittingRGBDInstance.rgbdShadowPointMask[i] != 0) {
                    cout << "      " << "    occulution on position : " << testUtilsGetLandmarkNameFromLandmarkIndex(i) << " shadow by " << 
                    testUtilsGetLandmarkNameFromLandmarkIndex(fittingRGBDInstance.rgbdShadowPointMask[i]) << endl;
                }
            }
        }
    }
    cout << "===  starting testFittingRGBDOcculusionFullSquence ===" << endl;
}

#endif