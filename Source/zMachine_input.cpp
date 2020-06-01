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

  //Stream 0 (console)
  if (m_istreamSelection & (1 << 0)) {
    std::getline(std::cin, str);
  }
  else {
    //Stream 1 (file)
    //TODO: Read from selected file instead
  }

  for (auto c : str) {
    if (i > pText[0] - 2) {
      break;
    }
    //TODO: Convert Unicode/locale to ZASCII. Currently assuming ascii only.
    pText[i++] = std::tolower(c); 
  }
  pText[i] = 0;
}

