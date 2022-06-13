#ifndef FITPLAY_testfittingStandPose
#define FITPLAY_testfittingStandPose

#include <iostream>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"
#include "testUtils.hpp"
#include "testUtilsMocLandmark.hpp"

using namespace std;
using namespace glm;
using namespace fitplay;

void testFittingStandPose(fitplay::fitting fitInstance, bool detailPrint = false) {
    //mock landmarks
    fitplay::landmarks landmarkData;
    fitplay::FittingLandmark landmarkFittingInstance;
    fitplay::fitting fittingInstance;

    //emulating landmarks reflecting y from right hand camera axis to left hand unity axis
    landmarkData.hipcenter = - vec3( 0.0, 0.0, 0.0);
    landmarkData.neck = - vec3( -0.011229954659938812, -0.496501088142395, -0.05136949568986893);
    landmarkData.head = - vec3( -0.04985255375504494, -0.6285411715507507, -0.28715386986732483);
    landmarkData.lshoulder = - vec3( 0.13641440868377686, -0.47245922684669495, -0.07498262077569962);
    landmarkData.rshoulder = - vec3( -0.15887431800365448, -0.520542979240417, -0.027756372466683388);
    landmarkData.larm = - vec3(0.19456812739372253, -0.27366429567337036, -0.046901553869247437);
    landmarkData.rarm = - vec3(-0.19471660256385803, -0.22830739617347717, -0.061671897768974304);
    landmarkData.lwrist = - vec3(0.20017600059509277, -0.03678751364350319,  -0.02339997328817844);
    landmarkData.rwrist = - vec3( -0.18428128957748413, -0.029323451220989227, -0.12457260489463806);
    landmarkData.lhand = - vec3(0.1564548909664154,  0.04059676080942154, -0.04097369313240051);
    landmarkData.rhand = - vec3( -0.1175754964351654, -0.0048469556495547295,  -0.18125954270362854);
    landmarkData.lhip = - vec3(0.09397126734256744, -0.00013560160004999489, 0.0292273610830307);
    landmarkData.rhip = - vec3(-0.09122884273529053, -0.002662896877154708, -0.027753902599215508);
    landmarkData.lknee = - vec3(0.09948822855949402,  0.403020977973938,  -0.005365443415939808);
    landmarkData.rknee = - vec3( -0.042817454785108566,  0.3305259644985199, -0.016515379771590233);
    landmarkData.lankle = - vec3(0.11902331560850143, 0.7213556170463562, 0.18322911858558655);
    landmarkData.rankle = - vec3(-0.07252103090286255, 0.6923431158065796, 0.1799028366804123);
    landmarkData.lfoot = - vec3(0.11197564750909805, 0.8096407055854797,  0.1629805862903595);
    landmarkData.rfoot = - vec3(-0.07256607711315155, 0.7641468644142151, 0.09375570714473724);

    //start call fit landmark
    cout << "===  starting fitting landmark stand pose init testing.  ===" << endl ;
    landmarkFittingInstance.handcraftFitting(landmarkData, fittingInstance);
    //print current bone length error calculation and gredients
    if(detailPrint) {
        for(int i = 0; i < fitplay::jointPointSize; i++) {
            cout << "joint:" << fittingInstance.jointPoints[i].jointName << endl;
            cout << "      " << "bone error: " << endl;
            cout << "      " << "    from :" << landmarkFittingInstance.errorDataList[2].landmarkErrorData[i].fromPointIndex;
            printVector(landmarkFittingInstance.errorDataList[2].landmarkErrorData[i].fromPointPosition);
            cout << "      " << "    to   :" << landmarkFittingInstance.errorDataList[2].landmarkErrorData[i].toPointIndex;
            printVector(landmarkFittingInstance.errorDataList[2].landmarkErrorData[i].toPointPosition);
            cout <<"      " << "    length:   " 
            << glm::length(landmarkFittingInstance.errorDataList[2].landmarkErrorData[i].toPointPosition - landmarkFittingInstance.errorDataList[2].landmarkErrorData[i].fromPointPosition)
            << endl;
        }
    }
    landmarkFittingInstance.handcraftFitting(landmarkData, fittingInstance);
    landmarkFittingInstance.handcraftFitting(landmarkData, fittingInstance);
    landmarkFittingInstance.handcraftFitting(landmarkData, fittingInstance);
}

void testFittingStandPoseFullData(fitplay::fitting fitInstance, int index, bool detailPrint = false) {
    fitplay::landmarks landmarkData;
    fitplay::FittingLandmark landmarkFittingInstance;
    fitplay::fitting fittingInstance;

    landmarkData = readLandmarks(index);
    // cout << "===  starting fitting landmark stand pose init testing.  ===" << " index " << index << endl ;
    landmarkFittingInstance.handcraftFitting(landmarkData, fittingInstance);
    if(detailPrint) {
        for(int i = 0; i < fitplay::jointPointSize; i++) {
            cout << "joint:" << fittingInstance.jointPoints[i].jointName << endl;
            cout << "      " << "bone error: " << endl;
            cout << "      " << "    from :" << landmarkFittingInstance.errorDataList[2].landmarkErrorData[i].fromPointIndex;
            printVector(landmarkFittingInstance.errorDataList[2].landmarkErrorData[i].fromPointPosition);
            cout << "      " << "    to   :" << landmarkFittingInstance.errorDataList[2].landmarkErrorData[i].toPointIndex;
            printVector(landmarkFittingInstance.errorDataList[2].landmarkErrorData[i].toPointPosition);
            cout <<"      " << "    length:   " 
            << glm::length(landmarkFittingInstance.errorDataList[2].landmarkErrorData[i].toPointPosition
                 - landmarkFittingInstance.errorDataList[2].landmarkErrorData[i].fromPointPosition)
            << endl;
        }
    }

    landmarkFittingInstance.handcraftFitting(landmarkData, fittingInstance);
}

void testFittingBoneLengthBiasCalculation(fitplay::fitting fitInstance, bool detailPrint = false) {
    fitplay::landmarks landmarkData;
    fitplay::FittingLandmark landmarkFittingInstance;
    fitplay::fitting fittingInstance;

    //calculate mean error score
    vector<vector<float>> errorList;
    for(int i =0; i < jointPointSize; i++) {
        errorList.push_back(vector<float>());
    }

    for(int i = 0 ; i< 3000 ; i ++) {
        landmarkData = readLandmarks(i);
        landmarkFittingInstance.handcraftFitting(landmarkData, fittingInstance);
        for(int index = 0; index < jointPointSize; index++) {
            errorList[index].push_back(landmarkFittingInstance.errorDataList[2].landmarkErrorData[index].errorScore);
        }
    }
    
    //print all bias 
    if(detailPrint) {
        for (int index = 0 ; index < jointPointSize; index ++) {
            float sum = std::accumulate(std::begin(errorList[index]), std::end(errorList[index]), 0.0);  
            float meanError = sum / errorList[index].size();

            cout << " bone length bias " << index << "  " <<fitInstance.jointPoints[index].jointName << "  " 
            <<  landmarkFittingInstance.errorDataList[2].landmarkErrorData[index].statisticBias << " mean error: " << meanError << endl;
        }
    }
}

void testFittingBoneLengthBiasCalculationErrorCheck(fitplay::fitting fitInstance, bool detailPrint = false) {
    fitplay::landmarks landmarkData;
    fitplay::FittingLandmark landmarkFittingInstance;
    fitplay::fitting fittingInstance;

    //calculate mean error score
    vector<vector<float>> errorList;
    for(int i =0; i < jointPointSize; i++) {
        errorList.push_back(vector<float>());
    }

    landmarkData = readLandmarks(550);
    cout << "landmark debug larm z " << landmarkData.larm.z << endl;
    landmarkFittingInstance.handcraftFitting(landmarkData, fittingInstance);
    for(int index = 0; index < jointPointSize; index++) {
        errorList[index].push_back(landmarkFittingInstance.errorDataList[2].landmarkErrorData[index].errorScore);
    }
    
    if(detailPrint == true){
        cout << "before error: " << landmarkFittingInstance.beforeFittingError << endl;
        cout << "after error: " << landmarkFittingInstance.currentFittingError<< endl;
    }
}

    //intentionally set landmark data z axis error and check mean error score 

    //6.13 todo write
    //1. test cases on update 
    //2. test error value calculate 
    //3. test update with error fitting 
    //4. test with error fitting results 
#endif