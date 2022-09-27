#include <string>
#include <vector>
#include <chrono>
#include "fitting/fitting_component.hpp"
#include "poseData_generated.h"
#include "actionData_generated.h"
#include "featureConfig_generated.h"
#include "inputMessage_generated.h"
#include "outputMessage_generated.h"
#include "actionDetection/actionDetectionComponent.hpp"
#include "gazeTracking/gazeTracking.hpp"
#include "groundLocation/groundLocation.hpp"
#include "midwareComponent/midwareComponent.hpp"
#include "midwareComponent/midwareManager.hpp"
#include "generalDetection/generalCalculator.hpp"
#include "json.hpp"

#define FITPLAY_ENGINE_VERSION "0.1.0"

namespace fitplayBridge {
  class BridgeClass {
    private:
      fitplay::fittingComponent fittingInstance;
      actionDetection::actionDetectionComponent actionDetectionInstance;
      gaze::gazeTracking gazeInstance;
      ground::groundLocation groundInstance;
      flatbuffers::FlatBufferBuilder outputData;
      Midware::MidwareManager midwareManager;
      generalDetection::generalCalculator generalInstance;
      bool hasOutputMessage;
    public:
      BridgeClass();
      ~BridgeClass();
      void release();
      int64_t test();
      void processData(const char*);
      uint8_t* getCurrentBuffer();
      int getCurrentBufferSize();
      bool hasOutput();
      std::string getCurrentJSON();
  };

  BridgeClass::BridgeClass() {
    midwareManager.registerMidwareComponent(&groundInstance);
    midwareManager.registerMidwareComponent(&gazeInstance);
    midwareManager.registerMidwareComponent(&actionDetectionInstance);
    midwareManager.registerMidwareComponent(&fittingInstance);
    midwareManager.registerMidwareComponent(&generalInstance);
  }
  BridgeClass::~BridgeClass() {}

  void BridgeClass::release() {
    if (outputData.GetSize() > 0) {
      outputData.Release();
    }
  }

  int64_t BridgeClass::test() {
    return 7;
  }

  void BridgeClass::processData(const char* inputData) {
    auto processingTime = std::chrono::duration_cast<std::chrono::microseconds>
        (std::chrono::system_clock::now().time_since_epoch()).count();
    const Input::InputMessage* inputMessage = Input::GetInputMessage(&inputData[0]);
    hasOutputMessage = false;
    vector<int> componentIndexWithOutput;
    for (int i = 0; i < midwareManager.componentList.size(); i++) {
      bool hasOutput = midwareManager.componentList.at(i)->handleInput(inputMessage, outputData);
      if (hasOutput) {
        componentIndexWithOutput.push_back(i);
      }
    }

    if (componentIndexWithOutput.size() > 0) {
      hasOutputMessage = true;
      actionData::ActionBuilder actionBuilder(outputData);
      for (int i = 0; i < componentIndexWithOutput.size(); i++) {
        midwareManager.componentList.at(componentIndexWithOutput.at(i))->writeToFlatbuffers(actionBuilder);    
        }
      auto actionOffset = actionBuilder.Finish();

      std::vector<flatbuffers::Offset<PoseData::Point>> keypoints;
      std::vector<flatbuffers::Offset<PoseData::Point>> keypoints3D;
      if (inputMessage->type() == Input::MessageType::MessageType_Pose) {
        const PoseData::Pose* pose = inputMessage->pose();
        for (int i = 0; i < pose->keypoints()->size(); i++) {
          keypoints.push_back(PoseData::CreatePoint(outputData,
                                                    pose->keypoints()->Get(i)->x(),
                                                    pose->keypoints()->Get(i)->y(),
                                                    pose->keypoints()->Get(i)->z(),
                                                    pose->keypoints()->Get(i)->score()));
        }
        for (int i = 0; i < pose->keypoints3D()->size(); i++) {
          keypoints3D.push_back(PoseData::CreatePoint(outputData,
                                                    pose->keypoints3D()->Get(i)->x(),
                                                    pose->keypoints3D()->Get(i)->y(),
                                                    pose->keypoints3D()->Get(i)->z(),
                                                    pose->keypoints3D()->Get(i)->score()));
        }

      }
      auto keypointsOffset = outputData.CreateVector(keypoints);
      auto keypointsOffset3D = outputData.CreateVector(keypoints3D);
      PoseData::PoseBuilder poseBuilder(outputData);
      if (inputMessage->type() == Input::MessageType::MessageType_Pose) {
        poseBuilder.add_keypoints(keypointsOffset);
        poseBuilder.add_keypoints3D(keypointsOffset3D);
      }
      auto poseOffset = poseBuilder.Finish();

      auto typeOffset = outputData.CreateString("application_frame");
      auto sensorTypeOffset = outputData.CreateString("camera");

      auto beforeSendTime = std::chrono::duration_cast<std::chrono::microseconds>
        (std::chrono::system_clock::now().time_since_epoch()).count();
      Output::TimeProfilingBuilder timeProfilingBuilder(outputData);
      timeProfilingBuilder.add_processingTime(processingTime);
      timeProfilingBuilder.add_beforeSendTime(beforeSendTime);
      auto timeProfilingOffset = timeProfilingBuilder.Finish();

      Output::OutputMessageBuilder outputBuilder(outputData);
      outputBuilder.add_type(typeOffset);
      outputBuilder.add_sensorType(sensorTypeOffset);
      outputBuilder.add_pose(poseOffset);
      outputBuilder.add_detectionResult(actionOffset);
      outputBuilder.add_timeProfiling(timeProfilingOffset);
      outputData.Finish(outputBuilder.Finish());  
    }
  }

  int BridgeClass::getCurrentBufferSize() {
    return outputData.GetSize();
  }

  uint8_t* BridgeClass::getCurrentBuffer() {
    return outputData.GetBufferPointer();
  }

  bool BridgeClass::hasOutput() {
    return hasOutputMessage;
  }

  static const string LANDMARK_NAME[33] = {
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

  using nlohmann::json;
  std::string BridgeClass::getCurrentJSON() {
    json result;

    if (outputData.GetSize() > 0) {
      result["type"] = "application_frame";
      result["sensor_type"] = "camera";

      const Output::OutputMessage* outputMessage = Output::GetOutputMessage(getCurrentBuffer());

      vector<json> keypoints_json_vec;
      vector<json> keypoints3d_json_vec;

      const PoseData::Pose* pose = outputMessage->pose();
 
      for(int i = 0; i < pose->keypoints()->size(); i++){
          json keypoint_json;
          keypoint_json["x"] = pose->keypoints()->Get(i)->x();
          keypoint_json["y"] = pose->keypoints()->Get(i)->y();
          keypoint_json["z"] = pose->keypoints()->Get(i)->z();
          keypoint_json["score"] = pose->keypoints()->Get(i)->score();
          keypoint_json["name"] = LANDMARK_NAME[i];

          keypoints_json_vec.push_back(keypoint_json);
      }
      for(int i = 0; i < pose->keypoints3D()->size(); i++){
          json keypoint_json;
          keypoint_json["x"] = pose->keypoints3D()->Get(i)->x();
          keypoint_json["y"] = pose->keypoints3D()->Get(i)->y();
          keypoint_json["z"] = pose->keypoints3D()->Get(i)->z();
          keypoint_json["score"] = pose->keypoints3D()->Get(i)->score();
          keypoint_json["name"] = LANDMARK_NAME[i];

          keypoints3d_json_vec.push_back(keypoint_json);
      }
      result["pose_landmark"]["keypoints"] = keypoints_json_vec;
      result["pose_landmark"]["keypoints3D"] = keypoints3d_json_vec;
      result["pose_landmark"]["version"] = FITPLAY_ENGINE_VERSION;

      const actionData::Action* action = outputMessage->detectionResult();

      if(action->fitting() != NULL){
          cout << __FUNCTION__ << ": has fitting output" << endl;
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
              result["fitting"]["rotation"] = rotation_json_vec;
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
              result["fitting"]["mirrorRotation"] = mirror_rotation_json_vec;
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
              result["fitting"]["localRotation"] = local_rotation_json_vec;
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
              result["fitting"]["mirrorLocalRotation"] = mirror_local_rotation_json_vec;
          }
      }

      if(action->ground() != NULL) {
          cout << __FUNCTION__ << ": has ground output" << endl;
          result["ground_location"]["x"] = action->ground()->x();
          result["ground_location"]["y"] = action->ground()->y();
          result["ground_location"]["z"] = action->ground()->z();
          result["ground_location"]["legLength"] = action->ground()->legLength();
          result["ground_location"]["tracing"] = action->ground()->tracing();
      }

      if(action->walk() != NULL){
          // cout << __FUNCTION__ << ": has walk output" << endl;
          result["action_detection"]["version"] = "0.2";
          result["action_detection"]["walk"]["leftLeg"] = action->walk()->leftLeg();
          result["action_detection"]["walk"]["rightLeg"] = action->walk()->rightLeg(); 
          result["action_detection"]["walk"]["leftFrequency"] = action->walk()->leftFrequency(); 
          result["action_detection"]["walk"]["rightFrequency"] = action->walk()->rightFrequency(); 
          result["action_detection"]["walk"]["leftHipAng"] = action->walk()->leftHipAng(); 
          result["action_detection"]["walk"]["rightHipAng"] = action->walk()->rightHipAng(); 
          result["action_detection"]["walk"]["leftStepLength"] = action->walk()->leftStepLength(); 
          result["action_detection"]["walk"]["rightStepLength"] = action->walk()->rightStepLength(); 
          result["action_detection"]["walk"]["leftProgress"] = action->walk()->leftProgress(); 
          result["action_detection"]["walk"]["rightProgress"] = action->walk()->rightProgress();  
          result["action_detection"]["walk"]["turn"] = action->walk()->turn(); 
          result["action_detection"]["walk"]["stepRate"] = action->walk()->stepRate(); 
          result["action_detection"]["walk"]["stepLen"] = action->walk()->stepLen(); 
          result["action_detection"]["walk"]["velocity"] = action->walk()->velocity();
          result["action_detection"]["walk"]["velocityThreshold"] = action->walk()->velocityThreshold(); 
          result["action_detection"]["walk"]["realtimeLeftLeg"] = action->walk()->realtimeLeftLeg(); 
          result["action_detection"]["walk"]["realtimeRightLeg"] = action->walk()->realtimeRightLeg();  
      }

      if(action->squat() != NULL){
          cout << __FUNCTION__ << ": has squat output" << endl;
          result["action_detection"]["squat"]["squat"] = (int)(action->squat()->status());
      }

      if(action->jump() != NULL){
          cout << __FUNCTION__ << ": has jump output" << endl;
          result["action_detection"]["jump"]["onTheGround"] = action->jump()->onTheGround();
          result["action_detection"]["jump"]["velocity"] = action->jump()->velocity();
      }

      if(action->gaze() != NULL){
          cout << __FUNCTION__ << ": has gaze output" << endl;
          result["gaze_tracking"]["x"] = action->gaze()->x();
          result["gaze_tracking"]["y"] = action->gaze()->y();
          result["gaze_tracking"]["z"] = action->gaze()->z();
      }

      if(action->general() != NULL){
          cout << __FUNCTION__ << ": has general output" << endl;
          result["general_detection"]["confidence"] = action->general()->confidence();
      }

      if(action->stand() != NULL){
          cout << __FUNCTION__ << ": has stand output" << endl;
           result["stand_detection"]["mode"] = action->stand()->mode();
        }
    }

    return result.dump();
    // return "test";
  }
}
