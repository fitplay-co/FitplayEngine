#ifndef FITPLAY_fitting
#define FITPLAY_fitting

#include <string>
#include <vector>
#include "json.hpp"
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"

using namespace nlohmann;
using namespace glm;
using namespace std;

const int jointPointSize = 18;
static constexpr quat rotateFromLookatZtoX = quat(0.5, -0.5, -0.5, -0.5);

namespace fitplay {
    static constexpr vec3 up = vec3 (0.0f, 1.0f, 0.0f);
    static constexpr vec3 down = vec3 (0.0f, -1.0f, 0.0f);
    static constexpr vec3 left = vec3 (-1.0f, 0.0f, 0.0f);
    static constexpr vec3 right = vec3 (1.0f, 0.0f, 0.0f);  
    static constexpr vec3 forward = vec3(0.0f, 0.0f, 1.0f);
    static constexpr vec3 backward = vec3(0.0f, 0.0f, -1.0f);

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
            fitting();
            ~ fitting();
            void addJointPoint(jointPoint& p, std::string name, int parentIndex, float boneLength,  vec3 const& tposeDirection);
            //update joint point in each process, to calculate fk landmark and rotation and do fitting
            //joint jointForward is right aixs of joint look at and refers to t pose y asix in unity 
            void updateJointPoint(jointPoint& p, vec3 const& startPoint, vec3 const& endPoint, vec3 const& jointForward);
            void process(json& data);
            void updateLandmarks(landmarks const & landmarkData);
            vec3 vectorFromToPoint(vec3 const & from, vec3 const & to);
            vec3 readLandmarkPointVector(int point,const json& data);
            vec3 fkToNextPoint(vec3 const& startPoint, vec3 const& boneDirection, quat const& boneRotation, float const& bontLength);
            vec3 crossSafe(glm::vec3 const& direction, glm::vec3 const& fov);
            quat lookatRotationSafe(glm::vec3 const& direction, glm::vec3 const& up, glm::vec3 const& alternativeUp);
            void debugPrintQuat(std::string jointName, std::string name, quat& quat, json &data);
            void writeRotation(glm::quat const & r, json & data, int index, std::string name);
            void writeFK(vec3 const& point, json & data, int index, std::string name);
    };

    fitting::fitting() {
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

        //last point for root reference from neck, hip points
        addJointPoint(jointPoints[18], "root", -1, 0.4f, down);

        addJointPoint(jointPoints[0], "neck", 18, 0.4f, down);
        addJointPoint(jointPoints[1], "head", 0, 0.1f, down);
        addJointPoint(jointPoints[2], "lshoulder", 0, 0.1f, right);
        addJointPoint(jointPoints[3], "rshoulder", 0, 0.1f, left);
        addJointPoint(jointPoints[4], "luparm", 2, 0.2f, right);
        addJointPoint(jointPoints[5], "ruparm", 3, 0.2f, left);
        addJointPoint(jointPoints[6], "llowarm", 4, 0.2f, right);
        addJointPoint(jointPoints[7], "rlowarm", 5, 0.2f, left);
        addJointPoint(jointPoints[8], "lhand", 6, 0.1f, right);
        addJointPoint(jointPoints[9], "rhand", 7, 0.1f, left);
        addJointPoint(jointPoints[10], "lhip", 18, 0.1f, right);
        addJointPoint(jointPoints[11], "rhip", 18, 0.1f, right);
        addJointPoint(jointPoints[12], "lupleg", 10, 0.3, up);
        addJointPoint(jointPoints[13], "rupleg", 11, 0.3f, up);
        addJointPoint(jointPoints[14], "llowleg", 12, 0.3f, up);
        addJointPoint(jointPoints[15], "rlowleg", 13, 0.3f, up);
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

    void fitting::debugPrintQuat(std::string jointName, std::string quatName, quat& quat, json& data) {
        glm::vec3 euler = eulerAngles(quat) * 180.f/ 3.14159f ;
        data["fitting"][jointName][quatName]["x"] =  quat.x;
        data["fitting"][jointName][quatName]["y"] =  quat.y;
        data["fitting"][jointName][quatName]["z"] =  quat.z;
        data["fitting"][jointName][quatName]["w"] =  quat.w;

        data["fitting"][jointName][quatName]["e1"] =  euler[0];
        data["fitting"][jointName][quatName]["e2"] =  euler[1];
        data["fitting"][jointName][quatName]["e3"] =  euler[2];

    }

    void fitting::process(json& data) {
        data["fitting"]["wasm_fitting_version"] = "0.0.1";
        landmarks landmarkData;

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
        // infer nect and hip center
        landmarkData.hipcenter = vec3(0.0f ,0.0f ,0.0f);
        landmarkData.neck = vec3((landmarkData.lshoulder[0] 
                + landmarkData.rshoulder[0])/2.0f, (landmarkData.lshoulder[1] 
                + landmarkData.rshoulder[1])/2.0f, (landmarkData.lshoulder[2] + landmarkData.rshoulder[2])/2.0f);

        updateLandmarks(landmarkData);

        writeFK(landmarkData.hipcenter, data, 0 , "hipcenter");
        writeFK(landmarkData.head, data, 1, "head");
        writeFK(landmarkData.lshoulder, data, 2,"lshoulder");
        writeFK(landmarkData.rshoulder, data, 3, "rshoulder");
        writeFK(landmarkData.larm, data, 4, "larm");
        writeFK(landmarkData.rarm, data, 5, "rarm");
        writeFK(landmarkData.lwrist, data, 6, "lwrist");
        writeFK(landmarkData.rwrist, data, 7, "rwrist");
        writeFK(landmarkData.lhand, data, 8, "lhand");
        writeFK(landmarkData.rhand, data, 9, "rhand");
        writeFK(landmarkData.lhip, data, 10, "lhip");
        writeFK(landmarkData.rhip, data, 11, "rhip");
        writeFK(landmarkData.lknee, data, 12, "lknee");
        writeFK(landmarkData.rknee, data, 13, "rknee");
        writeFK(landmarkData.lankle, data, 14, "lankle");
        writeFK(landmarkData.rankle, data, 15, "rankle");
        writeFK(landmarkData.lfoot, data, 16, "lfoot");
        writeFK(landmarkData.rfoot, data, 17, "rfoot");
        writeFK(landmarkData.neck, data, 18, "neck");

        for(int i = 0; i < jointPointSize; i ++) {
            writeRotation(jointPoints[i].fkRotation, data, i, jointPoints[i].jointName);
        }
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
        updateJointPoint(jointPoints[10], landmarkData.hipcenter, landmarkData.lhip, hipRightForwarding);
        updateJointPoint(jointPoints[11],  landmarkData.rhip, landmarkData.hipcenter, hipRightForwarding); // rhip sprcial 
        updateJointPoint(jointPoints[12], landmarkData.lhip, landmarkData.lknee, llegToAnkleKinamaticForwarding);
        updateJointPoint(jointPoints[13], landmarkData.rhip, landmarkData.rknee, rlegToAnkleKinamaticForwarding);
        updateJointPoint(jointPoints[14], landmarkData.lknee, landmarkData.lankle, lAnkleToLegKinamaticForwarding);
        updateJointPoint(jointPoints[15], landmarkData.rknee, landmarkData.rankle, rankleToLegKinamaticForwarding);
        updateJointPoint(jointPoints[16], landmarkData.lankle, landmarkData.lfoot, lankleForwarding);
        updateJointPoint(jointPoints[17], landmarkData.rankle, landmarkData.rfoot, rankleForwarding);
    }

    void fitting::writeFK(vec3 const& point, json & data, int index, std::string name) {
        data["fitting"]["keypoints3D"][index]["x"] = point[0];
        data["fitting"]["keypoints3D"][index]["y"] = point[1];
        data["fitting"]["keypoints3D"][index]["z"] = point[2];
        data["fitting"]["keypoints3D"][index]["name"] = name;
    }

    void fitting::writeRotation(glm::quat const & r, json & data, int index, std::string name) {
        data["fitting"]["rotation"][index]["w"] = r.w;
        data["fitting"]["rotation"][index]["x"] = r.x;
        data["fitting"]["rotation"][index]["y"] = r.y;
        data["fitting"]["rotation"][index]["z"] = r.z;
        data["fitting"]["rotation"][index]["name"] = name;
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

    vec3 fitting::readLandmarkPointVector(int point,const json& data) {
        float xStart = data["keypoints3D"][point]["x"];
        float yStart = (0.0f - float(data["keypoints3D"][point]["y"]));
        float zStart = data["keypoints3D"][point]["z"];
        return vec3(xStart, yStart, zStart);
    }

    // vec3 fitting::calculatejointQuat(int startPoint, int endPoint) {

    // }
}

#endif
        // data["fitting"][p.jointName]["fromx"] =  p.fromPoint[0];
        // data["fitting"][p.jointName]["fromy"] =  p.fromPoint[1];
        // data["fitting"][p.jointName]["fromz"] =  p.fromPoint[2];
        // data["fitting"][p.jointName]["tox"] =  p.toPoint[0];
        // data["fitting"][p.jointName]["toy"] =  p.toPoint[1];
        // data["fitting"][p.jointName]["toz"] =  p.toPoint[2];
        
        // data["fitting"][p.jointName]["fx"] =  p.posDirection3d[0];
        // data["fitting"][p.jointName]["fy"] =  p.posDirection3d[1];
        // data["fitting"][p.jointName]["fz"] =  p.posDirection3d[2];

        // data["fitting"][p.jointName]["upx"] =  p.alternativeUp[0];
        // data["fitting"][p.jointName]["upy"] =  p.alternativeUp[1];
        // data["fitting"][p.jointName]["upz"] =  p.alternativeUp[2];
        
        // data["fitting"][p.jointName]["px"] =  parentPosDirection3d[0];
        // data["fitting"][p.jointName]["py"] =  parentPosDirection3d[1];
        // data["fitting"][p.jointName]["pz"] =  parentPosDirection3d[2];
        
        // debugPrintQuat(p.jointName, "tposeInitRotation", p.initRotation, data);
        // debugPrintQuat(p.jointName, "tposeDefaultJointRotation", p.tposeDefaultJointRotation, data);
        // debugPrintQuat(p.jointName, "fkRotation", p.fkRotation, data);
        // debugPrintQuat(p.jointName, "parentfkRotation", parentFkRotation, data);
        // debugPrintQuat(p.jointName, "jointLocalRotation", p.jointLocalRotation, data);
        
        //test default joint rotation to fk rotation

        //}