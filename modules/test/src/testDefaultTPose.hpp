#ifndef FITPLAY_testdefaultTPose
#define FITPLAY_testdefaultTPose

#include <iostream>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"
#include "testUtils.hpp"

using namespace std;
using namespace glm;

void testDefaultTPose(fitplay::fitting fitInstance) {
    cout << "starting fitting rotation init testing." << 1 << endl ;
    cout << "___________________________________________________" << endl;
    //print each init rotation 
    cout << "===  t pose ===" << endl;
    for(int i = 0; i < jointPointSize ; i++) {
        cout << fitInstance.jointPoints[i].jointName << endl;
        cout << "    - initRotation -  y arrays point to (0,0,1), ";
        quat init_rotation = fitInstance.jointPoints[i].initRotation;
        testPrintQuat(init_rotation);
        //up axis always point to z minors direction in human pose modeling
        testUtilsAssertEqual(init_rotation * testup, testforward);
        cout << endl;
    }

    cout << "===  t pose default rotation ===" << endl;
    //print default rotation and assert default rotations
    for(int i = 0; i < jointPointSize ; i++) {
        std::string name = fitInstance.jointPoints[i].jointName;
        cout << name << endl;
        quat default_rotation = fitInstance.jointPoints[i].tposeDefaultJointRotation;
        cout << "    - t pose default rotation - " << endl;
        if (name == "lshoulder") {
            testUtilsAssertEqual(default_rotation, quat(0.707107, 0, 0.707107, 0));
            cout << "    - t pose default lshoulder correct - " << endl;
        }
        if (name == "rshoulder") {
            testUtilsAssertEqual(default_rotation, quat(0.707107, 0, -0.707107, 0));
            cout << "    - t pose default rshoulder correct - " << endl;
        }
        if (name == "lupleg") {
            testUtilsAssertEqual(default_rotation, quat(-0.707107, 0, -0.707107, 0));
            cout << "    - t pose default lupleg correct - " << endl;
        }
        if (name == "rupleg") {
            testUtilsAssertEqual(default_rotation, quat(0.707107, 0, -0.707107, 0));
            cout << "    - t pose default rupleg correct - " << endl;
        }
        testPrintQuat(default_rotation);
    }
}

#endif