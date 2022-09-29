/**
 * @file midwareBridge.h
 * @author Sijia Rong (sijia.rong@fitplay.co)
 * @brief It is an interface to Fitplay OS midware layer.
 *  You can (un)subscribe midware services, and modify subscribtions through application.
 * @version 0.1
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
 * @param output_json pointer to a json buffer holding output.
 * @return const int return 0 if success, others if failed.
 */
const int bridge_perform(void* handler, MidWareLandmark* landmarks, int num, 
                                    MidWareLandmark3D* landmarks3d, int num3d, 
                                    json* output_json);

/**
 * @brief process user request
 * 
 * @param handler the bridge used to process data.
 * @param app_json request info.
 * @return const int return 0 if success, others if failed.
 */
const int bridge_process_request(void* handler, json app_json);

#if __cplusplus
}
#endif

#endif /*MIDWAREBRIDGE_H_*/
