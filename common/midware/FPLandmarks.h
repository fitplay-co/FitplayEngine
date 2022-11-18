/**
 * @file FPLandmarks.h
 * @author Sijia Rong (sijia.rong@fitplay.co)
 * @brief 
 * @version 0.1
 * @date 2022-11-08
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef FPLANDMARKS_H_
#define FPLANDMARKS_H_

#define FP_HEAD_T 0
#define FP_NECK 1
#define FP_R_SHOULDER 2
#define FP_R_ARM 3
#define FP_R_WRIST 4
#define FP_L_SHOULDER 5
#define FP_L_ARM 6
#define FP_L_WRIST 7
#define FP_R_HIP 8
#define FP_R_KNEE 9
#define FP_R_ANKLE 10
#define FP_L_HIP 11
#define FP_L_KNEE 12
#define FP_L_ANKLE 13
#define FP_CENTER_HIP 14
#define FP_SPINE 15
#define FP_HEAD_B 16
#define FP_R_HAND 17
#define FP_L_HAND 18
#define FP_R_FOOT 19
#define FP_L_FOOT 20
#define FP_LANDMARK_NUM 21

#if __cplusplus
extern "C" {
#endif

typedef struct MidWareLandmark{
    float x;
    float y;
    float z;
    float confidence;
}MidWareLandmark;

typedef struct MidWareLandmark3D{
    float x;
    float y;
    float z;
    float confidence;
}MidWareLandmark3D;

#if __cplusplus
}
#endif

#endif /*FPLANDMARKS*/