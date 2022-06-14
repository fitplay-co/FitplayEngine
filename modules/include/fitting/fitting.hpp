#ifndef FITPLAY_fitting
#define FITPLAY_fitting

#include <string>
#include <vector>
#include "json.hpp"
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"

#include "flatbuffer/poseData_generated.h"
#include "flatbuffer/actionData_generated.h"
#include "fitting_data.hpp"
#include "fitting_fk.hpp"
#include "fitting_landmark.hpp"

using namespace nlohmann;
using namespace glm;
using namespace std;

namespace fitplay {

    class fitting {
        private:
        public: 
            bool mirror = true;
            FittingFk fkInstance;
            void readPoseDataToLandmark(const PoseData::Pose* data)
            void process(const PoseData::Pose* data);
            vec3 readLandmarkPointVector(int point, const PoseData::Pose* data);
            vec3 readLandmarkPointVectorMirror(int point, const PoseData::Pose* data);
            flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<actionData::Joint>>> writeFlatBuffer(flatbuffers::FlatBufferBuilder& resultBuilder);
    };

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

        fkInstance.updateLandmarks(landmarkData);
    }

    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<actionData::Joint>>> fitting::writeFlatBuffer(flatbuffers::FlatBufferBuilder& resultBuilder) {
        std::vector<flatbuffers::Offset<actionData::Joint>> nodeVector;
        for(int i = 0; i < jointPointSize; i ++) {
            auto nodeOffset = actionData::CreateJointDirect(resultBuilder,
                fkInstance.jointPoints[i].fkRotation.w, 
                fkInstance.jointPoints[i].fkRotation.x, 
                fkInstance.jointPoints[i].fkRotation.y, 
                fkInstance.jointPoints[i].fkRotation.z, 
                fkInstance.jointPoints[i].jointName.c_str());
            nodeVector.push_back(nodeOffset);
        }
        return resultBuilder.CreateVector(nodeVector);
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

}


#endif