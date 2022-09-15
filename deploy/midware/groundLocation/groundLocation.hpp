#include <string>
#include <vector>
#include <list>
#include <math.h>
#include "glm/glm.hpp"
#include "actionData_generated.h"
#include "midwareComponent/midwareComponent.hpp"

using namespace glm;

namespace ground {
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
            flatbuffers::Offset<actionData::Ground> flatbuffersOffset;
            
        public:
            groundLocation();
            ~groundLocation();
            bool process(const Input::InputMessage*, flatbuffers::FlatBufferBuilder&);
            void writeToFlatbuffers(actionData::ActionBuilder&);
            float distance_finder_z_filtered(const PoseData::Pose* data, int num1, int num2);
            void distance_finder_leg(const PoseData::Pose* data);
    };

    groundLocation::groundLocation(): MidwareComponent("ground_location") {}
    groundLocation::~groundLocation() {}

    bool groundLocation::process(const Input::InputMessage* data, flatbuffers::FlatBufferBuilder& builder) {
        if (data->type() == Input::MessageType::MessageType_Pose) {
            const PoseData::Pose* pose = data->pose();
            mat3 cameraParam = mat3(f_dx, 0, centerPointX,
                                0, f_dy, centerPointY,
                                0, 0, 1);
            mat3 cameraInverse = inverse(cameraParam);

            if (pose->rgbdEnabled()) {
                z_down = (pose->keypoints()->Get(24)->z() + pose->keypoints()->Get(23)->z())/2;
            }
            else {
                z_down = distance_finder_z_filtered(pose, 23, 24)*2.6;
            }

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

            flatbuffersOffset = actionData::CreateGround(builder, ground_data[0], ground_data[1], ground_data[2], ground_data[3], ground_data[4]);
            return true;
        } else if (data->type() == Input::MessageType::MessageType_ApplicationControl) {
            const ApplicationControl::Control* control = data->control();
            if (control->action()->str() == "reset") {
                startX = pre_x;
                startY = pre_y;
                startZ = pre_z;
            }
        }
        
        return false;
    }

    void groundLocation::writeToFlatbuffers(actionData::ActionBuilder& actionBuilder) {
        actionBuilder.add_ground(flatbuffersOffset);
    }

    float groundLocation::distance_finder_z_filtered(const PoseData::Pose* data, int num1, int num2) {
        float x1 = data->keypoints()->Get(num1)->x();
        float x2 = data->keypoints()->Get(num2)->x();
        float u1 = widthScale * x1;
        float u2 = widthScale * x2;
        return 35/fabs(0.8*(u2-u1));
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