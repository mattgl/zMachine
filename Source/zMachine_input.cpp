#include "zMachine.h"
#include <iostream>
#include <sstream>
#include <cctype>

// *
//  Reads a single line from the selected input stream
// *
void zMachine::stringRead(uint16_t textAddr)

{ std::string str;
  uint8_t    *pText = m_mem + textAddr;
  uint8_t     i = 1;

  //TODO: Check if stream 1 is selected
  std::getline(std::cin, str);

  for (auto c : str) {
    if (i > pText[0] - 2) {
      break;
    }
    //TODO: Convert Unicode/locale to ZASCII. Currently assuming ascii only.
    pText[i++] = std::tolower(c); 
  }
  pText[i] = 0;
  m_ostreamCommands << str << std::endl;
}

