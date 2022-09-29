#include <gtest/gtest.h>
#include <fstream>
#include <iostream>

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
    //read test file csv

      std::cout << "start !" << std::endl;

    std::ifstream in("test/data/testfile.csv");
     if (!in)
  {
    std::cerr << "Can not open file!";
  }

  std::string message;

  if (!(in >> message))
  {
    std::cerr << "Can not read file content!";
  }

  std::cout << message << std::endl;

}
