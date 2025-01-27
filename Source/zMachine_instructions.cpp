#include "zMachine.h"

// *
//  Executes a 0OP instruction
// *
void zMachine::evaluate0OP()

{
  switch (m_opNumber) {
    case 0x00:
      m_op = "rtrue";
      stackPopFrame(1);
      break;

    case 0x01:
      m_op = "rfalse";
      stackPopFrame(0);
      break;

    case 0x2:
      m_op = "print";
      printString(m_PC);
      break;

    case 0x3:
      m_op = "print_ret";
      printString(m_PC);
      printZSCII(0x0A);
      stackPopFrame(1);
      break;

    case 0x7:
      m_op = "restart";
      Load(m_pathStoryfile);
      Boot();
      break;

    case 0x8:
      m_op = "ret_popped";
      stackPopFrame(stackPopWord());
      break;

    case 0xA:
      m_op = "quit";
      m_quit = true;
      break;

    case 0xB:
      m_op = "newline";
      printZSCII(0x0A);
      break;

    default:
      Error("Unknown opcode");
      break;
  }
}

// *
//  Executes a 1OP instruction
// *
void zMachine::evaluate1OP()

{ int16_t tmp = 0;
  uint8_t bTmp = 0;
  uint16_t wTmp = 0;

  switch (m_opNumber) {
    case 0x0:
      m_op = "jz";
      branch(m_opOperands[0] == 0);
      break;

    case 0x1:
      m_op = "getSibling";
      bTmp = memRead8(OBJ_SIBLING(m_opOperands[0]));
      resultWrite(bTmp);
      branch(bTmp != 0);
      break;

    case 0x2:
      m_op = "getChild";
      bTmp = memRead8(OBJ_CHILD(m_opOperands[0]));
      resultWrite(bTmp);
      branch(bTmp != 0);
      break;

    case 0x3:
      m_op = "getParent";
      bTmp = memRead8(OBJ_PARENT(m_opOperands[0]));
      resultWrite(bTmp);
      break;

    case 0x4:
      m_op = "get_prop_len";
      if (m_opOperands[0] == 0) {
        resultWrite(0);
      }
      bTmp = memRead8(m_opOperands[0] - 1);
      bTmp = ((bTmp & 0xE0) >> 5) + 1;
      ASSERT(bTmp <= 8)
      ASSERT(bTmp >= 1)
      resultWrite(bTmp);
      break;

    case 0x5:
      m_op = "inc";
      tmp = (int16_t)variableRead(m_opOperands[0]);
      variableWrite(m_opOperands[0], ++tmp);
      break;

    case 0x6:
      m_op = "dec";
      tmp = (int16_t)variableRead(m_opOperands[0]);
      variableWrite(m_opOperands[0], --tmp);
      break;

    case 0x7:
      m_op = "print_addr";
      printString(m_opOperands[0]);
      break;

    case 0xA:
      m_op = "printObj";
      wTmp = objShortName(m_opOperands[0]);
      printString(wTmp);
      break;

    case 0xB:
      m_op = "ret";
      stackPopFrame(m_opOperands[0]);
      break;

    case 0xC:
      m_op = "jump";
      m_PC = m_PC + m_opOperands[0] - 2;
      break;

    case 0xD:
      m_op = "print_paddr";
      wTmp = m_opOperands[0] << 1;
      printString(wTmp);
      break;

    default:
      Error("Unknown opcode");
      break;
  }
}

// *
//  Executes a 2OP instruction
// *
void zMachine::evaluate2OP()

{ propertyInfo_t propertyInfo;
  int16_t        tmp = 0;
  uint8_t        bTmp = 0;
  bool           boolTmp = false;

  switch (m_opNumber) {
    case 0x01:
      m_op = "je";
      for (auto ix = 1; ix < m_opOperandCount; ix++) {
        boolTmp |= (m_opOperands[0] == m_opOperands[ix]);
      }
      branch(boolTmp);
      break;

    case 0x02:
      m_op = "jl";
      branch((int16_t)m_opOperands[0] < (int16_t)m_opOperands[1]);
      break;

    case 0x03:
      m_op = "jg";
      branch((int16_t)m_opOperands[0] >(int16_t)m_opOperands[1]);
      break;

    case 0x04:
      m_op = "dec chk";
      tmp = (int16_t)variableRead(m_opOperands[0]);
      variableWrite(m_opOperands[0], --tmp);
      branch(tmp < (int16_t)m_opOperands[1]);
      break;

    case 0x05:
      m_op = "inc chk";
      tmp = (int16_t)variableRead(m_opOperands[0]);
      variableWrite(m_opOperands[0], ++tmp);
      branch(tmp >(int16_t)m_opOperands[1]);
      break;

    case 0x06:
      m_op = "jin";
      branch(memRead8(OBJ_PARENT(m_opOperands[0])) == m_opOperands[1]);
      break;

    case 0x07:
      m_op = "test bitmap";
      branch((m_opOperands[0] & m_opOperands[1]) == m_opOperands[1]);
      break;

    case 0x09:
      m_op = "AND";
      resultWrite(m_opOperands[0] & m_opOperands[1]);
      break;

    case 0x0A:
      m_op = "test_attr";
      branch(objAttrGet(m_opOperands[0], m_opOperands[1]));
      break;

    case 0x0B:
      m_op = "set_attr";
      objAttrSet(m_opOperands[0], m_opOperands[1]);
      break;

    case 0x0C:
      m_op = "clear_attr";
      objAttrClear(m_opOperands[0], m_opOperands[1]);
      break;

    case 0x10:
      m_op = "loadb";
      resultWrite(memRead8(m_opOperands[0] + m_opOperands[1]));
      break;

    case 0x11:
      m_op = "get_prop";
      objPropertyAddrGet(m_opOperands[0], m_opOperands[1], propertyInfo);
      ASSERT(propertyInfo.size <= 2 && propertyInfo.size > 0);
      if (propertyInfo.size == 1) {
        resultWrite(memRead8(propertyInfo.dataAdress));
      }
      else {
        resultWrite(memRead16(propertyInfo.dataAdress));
      }
      break;

    case 0x12:
      m_op = "get_prop_addr";
      if (objPropertyAddrGet(m_opOperands[0], m_opOperands[1], propertyInfo)) {
        resultWrite(propertyInfo.dataAdress);
      }
      else {
        resultWrite(0);
      }
      break;

    case 0x0D:
      m_op = "store";
      variableWrite(m_opOperands[0], m_opOperands[1]);
      break;

    case 0x0E:
      m_op = "insert obj";
      //Detach object from old parent
      bTmp = memRead8(OBJ_PARENT(m_opOperands[0]));
      if (bTmp != 0) {
        memWrite8(OBJ_CHILD(bTmp), memRead8(OBJ_SIBLING(m_opOperands[0])));
      }

      //Attach object to new parent
      bTmp = memRead8(OBJ_CHILD(m_opOperands[1]));
      memWrite8(OBJ_CHILD(m_opOperands[1]), (uint8_t)m_opOperands[0]);
      memWrite8(OBJ_SIBLING(m_opOperands[0]), (uint8_t)bTmp);
      memWrite8(OBJ_PARENT(m_opOperands[0]), (uint8_t)m_opOperands[1]);
      break;

    case 0x0F:
      m_op = "load_w";
      tmp = memRead16(m_opOperands[0] + 2 * m_opOperands[1]);
      resultWrite(tmp);
      break;

    case 0x14:
      m_op = "add";
      tmp = (int16_t)m_opOperands[0] + (int16_t)m_opOperands[1];
      resultWrite(tmp);
      break;

    case 0x15:
      m_op = "sub";
      tmp = (int16_t)m_opOperands[0] - (int16_t)m_opOperands[1];
      resultWrite(tmp);
      break;

    case 0x16:
      m_op = "mul";
      tmp = (int16_t)m_opOperands[0] * (int16_t)m_opOperands[1];
      resultWrite(tmp);
      break;

    case 0x17:
      m_op = "div";
      ASSERT_MSG(m_opOperands[1] != 0, "Division by zero");
      tmp = (int16_t)m_opOperands[0] / (int16_t)m_opOperands[1];
      resultWrite(tmp);
      break;

    default:
      Error("Unknown opcode");
      break;
  }
}

// *
//  Executes a VAR instruction
// *
void zMachine::evaluateVAR()

{ propertyInfo_t propertyInfo;

  switch (m_opNumber) {
    case 0x00: //Call
      m_op = "call";

      //Create new stackframe
      stackPushFrame(2 * m_opOperands[0]);

      //Insert ops if any
      for (auto ix = 1; ix < m_opOperandCount && ix <= memRead8(stackAddrLocalVarCount(), m_stack); ix++) {
        variableWrite(ix, m_opOperands[ix]);
      }
      if (m_opOperands[0] == 0) {
        stackPopFrame(0);
      }
      break;

    case 0x01:
      m_op = "store word";
      ASSERT(m_opOperandCount == 3);
      memWrite16(m_opOperands[0] + 2 * m_opOperands[1], m_opOperands[2]);
      break;

    case 0x02:
      m_op = "store_byte";
      ASSERT(m_opOperandCount == 3);
      memWrite8(m_opOperands[0] + m_opOperands[1], (uint8_t)m_opOperands[2]);
      break;

    case 0x03:
      m_op = "put_prop";
      if (!objPropertyAddrGet(m_opOperands[0], m_opOperands[1], propertyInfo)) {
        Error("Trying to set non-existing property");
      }
      ASSERT(propertyInfo.size <= 2 && propertyInfo.size > 0);
      if (propertyInfo.size == 1) {
        memWrite8(propertyInfo.dataAdress, (uint8_t)m_opOperands[2]);
      }
      else {
        memWrite16(propertyInfo.dataAdress, m_opOperands[2]);
      }
      break;

    case 0x04:
      ASSERT(m_opOperandCount == 2);
      m_op = "sread";
      stringRead(m_opOperands[0]);

      memWrite8(m_opOperands[1]+1 , 0);
      stringParse(m_mem + m_opOperands[0], m_mem + m_opOperands[1]);
      break;

    case 0x05:
      ASSERT(m_opOperandCount == 1);
      m_op = "print char";
      printZSCII((uint8_t)m_opOperands[0]);
      break;

    case 0x06:
      m_op = "print num";
      ASSERT(m_opOperandCount == 1);
      printNum(m_opOperands[0]);
      break;

    case 0x07:
      m_op = "random";
      ASSERT(m_opOperandCount == 1);
      if (m_opOperands[0] == 0) {
        m_rndGenerator.seed((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());
        resultWrite(0);
      }
      if ((int16_t)m_opOperands[0] < 0) {
        m_rndGenerator.seed(m_opOperands[0]);
        resultWrite(0);
      }
      if ((int16_t)m_opOperands[0] > 0) {
        std::uniform_int_distribution<int16_t> dist(1, m_opOperands[0]);
        resultWrite(dist(m_rndGenerator));
      }
      break;

    case 0x08:
      m_op = "push";
      ASSERT(m_opOperandCount == 1);
      stackPushWord(m_opOperands[0]);
      break;

    case 0x09:
      m_op = "pull";
      ASSERT(m_opOperandCount == 1);
      variableWrite(m_opOperands[0], stackPopWord());
      break;

    case 0x13:
      m_op = "output_stream";
      ASSERT(abs((int16_t)m_opOperands[0]) <= 4);
      if ((int16_t)m_opOperands[0] > 0) {
        m_ostreams[abs((int16_t)m_opOperands[0])-1]->clear();
      }
      else {
        m_ostreams[abs((int16_t)m_opOperands[0])-1]->setstate(std::ios_base::failbit);
      }
      break;

    default:
      Error("Unknown opcode");
      break;
  }
}

// *
//  Reads and executes a branch instruction based on the condition
// *
void zMachine::branch(bool condition)

{
  int16_t offset = 0;
  uint8_t byte1 = 0;
  uint8_t byte2 = 0;
  bool    jumpIfTrue;

  byte1 = memRead8(m_PC++);
  jumpIfTrue = CheckBit(byte1, 7);
  if (CheckBit(byte1, 6)) {
    //Offset is bottom 6 bits
    offset = byte1 & 0x3F; 
  }
  else {
    //Offset is now a 14 bit signed int
    byte2 = memRead8(m_PC++);
    offset = (byte1 << 8) + byte2;
    offset &= 0x3FFF;

    if (offset >= (1 << 13)) {
      offset -= (1 << 14);
    }
  }

  if (jumpIfTrue == condition) {
    switch (offset) {
    case 0:
      stackPopFrame(0);
      break;

    case 1:
      stackPopFrame(1);
      break;

    default:
      m_PC = m_PC + offset - 2;
      break;
    }
  }
}