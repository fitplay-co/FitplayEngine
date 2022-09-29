#include "MidwareBridge.h"
#include "bridgeClass.hpp"
#include "json.hpp"
#include <chrono>

#define FITPLAY_ENGINE_VERSION "0.1.0"
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
                                    json* output_json){
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

    for(int i = 0; i < num; i++){
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

    for(int i = 0; i < num3d; i++){
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

    Input::InputMessageBuilder input_builder(builder);
    input_builder.add_pose(pose);
    input_builder.add_type(Input::MessageType::MessageType_Pose);
    auto input_msg = input_builder.Finish();

    builder.Finish(input_msg);

    cout << __FUNCTION__ << ": built input flatbuffer" << endl;

    bridge_handler->processData(reinterpret_cast<const char*>(builder.GetBufferPointer()));

    // uint8_t* flatbuffer = bridge_handler->getCurrentBuffer();
    int flatbuffer_size = bridge_handler->getCurrentBufferSize();
    if(flatbuffer_size == 0){
        cout << __FUNCTION__ << ": no output flatbuffer avaliable" <<endl;
        return 1;
    }else{
        cout << __FUNCTION__ << ": flatbuffer size: " << flatbuffer_size <<endl;
        uint8_t* flatbuffer = bridge_handler->getCurrentBuffer();

        const ActionData::Action* action = ActionData::GetAction(flatbuffer);

        if(action->fitting() != NULL){
            cout << __FUNCTION__ << ": has fitting output" << endl;
            // (*output_json)["fitting"]["version"] = "0.0.1";

            auto rotation = action->fitting()->rotation();
            if(rotation != NULL){
                vector<json> rotation_json_vec;
                for(int i = 0; i < rotation->size(); i++){
                    auto joint = rotation->Get(i);
                    json joint_json;
                    joint_json["name"] = joint->name()->c_str();
                    joint_json["x"] = joint->x();
                    joint_json["y"] = joint->y();
                    joint_json["z"] = joint->z();
                    joint_json["w"] = joint->w();
                    rotation_json_vec.push_back(joint_json);
                }
                (*output_json)["fitting"]["rotation"] = rotation_json_vec;
            }

            auto mirrorRotation = action->fitting()->mirrorRotation();
            if(mirrorRotation != NULL){
                vector<json> mirror_rotation_json_vec;
                for(int i = 0; i < mirrorRotation->size(); i++){
                    auto joint = mirrorRotation->Get(i);
                    json joint_json;
                    joint_json["name"] = joint->name()->c_str();
                    joint_json["x"] = joint->x();
                    joint_json["y"] = joint->y();
                    joint_json["z"] = joint->z();
                    joint_json["w"] = joint->w();
                    mirror_rotation_json_vec.push_back(joint_json);
                }
                (*output_json)["fitting"]["mirrorRotation"] = mirror_rotation_json_vec;
            }

            auto localRotation = action->fitting()->localRotation();
            if(localRotation != NULL){
                vector<json> local_rotation_json_vec;
                for(int i = 0; i < localRotation->size(); i++){
                    auto joint = localRotation->Get(i);
                    json joint_json;
                    joint_json["name"] = joint->name()->c_str();
                    joint_json["x"] = joint->x();
                    joint_json["y"] = joint->y();
                    joint_json["z"] = joint->z();
                    joint_json["w"] = joint->w();
                    local_rotation_json_vec.push_back(joint_json);
                }
                (*output_json)["fitting"]["localRotation"] = local_rotation_json_vec;
            }
            

            auto mirrorLocalRotation = action->fitting()->mirrorLocalRotation();
            if(mirrorLocalRotation != NULL){
                vector<json> mirror_local_rotation_json_vec;
                for(int i = 0; i < mirrorLocalRotation->size(); i++){
                    auto joint = mirrorLocalRotation->Get(i);
                    json joint_json;
                    joint_json["name"] = joint->name()->c_str();
                    joint_json["x"] = joint->x();
                    joint_json["y"] = joint->y();
                    joint_json["z"] = joint->z();
                    joint_json["w"] = joint->w();
                    mirror_local_rotation_json_vec.push_back(joint_json);
                }
                (*output_json)["fitting"]["mirrorLocalRotation"] = mirror_local_rotation_json_vec;
            }
            
            // auto fittedLandmarks = action->fitting()->fittedLandmarks();
            // vector<json> fkLandmark_json_vec;
            // for(int i = 0; i < fittedLandmarks->size(); i++){
            //     auto fkLandmark = fittedLandmarks->Get(i);
            //     json fkLandmark_json;
            //     fkLandmark_json["name"] = fkLandmark->name()->c_str();
            //     fkLandmark_json["x"] = fkLandmark->x();
            //     fkLandmark_json["y"] = fkLandmark->y();
            //     fkLandmark_json["z"] = fkLandmark->z();
            //     fkLandmark_json_vec.push_back(fkLandmark_json);
            // }
            // (*output_json)["fitting"]["fittedLandmarks"] = fkLandmark_json_vec;

            // auto fittedError = action->fitting()->fittedError();
            // (*output_json)["fitting"]["fittedError"] = fittedError;
        }

        if(action->ground() != NULL){
            cout << __FUNCTION__ << ": has ground output" << endl;
            (*output_json)["ground_location"]["x"] = action->ground()->x();
            (*output_json)["ground_location"]["y"] = action->ground()->y();
            (*output_json)["ground_location"]["z"] = action->ground()->z();
            (*output_json)["ground_location"]["legLength"] = action->ground()->legLength();
            (*output_json)["ground_location"]["tracing"] = action->ground()->tracing();
        }

        if(action->walk() != NULL){
            cout << __FUNCTION__ << ": has walk output" << endl;
            (*output_json)["action_detection"]["version"] = "0.2";
            (*output_json)["action_detection"]["walk"]["leftLeg"] = action->walk()->leftLeg();
            (*output_json)["action_detection"]["walk"]["rightLeg"] = action->walk()->rightLeg(); 
            (*output_json)["action_detection"]["walk"]["leftFrequency"] = action->walk()->leftFrequency(); 
            (*output_json)["action_detection"]["walk"]["rightFrequency"] = action->walk()->rightFrequency(); 
            (*output_json)["action_detection"]["walk"]["leftHipAng"] = action->walk()->leftHipAng(); 
            (*output_json)["action_detection"]["walk"]["rightHipAng"] = action->walk()->rightHipAng(); 
            (*output_json)["action_detection"]["walk"]["leftStepLength"] = action->walk()->leftStepLength(); 
            (*output_json)["action_detection"]["walk"]["rightStepLength"] = action->walk()->rightStepLength(); 
            (*output_json)["action_detection"]["walk"]["leftProgress"] = action->walk()->leftProgress(); 
            (*output_json)["action_detection"]["walk"]["rightProgress"] = action->walk()->rightProgress();  
            (*output_json)["action_detection"]["walk"]["turn"] = action->walk()->turn(); 
            (*output_json)["action_detection"]["walk"]["stepRate"] = action->walk()->stepRate(); 
            (*output_json)["action_detection"]["walk"]["stepLen"] = action->walk()->stepLen(); 
            (*output_json)["action_detection"]["walk"]["velocity"] = action->walk()->velocity(); 
            // (*output_json)["action_detection"]["walk"]["velocityThreshold"] = action->walk()->velocityThreshold(); 
            // (*output_json)["action_detection"]["walk"]["realtimeLeftLeg"] = action->walk()->realtimeLeftLeg(); 
            // (*output_json)["action_detection"]["walk"]["realtimeRightLeg"] = action->walk()->realtimeRightLeg(); 
        }

        if(action->squat() != NULL){
            cout << __FUNCTION__ << ": has squat output" << endl;
            (*output_json)["action_detection"]["squat"]["squat"] = (int)(action->squat()->status());
        }

        if(action->jump() != NULL){
            cout << __FUNCTION__ << ": has jump output" << endl;
            (*output_json)["action_detection"]["jump"]["onTheGround"] = action->jump()->onTheGround();
            (*output_json)["action_detection"]["jump"]["velocity"] = action->jump()->velocity();
        }

        if(action->gaze() != NULL){
            cout << __FUNCTION__ << ": has gaze output" << endl;
            (*output_json)["gaze_tracking"]["x"] = action->gaze()->x();
            (*output_json)["gaze_tracking"]["y"] = action->gaze()->y();
            (*output_json)["gaze_tracking"]["z"] = action->gaze()->z();
        }

        if(action->general() != NULL){
            cout << __FUNCTION__ << ": has general output" << endl;
            (*output_json)["general_detection"]["confidence"] = action->general()->confidence();
        }

        if(action->stand() != NULL){
           cout << __FUNCTION__ << ": has stand output" << endl;
           (*output_json)["stand_detection"]["mode"] = action->stand()->mode();
        }
    }

    (*output_json)["type"] = "application_frame";
    (*output_json)["sensor_type"] = "camera";
    if(keypoints_json_vec.size() > 0){
        (*output_json)["pose_landmark"]["keypoints"] = keypoints_json_vec;
    }
    if(keypoints3d_json_vec.size() > 0){
        (*output_json)["pose_landmark"]["keypoints3D"] = keypoints3d_json_vec;
    }
    
    (*output_json)["pose_landmark"]["timestamp"] = chrono::duration_cast<chrono::milliseconds>(
                                                chrono::system_clock::now().time_since_epoch()).count();
    (*output_json)["pose_landmark"]["version"] = FITPLAY_ENGINE_VERSION;

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
        
        Input::InputMessageBuilder input_builder(builder);
        input_builder.add_type(Input::MessageType::MessageType_ApplicationClient);
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

        Input::InputMessageBuilder input_builder(builder);
        input_builder.add_type(Input::MessageType::MessageType_ApplicationControl);
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
