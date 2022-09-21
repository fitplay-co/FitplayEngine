#include <string>
#include <vector>
#include "fitting/fitting_component.hpp"
#include "poseData_generated.h"
#include "actionData_generated.h"
#include "featureConfig_generated.h"
#include "inputMessage_generated.h"
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
      flatbuffers::FlatBufferBuilder action_data;
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
    if (action_data.GetSize() > 0) {
      action_data.Release();
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
      bool hasOutput = midwareManager.componentList.at(i)->handleInput(inputMessage, action_data);
      if (hasOutput) {
        componentIndexWithOutput.push_back(i);
      }
    }

    if (componentIndexWithOutput.size() > 0) {
      hasOutputMessage = true;
      actionData::ActionBuilder actionBuilder(action_data);
      for (int i = 0; i < componentIndexWithOutput.size(); i++) {
        midwareManager.componentList.at(componentIndexWithOutput.at(i))->writeToFlatbuffers(actionBuilder);
      }
      auto build = actionBuilder.Finish();
      action_data.Finish(build);
    }
  }

  int BridgeClass::getCurrentBufferSize() {
    return action_data.GetSize();
  }

  uint8_t* BridgeClass::getCurrentBuffer() {
    return action_data.GetBufferPointer();
  }

  bool BridgeClass::hasOutput() {
    return hasOutputMessage;
  }
}
