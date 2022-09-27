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
  
    // LOG(INFO) << "Found " << "glog started where to go" << " cookies";
    // cout << "enter wasm module careful, printing will cause performance lag!!!" << endl;
    // cout << "cere hello world start" << endl;

      // The variable to solve for with its initial value.
  double initial_x = 5.0;
  double x = initial_x;

  // Build the problem.
  Problem problem;

  // Set up the only cost function (also known as residual). This uses
  // auto-differentiation to obtain the derivative (jacobian).
  CostFunction* cost_function =
      new AutoDiffCostFunction<CostFunctor, 1, 1>(new CostFunctor);
  problem.AddResidualBlock(cost_function, nullptr, &x);

  // Run the solver!
  Solver::Options options;
  options.linear_solver_type = ceres::DENSE_QR;
  options.minimizer_progress_to_stdout = true;
  Solver::Summary summary;
  Solve(options, &problem, &summary);

  std::cout << summary.BriefReport() << "\n";
  std::cout << "x : " << initial_x
            << " -> " << x << "\n";

    cout << "cere hello world end" << endl;

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