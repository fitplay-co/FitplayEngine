/**
 * @file FPBody.hpp
 * 
 * @author Sijia Rong (sijia.rong@fitplay.co)
 * @brief Fitplay defined body data structure
 * @version 0.1
 * @date 2022-11-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef FPBODY_HPP_
#define FPBODY_HPP_

#include "FPJoint.hpp"

namespace fitplay {

class FPBody
{
private:
    
fitplay::FPJoint* _root;
fitplay::FPJoint* _joint_list[FP_LANDMARK_NUM];

public:
    FPBody();
    FPBody(MidWareLandmark3D* midware_landmarks, int num);
    ~FPBody();

    const bool GetLandmarks(MidWareLandmark3D* midware_landmarks, int num);
    FPJoint* GetJoint(int num);
    int UpdateBody(MidWareLandmark3D* midware_landmarks, int num);
};

FPBody::FPBody(){
    // TODO: init with T-Pose
    MidWareLandmark3D* midware_landmarks = (MidWareLandmark3D*)calloc(FP_LANDMARK_NUM, sizeof(MidWareLandmark3D));
    FPBody(midware_landmarks, FP_LANDMARK_NUM);
    free(midware_landmarks);
}

FPBody::FPBody(MidWareLandmark3D* midware_landmarks, int num)
{
    if(num == FP_LANDMARK_NUM){
        vec3 landmarks[FP_LANDMARK_NUM];
        for(int i = 0; i < FP_LANDMARK_NUM; i++){
            landmarks[i] = vec3(midware_landmarks[i].x,
                                midware_landmarks[i].y,
                                midware_landmarks[i].z);
        }

        _root = new FPJoint(FP_CENTER_HIP);
        _joint_list[FP_CENTER_HIP] = _root;

        _joint_list[FP_L_HIP] = new FPJoint(landmarks[FP_L_HIP], /* bone len: */0.0955f * 0.9f, _joint_list[FP_CENTER_HIP], FP_L_HIP);
        _joint_list[FP_R_HIP] = new FPJoint(landmarks[FP_R_HIP], /* bone len: */0.0955f * 0.9f, _joint_list[FP_CENTER_HIP], FP_R_HIP);
        _joint_list[FP_SPINE] = new FPJoint(landmarks[FP_SPINE], /* bone len: */0.19f, _joint_list[FP_CENTER_HIP], FP_SPINE);

        _joint_list[FP_L_KNEE] = new FPJoint(landmarks[FP_L_KNEE] - landmarks[FP_L_HIP],/* bone len: */0.245f * 1.38f, _joint_list[FP_L_HIP], FP_L_KNEE);
        _joint_list[FP_L_ANKLE] = new FPJoint(landmarks[FP_L_ANKLE] - landmarks[FP_L_KNEE], /* bone len: */0.246f* 1.22f, _joint_list[FP_L_KNEE], FP_L_ANKLE);
        _joint_list[FP_L_FOOT] = new FPJoint(landmarks[FP_L_FOOT] - landmarks[FP_L_ANKLE], /* bone len: */0.152f, _joint_list[FP_L_ANKLE], FP_L_FOOT);

        _joint_list[FP_R_KNEE] = new FPJoint(landmarks[FP_R_KNEE] - landmarks[FP_R_HIP], /* bone len: */0.245f* 1.38f, _joint_list[FP_R_HIP], FP_R_KNEE);
        _joint_list[FP_R_ANKLE] = new FPJoint(landmarks[FP_R_ANKLE] - landmarks[FP_R_KNEE], /* bone len: */0.246f* 1.22f, _joint_list[FP_R_KNEE], FP_R_ANKLE);
        _joint_list[FP_R_FOOT] = new FPJoint(landmarks[FP_R_FOOT] - landmarks[FP_R_ANKLE], /* bone len: */0.152f, _joint_list[FP_R_ANKLE], FP_R_FOOT);

        _joint_list[FP_NECK] = new FPJoint(landmarks[FP_NECK] - landmarks[FP_SPINE], /* bone len: */0.098f * 1.16f, _joint_list[FP_SPINE], FP_NECK);
        _joint_list[FP_HEAD_B] = new FPJoint(landmarks[FP_HEAD_B] - landmarks[FP_NECK], /* bone len: */0.052f, _joint_list[FP_NECK], FP_HEAD_B);
        _joint_list[FP_HEAD_T] = new FPJoint(landmarks[FP_HEAD_T] - landmarks[FP_HEAD_B], /* bone len: */0.13f, _joint_list[FP_HEAD_B], FP_HEAD_T);

        _joint_list[FP_L_SHOULDER] = new FPJoint(landmarks[FP_L_SHOULDER] - landmarks[FP_NECK], /* bone len: */0.129f * 0.76f, _joint_list[FP_NECK], FP_L_SHOULDER);
        _joint_list[FP_L_ARM] = new FPJoint(landmarks[FP_L_ARM] - landmarks[FP_L_SHOULDER], /* bone len: */0.186f * 0.88f, _joint_list[FP_L_SHOULDER], FP_L_ARM);
        _joint_list[FP_L_WRIST] = new FPJoint(landmarks[FP_L_WRIST] - landmarks[FP_L_ARM], /* bone len: */0.146f, _joint_list[FP_L_ARM], FP_L_WRIST);
        _joint_list[FP_L_HAND] = new FPJoint(landmarks[FP_L_HAND] - landmarks[FP_L_WRIST], /* bone len: */0.108f, _joint_list[FP_L_WRIST], FP_L_HAND);

        _joint_list[FP_R_SHOULDER] = new FPJoint(landmarks[FP_R_SHOULDER] - landmarks[FP_NECK], /* bone len: */0.129f * 0.76f, _joint_list[FP_NECK], FP_R_SHOULDER);
        _joint_list[FP_R_ARM] = new FPJoint(landmarks[FP_R_ARM] - landmarks[FP_R_SHOULDER], /* bone len: */0.186f* 0.88f, _joint_list[FP_R_SHOULDER], FP_R_ARM);
        _joint_list[FP_R_WRIST] = new FPJoint(landmarks[FP_R_WRIST] - landmarks[FP_R_ARM], /* bone len: */0.146f, _joint_list[FP_R_ARM], FP_R_WRIST);
        _joint_list[FP_R_HAND] = new FPJoint(landmarks[FP_R_HAND] - landmarks[FP_R_WRIST], /* bone len: */0.108f, _joint_list[FP_R_WRIST], FP_R_HAND);
        
    }else{
        std::cout << __FUNCTION__ << ": Error: expect landmark size " << LANDMARK_NUM 
        << ", but gets " << num << std::endl;
    }
}

FPBody::~FPBody()
{   
    // std::cout << __FUNCTION__ << ": Enter" << std::endl;
    for(auto joint: _joint_list){
        if(joint != nullptr){
            delete joint;
        }
    }
}

const bool FPBody::GetLandmarks(MidWareLandmark3D* midware_landmarks, int num){

    if(midware_landmarks == nullptr || num != FP_LANDMARK_NUM){
        return false;
    }

    for(int i = 0; i< FP_LANDMARK_NUM; i++){
        vec3 position = _joint_list[i]->GetPosition();
        midware_landmarks[i].x = position.x;
        midware_landmarks[i].y = position.y;
        midware_landmarks[i].z = position.z;
    }

    // for(int i = 0; i < FP_LANDMARK_NUM ; i++){
    //     std::cout << __FUNCTION__ <<": i: "<< i << "; position: " 
    //     << midware_landmarks[i].x << ", "<< midware_landmarks[i].y << ", " << midware_landmarks[i].z << std::endl;
    // }

    return true;

}

FPJoint* FPBody::GetJoint(int num){
    return _joint_list[num];
}

int FPBody::UpdateBody(MidWareLandmark3D* midware_landmarks, int num){
    // std::cout << __FUNCTION__ << ": Enter" << std::endl;
    if(num == FP_LANDMARK_NUM){
        vec3 landmarks[FP_LANDMARK_NUM];
        float confidence[FP_LANDMARK_NUM];
        for(int i = 0; i < FP_LANDMARK_NUM; i++){
            landmarks[i] = vec3(midware_landmarks[i].x,
                                midware_landmarks[i].y,
                                midware_landmarks[i].z);
            confidence[i] = midware_landmarks[i].confidence;
        }

        _joint_list[FP_L_HIP]->UpdateDirection(landmarks[FP_L_HIP], confidence[FP_L_HIP]);
        _joint_list[FP_R_HIP]->UpdateDirection(landmarks[FP_R_HIP], confidence[FP_L_HIP]);
        _joint_list[FP_SPINE]->UpdateDirection(landmarks[FP_SPINE], confidence[FP_L_HIP]);

        _joint_list[FP_L_KNEE]->UpdateDirection(landmarks[FP_L_KNEE] - landmarks[FP_L_HIP], confidence[FP_L_HIP]);
        _joint_list[FP_L_ANKLE]->UpdateDirection(landmarks[FP_L_ANKLE] - landmarks[FP_L_KNEE], confidence[FP_L_HIP]);
        _joint_list[FP_L_FOOT]->UpdateDirection(landmarks[FP_L_FOOT] - landmarks[FP_L_ANKLE], confidence[FP_L_HIP]);

        _joint_list[FP_R_KNEE]->UpdateDirection(landmarks[FP_R_KNEE] - landmarks[FP_R_HIP], confidence[FP_L_HIP]);
        _joint_list[FP_R_ANKLE]->UpdateDirection(landmarks[FP_R_ANKLE] - landmarks[FP_R_KNEE], confidence[FP_L_HIP]);
        _joint_list[FP_R_FOOT]->UpdateDirection(landmarks[FP_R_FOOT] - landmarks[FP_R_ANKLE], confidence[FP_L_HIP]);

        _joint_list[FP_NECK]->UpdateDirection(landmarks[FP_NECK] - landmarks[FP_SPINE], confidence[FP_L_HIP]);
        _joint_list[FP_HEAD_B]->UpdateDirection(landmarks[FP_HEAD_B] - landmarks[FP_NECK], confidence[FP_L_HIP]);
        _joint_list[FP_HEAD_T]->UpdateDirection(landmarks[FP_HEAD_T] - landmarks[FP_HEAD_B], confidence[FP_L_HIP]);

        _joint_list[FP_L_SHOULDER]->UpdateDirection(landmarks[FP_L_SHOULDER] - landmarks[FP_NECK], confidence[FP_L_HIP]);
        _joint_list[FP_L_ARM]->UpdateDirection(landmarks[FP_L_ARM] - landmarks[FP_L_SHOULDER], confidence[FP_L_HIP]);
        _joint_list[FP_L_WRIST]->UpdateDirection(landmarks[FP_L_WRIST] - landmarks[FP_L_ARM], confidence[FP_L_HIP]);
        _joint_list[FP_L_HAND]->UpdateDirection(landmarks[FP_L_HAND] - landmarks[FP_L_WRIST], confidence[FP_L_HIP]);

        _joint_list[FP_R_SHOULDER]->UpdateDirection(landmarks[FP_R_SHOULDER] - landmarks[FP_NECK], confidence[FP_L_HIP]);
        _joint_list[FP_R_ARM]->UpdateDirection(landmarks[FP_R_ARM] - landmarks[FP_R_SHOULDER], confidence[FP_L_HIP]);
        _joint_list[FP_R_WRIST]->UpdateDirection(landmarks[FP_R_WRIST] - landmarks[FP_R_ARM], confidence[FP_L_HIP]);
        _joint_list[FP_R_HAND]->UpdateDirection(landmarks[FP_R_HAND] - landmarks[FP_R_WRIST], confidence[FP_L_HIP]);
        
    }else{
        std::cout << __FUNCTION__ << ": Error: expect landmark size " << LANDMARK_NUM 
        << ", but gets " << num << std::endl;
        return -1;
    }

    return 0;
}


}

#endif /* FPBODY_HPP_ */