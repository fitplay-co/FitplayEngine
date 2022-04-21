#include <emscripten.h>
#include <emscripten/bind.h>
#include <string>
#include <vector>
#include "json.hpp"

// for convenience
using json = nlohmann::json;
using namespace emscripten;

std::string jsonFunc(const std::string &str)
{
    // auto j = json::parse(R"({"happy": true, "pi": 3.141})");
    // convert string to json
    auto j = json::parse(str);
    // get data from json
    float num = j["pose_landmark"]["keypoints"][1]["y"];
    // convert json to string
    std::string s = j.dump();
	return s;
}

// bind function with javascript
EMSCRIPTEN_BINDINGS(module) {
  function("jsonFunc", &jsonFunc);
}