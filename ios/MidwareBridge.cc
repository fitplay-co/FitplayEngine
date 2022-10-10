#include "MidwareBridge.h"
#include "bridgeClass.hpp"
#include "json.hpp"
#include <chrono>

#define MEDIAPIPE_LANDMARK_SIZE 33

static const string landmark_name[33] = {
    "nose",
    "left_eye_inner",
    "left_eye",
    "left_eye_outer",
    "right_eye_inner",
    "right_eye",
    "right_eye_outer",
    "left_ear",
    "right_ear",
    "mouth_left",
    "mouth_right",
    "left_shoulder",
    "right_shoulder",
    "left_elbow",
    "right_elbow",
    "left_wrist",
    "right_wrist",
    "left_pinky",
    "right_pinky",
    "left_index",
    "right_index",
    "left_thumb",
    "right_thumb",
    "left_hip",
    "right_hip",
    "left_knee",
    "right_knee",
    "left_ankle",
    "right_ankle",
    "left_heel",
    "right_heel",
    "left_foot_index",
    "right_foot_index"
};

static const int fitplayToMediapipeLandmarkIndexMap[33] = {
    16, // nose[0]
    16, // left_eye_inner[1]
    16, // left_eye[2]
    16, // left_eye_outer[3]
    16, // right_eye_inner[4]
    16, // right_eye[5]
    16, // right_eye_outer[6]
    16, // left_ear[7]
    16, // right_ear[8]
    16, // mouth_left[9]
    16, // mouth_right[10]
    5, // left_shoulder[11]
    2, // right_shoulder[12]
    6, // left_elbow[13]
    3, // right_elbow[14]
    7, // left_wrist[15]
    4, // right_wrist[16]
    18, // left_pinky[17]
    17, // right_pinky[18]
    18, // left_index[19]
    17, // right_index[20]
    18, // left_thumb[21]
    17, // right_thumb[22]
    11, // left_hip[23]
    8, // right_hip[24]
    12, // left_knee[25]
    9, // right_knee[26]
    13, // left_ankle[27]
    10, // right_ankle[28]
    20, // left_heel[29]
    19, // right_heel[30]
    20, // left_foot_index[31]
    19, // right_foot_index[32]
}; 
    
void* bridge_create(){
    fitplayBridge::BridgeClass* bridge_handler = new fitplayBridge::BridgeClass();
    if(bridge_handler == nullptr){
        cout << __FUNCTION__ << ": Error: invalid bridge handler" << endl;
    }
    return reinterpret_cast<void*>(bridge_handler);
}

const int bridge_release_buffer(void* handler){
    fitplayBridge::BridgeClass* bridge_handler = reinterpret_cast<fitplayBridge::BridgeClass*>(handler);
    if(bridge_handler == nullptr){
        cout << __FUNCTION__ << ": Error: invalid bridge handler" << endl;
        return -1;
    }

    bridge_handler->release();
    return 0;
}

const int bridge_perform (void* handler, MidWareLandmark* landmarks, int num, 
                                    MidWareLandmark3D* landmarks3d, int num3d, 
                                    unsigned char** output, unsigned int* len){
    cout << "Enter: " << __FUNCTION__ << endl;

    fitplayBridge::BridgeClass* bridge_handler = reinterpret_cast<fitplayBridge::BridgeClass*>(handler);
    if(bridge_handler == nullptr){
        cout << __FUNCTION__ << ": Error: invalid bridge handler" << endl;
        return -1;
    }

    // TODO: optimize pre-allocated size.
    cout << __FUNCTION__ << ": start building input flatbuffer" << endl;
    flatbuffers::FlatBufferBuilder builder(10240);

    vector<flatbuffers::Offset<PoseData::Point>> keypoints_vec;
    vector<flatbuffers::Offset<PoseData::Point>> keypoints3d_vec;

    vector<json> keypoints_json_vec;
    vector<json> keypoints3d_json_vec;

    for(int i = 0; i < MEDIAPIPE_LANDMARK_SIZE; i++){
        auto name = builder.CreateString(to_string(i).c_str());
        int landmarkIndex = num < MEDIAPIPE_LANDMARK_SIZE ? fitplayToMediapipeLandmarkIndexMap[i] : i;
        auto point = PoseData::CreatePoint(builder, landmarks[landmarkIndex].x, landmarks[landmarkIndex].y, landmarks[landmarkIndex].z, landmarks[landmarkIndex].confidence, name);
        json keypoint_json;
        keypoint_json["x"] = landmarks[landmarkIndex].x;
        keypoint_json["y"] = landmarks[landmarkIndex].y;
        keypoint_json["z"] = landmarks[landmarkIndex].z;
        keypoint_json["score"] = landmarks[landmarkIndex].confidence;
        if(i < MEDIAPIPE_LANDMARK_SIZE){
            keypoint_json["name"] = landmark_name[i];
        }else{
            keypoint_json["name"] = "unknown";
        }
        keypoints_json_vec.push_back(keypoint_json);
        keypoints_vec.push_back(point);
    }
    auto keypoints = builder.CreateVector(keypoints_vec);

    for(int i = 0; i < MEDIAPIPE_LANDMARK_SIZE; i++){
        auto name = builder.CreateString(to_string(i).c_str());
        int landmarkIndex = num3d < MEDIAPIPE_LANDMARK_SIZE ? fitplayToMediapipeLandmarkIndexMap[i] : i;
        auto point3d = PoseData::CreatePoint(builder, landmarks3d[landmarkIndex].x, landmarks3d[landmarkIndex].y, landmarks3d[landmarkIndex].z, landmarks[landmarkIndex].confidence, name);
        json keypoint_json;
        keypoint_json["x"] = landmarks3d[landmarkIndex].x;
        keypoint_json["y"] = landmarks3d[landmarkIndex].y;
        keypoint_json["z"] = landmarks3d[landmarkIndex].z;
        keypoint_json["score"] = landmarks[landmarkIndex].confidence;
        if(i < MEDIAPIPE_LANDMARK_SIZE){
            keypoint_json["name"] = landmark_name[i];
        }else{
            keypoint_json["name"] = "unknown";
        }
        keypoints3d_json_vec.push_back(keypoint_json);
        keypoints3d_vec.push_back(point3d);
    }

    auto keypoints3d = builder.CreateVector(keypoints3d_vec);

    PoseData::PoseBuilder pose_builder(builder);
    pose_builder.add_keypoints(keypoints);
    pose_builder.add_keypoints3D(keypoints3d);
    auto pose = pose_builder.Finish();

    OsInput::InputMessageBuilder input_builder(builder);
    input_builder.add_pose(pose);
    input_builder.add_type(OsInput::MessageType::MessageType_Pose);
    auto input_msg = input_builder.Finish();

    builder.Finish(input_msg);

    cout << __FUNCTION__ << ": built input flatbuffer" << endl;

    bridge_handler->processData(reinterpret_cast<const char*>(builder.GetBufferPointer()));

    // uint8_t* flatbuffer = bridge_handler->getCurrentBuffer();
    *len = bridge_handler->getCurrentBufferSize();
    if(*len == 0){
        cout << __FUNCTION__ << ": no output flatbuffer avaliable" <<endl;
        return 1;
    }else{
        cout << __FUNCTION__ << ": flatbuffer size: " << *len <<endl;
        uint8_t* flatbuffer = bridge_handler->getCurrentBuffer();
        if(flatbuffer == nullptr){
            cout << __FUNCTION__ << " Error: current buffer is null" << endl;
            return -1;
        }

        *output = (unsigned char*)calloc(*len, sizeof(flatbuffer));
        if(*output == nullptr){
            cout << __FUNCTION__ << " Error: output buffer alloc failed" << endl;
        }else{
            cout << __FUNCTION__ << ": output buffer size "<< *len << endl;
        }
        memcpy(*output, flatbuffer, *len);

        const OsOutput::OutputMessage* output_msg = OsOutput::GetOutputMessage(*output);
    }

    return 0;
}

const int bridge_process_request_flatbuffer(void* handler, const char* input) {
    cout << "Enter: " << __FUNCTION__ << endl;
    fitplayBridge::BridgeClass* bridge_handler = reinterpret_cast<fitplayBridge::BridgeClass*>(handler);
    if(bridge_handler == nullptr){
        cout << __FUNCTION__ << ": Error: invalid bridge handler" << endl;
        return -1;
    }
    if(input == nullptr){
        cout << __FUNCTION__ << ": Error: input is NULL" << endl;
        return -1;
    }
    bridge_handler->processData(input);
    return 0;
}

const int bridge_process_request(void* handler, json app_json){
    cout << "Enter: " << __FUNCTION__ << endl;
    fitplayBridge::BridgeClass* bridge_handler = reinterpret_cast<fitplayBridge::BridgeClass*>(handler);
    if(bridge_handler == nullptr){
        cout << __FUNCTION__ << ": Error: invalid bridge handler" << endl;
        return -1;
    }

    if(app_json == NULL){
        cout << __FUNCTION__ << ": Error: app_json is NULL" << endl;
        return -1;
    }
    cout<< __FUNCTION__ << ": get json: " << app_json << endl;

    flatbuffers::FlatBufferBuilder builder(1024);
    if(app_json["type"].is_string() && app_json["type"].get<std::string>() == "application_client"){
        string id_str = "null";
        if(app_json["id"].is_string()){
            id_str = app_json["id"].get<string>();
        }
        auto id = builder.CreateString(id_str);
        auto app_client = ApplicationClient::CreateClient(builder, id);
        
        OsInput::InputMessageBuilder input_builder(builder);
        input_builder.add_type(OsInput::MessageType::MessageType_ApplicationClient);
        input_builder.add_client(app_client);
        auto input_msg = input_builder.Finish();
        builder.Finish(input_msg);
        bridge_handler->processData(reinterpret_cast<const char*>(builder.GetBufferPointer()));
    }else if(app_json["type"].is_string() && app_json["type"].get<std::string>() == "application_control"){
        flatbuffers::Offset<flatbuffers::String> feature_id;
        flatbuffers::Offset<flatbuffers::String> action;

        if(app_json["feature_id"].is_string()){
            feature_id = builder.CreateString(app_json["feature_id"].get<std::string>());
        }
        if(app_json["action"].is_string()){
            action = builder.CreateString(app_json["action"].get<std::string>());
        }
        // TODO: read real data from json
        int fps = 0;
        int height = 0;
        auto control_data = ApplicationControl::CreateControlData(builder, fps, height);
        auto app_control = ApplicationControl::CreateControl(builder, feature_id, action, control_data);

        OsInput::InputMessageBuilder input_builder(builder);
        input_builder.add_type(OsInput::MessageType::MessageType_ApplicationControl);
        input_builder.add_control(app_control);
        auto input_msg = input_builder.Finish();
        builder.Finish(input_msg);
        bridge_handler->processData(reinterpret_cast<const char*>(builder.GetBufferPointer()));
    }else{
        cout << __FUNCTION__ << ": Warnning: unknown type: "<< app_json["type"] << endl;
        return -1;
    }

    return 0;
}
