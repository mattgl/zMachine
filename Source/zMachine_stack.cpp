#include "zMachine.h"

//StackFrame
// uint16_t     return stackFrame (adress)  0
// uint16_t     return PC         (address) 2
// char         returnVal                   4
// char         numberOfLocalVars           5
// uint16_t[x]  local variables             x*2
// *bytes       evaluation_stack

void zMachine::stackPushFrame(uint16_t routineAddress)

{ uint8_t localVariableCount = 0;
  uint16_t nextStackFrame = m_stackPtr + 1;

  stackPushWord(m_currentStackFrame);
  stackPushWord(m_PC);

  m_PC = routineAddress;
  localVariableCount = memRead8(m_PC++);
  stackPushByte(localVariableCount);
  for (auto ix = 0; ix < localVariableCount; ix++) {
    stackPushWord(memRead16( m_PC));
    m_PC += 2;
  }
  m_currentStackFrame = nextStackFrame;
};

void zMachine::stackPopFrame(uint16_t returnValue) 

{
  m_stackPtr          = stackAddrLocalVarCount();
  stackPopByte();
  m_PC                = stackPopWord();
  m_currentStackFrame = stackPopWord();

  resultWrite(returnValue);
};

void    zMachine::stackPushWord(uint16_t val)

{
  memWrite16(++m_stackPtr,val, m_stack);
  m_stackPtr++;
};

uint16_t zMachine::stackPopWord()

{ 
  m_stackPtr--;
  return(memRead16(m_stackPtr--, m_stack));
};

void    zMachine::stackPushByte(uint8_t val)

{
  m_stack[++m_stackPtr] = val;
};

uint8_t    zMachine::stackPopByte()
{
  return(m_stack[m_stackPtr--]);
};

uint16_t zMachine::stackAddrLocalVarCount()

{
  return(m_currentStackFrame + 4);
}

uint16_t zMachine::stackAddrLocalVar(uint16_t x)

{
  return(m_currentStackFrame + 5 + (x - 1) * 2);
}
