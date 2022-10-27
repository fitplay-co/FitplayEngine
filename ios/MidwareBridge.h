/**
 * @file MidwareBridge.h
 * @author Sijia Rong (sijia.rong@fitplay.co)
 * @brief It is an interface to Fitplay OS midware layer.
 *  You can (un)subscribe midware services, and modify subscribtions through application.
 * @version 0.2
 * @date 2022-08-26
 * 
 * @copyright Copyright (c) 2022 Fitplay
 * 
 */

#ifndef MIDWAREBRIDGE_H_
#define MIDWAREBRIDGE_H_

#include "json.hpp"

using json = nlohmann::json;

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

/**
 * @brief create bridge class object and give back handler.
 * 
 * @return void* handler of a bridge.
 */
void* bridge_create(void);

/**
 * @brief release data buffer.
 * 
 * @param handler the bridge whose data buffer will be released.
 * @return int 0 if success, others if failed.
 */
const int bridge_release_buffer(void* handler);

/**
 * @brief process landmark data with given size and give output.
 * 
 * @param handler the bridge used to process data.
 * @param landmarks 2d landmarks.
 * @param num size of 2d landmarks.
 * @param landmarks3d 3d landmarks.
 * @param num3d size of 3d landmarks.
 * @param output pointer to a flatbuffer holding output.
 * @param len pointer to output length
 * @return const int 0 if success, others if failed.
 */
const int bridge_perform(void* handler, MidWareLandmark* landmarks, int num, 
                                    MidWareLandmark3D* landmarks3d, int num3d, 
                                    unsigned char** output, unsigned int* len);

/**
 * @brief process user request
 * 
 * @param handler the bridge used to process data.
 * @param app_json request info.
 * @return const int 0 if success, others if failed.
 */
const int bridge_process_request(void* handler, json app_json);


/**
 * @brief process user request for flatbuffer input
 * 
 * @param handler the bridge used to process data.
 * @param input request info.
 * @return const int return 0 if success, others if failed.
 */
const int bridge_process_request_flatbuffer(void* handler, const char* input);

/**
 * @brief set parameters
 * 
 * @param handler the bridge used to process data.
 * @param params parameters, which are expected to be:
 *  intrinsic matrix:
 *  0 1 2
 *  3 4 5
 *  6 7 8
 *  inversed intrinsic matrix:
 *  09 10 11
 *  12 13 14
 *  15 16 17
 * @param num num of elements in parameters
 * @return const int return 0 if success, others if failed.
 */
const int bridge_set_parameters(void* handler, float* params, int num);

#if __cplusplus
}
#endif

#endif /*MIDWAREBRIDGE_H_*/
