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
        // cout << "landmark index: " << index << endl;
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

void testFittingRGBDUpdateSequence(fitplay::fitting fitInstance, bool detailPrint = false) {
    fitplay::landmarks landmarkData3D;
    fitplay::landmarks landmarkData2D;
    fitplay::landmarks landmarkDataCompare;

    fitplay::FittingLandmark landmarkFittingInstance;
    fitplay::FittingFk fittingInstance;
    fitplay::FittingRGBD fittingRGBDInstance;

    cout << "===  starting testFittingRGBDUpdateSequence ===" << endl;
    for(int index = 0; index < 1; index ++) {
        // cout << "landmark index: " << index << endl;
        landmarkData3D = readLandmarks3D(index);
        landmarkData2D = readLandmarks2D(index);

        //read a backup for landmark data comparation 
        landmarkDataCompare = readLandmarks3D(index);

        fittingRGBDInstance.updateRGBDOcculusion(landmarkData3D, landmarkData2D);
        fittingRGBDInstance.update3DlandmarkWithRGBDChain(landmarkData3D, landmarkData2D, HIP_CENTER);

        if(detailPrint) {
            for(int i = 0; i < fitplay::jointPointSize + 1; i++) {
                cout << "on position : " << testUtilsGetLandmarkNameFromLandmarkIndex(i) << endl;
                cout << "       landmark X: " << landmarkData3D[i].x << " Y: " <<  landmarkData3D[i].y << " Z: " << landmarkData3D[i].z<< endl;
                cout << "       compared X: " << landmarkDataCompare[i].x <<  " Y: " <<  landmarkDataCompare[i].y << " Z: " << landmarkDataCompare[i].z << endl;
            }
        }
    }
    cout << "===  starting testFittingRGBDUpdateSequence ===" << endl;
}

#endif