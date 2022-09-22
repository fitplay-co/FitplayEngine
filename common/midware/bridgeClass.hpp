#include <string>
#include <vector>
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

      Output::OutputMessageBuilder outputBuilder(outputData);
      outputBuilder.add_type(typeOffset);
      outputBuilder.add_sensorType(sensorTypeOffset);
      outputBuilder.add_pose(poseOffset);
      outputBuilder.add_detectionResult(actionOffset);
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
}
