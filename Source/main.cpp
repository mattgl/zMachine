// main.cpp : Defines the entry point for the console application.
//
#include <iostream>
#include <stdexcept>
#include "zMachine.h"

int main(int argc, char* argv[])

{
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << "[story file path]" << std::endl;
    return 1;
  }

  try {
    zMachine cpu;

    cpu.Load(argv[1]);
    cpu.Boot();
    cpu.Run();
  }
  catch (const std::exception& e)
  {
    std::cerr << "zMachine failed :" << e.what();
    return 1;
  }
  return 0;
}

