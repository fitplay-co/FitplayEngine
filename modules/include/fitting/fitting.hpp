#ifndef FITPLAY_fitting
#define FITPLAY_fitting

#include <string>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"

#include "poseData_generated.h"
#include "actionData_generated.h"
#include "fitting_data.hpp"
#include "fitting_fk.hpp"
#include "fitting_landmark.hpp"
#include "fitting_rgbd.hpp"

using namespace glm;
using namespace std;

namespace fitplay {

    class fitting {
        private:
        public:
            bool mirror = true;
            FittingLandmark landmarkFittingInstance;
            FittingFk fkInstance;
            landmarks readPoseDataToLandmark(const PoseData::Pose* data);
            void process(const PoseData::Pose* data);
            vec3 readLandmarkPointVector(int point, const PoseData::Pose* data);
            vec3 readLandmarkPointVectorMirror(int point, const PoseData::Pose* data);
            flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<actionData::Joint>>> writeFlatBuffer(flatbuffers::FlatBufferBuilder& resultBuilder);
    };

    void fitting::process(const PoseData::Pose* data) {
        landmarks landmarkData = readPoseDataToLandmark(data);
        
        // landmarkFittingInstance.handcraftFitting(landmarkData, fkInstance.jointPoints);
        // if(mirror) {
        //     landmarkData = landmarkFittingInstance.currentFitLandmarkData;
        // }

        fkInstance.updateLandmarks(landmarkData);
    }

    landmarks fitting::readPoseDataToLandmark(const PoseData::Pose* data) {
        landmarks landmarkData;
        if(mirror) {
            landmarkData[HEAD] = readLandmarkPointVectorMirror(0, data);
            landmarkData[L_SHOULDER] = readLandmarkPointVectorMirror(12, data); 
            landmarkData[R_SHOULDER] = readLandmarkPointVectorMirror(11, data);
            landmarkData[L_ARM] = readLandmarkPointVectorMirror(14, data);
            landmarkData[R_ARM] = readLandmarkPointVectorMirror(13, data); 
            landmarkData[L_WRIST] = readLandmarkPointVectorMirror(16, data); 
            landmarkData[R_WRIST] = readLandmarkPointVectorMirror(15, data); 
            landmarkData[L_HAND] = readLandmarkPointVectorMirror(20, data); 
            landmarkData[R_HAND] = readLandmarkPointVectorMirror(19, data);
            landmarkData[L_HIP] = readLandmarkPointVectorMirror(24, data);
            landmarkData[R_HIP] = readLandmarkPointVectorMirror(23, data);
            landmarkData[L_KNEE] = readLandmarkPointVectorMirror(26, data);
            landmarkData[R_KNEE] = readLandmarkPointVectorMirror(25, data);
            landmarkData[L_ANKLE] = readLandmarkPointVectorMirror(28, data);
            landmarkData[R_ANKLE] = readLandmarkPointVectorMirror(27, data);
            landmarkData[L_FOOT] = readLandmarkPointVectorMirror(32, data);
            landmarkData[R_FOOT] = readLandmarkPointVectorMirror(31, data); 
        } else {
            //read inverse by mirror 
            landmarkData[HEAD] = readLandmarkPointVector(0, data);
            landmarkData[L_SHOULDER] = readLandmarkPointVector(11, data); 
            landmarkData[R_SHOULDER] = readLandmarkPointVector(12, data);
            landmarkData[L_ARM] = readLandmarkPointVector(13, data);
            landmarkData[R_ARM] = readLandmarkPointVector(14, data); 
            landmarkData[L_WRIST] = readLandmarkPointVector(15, data); 
            landmarkData[R_WRIST] = readLandmarkPointVector(16, data); 
            landmarkData[L_HAND] = readLandmarkPointVector(19, data); 
            landmarkData[R_HAND] = readLandmarkPointVector(20, data);
            landmarkData[L_HIP] = readLandmarkPointVector(23, data);
            landmarkData[R_HIP] = readLandmarkPointVector(24, data);
            landmarkData[L_KNEE] = readLandmarkPointVector(25, data);
            landmarkData[R_KNEE] = readLandmarkPointVector(26, data);
            landmarkData[L_ANKLE] = readLandmarkPointVector(27, data);
            landmarkData[R_ANKLE] = readLandmarkPointVector(28, data);
            landmarkData[L_FOOT] = readLandmarkPointVector(31, data);
            landmarkData[R_FOOT] = readLandmarkPointVector(32, data); 
        }

        // infer nect and hip center
        landmarkData[HIP_CENTER] = vec3(0.0f ,0.0f ,0.0f);
        landmarkData[NECK] = vec3((landmarkData[L_SHOULDER][0] 
                + landmarkData[R_SHOULDER][0])/2.0f, (landmarkData[L_SHOULDER][1] 
                + landmarkData[R_SHOULDER][1])/2.0f, (landmarkData[L_SHOULDER][2] 
                + landmarkData[R_SHOULDER][2])/2.0f);
                
        return landmarkData;
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