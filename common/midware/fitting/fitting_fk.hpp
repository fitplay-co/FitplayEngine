#ifndef FITPLAY_fitting_fk
#define FITPLAY_fitting_fk

#include <string>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"

#include "poseData_generated.h"
#include "actionData_generated.h"
#include "fitting_data.hpp"

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
    class FittingFk {
        private:
            /* data */
        public:
            std::vector<jointPoint> jointPoints;
            FittingFk();
            ~ FittingFk();
            void addJointPoint(jointPoint& p, std::string name, int parentIndex, float boneLength,  vec3 const& tposeDirection);
            //update joint point in each process, to calculate fk landmark and rotation and do fitting
            //joint jointForward is right aixs of joint look at and refers to t pose y asix in unity 
            void updateJointPoint(jointPoint& p, vec3 const& startPoint, vec3 const& endPoint, vec3 const& jointForward);

            void getLookRotation(jointPoint& p, vec3 const& parentJoint, vec3 const& childJoint);

            void updateLandmarks(landmarks const & landmarkData);
            vec3 vectorFromToPoint(vec3 const & from, vec3 const & to);
            vec3 readLandmarkPointVector(int point, const PoseData::Pose* data);
            vec3 readLandmarkPointVectorMirror(int point, const PoseData::Pose* data);
            vec3 fkToNextPoint(vec3 const& startPoint, quat const& boneRotation, float const& bontLength);//(vec3 const& startPoint, vec3 const& boneDirection, quat const& boneRotation, float const& bontLength);
            vec3 crossSafe(glm::vec3 const& direction, glm::vec3 const& fov);
            quat lookatRotationSafe(glm::vec3 const& direction, glm::vec3 const& up, glm::vec3 const& alternativeUp);
            // void debugPrintQuat(std::string jointName, std::string name, quat& quat, json &data);
            // void writeRotation(glm::quat const & r, json & data, int index, std::string name);
            // void writeFK(vec3 const& point, json & data, int index, std::string name);
    };

    FittingFk::FittingFk() {
        //last point for root reference from neck, hip points
        for(int i = 0; i< jointPointSize + 1; i++) {
            jointPoints.push_back(jointPoint());
        }
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

    FittingFk::~FittingFk() {}

    void FittingFk::addJointPoint(jointPoint& p, std::string name, int parentIndex, float boneLength, vec3 const& tposeDirection) {
        p.jointName = name;
        p.parentIndex = parentIndex;
        p.boneLength = boneLength;
        //tpose forward as right eusure right direction poing to forward 
        //auto forward = cross(tposeDirection, fitplay::forward);
        //p.initRotation = lookatRotationSafe(tposeDirection, upward, fitplay::forward);
        p.initRotation = quat(tposeDirection);
        p.initRotationInverse = inverse(p.initRotation);
        // parent.initRot * p.defaultRot = p.initRot;
        // p.defaultRot =  (parent.initRot)-1 * p,
        p.tposeDefaultJointRotation = jointPoints[p.parentIndex].initRotationInverse * tposeDirection;
        p.tposeDefaultJointRotationInverse = inverse(p.tposeDefaultJointRotation);

        p.alternativeUp = fitplay::forward;
    }

    void FittingFk::updateJointPoint(jointPoint& p, vec3 const& startPoint, vec3 const& endPoint, vec3 const& jointForward) {
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
        //vec3& parentPosDirection3d = jointPoints[p.parentIndex].posDirection3d;
        quat& parentFkRotation = jointPoints[p.parentIndex].fkRotation;
        //fkRotation = parent.fkRotation * (defatultJointRotation * jointLocationRotation)
        p.jointGlobalRotation = inverse(parentFkRotation) * p.fkRotation;
        //global = default * local 
        p.jointLocalRotation = inverse(p.tposeDefaultJointRotation) * p.jointGlobalRotation;
        //calculate fk
        p.fromPointFk = jointPoints[p.parentIndex].toPointFk;
        //TODO fk calculation
        p.toPointFk = fkToNextPoint(p.fromPointFk, p.fkRotation, p.boneLength); //(p.fromPointFk, parentPosDirection3d, p.fkRotation, p.boneLength);
    }

        
    void FittingFk::getLookRotation(jointPoint& p, vec3 const& parentJoint, vec3 const& childJoint){
        
        //UnityTranslation x,y revert
        p.fkRotation = lookatRotationSafe( childJoint,parentJoint,p.alternativeUp);
        

        //here read from parent pose direction 
        //vec3& parentPosDirection3d = jointPoints[p.parentIndex].posDirection3d;
        quat& parentFkRotation = jointPoints[p.parentIndex].fkRotation;
        //fkRotation = parent.fkRotation * (defatultJointRotation * jointLocationRotation)
        p.jointGlobalRotation = inverse(parentFkRotation) * p.fkRotation;
        //global = default * local 
        p.jointLocalRotation = inverse(p.tposeDefaultJointRotation) * p.jointGlobalRotation;
        //calculate fk
        p.fromPointFk = jointPoints[p.parentIndex].toPointFk;
        //TODO fk calculation
        p.toPointFk = fkToNextPoint(p.fromPointFk, p.fkRotation, p.boneLength); //(p.fromPointFk, parentPosDirection3d, p.fkRotation, p.boneLength);

    }

   
    void FittingFk::updateLandmarks(landmarks const & landmarkData) {
        /**
         * @brief simple kinamatic constraints applied here
         */
        // hip forwarding
        vec3 hipRightForwarding = normalize(cross(landmarkData[R_HIP] - landmarkData[L_HIP], fitplay::up));
        // update root joint with landmarks froward 
        jointPoints[18].posLeft3d = normalize(crossSafe(fitplay::down, hipRightForwarding));
        jointPoints[18].fkRotation = lookatRotationSafe(fitplay::down, jointPoints[18].posLeft3d, fitplay::forward);
        // up body forwarding
        vec3 spineTwistingForwarding = normalize(cross(landmarkData[R_SHOULDER] - landmarkData[L_SHOULDER], fitplay::up));
        // armpit forwarding 
        // add 0.1 forward to affiate default totate to font
        vec3 rshoulderToArmKinamaticForwarding = normalize(landmarkData[R_WRIST] - landmarkData[R_ARM]);
        vec3 lshoulderToArmKinamaticForwarding = normalize(landmarkData[L_WRIST] - landmarkData[L_ARM]);
        vec3 rarmToShoulderKinamaticForwarding = normalize(landmarkData[R_SHOULDER] - landmarkData[R_ARM]);
        vec3 larmToShoulderKinamaticForwarding = normalize(landmarkData[L_SHOULDER] - landmarkData[L_ARM]);

        // knee forwarding  
        /** TODO need test */
        vec3 rlegToAnkleKinamaticForwarding = - normalize(- hipRightForwarding *0.3f + landmarkData[R_ANKLE] - landmarkData[R_KNEE]);
        vec3 llegToAnkleKinamaticForwarding = - normalize(- hipRightForwarding *0.3f + landmarkData[L_ANKLE] - landmarkData[L_KNEE]);
        vec3 rankleToLegKinamaticForwarding = - normalize(- hipRightForwarding *0.3f + landmarkData[R_HIP] - landmarkData[R_KNEE]);
        vec3 lAnkleToLegKinamaticForwarding = - normalize(- hipRightForwarding *0.3f + landmarkData[L_HIP] - landmarkData[L_KNEE]);

        //TODO:
        // wrist forwarding 
        vec3 rwristForwarding = fitplay::forward;
        vec3 lwristForwarding = fitplay::forward;
        // ankle forwarding 
        vec3 rankleForwarding = fitplay::forward;
        vec3 lankleForwarding = fitplay::forward;

        //calculate rotation in each joints, to generate bones
        //up half human
        //glm::quat neckRotation = glm::rotation(up, vectorFromToPoint(hipcenter, neck));
        updateJointPoint(jointPoints[0], landmarkData[HIP_CENTER], landmarkData[NECK], hipRightForwarding);
        updateJointPoint(jointPoints[1], landmarkData[NECK], landmarkData[HEAD], spineTwistingForwarding);
        updateJointPoint(jointPoints[2], landmarkData[NECK], landmarkData[L_SHOULDER], spineTwistingForwarding);
        updateJointPoint(jointPoints[3], landmarkData[NECK], landmarkData[R_SHOULDER], spineTwistingForwarding);
        updateJointPoint(jointPoints[4], landmarkData[L_SHOULDER], landmarkData[L_ARM], lshoulderToArmKinamaticForwarding);
        updateJointPoint(jointPoints[5], landmarkData[R_SHOULDER], landmarkData[R_ARM], rshoulderToArmKinamaticForwarding);
        updateJointPoint(jointPoints[6], landmarkData[L_ARM], landmarkData[L_WRIST], larmToShoulderKinamaticForwarding);
        updateJointPoint(jointPoints[7], landmarkData[R_ARM], landmarkData[R_WRIST], rarmToShoulderKinamaticForwarding);
        updateJointPoint(jointPoints[8], landmarkData[L_WRIST], landmarkData[L_HAND], lwristForwarding);
        updateJointPoint(jointPoints[9], landmarkData[R_WRIST], landmarkData[R_HAND], rwristForwarding);
        updateJointPoint(jointPoints[10], fitplay::down, landmarkData[HIP_CENTER], hipRightForwarding);
        updateJointPoint(jointPoints[11], fitplay::down, landmarkData[HIP_CENTER], hipRightForwarding); // rhip sprcial 
        updateJointPoint(jointPoints[12], landmarkData[L_HIP], landmarkData[L_KNEE], llegToAnkleKinamaticForwarding);
        updateJointPoint(jointPoints[13], landmarkData[R_HIP], landmarkData[R_KNEE], rlegToAnkleKinamaticForwarding);
        updateJointPoint(jointPoints[14], landmarkData[L_KNEE], landmarkData[L_ANKLE], lAnkleToLegKinamaticForwarding);
        updateJointPoint(jointPoints[15], landmarkData[R_KNEE], landmarkData[R_ANKLE], rankleToLegKinamaticForwarding);
        updateJointPoint(jointPoints[16], landmarkData[L_ANKLE], landmarkData[L_FOOT], lankleForwarding);
        updateJointPoint(jointPoints[17], landmarkData[R_ANKLE], landmarkData[R_FOOT], rankleForwarding);

        //GetWorldForwardRotation
        //X&ZDir onCameraPlane ---GetLookRotation(A,B) B-->X,A -->-Z

        getLookRotation(jointPoints[0],normalize(landmarkData[HIP_CENTER]-landmarkData[R_HIP]),normalize(landmarkData[HIP_CENTER]-landmarkData[NECK]));
        getLookRotation(jointPoints[1],normalize(landmarkData[NECK]-landmarkData[R_SHOULDER]),normalize(landmarkData[NECK]-landmarkData[HEAD]));
        getLookRotation(jointPoints[2],normalize(landmarkData[L_HIP]-landmarkData[L_SHOULDER]),normalize(landmarkData[R_SHOULDER]-landmarkData[L_SHOULDER]));
        getLookRotation(jointPoints[3],normalize(landmarkData[R_SHOULDER]-landmarkData[R_HIP]),normalize(landmarkData[L_SHOULDER]-landmarkData[R_SHOULDER]));
        getLookRotation(jointPoints[4],normalize(landmarkData[L_HIP]-landmarkData[L_SHOULDER]),normalize(landmarkData[L_SHOULDER]-landmarkData[L_ARM]));
        getLookRotation(jointPoints[5],normalize(landmarkData[R_SHOULDER]-landmarkData[R_HIP]),normalize(landmarkData[R_SHOULDER]-landmarkData[R_ARM]));

        getLookRotation(jointPoints[6],normalize(landmarkData[L_HIP]-landmarkData[L_SHOULDER]),normalize(landmarkData[L_ARM]-landmarkData[L_WRIST]));
        getLookRotation(jointPoints[7],normalize(landmarkData[R_SHOULDER]-landmarkData[R_HIP]),normalize(landmarkData[R_ARM]-landmarkData[R_WRIST]));

        getLookRotation(jointPoints[8],fitplay::backward,normalize(landmarkData[L_WRIST]-landmarkData[L_HAND]));
        getLookRotation(jointPoints[9],fitplay::forward,normalize(landmarkData[R_WRIST]-landmarkData[R_HAND]));
        getLookRotation(jointPoints[10],normalize(landmarkData[HIP_CENTER]-landmarkData[R_HIP]),fitplay::down);//Pelvis
        getLookRotation(jointPoints[11],normalize(landmarkData[HIP_CENTER]-landmarkData[R_HIP]),fitplay::down);
        getLookRotation(jointPoints[12],normalize(landmarkData[R_HIP]-landmarkData[L_HIP]),normalize(landmarkData[L_HIP]-landmarkData[L_KNEE]));
        getLookRotation(jointPoints[13],normalize(landmarkData[R_HIP]-landmarkData[L_HIP]),normalize(landmarkData[R_HIP]-landmarkData[R_KNEE]));

        vec3 leftFootRight = normalize(cross(normalize(landmarkData[L_KNEE]-landmarkData[L_ANKLE]),normalize(landmarkData[L_FOOT]-landmarkData[L_ANKLE])));
        vec3 rightFootRight = normalize(cross(normalize(landmarkData[R_KNEE]-landmarkData[R_ANKLE]),normalize(landmarkData[R_FOOT]-landmarkData[R_ANKLE])));
        vec3 leftFoot = normalize(landmarkData[L_FOOT]-landmarkData[L_ANKLE]);
        vec3 rightFoot = normalize(landmarkData[R_FOOT]-landmarkData[R_ANKLE]);
        
        quat l_angleAxis = angleAxis(footPitchAngle,vec3(leftFootRight.x,leftFootRight.y,leftFootRight.z));
        quat r_angleAxis = angleAxis(footPitchAngle,vec3(rightFootRight.x,rightFootRight.y,rightFootRight.z));

        //Rotate Vector
        vec3 l_u = vec3(l_angleAxis.x, l_angleAxis.y, l_angleAxis.z);
        vec3 r_u = vec3(r_angleAxis.x, r_angleAxis.y, r_angleAxis.z);
        // Extract the scalar part of the quaternion
        float l_s = l_angleAxis.w;
        float r_s = r_angleAxis.w;
        // Do the math
        vec3 lv_prime = 2.0f * dot(l_u, leftFoot) * l_u + (l_s*l_s - dot(l_u, l_u)) * leftFoot + 2.0f * l_s * cross(l_u, leftFoot);
        vec3 rv_prime = 2.0f * dot(r_u, rightFoot) * r_u + (r_s*r_s - dot(r_u, r_u)) * rightFoot + 2.0f * r_s * cross(r_u, rightFoot);
        getLookRotation(jointPoints[14],leftFootRight,normalize(landmarkData[L_KNEE]-landmarkData[L_ANKLE]));
        getLookRotation(jointPoints[15],rightFootRight,normalize(landmarkData[R_KNEE]-landmarkData[R_ANKLE]));

        getLookRotation(jointPoints[16],leftFootRight,lv_prime);
        getLookRotation(jointPoints[17],rightFootRight,rv_prime);



        
    }

    vec3 FittingFk::crossSafe(glm::vec3 const& direction, glm::vec3 const& fov) {
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

    quat FittingFk::lookatRotationSafe(glm::vec3 const& direction, glm::vec3 const& up, glm::vec3 const& alternativeUp) {
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

    //vec3 FittingFk::fkToNextPoint(vec3 const & startPoint, vec3 const & boneDirection, quat const & boneRotation, float const & bontLength) {
    vec3 FittingFk::fkToNextPoint(vec3 const & startPoint, quat const & boneRotation, float const & bontLength) {
        //toward x direction 
        auto direction = glm::rotate(boneRotation, vec3(bontLength, 0.0f, 0.0f));
        return vec3(startPoint[0] + direction[0] * bontLength, startPoint[1] + direction[1] * bontLength, startPoint[2] + direction[2] * bontLength);
    }

    vec3 FittingFk::vectorFromToPoint(vec3 const & from, vec3 const & to) {
        return glm::normalize(vec3(to[0] - from[0], to[1] - from[1], to[2] - from[2]));
    }
}

#endif