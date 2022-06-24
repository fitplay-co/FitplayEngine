#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <string>
#include <vector>
#include "fitting/fitting.hpp"
#include "poseData_generated.h"
#include "actionData_generated.h"
#include "featureConfig_generated.h"
#include "actionDetection/walkDetection.hpp"
#include "actionDetection/jumpDetection.hpp"
#include "actionDetection/squatDetection.hpp"
#include "gazeTracking/gazeTracking.hpp"
#include "groundLocation/groundLocation.hpp"

using namespace emscripten;

class BridgeClass {
public:
  BridgeClass(int x, std::string y)
    : x(x)
    , y(y)
  {
    fitInstance.mirror = false;
    mirrorFitInstance.mirror = true;
  }

  void incrementX() {
    ++x;
  }

  int getX() const { return x; }
  void setX(int x_) { x = x_; }

  void release() {
    if (action_data.GetSize() > 0) {
      action_data.Release();
    }
  }

  val entry(std::string inputData, std::string configs) {
    const PoseData::Pose* data = PoseData::GetPose(&inputData[0]);
    const FeatureConfigs::FeatureConfigList* featureConfigs = FeatureConfigs::GetFeatureConfigList(&configs[0]);

    bool actionDetectionEnable = false;
    bool gazeTrackingEnable = false;
    bool groundLocationEnable = false;
    bool groundLocationReset = false;
    bool groundLocationRgbdEnable = false;
    bool fittingEnable = false;
    for(int i = 0; i < featureConfigs->configs()->Length(); i++) {
      auto config = featureConfigs->configs()->Get(i);
      if ("action_detection" == config->feature_id()->str()) {
        actionDetectionEnable = config->enable();
      } else if ("ground_location" == config->feature_id()->str()) {
        groundLocationEnable = config->enable();
        groundLocationReset = config->action()->str() == "reset";
        groundLocationRgbdEnable = config->type()->str() == "rgbd";
      } else if ("gaze_tracking" == config->feature_id()->str()) {
        gazeTrackingEnable = config->enable();
      } else if ("fitting" == config->feature_id()->str()) {
        fittingEnable = config->enable();
      }
    }

    float walk_data[5] = {0,0,0,0,0};
    float jump_data[2] = {0,0};
    float gaze_data[3] = {0,0,0};
    float ground_data[5] = {0,0,0,0,0};
    float squat_data[1] = {0};
    flatbuffers::Offset<actionData::Walk> walk;
    flatbuffers::Offset<actionData::Jump> jump;
    flatbuffers::Offset<actionData::Squat> squat; 
    flatbuffers::Offset<actionData::Gaze> gazeOffset; 
    flatbuffers::Offset<actionData::Ground> groundLocation;
    flatbuffers::Offset<actionData::Fitting> fittingOffset;
    if (actionDetectionEnable) {
      walkInstance.process(walk_data, data);
      jumpInstance.process(jump_data, data);
      squatInstance.process(squat_data, data);
      walk = actionData::CreateWalk(action_data, walk_data[0], walk_data[1], walk_data[2], walk_data[3], walk_data[4]);
      jump = actionData::CreateJump(action_data, jump_data[0], jump_data[1]);
      squat = actionData::CreateSquat(action_data, squat_data[0]);
    }
    if (gazeTrackingEnable) {
      gazeInstance.process(gaze_data, data);
      gazeOffset = actionData::CreateGaze(action_data, gaze_data[0], gaze_data[1], gaze_data[2]);
    }
    if (groundLocationEnable) {
      groundInstance.process(ground_data, data, groundLocationReset, groundLocationRgbdEnable);
      groundLocation = actionData::CreateGround(action_data, ground_data[0], ground_data[1], ground_data[2], ground_data[3], ground_data[4]);
    }
    if (fittingEnable) {
      fitInstance.process(data);
      mirrorFitInstance.process(data);
      auto p1 = fitInstance.writeFlatBuffer(action_data);
      auto p2 = mirrorFitInstance.writeFlatBuffer(action_data);
      fittingOffset = actionData::CreateFitting(action_data, p1, p2);
    }

    actionData::ActionBuilder actionBuilder(action_data);

    if (actionDetectionEnable) {
      actionBuilder.add_walk(walk);
      actionBuilder.add_jump(jump);
      actionBuilder.add_squat(squat);
    }
    if (gazeTrackingEnable) {
      actionBuilder.add_gaze(gazeOffset);
    }
    if (groundLocationEnable) {
      actionBuilder.add_ground(groundLocation);
    }
    if (fittingEnable) {
      actionBuilder.add_fitting(fittingOffset);
    }

    auto build = actionBuilder.Finish();
    action_data.Finish(build);
    uint8_t *byteBuffer = action_data.GetBufferPointer();
    size_t bufferLength = action_data.GetSize();
    return val(typed_memory_view(bufferLength, byteBuffer));
  }
  // val jump_pose() {
  //   jump_data = jumpInstance.process(data);
  //   uint8_t *byteBuffer = jump_data.GetBufferPointer();
  //   size_t bufferLength = jump_data.GetSize();
  //   return val(typed_memory_view(bufferLength, byteBuffer));
  // }

  static std::string getStringFromInstance(const BridgeClass& instance) {
    return instance.y;
  }

private:
  int x;
  std::string y;
  fitplay::fitting fitInstance;
  fitplay::fitting mirrorFitInstance;
  actionwalk::walk walkInstance;
  actionjump::jump jumpInstance;
  actionsquat::squat squatInstance;
  gaze::gazeTracking gazeInstance;
  ground::groundLocation groundInstance;
  flatbuffers::FlatBufferBuilder action_data;
};

// Binding code
EMSCRIPTEN_BINDINGS(my_class_example) {
  class_<BridgeClass>("BridgeClass")
    .constructor<int, std::string>()
    .function("entry", &BridgeClass::entry)
    .function("release", &BridgeClass::release)
    .property("x", &BridgeClass::getX, &BridgeClass::setX)
    .class_function("getStringFromInstance", &BridgeClass::getStringFromInstance)
    ;
}