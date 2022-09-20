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

class BridgeClass {
public:
  BridgeClass()
  {
    midwareManager.registerMidwareComponent(&groundInstance);
    midwareManager.registerMidwareComponent(&gazeInstance);
    midwareManager.registerMidwareComponent(&actionDetectionInstance);
    midwareManager.registerMidwareComponent(&fittingInstance);
    midwareManager.registerMidwareComponent(&generalInstance);

    //init glog 
    google::InitGoogleLogging("FitplayEngine");
  }

  void release() {
    if (action_data.GetSize() > 0) {
      action_data.Release();
    }
  }

  val entry(std::string inputData) {
  
    const Input::InputMessage* inputMessage = Input::GetInputMessage(&inputData[0]);
    
    LOG(INFO) << "Found " << "glog started where to go" << " cookies";
    cout << "enter wasm module careful, printing will cause performance lag!!!" << endl;
    cout << "cere hello world start" << endl;

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