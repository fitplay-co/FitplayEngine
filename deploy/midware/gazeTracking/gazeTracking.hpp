#include <string>
#include <vector>
#include <list>
#include <math.h>
#include "glm/glm.hpp"
#include "actionData_generated.h"
#include "midwareComponent/midwareComponent.hpp"

using namespace glm;

namespace gaze {
    class gazeTracking: public Midware::MidwareComponent {
        private:
            float z = 0;
            float widthScale = 640;
            float heightScale = 480;
            float pre_x = 0;
            float pre_y = 0;
            float pre_z = 0;
            float centerPointX = 320;
            float centerPointY = 240;
            float f_dx = 500; //  f/dx
            float f_dy = 900;
            flatbuffers::Offset<actionData::Gaze> flatbuffersOffset;
        public:
            gazeTracking();
            ~gazeTracking();
            bool process(const Input::InputMessage*, flatbuffers::FlatBufferBuilder&);
            void writeToFlatbuffers(actionData::ActionBuilder&);
            float distance_finder_z_filtered(const PoseData::Pose* data, int num1, int num2);
    };

    gazeTracking::gazeTracking(): MidwareComponent("gaze_tracking") {}
    gazeTracking::~gazeTracking() {}

    bool gazeTracking::process(const Input::InputMessage* data, flatbuffers::FlatBufferBuilder& builder) {
        if (data->type() == Input::MessageType::MessageType_Pose) {
            const PoseData::Pose* pose = data->pose();
            mat3 cameraParam = mat3(f_dx, 0, centerPointX,
                                0, f_dy, centerPointY,
                                0, 0, 1);
            z = distance_finder_z_filtered(pose, 2, 5);
            mat3 cameraInverse = inverse(cameraParam);
            vec3 arr_3 =  vec3 (pose->keypoints()->Get(0)->x()*widthScale*z, (1-pose->keypoints()->Get(0)->y())*heightScale*z, z);
            vec3 res = cameraInverse * arr_3;
            float res_x = res[0]*0.1+pre_x*0.9;
            float res_y = res[1]*0.1+pre_y*0.9;
            z = z*0.1+pre_z*0.9;
        
            pre_x = res_x;
            pre_y = res_y;
            pre_z = z;

            flatbuffersOffset = actionData::CreateGaze(builder, res_x, res_y, z);

            return true;
        }

        return false;
    }

    void gazeTracking::writeToFlatbuffers(actionData::ActionBuilder& actionBuilder) {
        actionBuilder.add_gaze(flatbuffersOffset);
    }

    float gazeTracking::distance_finder_z_filtered(const PoseData::Pose* data, int num1, int num2) {
        float x1 = data->keypoints()->Get(num1)->x();
        float x2 = data->keypoints()->Get(num2)->x();
        float u1 = widthScale * x1;
        float u2 = widthScale * x2;
        return 35/fabs(0.8*(u2-u1));
    } 
}