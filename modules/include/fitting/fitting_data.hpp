#ifndef FITPLAY_fitting_data
#define FITPLAY_fitting_data

#include <string>
#include <vector>
#include "json.hpp"
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"

using namespace glm;
using namespace std;

namespace fitplay {

    static constexpr int jointPointSize = 18;
    static constexpr quat rotateFromLookatZtoX = quat(0.5, -0.5, -0.5, -0.5);

    static constexpr vec3 up = vec3 (0.0f, 1.0f, 0.0f);
    static constexpr vec3 down = vec3 (0.0f, -1.0f, 0.0f);
    static constexpr vec3 left = vec3 (-1.0f, 0.0f, 0.0f);
    static constexpr vec3 right = vec3 (1.0f, 0.0f, 0.0f);  
    static constexpr vec3 forward = vec3(0.0f, 0.0f, 1.0f);
    static constexpr vec3 backward = vec3(0.0f, 0.0f, -1.0f);

    static constexpr int HIP_CENTER = 0; 
    static constexpr int NECK = 1;      
    static constexpr int HEAD = 2;  
    static constexpr int L_SHOULDER = 3; 
    static constexpr int R_SHOULDER = 4;
    static constexpr int L_ARM = 5;
    static constexpr int R_ARM = 6;
    static constexpr int L_WRIST = 7;
    static constexpr int R_WRIST = 8;  
    static constexpr int L_HAND = 9;
    static constexpr int R_HAND = 10;
    static constexpr int L_HIP = 11;
    static constexpr int R_HIP = 12;
    static constexpr int L_KNEE = 13;
    static constexpr int R_KNEE = 14;
    static constexpr int L_ANKLE = 15;
    static constexpr int R_ANKLE = 16;
    static constexpr int L_FOOT = 17;
    static constexpr int R_FOOT = 18;
    static constexpr int NO_NEXT = -1;

    static constexpr int kinamaticChainNext[jointPointSize + 1][3] {
        { NECK, L_HIP, R_HIP }, //0
        { HEAD, L_SHOULDER, R_SHOULDER }, //1
        { NO_NEXT }, //2 
        { L_ARM, NO_NEXT},  //3
        { R_ARM, NO_NEXT},  //4
        { L_WRIST, NO_NEXT},//5
        { R_WRIST, NO_NEXT},//6
        { L_HAND,  NO_NEXT},//7
        { R_HAND,  NO_NEXT}, //8
        { NO_NEXT }, //9
        { NO_NEXT }, //10 
        { L_KNEE, NO_NEXT}, //11
        { R_KNEE, NO_NEXT}, //12
        { L_ANKLE, NO_NEXT}, //13
        { R_ANKLE, NO_NEXT}, //14
        { L_FOOT, NO_NEXT}, //15
        { R_FOOT, NO_NEXT}, //16
        { NO_NEXT }, //17
        { NO_NEXT }  //18 
    };


    constexpr int errorDataDims = 4;
    constexpr int fittingRoundMaxium = 1;
    constexpr float fittingErrorThreshold = 10.0f;

    struct jointPoint {
        std::string jointName;

        int parentIndex = 0;
        float boneLength = 0.0f;

        vec3 fromPoint;
        vec3 toPoint;

        //joint direciton ,forward ,and right
        vec3 posDirection3d = vec3 (0.0f, 1.0f, 0.0f); //by default to up 
        vec3 posForward3d;
        vec3 posLeft3d;

        //current global fk direction 
        quat fkRotation;

        //tpose current bone direction 
        quat initRotation;
        quat initRotationInverse;
        //t pose default joint rotation from parent rotation
        quat tposeDefaultJointRotation;
        quat tposeDefaultJointRotationInverse;

        //local joint current rotation will be calculated by update
        quat jointLocalRotation;
        quat jointGlobalRotation;

        vec3 fromPointFk =  vec3 (0.0f, 0.0f, 0.0f);
        vec3 alternativeUp = vec3 (1.0f, 0.0f, 0.0f);
        vec3 toPointFk =  vec3 (0.0f, 0.0f, 0.0f);
    };

    struct landmarks {
        // infer nect and hip center
        vec3 hipcenter;
        vec3 neck;
        vec3 head;
        vec3 lshoulder;
        vec3 rshoulder;
        vec3 larm;
        vec3 rarm;
        vec3 lwrist;
        vec3 rwrist; 
        vec3 lhand;
        vec3 rhand;
        vec3 lhip;
        vec3 rhip;
        vec3 lknee;
        vec3 rknee;
        vec3 lankle;
        vec3 rankle;
        vec3 lfoot;
        vec3 rfoot;
    };

    struct LandmarkErrorData {
        float errorScore = 0.0f;
        //bias for random error value correction. e.g take mean value in runtime fit human
        //body bone length recurrent 
        float statisticBias = 0.0f;
        int fromPointIndex = 0;
        vec3 fromPointPosition = vec3(0,0,0);
        vec3 toPointPosition = vec3(0,0,0);
        int toPointIndex = 0;
        vec3 toPointHandcraftSolution = vec3(0,0,0);
    };

    struct BodyErrorData {
        LandmarkErrorData landmarkErrorData[jointPointSize + 1];
        float errorWeight = 0.0f;
        float errorScore = 0.0f;
    };

}

#endif