#include "zMachine.h"
#include <iostream>

using namespace std;

// *
//  Writes a result to the return variable (and increasing the program counter)
// *
void zMachine::resultWrite(uint16_t value)

{
  m_opReturnVar = memRead8(m_PC++);
  variableWrite(m_opReturnVar, value);
  m_opReturnValue = value;
}

// *
//  Reads and stores the value of an operand (increases program counter accordingly)
// *
bool zMachine::operandEvaluate(uint8_t operandType, uint8_t operandNumber)

{ uint16_t value;
  uint8_t  byte = 0xFF;

  switch (operandType) {
    case OP_LARGE_CONSTANT: 
      value = memRead16( m_PC);
      m_PC += 2;
      break;

    case OP_SMALL_CONSTANT: 
      value = memRead8(m_PC++);
      break;

    case OP_VARIABLE: 
      byte = memRead8(m_PC++);
      value = variableRead(byte);
      break;

    case OP_OMITTED: 
      return(false);
      break;

    default:
      Error("Unknown operand type");
      return(false);
      break;
  }
  m_opOperands[operandNumber] = value;
  m_opOperandsVar[operandNumber] = byte;
  return(true);
}

// *
//  Reads a variable
// *
uint16_t zMachine::variableRead(uint16_t var)

{
  ASSERT(var <= 0x00FF);
  if (var == 0x00) {
    return(stackPopWord());
  }
  else if (var < 0x10) {
    ASSERT((var) <= memRead8(stackAddrLocalVarCount(), m_stack));
    return(memRead16(stackAddrLocalVar(var), m_stack));
  }
  else {
    return(memRead16( m_addrTableGlobalVar + (var - 0x10)*2));
  }
}

// *
//  Writes a variable
// *
void zMachine::variableWrite(uint16_t var, uint16_t value)

{
  ASSERT(var <= 0x00FF);
  if (var == 0x00) {
    stackPushWord(value);
  }
  else if (var < 0x10) {
    ASSERT((var) <= memRead8(stackAddrLocalVarCount(), m_stack));
    memWrite16(stackAddrLocalVar(var),value, m_stack);
  }
  else {
    memWrite16( m_addrTableGlobalVar +(var-0x10)*2,value);
  }
}
