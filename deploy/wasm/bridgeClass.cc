#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <string>
#include <vector>
#include "fitting/fitting_component.hpp"
#include "poseData_generated.h"
#include "actionData_generated.h"
#include "featureConfig_generated.h"
#include "inputMessage_generated.h"
#include "generalDetection/generalCalculator.hpp"
#include "actionDetection/actionDetectionComponent.hpp"
#include "gazeTracking/gazeTracking.hpp"
#include "groundLocation/groundLocation.hpp"
#include "midwareComponent/midwareComponent.hpp"
#include "midwareComponent/midwareManager.hpp"

using namespace emscripten;

class BridgeClass {
public:
  BridgeClass()
  {
    midwareManager.registerMidwareComponent(&groundInstance);
    midwareManager.registerMidwareComponent(&gazeInstance);
    midwareManager.registerMidwareComponent(&actionDetectionInstance);
    midwareManager.registerMidwareComponent(&fittingInstance);
    midwareManager.registerMidwareComponent(&generalInstance);
  }

  void release() {
    if (action_data.GetSize() > 0) {
      action_data.Release();
    }
  }

  val entry(std::string inputData) {
    const Input::InputMessage* inputMessage = Input::GetInputMessage(&inputData[0]);

    vector<int> componentIndexWithOutput;
    for (int i = 0; i < midwareManager.componentList.size(); i++) {
      bool hasOutput = midwareManager.componentList.at(i)->handleInput(inputMessage, action_data);
      if (hasOutput) {
        componentIndexWithOutput.push_back(i);
      }
    }

    actionData::ActionBuilder actionBuilder(action_data);

    for (int i = 0; i < componentIndexWithOutput.size(); i++) {
      midwareManager.componentList.at(componentIndexWithOutput.at(i))->writeToFlatbuffers(actionBuilder);
    }

    auto build = actionBuilder.Finish();
    action_data.Finish(build);
    uint8_t *byteBuffer = action_data.GetBufferPointer();
    size_t bufferLength = action_data.GetSize();
    return val(typed_memory_view(componentIndexWithOutput.size() <= 0 ? 0 : bufferLength, byteBuffer));
  }


private:
  fitplay::fittingComponent fittingInstance;
  generalDetection::generalCalculator generalInstance;
  actionDetection::actionDetectionComponent actionDetectionInstance;
  gaze::gazeTracking gazeInstance;
  ground::groundLocation groundInstance;
  flatbuffers::FlatBufferBuilder action_data;
  Midware::MidwareManager midwareManager;
};

// Binding code
EMSCRIPTEN_BINDINGS(my_class_example) {
  class_<BridgeClass>("BridgeClass")
    .constructor<>()
    .function("entry", &BridgeClass::entry)
    .function("release", &BridgeClass::release)
    ;
}