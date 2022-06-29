#ifndef FITPLAY_testdefaultStandPose
#define FITPLAY_testdefaultStandPose

#include <iostream>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"
#include "testUtils.hpp"

using namespace std;
using namespace glm;

//              y
//    <---- -x    x  ----->  
//             -y                      head
//                                      |
// lhand - lwrist - larm - lshoulder - neck -  rshoulder - rarm - rwrist - rhand
//                                      |
//                           lhip - hipcenter - rhip
//                             |                  |
//                           lknee              rknee
//                             |                  |
//                           lankle             rankle
//                             |                  |
//                           lfoot              rfoot
// 

void testDefaultStandPose1(fitplay::FittingFk fitInstance, bool detailPrint = false) {
        //mock landmarks
    landmarks landmarkData(19);

    //emulating landmarks reflecting y from right hand camera axis to left hand unity axis
    landmarkData[HIP_CENTER] = vec3(0.0f ,0.0f ,0.0f);
    landmarkData[NECK] = landmarkData[HIP_CENTER] + (testup);
    landmarkData[HEAD] = landmarkData[NECK] + (testup);
    landmarkData[L_SHOULDER] = landmarkData[NECK] + testleft;
    landmarkData[R_SHOULDER] = landmarkData[NECK] + testright;
    landmarkData[L_ARM] = landmarkData[L_SHOULDER] + testleft;
    landmarkData[R_ARM] = landmarkData[R_SHOULDER] + testright;
    landmarkData[L_WRIST] = landmarkData[L_ARM] + testleft;
    landmarkData[R_WRIST] = landmarkData[R_ARM] + testright;
    landmarkData[L_HAND] = landmarkData[L_WRIST] + testleft;
    landmarkData[R_HAND] = landmarkData[R_WRIST] + testright;
    landmarkData[L_HIP] = landmarkData[HIP_CENTER] + testleft;
    landmarkData[R_HIP] = landmarkData[HIP_CENTER] + testright;
    landmarkData[L_KNEE] = landmarkData[L_HIP] + (testdown);
    landmarkData[R_KNEE] = landmarkData[R_HIP] + (testdown);
    landmarkData[L_ANKLE] = landmarkData[L_KNEE] + (testdown);
    landmarkData[R_ANKLE] = landmarkData[R_KNEE] + (testdown);
    landmarkData[L_FOOT] = landmarkData[L_ANKLE] + (testdown);
    landmarkData[R_FOOT] = landmarkData[R_ANKLE] + (testdown);

    fitInstance.updateLandmarks(landmarkData);
    //check fk rotation 

    cout << "===  stand pose 1 rotation ===" << endl;
    //print default rotation and assert default rotations
    for(int i = 0; i < 18 ; i++) {
        std::string name = fitInstance.jointPoints[i].jointName;
        if (detailPrint ) {
            quat fkRotation = fitInstance.jointPoints[i].fkRotation;
            cout << "    -  direction -     " << endl;
            cout << "    -  jointDirection -" 
                << "       x " << fitInstance.jointPoints[i].posDirection3d.x 
                << "       y " << fitInstance.jointPoints[i].posDirection3d.y
                << "       z " << fitInstance.jointPoints[i].posDirection3d.z  << endl;
            cout << "    -  jointForward -  " 
                << "       x " << fitInstance.jointPoints[i].posForward3d.x
                << "       y " << fitInstance.jointPoints[i].posForward3d.y
                << "       z " << fitInstance.jointPoints[i].posForward3d.z  << endl;
            cout << "    -  jointLeft -     " 
                << "       x " << fitInstance.jointPoints[i].posLeft3d.x 
                << "       y " << fitInstance.jointPoints[i].posLeft3d.y
                << "       z " << fitInstance.jointPoints[i].posLeft3d.z  << endl;
            cout << "    -  fromPoint -     " 
                << "       x " << fitInstance.jointPoints[i].fromPoint.x 
                << "       y " << fitInstance.jointPoints[i].fromPoint.y
                << "       z " << fitInstance.jointPoints[i].fromPoint.z  << endl;
            cout << "    -  toPoint -       " 
                << "       x " << fitInstance.jointPoints[i].toPoint.x 
                << "       y " << fitInstance.jointPoints[i].toPoint.y
                << "       z " << fitInstance.jointPoints[i].toPoint.z  << endl;
            cout << "    -  fkrotation -    " << endl;
            testPrintQuat(fkRotation);
            
            cout << "    -  globalRotation - " << endl;
            quat globalRotation = fitInstance.jointPoints[i].jointGlobalRotation;
            testPrintQuat(globalRotation);
            cout << "    -  defaultRotation - " << endl;
            quat defaultRotation = fitInstance.jointPoints[i].tposeDefaultJointRotation;
            testPrintQuat(defaultRotation);
            cout << "    -  localRotation - " << endl;
            quat localRotation = fitInstance.jointPoints[i].jointLocalRotation;
            testPrintQuat(localRotation);
        }
        //assert all local rotation identity  
        if (!testUtilsAssertEqual(fitInstance.jointPoints[i].jointLocalRotation, quat(1, 0, 0, 0))) {
            cout << name << endl;
            quat fkRotation = fitInstance.jointPoints[i].fkRotation;
            cout << "    -  direction -      " << endl;
            cout << "    -  boneDirection - " 
                << "       x " << fitInstance.jointPoints[i].posDirection3d.x 
                << "       y " << fitInstance.jointPoints[i].posDirection3d.y
                << "       z " << fitInstance.jointPoints[i].posDirection3d.z  << endl;
            cout << "    -  jointForward -  " 
                << "       x " << fitInstance.jointPoints[i].posForward3d.x
                << "       y " << fitInstance.jointPoints[i].posForward3d.y
                << "       z " << fitInstance.jointPoints[i].posForward3d.z  << endl;
            cout << "    -  jointLeft -     " 
                << "       x " << fitInstance.jointPoints[i].posLeft3d.x 
                << "       y " << fitInstance.jointPoints[i].posLeft3d.y
                << "       z " << fitInstance.jointPoints[i].posLeft3d.z  << endl;
            cout << "    -  fromPoint -     " 
                << "       x " << fitInstance.jointPoints[i].fromPoint.x 
                << "       y " << fitInstance.jointPoints[i].fromPoint.y
                << "       z " << fitInstance.jointPoints[i].fromPoint.z  << endl;
            cout << "    -  toPoint -       " 
                << "       x " << fitInstance.jointPoints[i].toPoint.x 
                << "       y " << fitInstance.jointPoints[i].toPoint.y
                << "       z " << fitInstance.jointPoints[i].toPoint.z  << endl;
            cout << "    -  fkrotation - " << endl;
            testPrintQuat(fkRotation);
            
            cout << "    -  globalRotation - " << endl;
            quat globalRotation = fitInstance.jointPoints[i].jointGlobalRotation;
            testPrintQuat(globalRotation);
            cout << "    -  defaultRotation - " << endl;
            quat defaultRotation = fitInstance.jointPoints[i].tposeDefaultJointRotation;
            testPrintQuat(defaultRotation);
            cout << "    -  localRotation - " << endl;
            quat localRotation = fitInstance.jointPoints[i].jointLocalRotation;
            testPrintQuat(localRotation);
        };
    }
    cout << "===  stand pose 1 rotation end ===" << endl;
}

//              y
//    <---- -x    x  ----->  
//             -y                      head
//                                      |
//                  larm - lshoulder - neck -  rshoulder - rarm - rwrist - rhand
//                    ｜                 |
//                  lwrist   lhip - hipcenter - rhip
//                    ｜      |                  |
//                 lhand    lknee              rknee
//                             |                  |
//                           lankle             rankle
//                             |                  |
//                           lfoot              rfoot
//

void testDefaultStandPose2(fitplay::FittingFk fitInstance, bool detailPrint = false) {
    //mock landmarks
    cout << "===  stand pose 2 rotation ===" << endl;

    landmarks landmarkData(19);

    //emulating landmarks reflecting y from right hand camera axis to left hand unity axis
    landmarkData[HIP_CENTER] = vec3(0.0f ,0.0f ,0.0f);
    landmarkData[NECK] = landmarkData[HIP_CENTER] + (testup);
    landmarkData[HEAD] = landmarkData[NECK] + (testup);
    landmarkData[L_SHOULDER] = landmarkData[NECK] + testleft;
    landmarkData[R_SHOULDER] = landmarkData[NECK] + testright;
    landmarkData[L_ARM] = landmarkData[L_SHOULDER] + testleft;
    landmarkData[R_ARM] = landmarkData[R_SHOULDER] + testright;
    landmarkData[L_WRIST] = landmarkData[L_ARM] + (testdown);
    landmarkData[R_WRIST] = landmarkData[R_ARM] + testright;
    landmarkData[L_HAND] = landmarkData[L_WRIST] + (testdown);
    landmarkData[R_HAND] = landmarkData[R_WRIST] + testright;
    landmarkData[L_HIP] = landmarkData[HIP_CENTER] + testleft;
    landmarkData[R_HIP] = landmarkData[HIP_CENTER] + testright;
    landmarkData[L_KNEE] = landmarkData[L_HIP] + (testdown);
    landmarkData[R_KNEE] = landmarkData[R_HIP] + (testdown);
    landmarkData[L_ANKLE] = landmarkData[L_KNEE] + (testdown);
    landmarkData[R_ANKLE] = landmarkData[R_KNEE] + (testdown);
    landmarkData[L_FOOT] = landmarkData[L_ANKLE] + (testdown);
    landmarkData[R_FOOT] = landmarkData[R_ANKLE] + (testdown);

    fitInstance.updateLandmarks(landmarkData);
    //check fk rotation 
    //print default rotation and assert default rotations
    for(int i = 0; i < 18 ; i++) {
        std::string name = fitInstance.jointPoints[i].jointName;
        if (detailPrint ) {
            quat fkRotation = fitInstance.jointPoints[i].fkRotation;
            cout << "    -  direction -      " << endl;
            cout << "    -  boneDirection - " 
                << "       x " << fitInstance.jointPoints[i].posDirection3d.x 
                << "       y " << fitInstance.jointPoints[i].posDirection3d.y
                << "       z " << fitInstance.jointPoints[i].posDirection3d.z  << endl;
            cout << "    -  fromPoint -     " 
                << "       x " << fitInstance.jointPoints[i].fromPoint.x 
                << "       y " << fitInstance.jointPoints[i].fromPoint.y
                << "       z " << fitInstance.jointPoints[i].fromPoint.z  << endl;
            cout << "    -  toPoint -       " 
                << "       x " << fitInstance.jointPoints[i].toPoint.x 
                << "       y " << fitInstance.jointPoints[i].toPoint.y
                << "       z " << fitInstance.jointPoints[i].toPoint.z  << endl;
            cout << "    -  fkrotation - " << endl;
            testPrintQuat(fkRotation);
            
            cout << "    -  globalRotation - " << endl;
            quat globalRotation = fitInstance.jointPoints[i].jointGlobalRotation;
            testPrintQuat(globalRotation);
            cout << "    -  defaultRotation - " << endl;
            quat defaultRotation = fitInstance.jointPoints[i].tposeDefaultJointRotation;
            testPrintQuat(defaultRotation);
            cout << "    -  localRotation - " << endl;
            quat localRotation = fitInstance.jointPoints[i].jointLocalRotation;
            testPrintQuat(localRotation);
        }

        //assert all local rotation identity
        bool correct = false;
        if (name == "luparm") {
            correct = testUtilsAssertEqual(fitInstance.jointPoints[i].jointLocalRotation, quat(-0.707107, -0.707107, 0, 0));    // unity  [ x: 90, y: 0, z: 0 ]
        } else if (name == "llowarm") {
            correct = testUtilsAssertEqual(fitInstance.jointPoints[i].jointLocalRotation, quat(0.707107, 0, 0, -0.707107));   // unity [ x: 0, y: 0, z: -90 ]
        } else if (name == "lhand") {
            correct = testUtilsAssertEqual(fitInstance.jointPoints[i].jointLocalRotation, quat(0.707107, -0.707107, 0, 0));   // unity [ x:-90, y: 0, z: 0 ]
        } else {
            correct = testUtilsAssertEqual(fitInstance.jointPoints[i].jointLocalRotation, quat(1, 0, 0, 0));   
        }
        
        if (!correct) {
            cout << name << endl;
            quat fkRotation = fitInstance.jointPoints[i].fkRotation;
            cout << "    -  direction -      " << endl;
            cout << "    -  boneDirection - " 
                << "       x " << fitInstance.jointPoints[i].posDirection3d.x 
                << "       y " << fitInstance.jointPoints[i].posDirection3d.y
                << "       z " << fitInstance.jointPoints[i].posDirection3d.z  << endl;
            cout << "    -  jointForward -  " 
                << "       x " << fitInstance.jointPoints[i].posForward3d.x
                << "       y " << fitInstance.jointPoints[i].posForward3d.y
                << "       z " << fitInstance.jointPoints[i].posForward3d.z  << endl;
            cout << "    -  jointLeft -     " 
                << "       x " << fitInstance.jointPoints[i].posLeft3d.x 
                << "       y " << fitInstance.jointPoints[i].posLeft3d.y
                << "       z " << fitInstance.jointPoints[i].posLeft3d.z  << endl;
            cout << "    -  alternativeUp - " 
                << "       x " << fitInstance.jointPoints[i].alternativeUp.x 
                << "       y " << fitInstance.jointPoints[i].alternativeUp.y
                << "       z " << fitInstance.jointPoints[i].alternativeUp.z  << endl;
            cout << "    -  fromPoint -     " 
                << "       x " << fitInstance.jointPoints[i].fromPoint.x 
                << "       y " << fitInstance.jointPoints[i].fromPoint.y
                << "       z " << fitInstance.jointPoints[i].fromPoint.z  << endl;
            cout << "    -  toPoint -       " 
                << "       x " << fitInstance.jointPoints[i].toPoint.x 
                << "       y " << fitInstance.jointPoints[i].toPoint.y
                << "       z " << fitInstance.jointPoints[i].toPoint.z  << endl;
            cout << "    -  fkrotation - " << endl;
            testPrintQuat(fkRotation);
            
            cout << "    -  globalRotation - " << endl;
            quat globalRotation = fitInstance.jointPoints[i].jointGlobalRotation;
            testPrintQuat(globalRotation);
            cout << "    -  defaultRotation - " << endl;
            quat defaultRotation = fitInstance.jointPoints[i].tposeDefaultJointRotation;
            testPrintQuat(defaultRotation);
            cout << "    -  localRotation - " << endl;
            quat localRotation = fitInstance.jointPoints[i].jointLocalRotation;
            testPrintQuat(localRotation);
        };        
    }
    cout << "===  stand pose 2 rotation end ===" << endl;
}

//              y
//    <---- -x    x  ----->  
//             -y                      head
//                                      |
//                  larm - lshoulder - neck     -     rshoulder
//                    ｜                 |                   |       
//                  lwrist   lhip - hipcenter - rhip       rarm   
//                    ｜      |                  |           |
//                 lhand    lknee              rknee        rwrist
//                             |                  |          |
//                           lankle             rankle      hand
//                             |                  |
//                           lfoot              rfoot
//

void testDefaultStandPose3(fitplay::FittingFk fitInstance, bool detailPrint = false) {
        //mock landmarks
    landmarks landmarkData(19);
        
    //emulating landmarks reflecting y from right hand camera axis to left hand unity axis
    landmarkData[HIP_CENTER] = vec3(0.0f ,0.0f ,0.0f);
    landmarkData[NECK] = landmarkData[HIP_CENTER] + (testup);
    landmarkData[HEAD] = landmarkData[NECK] + (testup);
    landmarkData[L_SHOULDER] = landmarkData[NECK] + testleft;
    landmarkData[R_SHOULDER] = landmarkData[NECK] + testright;
    landmarkData[L_ARM] = landmarkData[L_SHOULDER] + testleft;
    landmarkData[R_ARM] = landmarkData[R_SHOULDER] +  (testdown);
    landmarkData[L_WRIST] = landmarkData[L_ARM] + (testdown);
    landmarkData[R_WRIST] = landmarkData[R_ARM] +  (testdown);
    landmarkData[L_HAND] = landmarkData[L_WRIST] + (testdown);
    landmarkData[R_HAND] = landmarkData[R_WRIST] +  (testdown);
    landmarkData[L_HIP] = landmarkData[HIP_CENTER] + testleft;
    landmarkData[R_HIP] = landmarkData[HIP_CENTER] + testright;
    landmarkData[L_KNEE] = landmarkData[L_HIP] + (testdown);
    landmarkData[R_KNEE] = landmarkData[R_HIP] + (testdown);
    landmarkData[L_ANKLE] = landmarkData[L_KNEE] + (testdown);
    landmarkData[R_ANKLE] = landmarkData[R_KNEE] + (testdown);
    landmarkData[L_FOOT] = landmarkData[L_ANKLE] + (testdown);
    landmarkData[R_FOOT] = landmarkData[R_ANKLE] + (testdown);

    fitInstance.updateLandmarks(landmarkData);
    //check fk rotation 

    cout << "===  stand pose 3 rotation ===" << endl;
    //print default rotation and assert default rotations
    for(int i = 0; i < 18 ; i++) {
        std::string name = fitInstance.jointPoints[i].jointName;
        if (detailPrint ) {
            quat fkRotation = fitInstance.jointPoints[i].fkRotation;
            cout << "    -  direction -      " << endl;
            cout << "    -  boneDirection - " 
                << "       x " << fitInstance.jointPoints[i].posDirection3d.x 
                << "       y " << fitInstance.jointPoints[i].posDirection3d.y
                << "       z " << fitInstance.jointPoints[i].posDirection3d.z  << endl;
            cout << "    -  fromPoint -     " 
                << "       x " << fitInstance.jointPoints[i].fromPoint.x 
                << "       y " << fitInstance.jointPoints[i].fromPoint.y
                << "       z " << fitInstance.jointPoints[i].fromPoint.z  << endl;
            cout << "    -  jointForward -  " 
                << "       x " << fitInstance.jointPoints[i].posForward3d.x
                << "       y " << fitInstance.jointPoints[i].posForward3d.y
                << "       z " << fitInstance.jointPoints[i].posForward3d.z  << endl;
            cout << "    -  toPoint -       " 
                << "       x " << fitInstance.jointPoints[i].toPoint.x 
                << "       y " << fitInstance.jointPoints[i].toPoint.y
                << "       z " << fitInstance.jointPoints[i].toPoint.z  << endl;
            cout << "    -  fkrotation - " << endl;
            testPrintQuat(fkRotation);
            
            cout << "    -  globalRotation - " << endl;
            quat globalRotation = fitInstance.jointPoints[i].jointGlobalRotation;
            testPrintQuat(globalRotation);
            cout << "    -  defaultRotation - " << endl;
            quat defaultRotation = fitInstance.jointPoints[i].tposeDefaultJointRotation;
            testPrintQuat(defaultRotation);
            cout << "    -  localRotation - " << endl;
            quat localRotation = fitInstance.jointPoints[i].jointLocalRotation;
            testPrintQuat(localRotation);
        }

        //assert all local rotation identity
        bool correct = false;
        if (name == "luparm") {
            correct = testUtilsAssertEqual(fitInstance.jointPoints[i].jointLocalRotation, quat(-0.707107, -0.707107, 0, 0));    // unity  [ x: 90, y: 0, z: 0 ]
        } else if (name == "llowarm") {
            correct = testUtilsAssertEqual(fitInstance.jointPoints[i].jointLocalRotation, quat(0.707107, 0, 0, -0.707107));   // unity [ x: 0, y: 0, z: -90 ]
        } else if (name == "lhand") {
            correct = testUtilsAssertEqual(fitInstance.jointPoints[i].jointLocalRotation, quat(0.707107, -0.707107, 0, 0));   // unity [ x:-90, y: 0, z: 0 ]
        } else if (name == "ruparm") {
            correct = testUtilsAssertEqual(fitInstance.jointPoints[i].jointLocalRotation, quat(0.707107, 0, -0.707107, 0));   // unity [ x: 0, y: -90, z: 0 ]
        } else {
            correct = testUtilsAssertEqual(fitInstance.jointPoints[i].jointLocalRotation, quat(1, 0, 0, 0));        
        }
        
        if (!correct) {
            cout << name << endl;
            quat fkRotation = fitInstance.jointPoints[i].fkRotation;
            cout << "    -  direction -      " << endl;
            cout << "    -  boneDirection - " 
                << "       x " << fitInstance.jointPoints[i].posDirection3d.x 
                << "       y " << fitInstance.jointPoints[i].posDirection3d.y
                << "       z " << fitInstance.jointPoints[i].posDirection3d.z  << endl;
            cout << "    -  alternativeUp - " 
                << "       x " << fitInstance.jointPoints[i].alternativeUp.x 
                << "       y " << fitInstance.jointPoints[i].alternativeUp.y
                << "       z " << fitInstance.jointPoints[i].alternativeUp.z  << endl;
            cout << "    -  fromPoint -     " 
                << "       x " << fitInstance.jointPoints[i].fromPoint.x 
                << "       y " << fitInstance.jointPoints[i].fromPoint.y
                << "       z " << fitInstance.jointPoints[i].fromPoint.z  << endl;
            cout << "    -  toPoint -       " 
                << "       x " << fitInstance.jointPoints[i].toPoint.x 
                << "       y " << fitInstance.jointPoints[i].toPoint.y
                << "       z " << fitInstance.jointPoints[i].toPoint.z  << endl;
            cout << "    -  fkrotation - " << endl;
            testPrintQuat(fkRotation);
            
            cout << "    -  globalRotation - " << endl;
            quat globalRotation = fitInstance.jointPoints[i].jointGlobalRotation;
            testPrintQuat(globalRotation);
            cout << "    -  defaultRotation - " << endl;
            quat defaultRotation = fitInstance.jointPoints[i].tposeDefaultJointRotation;
            testPrintQuat(defaultRotation);
            cout << "    -  localRotation - " << endl;
            quat localRotation = fitInstance.jointPoints[i].jointLocalRotation;
            testPrintQuat(localRotation);
        };        
    }
    cout << "===  stand pose 3 rotation end ===" << endl;
}

// 京剧姿势
//              y
//    <---- -x    x  ----->  
//             -y                      head
//                                      |
//                  larm - lshoulder - neck -  rshoulder - rarm - rwrist - rhand
//                  /                   |
//             lwrist        lhip - hipcenter - rhip
//                  \          |                  |
//                 lhand    lknee              rknee
//                             |                  |
//                           lankle             rankle
//                             |                  |
//                           lfoot              rfoot
//

void testDefaultStandPose4(fitplay::FittingFk fitInstance, bool detailPrint = false) {
        //mock landmarks
    landmarks landmarkData(19);
        
    //emulating landmarks reflecting y from right hand camera axis to left hand unity axis
    landmarkData[HIP_CENTER] = vec3(0.0f ,0.0f ,0.0f);
    landmarkData[NECK] = landmarkData[HIP_CENTER] + (testup);
    landmarkData[HEAD] = landmarkData[NECK] + (testup);
    landmarkData[L_SHOULDER] = landmarkData[NECK] + testleft;
    landmarkData[R_SHOULDER] = landmarkData[NECK] + testright;
    landmarkData[L_ARM] = landmarkData[L_SHOULDER] + testleft;
    landmarkData[R_ARM] = landmarkData[R_SHOULDER] + testright;
    landmarkData[L_WRIST] = landmarkData[L_ARM] + (testdown) + testleft;
    landmarkData[R_WRIST] = landmarkData[R_ARM] + testright;
    landmarkData[L_HAND] = landmarkData[L_WRIST] + (testdown) + testright;
    landmarkData[R_HAND] = landmarkData[R_WRIST] + testright;
    landmarkData[L_HIP] = landmarkData[HIP_CENTER] + testleft;
    landmarkData[R_HIP] = landmarkData[HIP_CENTER] + testright;
    landmarkData[L_KNEE] = landmarkData[L_HIP] + (testdown);
    landmarkData[R_KNEE] = landmarkData[R_HIP] + (testdown);
    landmarkData[L_ANKLE] = landmarkData[L_KNEE] + (testdown);
    landmarkData[R_ANKLE] = landmarkData[R_KNEE] + (testdown);
    landmarkData[L_FOOT] = landmarkData[L_ANKLE] + (testdown);
    landmarkData[R_FOOT] = landmarkData[R_ANKLE] + (testdown);

    fitInstance.updateLandmarks(landmarkData);
    //check fk rotation 

    cout << "===  stand pose 4 rotation ===" << endl;
    //print default rotation and assert default rotations
    for(int i = 0; i < 18 ; i++) {
        std::string name = fitInstance.jointPoints[i].jointName;
        if (detailPrint ) {
            quat fkRotation = fitInstance.jointPoints[i].fkRotation;
            cout << "    -  direction -      " << endl;
            cout << "    -  boneDirection - " 
                << "       x " << fitInstance.jointPoints[i].posDirection3d.x 
                << "       y " << fitInstance.jointPoints[i].posDirection3d.y
                << "       z " << fitInstance.jointPoints[i].posDirection3d.z  << endl;
            cout << "    -  fromPoint -     " 
                << "       x " << fitInstance.jointPoints[i].fromPoint.x 
                << "       y " << fitInstance.jointPoints[i].fromPoint.y
                << "       z " << fitInstance.jointPoints[i].fromPoint.z  << endl;
            cout << "    -  toPoint -       " 
                << "       x " << fitInstance.jointPoints[i].toPoint.x 
                << "       y " << fitInstance.jointPoints[i].toPoint.y
                << "       z " << fitInstance.jointPoints[i].toPoint.z  << endl;
            cout << "    -  fkrotation - " << endl;
            testPrintQuat(fkRotation);
            
            cout << "    -  globalRotation - " << endl;
            quat globalRotation = fitInstance.jointPoints[i].jointGlobalRotation;
            testPrintQuat(globalRotation);
            cout << "    -  defaultRotation - " << endl;
            quat defaultRotation = fitInstance.jointPoints[i].tposeDefaultJointRotation;
            testPrintQuat(defaultRotation);
            cout << "    -  localRotation - " << endl;
            quat localRotation = fitInstance.jointPoints[i].jointLocalRotation;
            testPrintQuat(localRotation);
        }

        //assert all local rotation identity
        bool correct = false;
        if (name == "luparm") {
            correct = testUtilsAssertEqual(fitInstance.jointPoints[i].jointLocalRotation, quat(-0.707107, -0.707107, 0, 0));    // unity  [ x: 90, y: 0, z: 0 ]
        } else if (name == "llowarm") {
            correct = testUtilsAssertEqual(fitInstance.jointPoints[i].jointLocalRotation, quat(0.92388, 0, 0, -0.382683));   // unity [ x: 0, y: 0, z: -44.9999314 ]
        } else if (name == "lhand") {
            correct = testUtilsAssertEqual(fitInstance.jointPoints[i].jointLocalRotation, quat(0.5, -0.5, 0.5, -0.5));  // unity [ x: 0, y: 90, z: -90 ]
        } else {
            correct = testUtilsAssertEqual(fitInstance.jointPoints[i].jointLocalRotation, quat(1, 0, 0, 0));   
        }
        
        if (!correct) {
            cout << name << endl;
            quat fkRotation = fitInstance.jointPoints[i].fkRotation;
            cout << "    -  direction -      " << endl;
            cout << "    -  boneDirection - " 
                << "       x " << fitInstance.jointPoints[i].posDirection3d.x 
                << "       y " << fitInstance.jointPoints[i].posDirection3d.y
                << "       z " << fitInstance.jointPoints[i].posDirection3d.z  << endl;
            cout << "    -  alternativeUp - " 
                << "       x " << fitInstance.jointPoints[i].alternativeUp.x 
                << "       y " << fitInstance.jointPoints[i].alternativeUp.y
                << "       z " << fitInstance.jointPoints[i].alternativeUp.z  << endl;
            cout << "    -  jointLeft -     " 
                << "       x " << fitInstance.jointPoints[i].posLeft3d.x 
                << "       y " << fitInstance.jointPoints[i].posLeft3d.y
                << "       z " << fitInstance.jointPoints[i].posLeft3d.z  << endl;
            cout << "    -  jointForward -  " 
                << "       x " << fitInstance.jointPoints[i].posForward3d.x
                << "       y " << fitInstance.jointPoints[i].posForward3d.y
                << "       z " << fitInstance.jointPoints[i].posForward3d.z  << endl;
            cout << "    -  fromPoint -     " 
                << "       x " << fitInstance.jointPoints[i].fromPoint.x 
                << "       y " << fitInstance.jointPoints[i].fromPoint.y
                << "       z " << fitInstance.jointPoints[i].fromPoint.z  << endl;
            cout << "    -  toPoint -       " 
                << "       x " << fitInstance.jointPoints[i].toPoint.x 
                << "       y " << fitInstance.jointPoints[i].toPoint.y
                << "       z " << fitInstance.jointPoints[i].toPoint.z  << endl;
            cout << "    -  fkrotation - " << endl;
            testPrintQuat(fkRotation);
            
            cout << "    -  globalRotation - " << endl;
            quat globalRotation = fitInstance.jointPoints[i].jointGlobalRotation;
            testPrintQuat(globalRotation);
            cout << "    -  defaultRotation - " << endl;
            quat defaultRotation = fitInstance.jointPoints[i].tposeDefaultJointRotation;
            testPrintQuat(defaultRotation);
            cout << "    -  localRotation - " << endl;
            quat localRotation = fitInstance.jointPoints[i].jointLocalRotation;
            testPrintQuat(localRotation);
        };        
    }
    cout << "===  stand pose 2 rotation end ===" << endl;
}
#endif