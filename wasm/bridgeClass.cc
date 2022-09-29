#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <string>
#include <vector>
#include "bridgeClass.hpp"
#include <glog/logging.h>
#include <stdio.h>
#include "ceres/ceres.h"
#include "glog/logging.h"

using ceres::AutoDiffCostFunction;
using ceres::CostFunction;
using ceres::Problem;
using ceres::Solve;
using ceres::Solver;

using namespace emscripten;

struct CostFunctor {
  template <typename T>
  bool operator()(const T* const x, T* residual) const {
    residual[0] = 10.0 - x[0];
    return true;
  }
};

class JSMidwareBridgeClass {
public:
  JSMidwareBridgeClass()
  {
    //init glog 
    google::InitGoogleLogging("FitplayEngine");
  }

  void release() {
    midwareBridge.release();
  }

  std::string getCurrentJsonString() {
    return midwareBridge.getCurrentJSON();
  }

  val entry(std::string inputData) {
    midwareBridge.processData(&inputData[0]);
    if (midwareBridge.hasOutput()) {
      uint8_t *byteBuffer = midwareBridge.getCurrentBuffer();
      size_t bufferLength = midwareBridge.getCurrentBufferSize();
      return val(typed_memory_view(bufferLength <= 0 ? 0 : bufferLength, byteBuffer));
    } else {
      return val(typed_memory_view(0, &inputData[0]));
    }
    
  }


private:
  fitplayBridge::BridgeClass midwareBridge;
};

// Binding code
EMSCRIPTEN_BINDINGS(my_class_example) {
  class_<JSMidwareBridgeClass>("BridgeClass")
    .constructor<>()
    .function("entry", &JSMidwareBridgeClass::entry)
    .function("release", &JSMidwareBridgeClass::release)
    .function("json", &JSMidwareBridgeClass::getCurrentJsonString)
    ;
}