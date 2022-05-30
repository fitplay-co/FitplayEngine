#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <string>
#include <vector>
#include "json.hpp"
#include "fitting/fitting.hpp"
#include "flatbuffer/poseData_generated.h"
#include "actionDetection/walkDetection.hpp"

// for convenience
using json = nlohmann::json;
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
    if (walk_data.GetSize() > 0) {
      walk_data.Release();
    }
  }

  void entry(std::string input) {
    data = PoseData::GetMutablePose(&input[0]);
  }
  val walk_pose() {
    walk_data = walkInstance.process(data);
    uint8_t *byteBuffer = walk_data.GetBufferPointer();
    size_t bufferLength = walk_data.GetSize();
    return val(typed_memory_view(bufferLength, byteBuffer));
  }

  static std::string getStringFromInstance(const BridgeClass& instance) {
    return instance.y;
  }

private:
  int x;
  std::string y;
  fitplay::fitting fitInstance;
  action::walk walkInstance;
  flatbuffers::FlatBufferBuilder walk_data;
  PoseData::Pose* data;
};

// Binding code
EMSCRIPTEN_BINDINGS(my_class_example) {
  class_<BridgeClass>("BridgeClass")
    .constructor<int, std::string>()
    .function("entry", &BridgeClass::entry)
    .function("walk_pose", &BridgeClass::walk_pose)
    .function("release", &BridgeClass::release)
    .property("x", &BridgeClass::getX, &BridgeClass::setX)
    .class_function("getStringFromInstance", &BridgeClass::getStringFromInstance)
    ;
}