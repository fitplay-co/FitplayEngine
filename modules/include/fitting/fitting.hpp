#ifndef FITPLAY_fitting
#define FITPLAY_fitting

#include <string>
#include <vector>
#include "json.hpp"
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"

#include "flatbuffer/poseData_generated.h"
#include "flatbuffer/actionData_generated.h"

using namespace nlohmann;
using namespace glm;
using namespace std;

        //check each joint direction 
        //defination of landmarks
        //left hand axis and default up defined as z axis up = (0f, 0f, 1f)
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
        // here spine is ignored for faster fitting
        // transfer to vec3 for each landmarks

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

    class fitting {
        private:
            /* data */
        public:
            jointPoint jointPoints[jointPointSize + 1];
            bool mirror = true;
            fitting();
            ~ fitting();
            void addJointPoint(jointPoint& p, std::string name, int parentIndex, float boneLength,  vec3 const& tposeDirection);
            //update joint point in each process, to calculate fk landmark and rotation and do fitting
            //joint jointForward is right aixs of joint look at and refers to t pose y asix in unity 
            void updateJointPoint(jointPoint& p, vec3 const& startPoint, vec3 const& endPoint, vec3 const& jointForward);
            void process(const PoseData::Pose* data);
            void updateLandmarks(landmarks const & landmarkData);
            vec3 vectorFromToPoint(vec3 const & from, vec3 const & to);
            vec3 readLandmarkPointVector(int point, const PoseData::Pose* data);
            vec3 readLandmarkPointVectorMirror(int point, const PoseData::Pose* data);
            vec3 fkToNextPoint(vec3 const& startPoint, vec3 const& boneDirection, quat const& boneRotation, float const& bontLength);
            vec3 crossSafe(glm::vec3 const& direction, glm::vec3 const& fov);
            quat lookatRotationSafe(glm::vec3 const& direction, glm::vec3 const& up, glm::vec3 const& alternativeUp);
            flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<actionData::Joint>>> writeFlatBuffer(flatbuffers::FlatBufferBuilder& resultBuilder);
            // void debugPrintQuat(std::string jointName, std::string name, quat& quat, json &data);
            // void writeRotation(glm::quat const & r, json & data, int index, std::string name);
            // void writeFK(vec3 const& point, json & data, int index, std::string name);
    };

    fitting::fitting() {
        //last point for root reference from neck, hip points
        addJointPoint(jointPoints[18], "root", -1, 0.4f, down);
        addJointPoint(jointPoints[0], "spine", 18, 0.4f, down);
        addJointPoint(jointPoints[1], "neck", 0, 0.1f, down);
        addJointPoint(jointPoints[2], "lshoulder", 0, 0.16f, right);
        addJointPoint(jointPoints[3], "rshoulder", 0, 0.16f, left);
        addJointPoint(jointPoints[4], "luparm", 2, 0.24f, right);
        addJointPoint(jointPoints[5], "ruparm", 3, 0.24f, left);
        addJointPoint(jointPoints[6], "llowarm", 4, 0.2f, right);
        addJointPoint(jointPoints[7], "rlowarm", 5, 0.2f, left);
        addJointPoint(jointPoints[8], "lhand", 6, 0.1f, right);
        addJointPoint(jointPoints[9], "rhand", 7, 0.1f, left);
        addJointPoint(jointPoints[10], "lhip", 18, 0.1f, down);
        addJointPoint(jointPoints[11], "rhip", 18, 0.1f, down);
        addJointPoint(jointPoints[12], "lupleg", 10, 0.4f, up);
        addJointPoint(jointPoints[13], "rupleg", 11, 0.4f, up);
        addJointPoint(jointPoints[14], "llowleg", 12, 0.37f, up);
        addJointPoint(jointPoints[15], "rlowleg", 13, 0.37f, up);
        addJointPoint(jointPoints[16], "lfoot", 14, 0.1f, up);
        addJointPoint(jointPoints[17], "rfoot", 15, 0.1f, up);
    }

    fitting::~fitting() {}

    void fitting::addJointPoint(jointPoint& p, std::string name, int parentIndex, float boneLength, vec3 const& tposeDirection) {
        p.jointName = name;
        p.parentIndex = parentIndex;
        p.boneLength = boneLength;
        //tpose forward as right eusure right direction poing to forward 
        auto upward = cross(tposeDirection, fitplay::forward);
        p.initRotation = lookatRotationSafe(tposeDirection, upward, fitplay::forward);
        p.initRotationInverse = inverse(p.initRotation);
        // parent.initRot * p.defaultRot = p.initRot;
        // p.defaultRot =  (parent.initRot)-1 * p,
        p.tposeDefaultJointRotation = jointPoints[p.parentIndex].initRotationInverse * p.initRotation;
        p.tposeDefaultJointRotationInverse = inverse(p.tposeDefaultJointRotation);

        p.alternativeUp = fitplay::forward;
    }

    void fitting::updateJointPoint(jointPoint& p, vec3 const& startPoint, vec3 const& endPoint, vec3 const& jointForward) {
        //first step update start end points  here change from left hand to right hand landmarks
        p.fromPoint = vec3(startPoint[0], startPoint[1], startPoint[2]);
        //y should revert to fit unity standard merics
        p.toPoint = vec3(endPoint[0], endPoint[1], endPoint[2]);

        //calculate pose Direction 3d  node - child as forward vector 
        p.posDirection3d = vectorFromToPoint(p.toPoint, p.fromPoint);
        //tpose forward as right eusure right direction poing to forward 
        p.posForward3d = jointForward;
        //add forward axis 0.1 to enforce zero rotation to z axis 
        p.posLeft3d = normalize(crossSafe(p.posDirection3d, jointForward));
        p.fkRotation = lookatRotationSafe(p.posDirection3d, p.posLeft3d, p.alternativeUp);
        //glm::quatLookAtLH(p.posDirection3d, p.alternativeUp);
        //p.fkRotation = lookatRotationSafe(p.posDirection3d, p.alternativeUp , p.alternativeUp);

        //here read from parent pose direction 
        vec3& parentPosDirection3d = jointPoints[p.parentIndex].posDirection3d;
        quat& parentFkRotation = jointPoints[p.parentIndex].fkRotation;
        //fkRotation = jointLocationRotation *（parent.fkRotation * defatultJointRotation）
        p.jointGlobalRotation = inverse(parentFkRotation) * p.fkRotation ;
        p.jointLocalRotation = inverse(p.tposeDefaultJointRotation) * p.jointGlobalRotation;
        //calculate fk
        p.fromPointFk = jointPoints[p.parentIndex].toPointFk;
        //TODO fk calculation
        p.toPointFk = fkToNextPoint(p.fromPointFk, parentPosDirection3d, p.fkRotation, p.boneLength);
    }

    void fitting::process(const PoseData::Pose* data) {
        landmarks landmarkData;

        if(mirror) {
            landmarkData.head = readLandmarkPointVectorMirror(0, data);
            landmarkData.lshoulder = readLandmarkPointVectorMirror(12, data); 
            landmarkData.rshoulder = readLandmarkPointVectorMirror(11, data);
            landmarkData.larm = readLandmarkPointVectorMirror(14, data);
            landmarkData.rarm = readLandmarkPointVectorMirror(13, data); 
            landmarkData.lwrist = readLandmarkPointVectorMirror(16, data); 
            landmarkData.rwrist = readLandmarkPointVectorMirror(15, data); 
            landmarkData.lhand = readLandmarkPointVectorMirror(20, data); 
            landmarkData.rhand = readLandmarkPointVectorMirror(19, data);
            landmarkData.lhip = readLandmarkPointVectorMirror(24, data);
            landmarkData.rhip = readLandmarkPointVectorMirror(23, data);
            landmarkData.lknee = readLandmarkPointVectorMirror(26, data);
            landmarkData.rknee = readLandmarkPointVectorMirror(25, data);
            landmarkData.lankle = readLandmarkPointVectorMirror(28, data);
            landmarkData.rankle = readLandmarkPointVectorMirror(27, data);
            landmarkData.lfoot = readLandmarkPointVectorMirror(32, data);
            landmarkData.rfoot = readLandmarkPointVectorMirror(31, data); 
        } else {
            //read inverse by mirror 
            landmarkData.head = readLandmarkPointVector(0, data);
            landmarkData.lshoulder = readLandmarkPointVector(11, data); 
            landmarkData.rshoulder = readLandmarkPointVector(12, data);
            landmarkData.larm = readLandmarkPointVector(13, data);
            landmarkData.rarm = readLandmarkPointVector(14, data); 
            landmarkData.lwrist = readLandmarkPointVector(15, data); 
            landmarkData.rwrist = readLandmarkPointVector(16, data); 
            landmarkData.lhand = readLandmarkPointVector(19, data); 
            landmarkData.rhand = readLandmarkPointVector(20, data);
            landmarkData.lhip = readLandmarkPointVector(23, data);
            landmarkData.rhip = readLandmarkPointVector(24, data);
            landmarkData.lknee = readLandmarkPointVector(25, data);
            landmarkData.rknee = readLandmarkPointVector(26, data);
            landmarkData.lankle = readLandmarkPointVector(27, data);
            landmarkData.rankle = readLandmarkPointVector(28, data);
            landmarkData.lfoot = readLandmarkPointVector(31, data);
            landmarkData.rfoot = readLandmarkPointVector(32, data); 
        }

        // infer nect and hip center
        landmarkData.hipcenter = vec3(0.0f ,0.0f ,0.0f);
        landmarkData.neck = vec3((landmarkData.lshoulder[0] 
                + landmarkData.rshoulder[0])/2.0f, (landmarkData.lshoulder[1] 
                + landmarkData.rshoulder[1])/2.0f, (landmarkData.lshoulder[2] + landmarkData.rshoulder[2])/2.0f);

        updateLandmarks(landmarkData);
    }
    
    void fitting::updateLandmarks(landmarks const & landmarkData) {
        // update root joint with landmarks froward 
        jointPoints[18].fkRotation = jointPoints[18].initRotation;

        /**
         * @brief simple kinamatic constraints applied here
         */

        // hip forwarding
        vec3 hipRightForwarding = normalize(cross(landmarkData.rhip - landmarkData.lhip, fitplay::up));
        // up body forwarding
        vec3 spineTwistingForwarding = normalize(cross(landmarkData.rshoulder - landmarkData.lshoulder, fitplay::up));
        // armpit forwarding 
        // add 0.1 forward to affiate default totate to font
        vec3 rshoulderToArmKinamaticForwarding = normalize(landmarkData.rwrist - landmarkData.rarm);
        vec3 lshoulderToArmKinamaticForwarding = normalize(landmarkData.lwrist - landmarkData.larm);
        vec3 rarmToShoulderKinamaticForwarding = normalize(landmarkData.rshoulder - landmarkData.rarm);
        vec3 larmToShoulderKinamaticForwarding = normalize(landmarkData.lshoulder - landmarkData.larm);

        // knee forwarding  
        /** TODO need test */
        vec3 rlegToAnkleKinamaticForwarding = - normalize(landmarkData.rankle - landmarkData.rknee);
        vec3 llegToAnkleKinamaticForwarding = - normalize(landmarkData.lankle - landmarkData.lknee);
        vec3 rankleToLegKinamaticForwarding = - normalize(landmarkData.rhip - landmarkData.rknee);
        vec3 lAnkleToLegKinamaticForwarding = - normalize(landmarkData.lhip - landmarkData.lknee);

        // wrist forwarding 
        vec3 rwristForwarding = fitplay::forward;
        vec3 lwristForwarding = fitplay::forward;
        // ankle forwarding 
        vec3 rankleForwarding = fitplay::forward;
        vec3 lankleForwarding = fitplay::forward;

        //calculate rotation in each joints, to generate bones
        //up half human
        // glm::quat neckRotation = glm::rotation(up, vectorFromToPoint(hipcenter, neck));
        updateJointPoint(jointPoints[0], landmarkData.hipcenter, landmarkData.neck, hipRightForwarding);
        updateJointPoint(jointPoints[1], landmarkData.neck, landmarkData.head, spineTwistingForwarding);
        updateJointPoint(jointPoints[2], landmarkData.neck, landmarkData.lshoulder, spineTwistingForwarding);
        updateJointPoint(jointPoints[3], landmarkData.neck, landmarkData.rshoulder, spineTwistingForwarding);
        updateJointPoint(jointPoints[4], landmarkData.lshoulder, landmarkData.larm, lshoulderToArmKinamaticForwarding);
        updateJointPoint(jointPoints[5], landmarkData.rshoulder, landmarkData.rarm, rshoulderToArmKinamaticForwarding);
        updateJointPoint(jointPoints[6], landmarkData.larm, landmarkData.lwrist, larmToShoulderKinamaticForwarding);
        updateJointPoint(jointPoints[7], landmarkData.rarm, landmarkData.rwrist, rarmToShoulderKinamaticForwarding);
        updateJointPoint(jointPoints[8], landmarkData.lwrist, landmarkData.lhand, lwristForwarding);
        updateJointPoint(jointPoints[9], landmarkData.rwrist, landmarkData.rhand, rwristForwarding);
        updateJointPoint(jointPoints[10], fitplay::down, landmarkData.hipcenter, hipRightForwarding);
        updateJointPoint(jointPoints[11], fitplay::down, landmarkData.hipcenter, hipRightForwarding); // rhip sprcial 
        updateJointPoint(jointPoints[12], landmarkData.lhip, landmarkData.lknee, llegToAnkleKinamaticForwarding);
        updateJointPoint(jointPoints[13], landmarkData.rhip, landmarkData.rknee, rlegToAnkleKinamaticForwarding);
        updateJointPoint(jointPoints[14], landmarkData.lknee, landmarkData.lankle, lAnkleToLegKinamaticForwarding);
        updateJointPoint(jointPoints[15], landmarkData.rknee, landmarkData.rankle, rankleToLegKinamaticForwarding);
        updateJointPoint(jointPoints[16], landmarkData.lankle, landmarkData.lfoot, lankleForwarding);
        updateJointPoint(jointPoints[17], landmarkData.rankle, landmarkData.rfoot, rankleForwarding);
    }

    vec3 fitting::crossSafe(glm::vec3 const& direction, glm::vec3 const& fov) {
        //in unity backend and camera / depth model using left hand  
        //look at will rotate to z axis and y as up; whiles joints has x axis as direction and z as up; 
        //should do rotation to match with unity bone implementation
        if(glm::abs(glm::dot(direction, fov)) > .9999f) {
            // Use alternative up 
            return cross(direction, fitplay::forward); 
        }
        else {
            return cross(direction, fov); //glm::quatLookAtLH(direction, up);
        }
    }

    quat fitting::lookatRotationSafe(glm::vec3 const& direction, glm::vec3 const& up, glm::vec3 const& alternativeUp) {
        //in unity backend and camera / depth model using left hand  
        //look at will rotate to z axis and y as up; whiles joints has x axis as direction and z as up; 
        //should do rotation to match with unity bone implementation
        if(glm::abs(glm::dot(direction, up)) > .9999f) {
            // Use alternative up 
            return quat(1.0f, 0.0f, 0.0f, 0.0f) ; // glm::quatLookAtLH(direction, alternativeUp);
        }
        else {
            return glm::quatLookAtLH(direction, up) * rotateFromLookatZtoX; //glm::quatLookAtLH(direction, up);
        }
    }

    vec3 fitting::fkToNextPoint(vec3 const & startPoint, vec3 const & boneDirection, quat const & boneRotation, float const & bontLength) {
        //toward x direction 
        auto direction = glm::rotate(boneRotation, vec3(bontLength, 0.0f, 0.0f));
        return vec3(startPoint[0] + direction[0] * bontLength, startPoint[1] + direction[1] * bontLength, startPoint[2] + direction[2] * bontLength);
    }

    vec3 fitting::vectorFromToPoint(vec3 const & from, vec3 const & to) {
        return glm::normalize(vec3(to[0] - from[0], to[1] - from[1], to[2] - from[2]));
    }

    vec3 fitting::readLandmarkPointVector(int point, const PoseData::Pose* data) {
        float xStart = data->keypoints3D()->Get(point)->x();
        float yStart = 0.0f - data->keypoints3D()->Get(point)->y();
        float zStart = data->keypoints3D()->Get(point)->z();
        return vec3(xStart, yStart, zStart);
    }

    vec3 fitting::readLandmarkPointVectorMirror(int point, const PoseData::Pose* data) {
        float xStart = 0.0f - data->keypoints3D()->Get(point)->x();
        float yStart = 0.0f - data->keypoints3D()->Get(point)->y();
        float zStart = data->keypoints3D()->Get(point)->z();
        return vec3(xStart, yStart, zStart);
    }

    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<actionData::Joint>>> fitting::writeFlatBuffer(flatbuffers::FlatBufferBuilder& resultBuilder) {
        std::vector<flatbuffers::Offset<actionData::Joint>> nodeVector;
        for(int i = 0; i < jointPointSize; i ++) {
            auto nodeOffset = actionData::CreateJointDirect(resultBuilder,
                jointPoints[i].fkRotation.w, 
                jointPoints[i].fkRotation.x, 
                jointPoints[i].fkRotation.y, 
                jointPoints[i].fkRotation.z, 
                jointPoints[i].jointName.c_str());
            nodeVector.push_back(nodeOffset);
        }
        return resultBuilder.CreateVector(nodeVector);
    }

    inline std::vector<vec3> readLandmarkData(const landmarks & src) {
        return std::vector<vec3> {
            src.hipcenter, //0
            src.neck,      //1
            src.head,      //2
            src.lshoulder, //3
            src.rshoulder, //4
            src.larm,      //5
            src.rarm,      //6
            src.lwrist,    //7
            src.rwrist,    //8
            src.lhand,     //9
            src.rhand,     //10
            src.lhip,      //11
            src.rhip,      //12
            src.lknee,     //13
            src.rknee,     //14
            src.lankle,    //15
            src.rankle,    //16
            src.lfoot,     //17
            src.rfoot      //18
        };
    }

}

#endif