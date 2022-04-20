#include <emscripten/val.h>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <string>
#include <vector>
#include "json.hpp"

// for convenience
using json = nlohmann::json;

using namespace emscripten;

float lerp(float a, float b, float t) {
    return (1 - t) * a + t * b;
}

float json_func(const std::string &str)
{
    // auto j = json::parse(R"({"happy": true, "pi": 3.141})");
    auto j = json::parse(str);
    float num = j["pi"];
	return num;
}

std::string test_func(const std::string &str)
{
	val res = val::undefined();
	
	if (str.compare("int") == 0) res = val(123);
	else if (str.compare("string") == 0) res = val("abc");
	else if (str.compare("bool") == 0) res = val(true);
	
	return "tested";
}

std::vector<int> returnVectorData () {
    std::vector<int> v(10, 1);
    return v;
}

std::map<int, std::string> returnMapData () {
    std::map<int, std::string> m;
    m.insert(std::pair<int, std::string>(10, "This is a string."));
    return m;
}

EMSCRIPTEN_BINDINGS(module) {
  function("returnVectorData", &returnVectorData);
  function("returnMapData", &returnMapData);
  function("json_func", &json_func);
  function("lerp", &lerp);
  function("test_func", &test_func);

  // register bindings for std::vector<int> and std::map<int, std::string>.
  register_vector<int>("vector<int>");
  register_map<int, std::string>("map<int, string>");
}