#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <string>
#include <vector>
#include "fitting/fitting.hpp"
#include "flatbuffer/poseData_generated.h"
#include "flatbuffer/actionData_generated.h"
#include "actionDetection/walkDetection.hpp"
#include "actionDetection/jumpDetection.hpp"
#include "gazeTracking/gazeTracking.hpp"

using namespace emscripten;

class BridgeClass {
public:
  BridgeClass(int x, std::string y)
    : x(x)
    , y(y)
  {}

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

  val entry(std::string input) {
    PoseData::Pose* data = PoseData::GetMutablePose(&input[0]);

    float walk_data[3] = {0,0,0};
    float jump_data[2] = {0,0};
    float gaze_data[3] = {0,0,0};

    walkInstance.process(walk_data, data);
    jumpInstance.process(jump_data, data);
    gazeInstance.process(gaze_data, data);

    auto p0 = actionData::CreateWalk(action_data, walk_data[0], walk_data[1], walk_data[2]);
    auto p1 = actionData::CreateJump(action_data, jump_data[0], jump_data[1]);
    auto p2 = actionData::CreateGaze(action_data, gaze_data[0], gaze_data[1], gaze_data[2]);
    //walkInstance.process(action_data, data)
    auto build = actionData::CreateAction(action_data, p0, p1, p2);
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
  actionwalk::walk walkInstance;
  actionjump::jump jumpInstance;
  gaze::gazeTracking gazeInstance;
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