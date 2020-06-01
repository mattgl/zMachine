#include "zMachine.h"
#include <iostream>

// *
//  Prints a single ZSCII character to the selected streams
// *
void zMachine::printZSCII(uint16_t data)

{
  //TODO: Add support for unicode characters according to default unicode table.
  //TODO: Implement one stream that routes to correct destination

  ASSERT(data <= 0xFF); //Values greater are undefined in ZSCII

  //Stream 3 (Z machine memory)
  if (m_ostreamSelection & (1 << 4)) {
    Error("Outputstream 3 (memory) not implemented");
    return;
  }

  //Stream 1 (screen)
  if (m_ostreamSelection & (1 << 1)) {
    std::cout << (char)data;
  }

}

// *
//  Prints a single ZSCII character to the selected streams
// *
void zMachine::printNum(uint16_t data)

{
  std::cout << (int16_t)data;
}