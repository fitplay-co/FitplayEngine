#include <emscripten.h>
#include <emscripten/bind.h>
#include <string>
#include <vector>
#include "json.hpp"
#include "fitting/fitting.hpp"
#include "flatbuffer/test_generated.h"

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

  std::string jsonFunc(const std::string &str) {
    // auto j = json::parse(R"({"happy": true, "pi": 3.141})");
    // convert string to json
    json j = json::parse(str);
    // get data from json
    // float num = j["pose_landmark"]["keypoints"][1]["y"];
    
    // TODO: process input data
    // fitInstance.process(j);
    flatbuffers::FlatBufferBuilder builder(1024);
    auto name = builder.CreateString("test");
    auto testClass = Test::CreateTestC(builder, name);
    builder.Finish(orc);
  
    // add property to json object
    j["wasm_bridge_version"] = "0.0.1";
    // convert json to string
    std::string s = j.dump();
	  return s;
  }

  static std::string getStringFromInstance(const BridgeClass& instance) {
    return instance.y;
  }

private:
  int x;
  std::string y;
  fitplay::fitting fitInstance;
};

// Binding code
EMSCRIPTEN_BINDINGS(my_class_example) {
  class_<BridgeClass>("BridgeClass")
    .constructor<int, std::string>()
    .function("jsonFunc", &BridgeClass::jsonFunc)
    .property("x", &BridgeClass::getX, &BridgeClass::setX)
    .class_function("getStringFromInstance", &BridgeClass::getStringFromInstance)
    ;
}