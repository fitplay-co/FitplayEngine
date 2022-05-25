#include <iostream>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"
#include "testUtils.hpp"
#include "fitting/fitting.hpp"

//test cases
#include "testGlm.hpp"
#include "testDefaultTPose.hpp"
#include "testDefaultStandPose.hpp"

using namespace std;
using namespace glm;

fitplay::fitting fitInstance;

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

int main() {
    cout << "starting module testing." << endl;
    testGlmLookat();
    testDefaultTPose(fitInstance);
    testDefaultStandPose1(fitInstance);
    testDefaultStandPose2(fitInstance);
}

