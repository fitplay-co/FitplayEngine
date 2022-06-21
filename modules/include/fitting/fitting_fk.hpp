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
            void updateLandmarks(landmarks const & landmarkData);
            vec3 vectorFromToPoint(vec3 const & from, vec3 const & to);
            vec3 readLandmarkPointVector(int point, const PoseData::Pose* data);
            vec3 readLandmarkPointVectorMirror(int point, const PoseData::Pose* data);
            vec3 fkToNextPoint(vec3 const& startPoint, vec3 const& boneDirection, quat const& boneRotation, float const& bontLength);
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
        auto upward = cross(tposeDirection, fitplay::forward);
        p.initRotation = lookatRotationSafe(tposeDirection, upward, fitplay::forward);
        p.initRotationInverse = inverse(p.initRotation);
        // parent.initRot * p.defaultRot = p.initRot;
        // p.defaultRot =  (parent.initRot)-1 * p,
        p.tposeDefaultJointRotation = jointPoints[p.parentIndex].initRotationInverse * p.initRotation;
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
    
    void FittingFk::updateLandmarks(landmarks const & landmarkData) {
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

    vec3 FittingFk::fkToNextPoint(vec3 const & startPoint, vec3 const & boneDirection, quat const & boneRotation, float const & bontLength) {
        //toward x direction 
        auto direction = glm::rotate(boneRotation, vec3(bontLength, 0.0f, 0.0f));
        return vec3(startPoint[0] + direction[0] * bontLength, startPoint[1] + direction[1] * bontLength, startPoint[2] + direction[2] * bontLength);
    }

    vec3 FittingFk::vectorFromToPoint(vec3 const & from, vec3 const & to) {
        return glm::normalize(vec3(to[0] - from[0], to[1] - from[1], to[2] - from[2]));
    }
}

#endif