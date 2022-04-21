#include <emscripten.h>
#include <emscripten/bind.h>
#include <string>
#include <vector>
#include "json.hpp"

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
    //TODO: process input data
	  return str;
  }

  static std::string getStringFromInstance(const BridgeClass& instance) {
    return instance.y;
  }

private:
  int x;
  std::string y;
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