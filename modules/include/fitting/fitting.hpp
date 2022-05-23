#include <string>
#include <vector>
#include "json.hpp"
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"

using namespace nlohmann;
using namespace glm;

const int jointPointSize = 18;
static constexpr quat rotateFromLookatZtoX = quat(0.5, -0.5, -0.5, -0.5);

namespace fitplay {
    struct jointPoint {
        std::string jointName;
        int parentIndex = 0;
        float boneLength = 0.0f;

        vec3 fromPoint;
        vec3 toPoint;

        vec3 posDirection3d = vec3 (0.0f, 1.0f, 0.0f); //by default to up 

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

        vec3 fromPointFk =  vec3 (0.0f, 0.0f, 0.0f);
        vec3 alternativeUp = vec3 (1.0f, 0.0f, 0.0f);
        vec3 toPointFk =  vec3 (0.0f, 0.0f, 0.0f);
    };

    class fitting {
        private:
            /* data */
        public:
            jointPoint jointPoints[jointPointSize + 1];
            fitting();
            ~ fitting();
            void addJointPoint(jointPoint& p, std::string name, int parentIndex, float boneLength,  vec3 const& tposeDirection, vec3 const& upward);
            //update joint point in each process, to calculate fk landmark and rotation and do fitting
            void updateJointPoint(jointPoint& p, vec3 startPoint, vec3 endPoint, json &data);
            void process(json& data);
            vec3 vectorFromToPoint(vec3 const & from, vec3 const & to);
            vec3 readLandmarkPointVector(int point,const json& data);
            vec3 fkToNextPoint(vec3 const& startPoint, vec3 const& boneDirection, quat const& boneRotation, float const& bontLength);
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
        vec3 up = vec3 (0.0f, 1.0f, 0.0f);
        vec3 down = vec3 (0.0f, -1.0f, 0.0f);
        vec3 left = vec3 (1.0f, 0.0f, 0.0f);
        vec3 right = vec3 (-1.0f, 0.0f, 0.0f);  // TODO still dont know why right direction towards here based on experiment
        vec3 forward = vec3(0.0f, 0.0f, 1.0f);
        vec3 backward = vec3(0.0f, 0.0f, -1.0f);

        addJointPoint(jointPoints[0], "neck", 18, 0.4f, down, left);
        addJointPoint(jointPoints[1], "head", 0, 0.1f, down, left);
        addJointPoint(jointPoints[2], "lshoulder", 0, 0.1f, right, down);
        addJointPoint(jointPoints[3], "rshoulder", 0, 0.1f, left, up);
        addJointPoint(jointPoints[4], "luparm", 2, 0.2f, right, down);
        addJointPoint(jointPoints[5], "ruparm", 3, 0.2f, left, up);
        addJointPoint(jointPoints[6], "llowarm", 4, 0.2f, right, down);
        addJointPoint(jointPoints[7], "rlowarm", 5, 0.2f, left, up);
        addJointPoint(jointPoints[8], "lhand", 6, 0.1f, right, down);
        addJointPoint(jointPoints[9], "rhand", 7, 0.1f, left, up);
        addJointPoint(jointPoints[10], "lhip", 18, 0.1f, right, down);
        addJointPoint(jointPoints[11], "rhip", 18, 0.1f, left, up);
        addJointPoint(jointPoints[12], "lupleg", 10, 0.3, up, right);
        addJointPoint(jointPoints[13], "rupleg", 11, 0.3f, up, right);
        addJointPoint(jointPoints[14], "llowleg", 12, 0.3f, up, right);
        addJointPoint(jointPoints[15], "rlowleg", 13, 0.3f, up, right);
        addJointPoint(jointPoints[16], "lfoot", 14, 0.1f, up, right);
        addJointPoint(jointPoints[17], "rfoot", 15, 0.1f, up, right);

        //last point for root reference from neck, hip points
        addJointPoint(jointPoints[18], "root", -1, 0.4f, down, up);
    }

    fitting::~fitting() {}

    void fitting::addJointPoint(jointPoint& p, std::string name, int parentIndex, float boneLength, vec3 const& tposeDirection, vec3 const& upward) {
        p.jointName = name;
        p.parentIndex = parentIndex;
        p.boneLength = boneLength;
        p.initRotation = lookatRotationSafe(tposeDirection, upward, vec3 (0.0f, 0.0f, 1.0f));
        p.initRotationInverse = inverse(p.initRotation);
        p.tposeDefaultJointRotation = jointPoints[p.parentIndex].initRotationInverse * p.initRotation;
        p.tposeDefaultJointRotationInverse = inverse(p.tposeDefaultJointRotation);
    }

    void fitting::updateJointPoint(jointPoint& p, vec3 startPoint, vec3 endPoint, json& data) {
        //first step update start end points 
        p.fromPoint = startPoint;
        p.toPoint = endPoint;

        //calculate pose Direction 3d  node - child as forward vector 
        p.posDirection3d = vectorFromToPoint(endPoint, startPoint);
        //here read from parent pose direction 
        vec3& parentPosDirection3d = jointPoints[p.parentIndex].posDirection3d;
        quat& parentFkRotation = jointPoints[p.parentIndex].fkRotation;

        p.fkRotation = lookatRotationSafe(p.posDirection3d, vec3(0.0f, 1.0f, 0.0f) , p.alternativeUp);

        //fkRotation = jointLocationRotation *（parent.fkRotation * defatultJointRotation）
        p.jointLocalRotation = inverse(p.tposeDefaultJointRotation * parentFkRotation) * p.fkRotation ;

        //calculate fk
        p.fromPointFk = jointPoints[p.parentIndex].toPointFk;
        p.toPointFk = fkToNextPoint(p.fromPointFk, parentPosDirection3d, p.fkRotation, p.boneLength);
        
        // data["fitting"][p.jointName]["fx"] =  p.posDirection3d[0];
        // data["fitting"][p.jointName]["fy"] =  p.posDirection3d[1];
        // data["fitting"][p.jointName]["fz"] =  p.posDirection3d[2];

        // data["fitting"]["rotationdebug"][p.jointName]["px"] =  parentPosDirection3d[0];
        // data["fitting"]["rotationdebug"][p.jointName]["py"] =  parentPosDirection3d[1];
        // data["fitting"]["rotationdebug"][p.jointName]["pz"] =  parentPosDirection3d[2];

        debugPrintQuat(p.jointName, "tposeInitRotation", p.initRotation, data);
        debugPrintQuat(p.jointName, "fkRotation", p.fkRotation, data);
        debugPrintQuat(p.jointName, "parentfkRotation", parentFkRotation, data);
        debugPrintQuat(p.jointName, "tposeDefaultJointRotation", p.tposeDefaultJointRotation, data);
        debugPrintQuat(p.jointName, "jointLocalRotation", p.jointLocalRotation, data);
        auto a  = quat(0.5, -0.5, -0.5, -0.5);
        debugPrintQuat(p.jointName, "zzjointLocalRotation", a, data);
        auto converted =  cross(p.initRotation , a);
        debugPrintQuat(p.jointName, "zzjointCrossLocalRotation", converted, data);

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

        vec3 head = readLandmarkPointVector(0, data);
        vec3 lshoulder = readLandmarkPointVector(11, data); 
        vec3 rshoulder = readLandmarkPointVector(12, data);
        vec3 larm = readLandmarkPointVector(13, data);
        vec3 rarm = readLandmarkPointVector(14, data); 
        vec3 lwrist = readLandmarkPointVector(15, data); 
        vec3 rwrist = readLandmarkPointVector(16, data); 
        vec3 lhand = readLandmarkPointVector(19, data); 
        vec3 rhand = readLandmarkPointVector(20, data);
        vec3 lhip = readLandmarkPointVector(23, data);
        vec3 rhip = readLandmarkPointVector(24, data);
        vec3 lknee = readLandmarkPointVector(25, data);
        vec3 rknee = readLandmarkPointVector(26, data);
        vec3 lankle = readLandmarkPointVector(27, data);
        vec3 rankle = readLandmarkPointVector(28, data);
        vec3 lfoot = readLandmarkPointVector(31, data);
        vec3 rfoot = readLandmarkPointVector(32, data); 
        // infer nect and hip center
        vec3 hipcenter = vec3(0.0f ,0.0f ,0.0f);
        vec3 neck = vec3((lshoulder[0] + rshoulder[0])/2.0f, (lshoulder[1] + rshoulder[1])/2.0f, (lshoulder[2] + rshoulder[2])/2.0f);
        //calculate rotation in each joints, to generate bones
        //up half human
        // glm::quat neckRotation = glm::rotation(up, vectorFromToPoint(hipcenter, neck));
        updateJointPoint(jointPoints[0], hipcenter, neck, data);
        updateJointPoint(jointPoints[1], neck, head, data);
        updateJointPoint(jointPoints[2], neck, lshoulder, data);
        updateJointPoint(jointPoints[3], neck, rshoulder, data);
        updateJointPoint(jointPoints[4], lshoulder, larm, data);
        updateJointPoint(jointPoints[5], rshoulder, rarm, data);
        updateJointPoint(jointPoints[6], larm, lwrist, data);
        updateJointPoint(jointPoints[7], rarm, rwrist, data);
        updateJointPoint(jointPoints[8], lwrist, lhand, data);
        updateJointPoint(jointPoints[9], rwrist, rhand, data);
        updateJointPoint(jointPoints[10], hipcenter, lhip, data);
        updateJointPoint(jointPoints[11], hipcenter, rhip, data);
        updateJointPoint(jointPoints[12], lhip, lknee, data);
        updateJointPoint(jointPoints[13], rhip, rknee, data);
        updateJointPoint(jointPoints[14], lknee, lankle, data);
        updateJointPoint(jointPoints[15], rknee, rankle, data);
        updateJointPoint(jointPoints[16], lankle, lfoot, data);
        updateJointPoint(jointPoints[17], rankle, rfoot, data);

        // for(int i = 0; i < jointPointSize; i ++) {
            // writeRotation(jointPoints[i].tposeDefaultJointRotation, data, i, jointPoints[i].jointName);
            // writeFK(jointPoints[i].toPointFk, data, i, jointPoints[i].jointName);
        // }
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

    quat fitting::lookatRotationSafe(glm::vec3 const& direction, glm::vec3 const& up, glm::vec3 const& alternativeUp) {
        //in unity backend and camera / depth model using left hand  
        //look at will rotate to z axis and y as up; whiles joints has x axis as direction and z as up; 
        //should do rotation to match with unity bone implementation
        if(glm::abs(glm::dot(direction, up)) > .9999f) {
            // Use alternative up 
            return quat(1.0f, 0.0f, 0.0f, 0.0f) ; // glm::quatLookAtLH(direction, alternativeUp);
        }
        else {
            return glm::quatLookAtLH(direction, up); //glm::quatLookAtLH(direction, up);
        }
    }

    vec3 fitting::fkToNextPoint(vec3 const & startPoint, vec3 const & boneDirection, quat const & boneRotation, float const & bontLength) {
        auto direction = glm::rotate(boneRotation, -boneDirection);
        return vec3(startPoint[0] + direction[0] * bontLength, startPoint[1] + direction[1] * bontLength, startPoint[2] + direction[2] * bontLength);
    }

    vec3 fitting::vectorFromToPoint(vec3 const & from, vec3 const & to) {
        return glm::normalize(vec3(to[0] - from[0], to[1] - from[1], to[2] - from[2]));
    }

    vec3 fitting::readLandmarkPointVector(int point,const json& data) {
        float xStart = data["keypoints3D"][point]["x"];
        float yStart = data["keypoints3D"][point]["y"];
        float zStart = data["keypoints3D"][point]["z"];
        return vec3(xStart, yStart, zStart);
    }

    // vec3 fitting::calculatejointQuat(int startPoint, int endPoint) {

    // }
}
