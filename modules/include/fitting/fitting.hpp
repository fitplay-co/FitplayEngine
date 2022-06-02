#include <string>
#include <vector>
#include "json.hpp"
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"

using namespace nlohmann;
using namespace glm;

const int jointPointSize = 18;

namespace fitplay {
    struct jointPoint {
        std::string jointName;
        int parentIndex = 0;
        float boneLength = 0.0f;

        vec3 fromPoint;
        vec3 toPoint;

        vec3 posDirection3d = vec3 (0.0f, 1.0f, 0.0f); //by default to up 
        quat fkRotation;
        vec3 fromPointFk =  vec3 (0.0f, 0.0f, 0.0f);
        vec3 toPointFk =  vec3 (0.0f, 0.0f, 0.0f);
    };

    class fitting {
        private:
            /* data */
        public:
            jointPoint jointPoints[jointPointSize + 1];
            fitting();
            ~ fitting();
            void addJointPoint(jointPoint& p, std::string name, int parentIndex, float boneLength);
            //update joint point in each process, to calculate fk landmark and rotation and do fitting
            void updateJointPoint(jointPoint& p, vec3 startPoint, vec3 endPoint);
            void process(json& data);
            vec3 vectorFromToPoint(vec3 const & from, vec3 const & to);
            vec3 readLandmarkPointVector(int point,const json& data);
            vec3 fkToNextPoint(vec3 const& startPoint, vec3 const& boneDirection, quat const& boneRotation, float const& bontLength);
            void writeRotation(glm::quat const & r, json & data, int index, std::string name);
            void writeFK(vec3 const& point, json & data, int index, std::string name);
    };

    fitting::fitting() {
        addJointPoint(jointPoints[0], "neck", 18, 0.4f);
        addJointPoint(jointPoints[1], "head", 0, 0.1f);
        addJointPoint(jointPoints[2], "lshoulder", 0, 0.1f);
        addJointPoint(jointPoints[3], "rshoulder", 0, 0.1f);
        addJointPoint(jointPoints[4], "luparm", 2, 0.2f);
        addJointPoint(jointPoints[5], "ruparm", 3, 0.2f);
        addJointPoint(jointPoints[6], "llowarm", 4, 0.2f);
        addJointPoint(jointPoints[7], "rlowarm", 5, 0.2f);
        addJointPoint(jointPoints[8], "lhand", 6, 0.1f);
        addJointPoint(jointPoints[9], "rhand", 7, 0.1f);
        addJointPoint(jointPoints[10], "lhip", 18, 0.1f);
        addJointPoint(jointPoints[11], "rhip", 18, 0.1f);
        addJointPoint(jointPoints[12], "lupleg", 10, 0.3);
        addJointPoint(jointPoints[13], "rupleg", 11, 0.3f);
        addJointPoint(jointPoints[14], "llowleg", 12, 0.3f);
        addJointPoint(jointPoints[15], "rlowleg", 13, 0.3f);
        addJointPoint(jointPoints[16], "lfoot", 14, 0.1f);
        addJointPoint(jointPoints[17], "rfoot", 15, 0.1f);

        //last point for root reference from neck, hip points
        addJointPoint(jointPoints[18], "root", -1, 0.4f);
    }

    fitting::~fitting() {}

    void fitting::addJointPoint(jointPoint& p, std::string name, int parentIndex, float boneLength) {
        p.jointName = name;
        p.parentIndex = parentIndex;
        p.boneLength = boneLength;
    }

    void fitting::updateJointPoint(jointPoint& p, vec3 startPoint, vec3 endPoint) {
        //first step update start end points 
        p.fromPoint = startPoint;
        p.toPoint = endPoint;

        //calculate pose Direction 3d
        p.posDirection3d = vectorFromToPoint(startPoint, endPoint);
        p.fkRotation = glm::rotation(jointPoints[p.parentIndex].posDirection3d, p.posDirection3d);

        //calculate fk
        p.fromPointFk = jointPoints[p.parentIndex].toPointFk;
        p.toPointFk = fkToNextPoint(p.fromPointFk, jointPoints[p.parentIndex].posDirection3d, p.fkRotation, p.boneLength);
    }

    void fitting::process(json& data) {
        data["fitting"]["wasm_fitting_version"] = "0.0.1";
        //check each joint direction 
        //                                     head
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
        vec3 up = vec3 (0.0f, 1.0f, 0.0f);
        vec3 down =  vec3 (0.0f, -1.0f, 0.0f);
        vec3 left =  vec3 (-1.0f, 0.0f, 0.0f);
        vec3 right =  vec3 (1.0f, 0.0f, 0.0f);

        // transfer to vec3 for each landmarks
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
        updateJointPoint(jointPoints[0], hipcenter, neck);
        updateJointPoint(jointPoints[1], neck, head);
        updateJointPoint(jointPoints[2], neck, lshoulder);
        updateJointPoint(jointPoints[3], neck, rshoulder);
        updateJointPoint(jointPoints[4], lshoulder, larm);
        updateJointPoint(jointPoints[5], rshoulder, rarm);
        updateJointPoint(jointPoints[6], larm, lwrist);
        updateJointPoint(jointPoints[7], rarm, rwrist);
        updateJointPoint(jointPoints[8], lwrist, lhand);
        updateJointPoint(jointPoints[9], rwrist, rhand);
        updateJointPoint(jointPoints[10], hipcenter, lhip);
        updateJointPoint(jointPoints[11], hipcenter, rhip);
        updateJointPoint(jointPoints[12], lhip, lknee);
        updateJointPoint(jointPoints[13], rhip, rknee);
        updateJointPoint(jointPoints[14], lknee, lankle);
        updateJointPoint(jointPoints[15], rknee, rankle);
        updateJointPoint(jointPoints[16], lankle, lfoot);
        updateJointPoint(jointPoints[17], rankle, rfoot);

        for(int i = 0; i < jointPointSize; i ++) {
            writeRotation(jointPoints[i].fkRotation, data, i, jointPoints[i].jointName);
            writeFK(jointPoints[i].toPointFk, data, i, jointPoints[i].jointName);
        }
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

    vec3 fitting::fkToNextPoint(vec3 const & startPoint, vec3 const & boneDirection, quat const & boneRotation, float const & bontLength) {
        auto direction = glm::rotate(boneRotation, boneDirection);
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
