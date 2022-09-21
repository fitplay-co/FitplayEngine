#ifndef FITPLAY_testutils
#define FITPLAY_testutils

#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"
#include "fitting/fitting_fk.hpp"
#include "fitting/fitting_landmark.hpp"

using namespace std;
using namespace glm;
using namespace fitplay;

//here defines left hand axis related to unity
vec3 testup = vec3 (0.0f, 1.0f, 0.0f);
vec3 testdown = vec3 (0.0f, -1.0f, 0.0f);
vec3 testleft = vec3 (-1.0f, 0.0f, 0.0f);
vec3 testright = vec3 (1.0f, 0.0f, 0.0f);  
vec3 testforward = vec3(0.0f, 0.0f, 1.0f);
vec3 testbackward = vec3(0.0f, 0.0f, -1.0f);

void printVector(vec3 v) {
    cout << "       x " << v.x 
    << "       y " << v.y
    << "       z " << v.z  << endl;
}

float filterTooSmallFloat(float s) {
    if(s< 0.00001 && s > -0.00001) {
        return 0;
    }
    return s;
}

void testPrintQuat(quat q){
    cout << "    x: " << q.x << " y: " << q.y << " z: " << q.z << " w: " << q.w << endl;
    vec3 rotatedX = q * testright;
    vec3 rotatedY = q * testup;
    vec3 rotatedZ = q * testforward;
    cout << "    - x axis ";
    cout << "      x " << filterTooSmallFloat(rotatedX.x) << " y " << filterTooSmallFloat(rotatedX.y) << " z " << filterTooSmallFloat(rotatedX.z) << endl;
    cout << "    - y axis ";
    cout << "      x " << filterTooSmallFloat(rotatedY.x) << " y " << filterTooSmallFloat(rotatedY.y) << " z " << filterTooSmallFloat(rotatedY.z)  << endl;
    cout << "    - z axis ";
    cout << "      x " << filterTooSmallFloat(rotatedZ.x) << " y " << filterTooSmallFloat(rotatedZ.y) << " z " << filterTooSmallFloat(rotatedZ.z)  << endl;
}

bool testUtilsAssertEqual(vec3 left ,vec3 right) {
    if(filterTooSmallFloat(left.x) - filterTooSmallFloat(right.x) > 0.01 ||
            filterTooSmallFloat(left.y) - filterTooSmallFloat(right.y) > 0.01|| 
                filterTooSmallFloat(left.z) - filterTooSmallFloat(right.z) > 0.01 || 
                    filterTooSmallFloat(left.x) - filterTooSmallFloat(right.x) < -0.01 ||
                        filterTooSmallFloat(left.y) - filterTooSmallFloat(right.y) < -0.01 ||
                            filterTooSmallFloat(left.z) - filterTooSmallFloat(right.z) < -0.01) {
        cout << " = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = ="  << endl; 
        cout << " = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = ="  << endl; 
        cout << " error occured in test utils equal vec assertion" << endl;

        cout << " left " << endl;
        cout << " x " << filterTooSmallFloat(left.x) << " y " << filterTooSmallFloat(left.y) << " z " << filterTooSmallFloat(left.z) << endl;
        cout << " right " << endl;
        cout << " x " << filterTooSmallFloat(right.x) << " y " << filterTooSmallFloat(right.y) << " z " << filterTooSmallFloat(right.z) << endl;

        cout << " = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =" << endl; 
        cout << " = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =" << endl; 
        return false;
    }
    return true;
}

bool testUtilsAssertEqual(quat left ,quat right){
    if(filterTooSmallFloat(left.x) - filterTooSmallFloat(right.x) > 0.01 ||
            filterTooSmallFloat(left.y) - filterTooSmallFloat(right.y) > 0.01|| 
                filterTooSmallFloat(left.z) - filterTooSmallFloat(right.z) > 0.01 || 
                    filterTooSmallFloat(left.w) - filterTooSmallFloat(right.w) > 0.01 || 
                        filterTooSmallFloat(left.x) - filterTooSmallFloat(right.x) < -0.01 ||
                            filterTooSmallFloat(left.y) - filterTooSmallFloat(right.y) < -0.01 ||
                                filterTooSmallFloat(left.z) - filterTooSmallFloat(right.z) < -0.01 ||
                                    filterTooSmallFloat(left.w) - filterTooSmallFloat(right.w) < -0.01 ) {
        cout << " = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = ="  << endl; 
        cout << " = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = ="  << endl; 
        cout << " error occured in test utils equal quat assertion" << endl;

        cout << " left " << endl;
        cout << " x " << filterTooSmallFloat(left.x) << " y " << filterTooSmallFloat(left.y) << " z " << filterTooSmallFloat(left.z) 
            <<  " w " << filterTooSmallFloat(left.w) 
            << endl;
        cout << " right " << endl;
        cout << " x " << filterTooSmallFloat(right.x) << " y " << filterTooSmallFloat(right.y) << " z " << filterTooSmallFloat(right.z) 
            << " w " << filterTooSmallFloat(right.w) 
            << endl;

        cout << " = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =" << endl; 
        cout << " = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =" << endl; 
        return false;
    }
    return true;
}

//from 0 to 18
std::string testUtilsGetLandmarkNameFromLandmarkIndex(int index){
    switch(index) {
        case HIP_CENTER : return "HIP_CENTER";
        case NECK : return "NECK";
        case HEAD : return "HEAD";
        case L_SHOULDER : return "L_SHOULDER";
        case R_SHOULDER : return "R_SHOULDER";
        case L_ARM : return "L_ARM";
        case R_ARM : return "R_ARM";
        case L_WRIST : return "L_WRIST";
        case R_WRIST : return "R_WRIST";
        case L_HAND : return "L_HAND";
        case R_HAND : return "R_HAND";
        case L_HIP : return "L_HIP";
        case R_HIP : return "R_HIP";
        case L_KNEE : return "L_KNEE";
        case R_KNEE : return "R_KNEE";
        case L_ANKLE : return "L_ANKLE";
        case R_ANKLE : return "R_ANKLE";
        case L_FOOT : return "L_FOOT";
        case R_FOOT : return "R_FOOT";
        default: return "NO_NEXT";
    }
}
#endif