#include <string>
#include <vector>
#include <list>
#include <math.h>
#include "glm/glm.hpp"
#include "flatbuffer/actionData_generated.h"

using namespace glm;

namespace gaze {
    class gazeTracking {
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
        public:
            gazeTracking();
            ~gazeTracking();
            void process(float gaze_data[],const  PoseData::Pose* data);
            float distance_finder_z_filtered(const PoseData::Pose* data, int num1, int num2);
    };

    gazeTracking::gazeTracking() {}
    gazeTracking::~gazeTracking() {}

    void gazeTracking::process(float gaze_data[], const PoseData::Pose* data) {
        mat3 cameraParam = mat3(f_dx, 0, centerPointX,
                                0, f_dy, centerPointY,
                                0, 0, 1);
        z = distance_finder_z_filtered(data, 2, 5);
        mat3 cameraInverse = inverse(cameraParam);
        vec3 arr_3 =  vec3 (data->keypoints()->Get(0)->x()*widthScale*z, (1-data->keypoints()->Get(0)->y())*heightScale*z, z);
        vec3 res = cameraInverse * arr_3;
        float res_x = res[0]*0.1+pre_x*0.9;
        float res_y = res[1]*0.1+pre_y*0.9;
        z = z*0.1+pre_z*0.9;
        gaze_data[0] = res_x;
        gaze_data[1] = res_y;
        gaze_data[2] = z;
        pre_x = res_x;
        pre_y = res_y;
        pre_z = z;
    }

    float gazeTracking::distance_finder_z_filtered(const PoseData::Pose* data, int num1, int num2) {
        float x1 = data->keypoints()->Get(num1)->x();
        float x2 = data->keypoints()->Get(num2)->x();
        float u1 = widthScale * x1;
        float u2 = widthScale * x2;
        return 35/fabs(0.8*(u2-u1));
    } 
}