#include <string>
#include <vector>
#include <list>
#include <math.h>
#include "glm/glm.hpp"
#include "actionData_generated.h"
#include "midwareComponent/midwareComponent.hpp"

using namespace glm;

namespace ground {
    typedef std::vector<vec3> landmarks;
    static constexpr int jointPointSize = 18;

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

    class groundLocation: public Midware::MidwareComponent {
        private:
            float tracing = 0;
            float z_down = 0;
            float current_distance = 0;
            float widthScale = 640;
            float heightScale = 480;
            float startX = 0;
            float startY = 0;
            float startZ = 0;
            float pre_x = 0;
            float pre_z = 0;
            float pre_y = 0;
            float centerPointX = 320;
            float centerPointY = 240;
            float f = 0.05;
            float f_dx = 500; //  f/dx
            float f_dy = 900;
            float legLength = 0;
            float whichLeg = 0; //用来识别以左右哪只脚的y为准，0代表左脚，1代表右脚
            float ground_data[5];
            flatbuffers::Offset<ActionData::Ground> flatbuffersOffset;

            float prevTall = 2.0;
            float centerTall = 2.0;
            float zScale = 0.8f;
        public:
            groundLocation();
            ~groundLocation();
            
            void readPoseDataToLandmark3D(const PoseData::Pose* data, landmarks & landmarkData);
            vec3 readLandmarkPointVector3D(int point, const PoseData::Pose* data);
            bool process(const OsInput::InputMessage*, flatbuffers::FlatBufferBuilder&);
            void writeToFlatbuffers(ActionData::ActionBuilder&);
            float distance_finder_z_filtered(const PoseData::Pose* data);
            void distance_finder_leg(const PoseData::Pose* data);
    };

    groundLocation::groundLocation(): MidwareComponent("ground_location") {}
    groundLocation::~groundLocation() {}

    bool groundLocation::process(const OsInput::InputMessage* data, flatbuffers::FlatBufferBuilder& builder) {
        if (data->type() == OsInput::MessageType::MessageType_Pose) {
            const PoseData::Pose* pose = data->pose();
            mat3 cameraParam = mat3(f_dx, 0, centerPointX,
                                0, f_dy, centerPointY,
                                0, 0, 1);
            mat3 cameraInverse = inverse(cameraParam);
            // z_down = (pose->keypoints()->Get(24)->z() + pose->keypoints()->Get(23)->z())/2;
            // if (pose->rgbdEnabled()) {
            //     z_down = (pose->keypoints()->Get(24)->z() + pose->keypoints()->Get(23)->z())/2;
            // }
            // else {
            z_down = distance_finder_z_filtered(pose);
            // }

            vec3 arr33 = vec3(pose->keypoints()->Get(24)->x()*widthScale*z_down, (1-pose->keypoints()->Get(24)->y())*heightScale*z_down, z_down);
            vec3 arr_3down = vec3((pose->keypoints()->Get(23)->x()+pose->keypoints()->Get(24)->x())*widthScale*z_down*0.5, (1-pose->keypoints()->Get(23)->y())*heightScale*z_down, z_down);
            vec3 res_down = cameraInverse * arr_3down;
            vec3 resDown = cameraInverse * arr33;
            distance_finder_leg(pose);
            res_down[1] = (res_down[1] + resDown[1])/2;
            if(tracing == 0){
                startX = res_down[0];
                startY = res_down[1];
                startZ = z_down;
                tracing = 1;
                pre_x = res_down[0];
                pre_y = res_down[1];
                pre_z = z_down;
            }
            res_down[0] = res_down[0]*0.5+pre_x*0.5;
            res_down[1] = res_down[1]*0.5+pre_y*0.5;
            z_down = z_down*0.5+pre_z*0.5;

            ground_data[0] = res_down[0] - startX;
            ground_data[1] = res_down[1] - startY + legLength;
            ground_data[2] = z_down - startZ;
            ground_data[3] = legLength;
            ground_data[4] = tracing;

            pre_x = res_down[0];
            pre_y = res_down[1];
            pre_z = z_down;

            flatbuffersOffset = ActionData::CreateGround(builder, ground_data[0], ground_data[1], ground_data[2], ground_data[3], ground_data[4]);
            return true;
        } else if (data->type() == OsInput::MessageType::MessageType_ApplicationControl) {
            const ApplicationControl::Control* control = data->control();
            if (control->action()->str() == "reset") {
                startX = pre_x;
                startY = pre_y;
                startZ = pre_z;
            }
        }
        
        return false;
    }

    void groundLocation::writeToFlatbuffers(ActionData::ActionBuilder& actionBuilder) {
        actionBuilder.add_ground(flatbuffersOffset);
    }

    float groundLocation::distance_finder_z_filtered(const PoseData::Pose* data) {
        landmarks landmarkData3D(jointPointSize + 1);
        readPoseDataToLandmark3D(data, landmarkData3D);

        float t1 = distance2(landmarkData3D[HEAD], landmarkData3D[NECK]);
        float t2 = distance2(landmarkData3D[NECK], landmarkData3D[HIP_CENTER]);
        float t3 = distance2(landmarkData3D[L_HIP], landmarkData3D[R_HIP]);
        float t4l = distance2(landmarkData3D[L_HIP], landmarkData3D[L_KNEE]);
        float t4r = distance2(landmarkData3D[R_HIP], landmarkData3D[R_KNEE]);
        float t4 = (t4l + t4r) / 2.0;
        float t5l = distance2(landmarkData3D[L_KNEE], landmarkData3D[L_ANKLE]);
        float t5r = distance2(landmarkData3D[R_KNEE], landmarkData3D[L_ANKLE]);
        float t5 = t5l + t5r; 
        float t = t1 + t2 + t3 + t4 + t5;

        // Low pass filter in z direction
        float tall = t * 0.7 + prevTall * 0.3f;
        prevTall = tall;

        if (tall == 0)
        {
            tall = centerTall;
        }
        float dz = (centerTall - tall) / centerTall * zScale;

        cout << "dz for z distance finder" << dz << endl;
        return dz;
    }  

    void groundLocation::readPoseDataToLandmark3D(const PoseData::Pose* data, landmarks & landmarkData) {
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

        // infer nect and hip center
        landmarkData[HIP_CENTER] = vec3((landmarkData[L_HIP][0] 
                    + landmarkData[R_HIP][0])/2.0f, (landmarkData[L_HIP][1] 
                    + landmarkData[R_HIP][1])/2.0f, (landmarkData[L_HIP][2] + landmarkData[R_HIP][2])/2.0f);
        landmarkData[NECK] = vec3((landmarkData[L_SHOULDER][0] 
                    + landmarkData[R_SHOULDER][0])/2.0f, (landmarkData[L_SHOULDER][1] 
                    + landmarkData[R_SHOULDER][1])/2.0f, (landmarkData[L_SHOULDER][2] + landmarkData[R_SHOULDER][2])/2.0f);
    }

    vec3 groundLocation::readLandmarkPointVector3D(int point, const PoseData::Pose* data) {
        float xStart = data->keypoints3D()->Get(point)->x();
        float yStart = 0.0f - data->keypoints3D()->Get(point)->y();
        float zStart = data->keypoints3D()->Get(point)->z();
        return vec3(xStart, yStart, zStart);
    }

    void groundLocation::distance_finder_leg(const PoseData::Pose* data) {
        float y_1 = fabs(data->keypoints3D()->Get(31)->y() - data->keypoints3D()->Get(23)->y());
        float y_2 = fabs(data->keypoints3D()->Get(32)->y() - data->keypoints3D()->Get(24)->y());
        if(y_1 > legLength){
            legLength = y_1;
            whichLeg = 0;
        }
        if(y_2 > legLength){
            legLength = y_2;
            whichLeg = 1;
        }
    }
}