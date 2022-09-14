#include "hello-time.h"
#include <ctime>
#include <iostream>
#include "flatbuffers/flatbuffers.h"
#include "glm/glm.hpp"

void print_localtime() {
  std::time_t result = std::time(nullptr);
  std::cout << std::asctime(std::localtime(&result));
}

int callFunction() {
  return 15;
}