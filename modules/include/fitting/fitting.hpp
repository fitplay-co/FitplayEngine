#include <string>
#include <vector>
#include "json.hpp"
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"

using namespace nlohmann;
using namespace glm;

namespace fitplay {
    class fitting {
        private:
            /* data */
        public:
            fitting();
            ~ fitting();
            void process(json& data);
            vec3 vectorFromToPoint(vec3 const & from, vec3 const & to);
            vec3 readLandmarkPointVector(int point,const json& data);
            vec3 fkToNextPoint(vec3 const& startPoint, vec3 const& boneDirection, quat const& boneRotation, float const& bontLength);
            void writeRotation(glm::quat const & r, json & data, int index, std::string name);
            void writeFK(vec3 const& point, json & data, int index, std::string name);
    };

    fitting::fitting() {}
    fitting::~fitting() {}

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
        vec3 left =  vec3 (1.0f, 0.0f, 0.0f);
        vec3 right =  vec3 (-1.0f, 0.0f, 0.0f);

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
        glm::quat neckRotation = glm::rotation(vectorFromToPoint(hipcenter, neck), up);
        glm::quat headRotation = glm::rotation(vectorFromToPoint(neck, head), up);
        glm::quat lshoulderRotation = glm::rotation(vectorFromToPoint(neck, lshoulder), left);
        glm::quat rshoulderRotation = glm::rotation(vectorFromToPoint(neck, rshoulder), right);
        glm::quat larmRotation = glm::rotation(vectorFromToPoint(lshoulder, larm), left);
        glm::quat rarmRotation = glm::rotation(vectorFromToPoint(rshoulder, rarm), right);
        glm::quat lwristRotation = glm::rotation(vectorFromToPoint(larm, lwrist), left);
        glm::quat rwristRotation = glm::rotation(vectorFromToPoint(rarm, rwrist), right);
        glm::quat lhandRotation = glm::rotation(vectorFromToPoint(lwrist, lhand), left);
        glm::quat rhandRotation = glm::rotation(vectorFromToPoint(rwrist, rhand), right);

        //down half human
        glm::quat lhipRotation =  glm::rotation(vectorFromToPoint(hipcenter, lhip), left);
        glm::quat rhipRotation =  glm::rotation(vectorFromToPoint(hipcenter, rhip), right);
        glm::quat lkneeRotation =  glm::rotation(vectorFromToPoint(lhip, lknee), down);
        glm::quat rkneeRotation =  glm::rotation(vectorFromToPoint(rhip, rknee), down);
        glm::quat lankleRotation = glm::rotation(vectorFromToPoint(lknee, lankle), down);
        glm::quat rankleRotation = glm::rotation(vectorFromToPoint(rknee, rankle), down);
        glm::quat lfootRotation = glm::rotation(vectorFromToPoint(lankle, lfoot), down);
        glm::quat rfootRotation = glm::rotation(vectorFromToPoint(rankle, rfoot), down);

        //write rotations 
        writeRotation(neckRotation, data, 0, "neckRotation");
        writeRotation(headRotation, data, 1, "headRotation");
        writeRotation(lshoulderRotation, data, 2, "lshoulderRotation");
        writeRotation(rshoulderRotation, data, 3, "rshoulderRotation");
        writeRotation(larmRotation, data, 4, "larmRotation");
        writeRotation(rarmRotation, data, 5, "rarmRotation");
        writeRotation(lwristRotation, data, 6, "lwristRotation");
        writeRotation(rwristRotation, data, 7, "rwristRotation");
        writeRotation(lhandRotation, data, 8, "lhandRotation");
        writeRotation(rhandRotation, data, 9, "rhandRotation");
        writeRotation(lhipRotation, data, 10, "lhipRotation");
        writeRotation(rhipRotation, data, 11, "rhipRotation");
        writeRotation(lkneeRotation, data, 12, "lkneeRotation");
        writeRotation(rkneeRotation, data, 13, "rkneeRotation");
        writeRotation(lankleRotation, data, 14, "lankleRotation");
        writeRotation(rankleRotation, data, 15, "rankleRotation");
        writeRotation(lfootRotation, data, 16, "lfootRotation");
        writeRotation(rfootRotation, data, 17, "rfootRotation");

        //write fks 
        vec3 neckFK = fkToNextPoint(hipcenter, up, neckRotation, 1.0f);
        vec3 headFK = fkToNextPoint(neckFK, up, headRotation, 1.0f);
        vec3 lshoulderFK = fkToNextPoint(neckFK, left, lshoulderRotation, 1.0f);
        vec3 rshoulderFK = fkToNextPoint(neckFK, right, rshoulderRotation, 1.0f);
        vec3 larmFK = fkToNextPoint(lshoulderFK, left, larmRotation, 1.0f);
        vec3 rarmFK = fkToNextPoint(rshoulderFK, right, rarmRotation, 1.0f);
        vec3 lwristFK = fkToNextPoint(larmFK, left, lwristRotation, 1.0f);
        vec3 rwristFK = fkToNextPoint(rarmFK, right, rwristRotation, 1.0f);
        vec3 lhandFK = fkToNextPoint(lwristFK, right, lhandRotation, 1.0f);
        vec3 rhandFK = fkToNextPoint(rwristFK, right, rhandRotation, 1.0f);
        vec3 lhipFK = fkToNextPoint(hipcenter, left, lhipRotation, 1.0f);
        vec3 rhipFK = fkToNextPoint(hipcenter, right, rhipRotation, 1.0f);
        vec3 lkneeFK = fkToNextPoint(lhipFK, down, lkneeRotation, 1.0f);
        vec3 rkneeFK = fkToNextPoint(rhipFK, down, rkneeRotation, 1.0f);
        vec3 lankleFK = fkToNextPoint(lkneeFK, down, lankleRotation, 1.0f);
        vec3 rankleFK = fkToNextPoint(rkneeFK, down, rankleRotation, 1.0f);
        vec3 lfootFK = fkToNextPoint(lfootFK, down, lfootRotation, 1.0f);
        vec3 rfootFK = fkToNextPoint(rfootFK, down, rfootRotation, 1.0f);

        writeFK(neckFK, data, 0 , "neckFK");
        writeFK(headFK, data, 1 , "headFK");
        writeFK(lshoulderFK, data, 2 , "lshoulderFK");
        writeFK(rshoulderFK, data, 3 , "rshoulderFK");
        writeFK(larmFK, data, 4 , "larmFK");
        writeFK(rarmFK, data, 5 , "rarmFK");
        writeFK(lwristFK, data, 6 , "lwristFK");
        writeFK(rwristFK, data, 7 , "rwristFK");
        writeFK(lhandFK, data, 8 , "lhandFK");
        writeFK(rhandFK, data, 9 , "rhandFK");
        writeFK(lhipFK, data, 10 , "lhipFK");
        writeFK(rhipFK, data, 11 , "rhipFK");
        writeFK(lkneeFK, data, 12 , "lkneeFK");
        writeFK(rkneeFK, data, 13 , "rkneeFK");
        writeFK(lankleFK, data, 14 , "lankleFK");
        writeFK(rankleFK, data, 15 , "rankleFK");
        writeFK(lfootFK, data, 16 , "lfootFK");
        writeFK(rfootFK, data, 17 , "rfootFK");
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
        return vec3(direction[0] * bontLength, direction[1] * bontLength, direction[2] * bontLength);
    }

    vec3 fitting::vectorFromToPoint(vec3 const & from, vec3 const & to) {
        return vec3(to[0] - from[0], to[1] - from[1], to[2] - from[2]);
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
