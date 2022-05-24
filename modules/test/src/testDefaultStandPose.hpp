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




//              y
        //    <---- -x    x  ----->  
        //             -y                      head
        //                                      |
        //                  larm - lshoulder - neck -  rshoulder - rarm
        //                    ｜                 |                   | 
        //                  lwrist   lhip - hipcenter - rhip       rwrist
        //                    ｜      |                  |           |
        //                 lhand    lknee              rknee        rhand
        //                             |                  |
        //                           lankle             rankle
        //                             |                  |
        //                           lfoot              rfoot
        //

#endif