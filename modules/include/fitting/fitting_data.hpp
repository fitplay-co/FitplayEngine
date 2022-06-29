#ifndef FITPLAY_fitting_data
#define FITPLAY_fitting_data

#include <string>
#include <vector>
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

    typedef std::vector<vec3> landmarks;

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
        { NECK, L_HIP, R_HIP }, //0 HIP_CENTER
        { HEAD, L_SHOULDER, R_SHOULDER }, //1 NECK
        { NO_NEXT }, //2 HEAD
        { L_ARM, NO_NEXT},  //3 L_SHOULDER
        { R_ARM, NO_NEXT},  //4 R_SHOULDER
        { L_WRIST, NO_NEXT},//5 L_ARM
        { R_WRIST, NO_NEXT},//6 R_ARM
        { L_HAND,  NO_NEXT},//7 L_WRIST
        { R_HAND,  NO_NEXT}, //8 R_WRIST
        { NO_NEXT }, //9 L_HAND
        { NO_NEXT }, //10 R_HAND
        { L_KNEE, NO_NEXT}, //11 L_HIP
        { R_KNEE, NO_NEXT}, //12 R_HIP
        { L_ANKLE, NO_NEXT}, //13 L_KNEE
        { R_ANKLE, NO_NEXT}, //14 R_KNEE
        { L_FOOT, NO_NEXT}, //15 L_ANKLE
        { R_FOOT, NO_NEXT}, //16 R_ANKLE
        { NO_NEXT }, //17 L_FOOT
        { NO_NEXT }  //18 R_FOOT
    };

    static constexpr float kinamaticChainRotationErrorParams[jointPointSize + 1] {
        0.0f, //0 HIP_CENTER
        0.4f, //1 NECK
        0.0f, //2 HEAD
        0.0f, //3 L_SHOULDER
        0.0f, //4 R_SHOULDER
        0.0f, //5 L_ARM
        0.0f, //6 R_ARM
        0.0f, //7 L_WRIST
        0.0f, //8 R_WRIST
        0.0f, //9 L_HAND
        0.0f, //10 R_HAND
        0.0f, //11 L_HIP
        0.0f, //12 R_HIP
       -0.3f, //13 L_KNEE
       -0.3f, //14 L_ANKLE
       -0.7f, //15 L_KNEE
       -0.7f, //16 R_ANKLE
        0.0f, //17 L_FOOT
        0.0f  //18 R_FOOT
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