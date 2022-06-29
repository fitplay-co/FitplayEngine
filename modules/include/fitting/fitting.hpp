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
            FittingRGBD rgbdFittingInstance;
            void readPoseDataToLandmark3D(const PoseData::Pose* data, landmarks & landmarkData);
            void readPoseDataToLandmark2D(const PoseData::Pose* data, landmarks & landmarkData);
            void process(const PoseData::Pose* data);
            vec3 readLandmarkPointVector3D(int point, const PoseData::Pose* data);
            vec3 readLandmarkPointVectorMirror3D(int point, const PoseData::Pose* data);
            vec3 readLandmarkPointVector2D(int point, const PoseData::Pose* data);
            vec3 readLandmarkPointVectorMirror2D(int point, const PoseData::Pose* data);
            flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<actionData::Joint>>> writeFlatBuffer(flatbuffers::FlatBufferBuilder& resultBuilder);
    };

    void fitting::process(const PoseData::Pose* data) {
        landmarks landmarkData3D(jointPointSize + 1);
        landmarks landmarkData2D(jointPointSize + 1);
        readPoseDataToLandmark3D(data, landmarkData3D);
    
        landmarkFittingInstance.handcraftFitting(landmarkData3D, fkInstance.jointPoints);
        if(mirror) {
            landmarkData3D = landmarkFittingInstance.currentFitLandmarkData;
        }

        //  if(mirror) {
        //     readPoseDataToLandmark2D(data, landmarkData2D);
        //     rgbdFittingInstance.update3DlandmarkWithRGBD(landmarkData3D, landmarkData2D);
        // }
        fkInstance.updateLandmarks(landmarkData3D);
    }

    void fitting::readPoseDataToLandmark3D(const PoseData::Pose* data, landmarks & landmarkData) {
        if(mirror) {
            landmarkData[HEAD] = readLandmarkPointVectorMirror3D(0, data);
            landmarkData[L_SHOULDER] = readLandmarkPointVectorMirror3D(12, data); 
            landmarkData[R_SHOULDER] = readLandmarkPointVectorMirror3D(11, data);
            landmarkData[L_ARM] = readLandmarkPointVectorMirror3D(14, data);
            landmarkData[R_ARM] = readLandmarkPointVectorMirror3D(13, data); 
            landmarkData[L_WRIST] = readLandmarkPointVectorMirror3D(16, data); 
            landmarkData[R_WRIST] = readLandmarkPointVectorMirror3D(15, data); 
            landmarkData[L_HAND] = readLandmarkPointVectorMirror3D(20, data); 
            landmarkData[R_HAND] = readLandmarkPointVectorMirror3D(19, data);
            landmarkData[L_HIP] = readLandmarkPointVectorMirror3D(24, data);
            landmarkData[R_HIP] = readLandmarkPointVectorMirror3D(23, data);
            landmarkData[L_KNEE] = readLandmarkPointVectorMirror3D(26, data);
            landmarkData[R_KNEE] = readLandmarkPointVectorMirror3D(25, data);
            landmarkData[L_ANKLE] = readLandmarkPointVectorMirror3D(28, data);
            landmarkData[R_ANKLE] = readLandmarkPointVectorMirror3D(27, data);
            landmarkData[L_FOOT] = readLandmarkPointVectorMirror3D(32, data);
            landmarkData[R_FOOT] = readLandmarkPointVectorMirror3D(31, data); 
        } else {
            //read inverse by mirror 
            landmarkData[HEAD] = readLandmarkPointVector3D(0, data);
            landmarkData[L_SHOULDER] = readLandmarkPointVector3D(11, data); 
            landmarkData[R_SHOULDER] = readLandmarkPointVector3D(12, data);
            landmarkData[L_ARM] = readLandmarkPointVector3D(13, data);
            landmarkData[R_ARM] = readLandmarkPointVector3D(14, data); 
            landmarkData[L_WRIST] = readLandmarkPointVector3D(15, data); 
            landmarkData[R_WRIST] = readLandmarkPointVector3D(16, data); 
            landmarkData[L_HAND] = readLandmarkPointVector3D(19, data); 
            landmarkData[R_HAND] = readLandmarkPointVector3D(20, data);
            landmarkData[L_HIP] = readLandmarkPointVector3D(23, data);
            landmarkData[R_HIP] = readLandmarkPointVector3D(24, data);
            landmarkData[L_KNEE] = readLandmarkPointVector3D(25, data);
            landmarkData[R_KNEE] = readLandmarkPointVector3D(26, data);
            landmarkData[L_ANKLE] = readLandmarkPointVector3D(27, data);
            landmarkData[R_ANKLE] = readLandmarkPointVector3D(28, data);
            landmarkData[L_FOOT] = readLandmarkPointVector3D(31, data);
            landmarkData[R_FOOT] = readLandmarkPointVector3D(32, data); 
        }

        // infer nect and hip center
        landmarkData[HIP_CENTER] = vec3(0.0f ,0.0f ,0.0f);
        landmarkData[NECK] = vec3((landmarkData[L_SHOULDER][0] 
                + landmarkData[R_SHOULDER][0])/2.0f, (landmarkData[L_SHOULDER][1] 
                + landmarkData[R_SHOULDER][1])/2.0f, (landmarkData[L_SHOULDER][2] 
                + landmarkData[R_SHOULDER][2])/2.0f);
    }

    void fitting::readPoseDataToLandmark2D(const PoseData::Pose* data, landmarks & landmarkData) {
        if(mirror) {
            landmarkData[HEAD] = readLandmarkPointVectorMirror2D(0, data);
            landmarkData[L_SHOULDER] = readLandmarkPointVectorMirror2D(12, data); 
            landmarkData[R_SHOULDER] = readLandmarkPointVectorMirror2D(11, data);
            landmarkData[L_ARM] = readLandmarkPointVectorMirror2D(14, data);
            landmarkData[R_ARM] = readLandmarkPointVectorMirror2D(13, data); 
            landmarkData[L_WRIST] = readLandmarkPointVectorMirror2D(16, data); 
            landmarkData[R_WRIST] = readLandmarkPointVectorMirror2D(15, data); 
            landmarkData[L_HAND] = readLandmarkPointVectorMirror2D(20, data); 
            landmarkData[R_HAND] = readLandmarkPointVectorMirror2D(19, data);
            landmarkData[L_HIP] = readLandmarkPointVectorMirror2D(24, data);
            landmarkData[R_HIP] = readLandmarkPointVectorMirror2D(23, data);
            landmarkData[L_KNEE] = readLandmarkPointVectorMirror2D(26, data);
            landmarkData[R_KNEE] = readLandmarkPointVectorMirror2D(25, data);
            landmarkData[L_ANKLE] = readLandmarkPointVectorMirror2D(28, data);
            landmarkData[R_ANKLE] = readLandmarkPointVectorMirror2D(27, data);
            landmarkData[L_FOOT] = readLandmarkPointVectorMirror2D(32, data);
            landmarkData[R_FOOT] = readLandmarkPointVectorMirror2D(31, data); 
        } else {
            //read inverse by mirror 
            landmarkData[HEAD] = readLandmarkPointVector2D(0, data);
            landmarkData[L_SHOULDER] = readLandmarkPointVector2D(11, data); 
            landmarkData[R_SHOULDER] = readLandmarkPointVector2D(12, data);
            landmarkData[L_ARM] = readLandmarkPointVector2D(13, data);
            landmarkData[R_ARM] = readLandmarkPointVector2D(14, data); 
            landmarkData[L_WRIST] = readLandmarkPointVector2D(15, data); 
            landmarkData[R_WRIST] = readLandmarkPointVector2D(16, data); 
            landmarkData[L_HAND] = readLandmarkPointVector2D(19, data); 
            landmarkData[R_HAND] = readLandmarkPointVector2D(20, data);
            landmarkData[L_HIP] = readLandmarkPointVector2D(23, data);
            landmarkData[R_HIP] = readLandmarkPointVector2D(24, data);
            landmarkData[L_KNEE] = readLandmarkPointVector2D(25, data);
            landmarkData[R_KNEE] = readLandmarkPointVector2D(26, data);
            landmarkData[L_ANKLE] = readLandmarkPointVector2D(27, data);
            landmarkData[R_ANKLE] = readLandmarkPointVector2D(28, data);
            landmarkData[L_FOOT] = readLandmarkPointVector2D(31, data);
            landmarkData[R_FOOT] = readLandmarkPointVector2D(32, data); 
        }

        // infer nect and hip center
        landmarkData[HIP_CENTER] = vec3((landmarkData[L_HIP][0] 
                    + landmarkData[R_HIP][0])/2.0f, (landmarkData[L_HIP][1] 
                    + landmarkData[R_HIP][1])/2.0f, (landmarkData[L_HIP][2] + landmarkData[R_HIP][2])/2.0f);
        landmarkData[NECK] = vec3((landmarkData[L_SHOULDER][0] 
                    + landmarkData[R_SHOULDER][0])/2.0f, (landmarkData[L_SHOULDER][1] 
                    + landmarkData[R_SHOULDER][1])/2.0f, (landmarkData[L_SHOULDER][2] + landmarkData[R_SHOULDER][2])/2.0f);
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

    vec3 fitting::readLandmarkPointVector3D(int point, const PoseData::Pose* data) {
        float xStart = data->keypoints3D()->Get(point)->x();
        float yStart = 0.0f - data->keypoints3D()->Get(point)->y();
        float zStart = data->keypoints3D()->Get(point)->z();
        return vec3(xStart, yStart, zStart);
    }

    vec3 fitting::readLandmarkPointVectorMirror3D(int point, const PoseData::Pose* data) {
        float xStart = 0.0f - data->keypoints3D()->Get(point)->x();
        float yStart = 0.0f - data->keypoints3D()->Get(point)->y();
        float zStart = data->keypoints3D()->Get(point)->z();
        return vec3(xStart, yStart, zStart);
    }
    
    vec3 fitting::readLandmarkPointVector2D(int point, const PoseData::Pose* data) {
        float xStart = data->keypoints()->Get(point)->x();
        float yStart = 0.0f - data->keypoints()->Get(point)->y();
        float zStart = data->keypoints()->Get(point)->z();
        return vec3(xStart, yStart, zStart);
    }

    vec3 fitting::readLandmarkPointVectorMirror2D(int point, const PoseData::Pose* data) {
        float xStart = 0.0f - data->keypoints()->Get(point)->x();
        float yStart = 0.0f - data->keypoints()->Get(point)->y();
        float zStart = data->keypoints()->Get(point)->z();
        return vec3(xStart, yStart, zStart);
    }
}

        
#endif