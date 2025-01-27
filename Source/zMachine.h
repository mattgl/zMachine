#include <string>
#include <vector>
#include <array>
#include <random>
#include <chrono>
#include "IOstreams\teeStream.h"
#include "zDefinitions.h"

#define ASSERT(x)           if(!(x)) {Error();}
#define ASSERT_MSG(x,y)     if(!(x)) {Error(y);}

typedef struct {
  uint8_t m_alphabet;
  uint8_t m_abbreviation;
  uint8_t m_zASCII_state;
  uint16_t m_zASCII;
} stringState_t;

typedef struct {
  uint16_t propId;
  uint16_t size;
  uint16_t dataAdress;
} propertyInfo_t;

typedef std::vector<uint8_t>  zChar_t;
typedef std::array<uint16_t, DICTIONARY_KEY_WORD_SIZE> dictionaryKey_t;

inline bool CheckBit(uint16_t val, uint8_t bit)
{
  return(((val) & (1 << bit)) != 0);
}

class zMachine {
public:
  zMachine();
  ~zMachine();
    
  void Load(std::string path);
  void Boot();
  void Run();

  void Error();
  void Error(const std::string &msg);

private: 

//Instructions
  void     Parse();
  void     evaluate0OP();
  void     evaluate1OP();
  void     evaluate2OP();
  void     evaluateVAR();
  void     branch(bool condition);

//Variables
  bool     operandEvaluate(uint8_t operandType, uint8_t operandNumber);
  void     resultWrite(uint16_t value);
  uint16_t variableRead(uint16_t var);
  void     variableWrite(uint16_t var, uint16_t value);

 //Stack
  void     stackPushFrame(uint16_t routineAddress);
  void     stackPopFrame(uint16_t returnValue);
  void     stackPushWord(uint16_t val);
  uint16_t stackPopWord();
  void     stackPushByte(uint8_t val);
  uint8_t  stackPopByte();
  uint16_t stackAddrLocalVar(uint16_t x);
  uint16_t stackAddrLocalVarCount();

//Memory
  void     memWrite8(uint16_t addr, uint8_t value, uint8_t *buffer = nullptr);
  void     memWrite16(uint16_t addr, uint16_t value, uint8_t *buffer = nullptr);
  void     memWrite32(uint16_t addr, uint32_t value, uint8_t *buffer = nullptr);
  uint8_t  memRead8(uint16_t addr, uint8_t *buffer = nullptr);
  uint16_t memRead16(uint16_t addr, uint8_t *buffer = nullptr);
  uint32_t memRead32(uint16_t addr, uint8_t *buffer = nullptr);

//Objects 
  void     objAttrSet(uint16_t obj, uint16_t attr);
  void     objAttrClear(uint16_t obj, uint16_t attr);
  bool     objAttrGet(uint16_t obj, uint16_t attr);
  uint16_t objPropertyGetNext(uint16_t obj, uint16_t propId);
  bool     objPropertyAddrGet(uint16_t obj, uint16_t propId, propertyInfo_t &propertyInfo);

  void     objPropertyFirst(uint16_t obj, uint16_t &adress);
  bool     objPropertyGet(uint16_t &address, propertyInfo_t &propertyInfo);
  
  uint16_t objShortName(uint16_t obj);

//Strings
  void     printString(uint16_t &adress);
  void     printZSCII(uint16_t data);
  void     printNum(uint16_t data);
  void     printZChar(uint8_t data, stringState_t &state);
  void     stringParse(uint8_t *textBuffer, uint8_t* parseBuffer);
  void     parseWord(uint8_t *textBuffer, uint8_t &tIndex, uint8_t* parseBuffer);
  void     wordEncode(uint8_t *textBuffer, uint8_t iStart, uint8_t iEnd, uint8_t* parseBuffer);
  uint16_t dictionaryLookup(dictionaryKey_t &key);

//Input
  void     stringRead(uint16_t textAddr);

//Debugging
  void     debugPrintInstruction();

public:
  //Memory
  uint8_t*       m_mem;
  uint16_t       m_PC;

  uint16_t       m_addrTableGlobalVar;
  uint16_t       m_addrTableObject;
  uint16_t       m_addrTableAbbreviations;
  uint16_t       m_addrTableDictionary;

  //Instruction set
  std::string    m_op;
  uint8_t        m_opNumber;
  uint16_t       m_opOperandCount;
  uint16_t       m_opOperands[4];
  uint16_t       m_opOperandsVar[4]; //debug info
  uint16_t       m_opReturnVar;
  uint16_t       m_opReturnValue;    //debug info

  //Stack
  uint8_t        m_stack[0xFFFF];
  uint16_t       m_stackPtr;
  uint16_t       m_currentStackFrame;

  //Dictionary 
  uint16_t       m_dictionaryEntriesAddr;
  uint16_t       m_dictionaryEntriesCount;
  uint8_t        m_dictionaryEntryLength;
  uint8_t*       m_dictionaryWordSeparators;
  uint8_t        m_dictionaryWordSeparatorsCount;

  //input/output
  teeStream      m_ostreamGame;
  teeStream      m_ostreamCommands;
  std::ostream  *m_ostreams[4];
  std::ofstream  m_ostream2;
  std::ofstream  m_ostream3; // TODO: Implement a memory writing stream
  std::ofstream  m_ostream4;

  //Misc
  std::string    m_pathStoryfile;
  std::mt19937   m_rndGenerator;

  bool           m_quit;
  bool           m_logEnabled;
 };
