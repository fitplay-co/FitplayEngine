#ifndef FITPLAY_testCapturedStandPose
#define FITPLAY_testCapturedStandPose

#include <iostream>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"
#include "testUtils.hpp"
#include "testUtilsMocLandmark.hpp"

using namespace std;
using namespace glm;

void testCapturedStandPose(fitplay::fitting fitInstance, bool detailPrint = false) {
    //mock landmarks
    landmarks landmarkData(19);
    landmarkData = readLandmarks3D(200);
    //emulating landmarks reflecting y from right hand camera axis to left hand unity axis
    // landmarkData[HIP_CENTER] = - vec3( 0.0, 0.0, 0.0);
    // landmarkData[NECK] = - vec3( -0.011229954659938812, -0.496501088142395, -0.05136949568986893);
    // landmarkData[HEAD] = - vec3( -0.04985255375504494, -0.6285411715507507, -0.28715386986732483);
    // landmarkData[L_SHOULDER] = - vec3( 0.13641440868377686, -0.47245922684669495, -0.07498262077569962);
    // landmarkData[R_SHOULDER] = - vec3( -0.15887431800365448, -0.520542979240417, -0.027756372466683388);
    // landmarkData[L_ARM] = - vec3(0.19456812739372253, -0.27366429567337036, -0.046901553869247437);
    // landmarkData[R_ARM] = - vec3(-0.19471660256385803, -0.22830739617347717, -0.061671897768974304);
    // landmarkData[L_WRIST] = - vec3(0.20017600059509277, -0.03678751364350319,  -0.02339997328817844);
    // landmarkData[R_WRIST] = - vec3( -0.18428128957748413, -0.029323451220989227, -0.12457260489463806);
    // landmarkData[L_HAND] = - vec3(0.1564548909664154,  0.04059676080942154, -0.04097369313240051);
    // landmarkData[R_HAND] = - vec3( -0.1175754964351654, -0.0048469556495547295,  -0.18125954270362854);
    // landmarkData[L_HIP] = - vec3(0.09397126734256744, -0.00013560160004999489, 0.0292273610830307);
    // landmarkData[R_HIP] = - vec3(-0.09122884273529053, -0.002662896877154708, -0.027753902599215508);
    // landmarkData[L_KNEE] = - vec3(0.09948822855949402,  0.403020977973938,  -0.005365443415939808);
    // landmarkData[R_KNEE] = - vec3( -0.042817454785108566,  0.3305259644985199, -0.016515379771590233);
    // landmarkData[L_ANKLE] = - vec3(0.11902331560850143, 0.7213556170463562, 0.18322911858558655);
    // landmarkData[R_ANKLE] = - vec3(-0.07252103090286255, 0.6923431158065796, 0.1799028366804123);
    // landmarkData[L_FOOT] = - vec3(0.11197564750909805, 0.8096407055854797,  0.1629805862903595);
    // landmarkData[R_FOOT] = - vec3(-0.07256607711315155, 0.7641468644142151, 0.09375570714473724);

    fitInstance.fkInstance.updateLandmarks(landmarkData);
    //check fk rotation 

    cout << "===  capture pose 1 rotation ===" << endl;
    //print default rotation and assert default rotations
    for(int i = 0; i < 18 ; i++) {
        std::string name = fitInstance.fkInstance.jointPoints[i].jointName;
        if (detailPrint ) {
            cout << name << endl;
            quat fkRotation = fitInstance.fkInstance.jointPoints[i].fkRotation;
            cout << "    -  direction -     " << endl;
            cout << "    -  jointDirection -" 
                << "       x " << fitInstance.fkInstance.jointPoints[i].posDirection3d.x 
                << "       y " << fitInstance.fkInstance.jointPoints[i].posDirection3d.y
                << "       z " << fitInstance.fkInstance.jointPoints[i].posDirection3d.z  << endl;
            cout << "    -  jointForward -  " 
                << "       x " << fitInstance.fkInstance.jointPoints[i].posForward3d.x
                << "       y " << fitInstance.fkInstance.jointPoints[i].posForward3d.y
                << "       z " << fitInstance.fkInstance.jointPoints[i].posForward3d.z  << endl;
            cout << "    -  jointLeft -     " 
                << "       x " << fitInstance.fkInstance.jointPoints[i].posLeft3d.x 
                << "       y " << fitInstance.fkInstance.jointPoints[i].posLeft3d.y
                << "       z " << fitInstance.fkInstance.jointPoints[i].posLeft3d.z  << endl;
            cout << "    -  fromPoint -     " 
                << "       x " << fitInstance.fkInstance.jointPoints[i].fromPoint.x 
                << "       y " << fitInstance.fkInstance.jointPoints[i].fromPoint.y
                << "       z " << fitInstance.fkInstance.jointPoints[i].fromPoint.z  << endl;
            cout << "    -  toPoint -       " 
                << "       x " << fitInstance.fkInstance.jointPoints[i].toPoint.x 
                << "       y " << fitInstance.fkInstance.jointPoints[i].toPoint.y
                << "       z " << fitInstance.fkInstance.jointPoints[i].toPoint.z  << endl;
            cout << "    -  fkrotation -    " << endl;
            testPrintQuat(fkRotation);
            cout << "    - unity - new Quaternion( " << fkRotation.x << ", " <<  fkRotation.y << ", " <<  fkRotation.z << ", " <<  fkRotation.w << ");" << endl;
            cout << "    -  globalRotation - " << endl;
            quat globalRotation = fitInstance.fkInstance.jointPoints[i].jointGlobalRotation;
            testPrintQuat(globalRotation);
            cout << "    -  defaultRotation - " << endl;
            quat defaultRotation = fitInstance.fkInstance.jointPoints[i].tposeDefaultJointRotation;
            testPrintQuat(defaultRotation);
            cout << "    -  localRotation - " << endl;
            quat localRotation = fitInstance.fkInstance.jointPoints[i].jointLocalRotation;
            testPrintQuat(localRotation);
        }
    }
    cout << "===  capture pose 1 rotation end ===" << endl;
}
#endif