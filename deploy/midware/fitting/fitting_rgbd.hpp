#ifndef FITPLAY_fitting_rgbd
#define FITPLAY_fitting_rgbd

#include <string>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"

#include "poseData_generated.h"
#include "actionData_generated.h"
#include "fitting_data.hpp"

using namespace glm;
using namespace std;

namespace fitplay {
    class FittingRGBD {
        public: 
        int rgbdShadowPointMask[jointPointSize + 1];
        float rgbdZOffset[jointPointSize + 1];
        float poseZOffset[jointPointSize + 1];

        void updateRGBDOcculusion(landmarks & landmarkData3D, landmarks const & landmarkData2D);
        void update3DlandmarkWithRGBD(landmarks & landmarkData3D, landmarks const & landmarkData2D) ;
        void update3DlandmarkWithRGBDChain(landmarks & landmarkData3D, landmarks const & landmarkData2D, int index);
        void updateToKinamaticChain(landmarks & landmarkData3D, landmarks const & landmarkData2D,  int index, float updateZ);
    };

    void FittingRGBD::updateRGBDOcculusion(landmarks & landmarkData3D, landmarks const & landmarkData2D) {
        for(int i = 0 ; i < jointPointSize + 1; i++ ){
            rgbdShadowPointMask[i] = 0; 
        }
        for(int i = 0 ; i < jointPointSize + 1; i++ ){
            for(int j = 0; j < jointPointSize + 1; j++) {
                if(i == j ){
                    continue;
                }
                //how-to-calculate-object-keypoint-similarity a simplified OSK algorithm 
                if(length(landmarkData2D[i] - landmarkData2D[j]) * landmarkData2D[i].z < 0.07 && i!= j) {
                    if(landmarkData3D[i].z < landmarkData3D[j].z) {
                        rgbdShadowPointMask[i] = j;
                    } else {
                        rgbdShadowPointMask[j] = i;
                    }
                }
            }
        }
    }

    void FittingRGBD::update3DlandmarkWithRGBD(landmarks & landmarkData3D, landmarks const & landmarkData2D ) {
        updateRGBDOcculusion(landmarkData3D, landmarkData2D);
        //start updating from chain 0 hipcenter
        update3DlandmarkWithRGBDChain(landmarkData3D, landmarkData2D, HIP_CENTER);
    }

    void FittingRGBD::update3DlandmarkWithRGBDChain(landmarks & landmarkData3D, landmarks const & landmarkData2D, int index) {
        for(int i = 0; i < 3; i++) {
            int nextIndex = kinamaticChainNext[index][i];
            if(nextIndex == NO_NEXT) {
                break;
            } else {
                if(rgbdShadowPointMask[index] == 0 && rgbdShadowPointMask[nextIndex] == 0) {
                    //when no shadow occured, update z when next position not been masked
                    poseZOffset[nextIndex] = landmarkData3D[nextIndex].z - landmarkData3D[index].z;
                    rgbdZOffset[nextIndex] = landmarkData2D[nextIndex].z - landmarkData2D[index].z;
                    float updateZ = rgbdZOffset[nextIndex]  - poseZOffset[nextIndex];
                    // cout << "calculate RGBD index in" << index << " to " << nextIndex << endl;

                    // cout << "    source next Index landmark 3D data " << landmarkData3D[nextIndex].z << endl;
                    // cout << "    source this Index landmark 3D data " << landmarkData3D[index].z << endl; 
                    // cout << "    source next Index landmark 2D data " << landmarkData2D[nextIndex].z << endl;
                    // cout << "    source this Index landmark 2D data " << landmarkData2D[index].z << endl;

                    // cout << "    pose z " << poseZOffset[nextIndex] << " rgbd " << rgbdZOffset[nextIndex] << endl;
                    // cout << "    update with RGBD chain "  << " with z offset " << updateZ << endl;
                    // cout << "  " << endl;
                    updateToKinamaticChain(landmarkData3D, landmarkData2D, nextIndex, updateZ);
                }
                update3DlandmarkWithRGBDChain(landmarkData3D, landmarkData2D, nextIndex);
            }
        }
    }

    void FittingRGBD::updateToKinamaticChain(landmarks & landmarkData3D, landmarks const & landmarkData2D,  int index, float updateZ) {
        landmarkData3D[index].z += updateZ;
        for(int i = 0; i < 3; i++) {
            int nextIndex = kinamaticChainNext[index][i];
            if(nextIndex == NO_NEXT) {
                break;
            } else {
                updateToKinamaticChain(landmarkData3D, landmarkData2D, nextIndex, updateZ);
            }
        }
    }
}

#endif