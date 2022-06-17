#include <string>
#include <vector>
#include <list>
#include <chrono> 
#include <math.h>
#include "flatbuffer/actionData_generated.h"

namespace actionsquat {
    class squat {
        private:
            float relativeHipDistance = 0;
            float preRelativeHipDistance = 0;
            float preHipYLeft = 0;
            float preHipYRight = 0;
            float firstTime = 0;
            float squatTime = 0;
            float upTime = 0;
            float stayTime = 0;
            float squat_status = 0;
            float legLength = 0;
            float preLegLength = 0;
        public:
            squat();
            ~squat();
            void process(float squat_data[], const PoseData::Pose* data);
            void calculate_relativeHip_distance(const PoseData::Pose* data);
            void calculate_squat_status();
    };

    squat::squat() {}
    squat::~squat() {}
    void squat::process(float squat_data[], const PoseData::Pose* data) {
        calculate_relativeHip_distance(data);
        calculate_squat_status();
        squat_data[0] = squat_status;
    }
    void squat::calculate_relativeHip_distance(const PoseData::Pose* data) {
        if (firstTime == 0){
            preHipYLeft = data->keypoints()->Get(23)->y();
            preHipYRight = data->keypoints()->Get(24)->y();
            firstTime = 1;
            preRelativeHipDistance = relativeHipDistance;
        }
        legLength = fmax(data->keypoints3D()->Get(27)->y()-data->keypoints3D()->Get(23)->y(),data->keypoints3D()->Get(28)->y()-data->keypoints3D()->Get(24)->y()); 
        relativeHipDistance = (data->keypoints()->Get(23)->y() - preHipYLeft + data->keypoints()->Get(24)->y() - preHipYRight);
        relativeHipDistance = 0.2*relativeHipDistance + 0.8*preRelativeHipDistance;
        preRelativeHipDistance = relativeHipDistance;
        preHipYLeft = data->keypoints()->Get(23)->y();
        preHipYRight = data->keypoints()->Get(24)->y();
    }
    void squat::calculate_squat_status(){
        if (relativeHipDistance > 0.005){
            squatTime = squatTime + 1;
        }
        else{
            squatTime = 0;
        }
        if (squatTime > 5){
            squat_status = -1;
        }
        if (abs(relativeHipDistance)<0.005){
            stayTime = stayTime + 1;
        }
        else {
            stayTime = 0;
        }
        if (stayTime > 10){
            squat_status = 0;
        }
        if (relativeHipDistance < -0.005 && legLength > preLegLength){
            upTime = upTime + 1;
        }
        else{
            upTime = 0;
        }
        if ( upTime > 5){
            squat_status = 1;
        }
        preLegLength = legLength;
    }
}