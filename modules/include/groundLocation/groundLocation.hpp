#include <string>
#include <vector>
#include <list>
#include "flatbuffer/actionData_generated.h"

namespace ground {
    class groundLocation {
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
            groundLocation();
            groundLocation();
            void process(PoseData::Pose* data);
    };
}