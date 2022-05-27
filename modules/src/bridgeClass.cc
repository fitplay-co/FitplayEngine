#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <string>
#include <vector>
#include "json.hpp"
#include "fitting/fitting.hpp"
#include "flatbuffer/poseData_generated.h"
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

  void release() {
    if (build_data.GetSize() > 0) {
      build_data.Release();
    }
  }

  val jsonFunc(std::string input) {

    json j;
    j["test_ver"] = "0.0.1";
    auto data = PoseData::GetMutablePose(&input[0]);
    for(int i = 0; i < 33; i++ ){
      j["keypoints"][i]["x"] = data->keypoints()->Get(i)->x();
      j["keypoints"][i]["y"] = data->keypoints()->Get(i)->y();
      j["keypoints"][i]["z"] = data->keypoints()->Get(i)->z();
      j["keypoints"][i]["score"] = data->keypoints()->Get(i)->score();
      j["keypoints"][i]["name"] = data->keypoints()->Get(i)->name()->str();
      j["keypoints3D"][i]["x"] = data->keypoints3D()->Get(i)->x();
      j["keypoints3D"][i]["y"] = data->keypoints3D()->Get(i)->y();
      j["keypoints3D"][i]["z"] = data->keypoints3D()->Get(i)->z();
      j["keypoints3D"][i]["score"] = data->keypoints3D()->Get(i)->score();
      j["keypoints3D"][i]["name"] = data->keypoints3D()->Get(i)->name()->str();
    }
    fitInstance.process(j);
    std::string s = j.dump();

    // flatbuffers::FlatBufferBuilder build_data(1024);
    auto point1 = PoseData::CreatePoint(build_data, 1, 1, 1, 1, build_data.CreateString("nose"));
    auto point2 = PoseData::CreatePoint(build_data, 1, 1, 1, 1, build_data.CreateString("arm"));
    auto point3 = PoseData::CreatePoint(build_data, 1, 1, 1, 1, build_data.CreateString("leg"));
    std::vector<flatbuffers::Offset<PoseData::Point>> nodeVector;
    nodeVector.push_back(point1);
    nodeVector.push_back(point2);
    nodeVector.push_back(point3);
    auto keypoint = build_data.CreateVector(nodeVector);
    auto keypoint3d = build_data.CreateVector(nodeVector);
    auto pose = PoseData::CreatePose(build_data, keypoint, keypoint3d);
    build_data.Finish(pose);
    // auto test = PoseData::GetPose(build_data.GetBufferPointer());
    // std::string res = test->keypoints3D()->Get(1)->name()->str();
    // return res;
    uint8_t *byteBuffer = build_data.GetBufferPointer();
    size_t bufferLength = build_data.GetSize();
    // std::string temp;
    // for (int i = 0; i < bufferLength; i++) {
    //   temp += byteBuffer[i];
    // }
    // return val(temp);
    // return reinterpret_cast<char*>(build_data.GetBufferPointer());
    // return reinterpret_cast<uint8_t*>(build_data.GetBufferPointer());
    // flatbuffers::SaveFile("pose.bin", reinterpret_cast<char*>(build_data.GetBufferPointer()), build_data.GetSize(), true);
    return val(typed_memory_view(bufferLength, byteBuffer));
    //flatbuffers::SaveFile("layer.bin", reinterpret_cast<char*>(build_data.GetBufferPointer()), build_data.GetSize(), true);
    // return build_data.GetBufferPointer();
    // return s;

    // auto j = json::parse(R"({"happy": true, "pi": 3.141})");
    // convert string to json
    // json j = json::parse(str);
    // get data from json
    // float num = j["pose_landmark"]["keypoints"][1]["y"];
    
    // TODO: process input data
    // fitInstance.process(j);
    //flatbuffers::FlatBufferBuilder builder(1024);
    // auto name = builder.CreateString("test");
    // auto testClass = Test::CreateTestC(builder, name);
    // builder.Finish(testClass);
    // uint8_t* bufferPointer = reinterpret_cast<uint8_t*>(input);
    //auto testC = Test::GetMutableTestC(&input[0]);
    // testC->mutate_name(builder.CreateString("test1"));
    // add property to json object
    // j["wasm_bridge_version"] = "0.0.1";
    // convert json to string
    // std::string s = j.dump();
	  //return testC->name()->str();
  }

  static std::string getStringFromInstance(const BridgeClass& instance) {
    return instance.y;
  }

private:
  int x;
  std::string y;
  fitplay::fitting fitInstance;
  flatbuffers::FlatBufferBuilder build_data;
};

// Binding code
EMSCRIPTEN_BINDINGS(my_class_example) {
  class_<BridgeClass>("BridgeClass")
    .constructor<int, std::string>()
    .function("jsonFunc", &BridgeClass::jsonFunc, allow_raw_pointers())
    .function("release", &BridgeClass::release)
    .property("x", &BridgeClass::getX, &BridgeClass::setX)
    .class_function("getStringFromInstance", &BridgeClass::getStringFromInstance)
    ;
}