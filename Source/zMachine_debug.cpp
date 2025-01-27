#include "zMachine.h"
#include <iostream>
#include <iomanip>

void zMachine::debugPrintInstruction()

{
  if (!m_logEnabled) {
    return;
  }
  std::cout << std::endl;
  std::cout << std::hex << std::setw(8);
  std::cout << m_PC << std::setw(8) << m_op;

  for (auto ix = 0; ix < m_opOperandCount; ix++) {
    std::cout << " OP:" << m_opOperands[ix];
    if (m_opOperandsVar[ix] != 0xFF) {
      std::cout << "(" << m_opOperandsVar[ix] << ")";
    }

  }
  if (m_opReturnVar != 0xFF) {
    std::cout << " RET:" << m_opReturnValue << "(" << m_opReturnVar << ")";
  }

  std::cout << std::endl;
}
