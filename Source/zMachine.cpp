#include <iostream>
#include <fstream>
#include <iomanip>
#include "zMachine.h"


using namespace std;

zMachine::zMachine()

{
  m_mem = nullptr;
}

zMachine::~zMachine()

{
  if (m_mem != nullptr) {
    delete m_mem;
  }
}

void zMachine::Load(std::string path)

{ std::fstream storyFile;
  int          size;

  //Read story file
  if (m_mem != nullptr) {
    delete m_mem;
  }
  m_pathStoryfile = path;
  storyFile.open(m_pathStoryfile, ios::in | ios::binary | ios::ate);
  ASSERT_MSG(storyFile.is_open(), "Unable to read story file");

  size = (int)storyFile.tellg();
  ASSERT_MSG(size <= 0xFFFF, "Corrupt input file");
  m_mem = new uint8_t[size];
  storyFile.seekg(0, ios::beg);
  storyFile.read((char*)m_mem, size);
  storyFile.close();
}

void zMachine::Boot()

{ uint8_t      flags;
  uint16_t      tmp;

  //Set flags 1
  flags = (1 << 4) & //Status line NOT available
          (0 << 5) & //Screen splitting NOT available
          (0 << 6);  //Variable pitch font NOT default
  memWrite8(hflags1,flags);

  //Set flags 2
  
  //Not applicable

  //Set misc
  memWrite8( hiptNumber, 0x3); // Version 3
  memWrite8( hiptVersion,0x6); // IBM PC

  //Set pointers
  m_addrTableGlobalVar      = memRead16( hlocTableGlobalVar);
  m_addrTableObject         = memRead16( hlocTableObject);
  m_addrTableAbbreviations  = memRead16( hlocTableAbbreviations);
  m_addrTableDictionary     = memRead16( hlocDictionary);

  // Initialize dictionary
  tmp = memRead16( hlocDictionary);
  m_dictionaryWordSeparatorsCount = memRead8(tmp++);
  m_dictionaryWordSeparators = m_mem + tmp;
  tmp += m_dictionaryWordSeparatorsCount;
  m_dictionaryEntryLength = memRead8(tmp++);
  m_dictionaryEntriesCount = memRead16( tmp);
  m_dictionaryEntriesAddr = tmp + 2;

  //Setup output streams
  m_ostreams[0] = &std::cout;
  m_ostreams[1] = &m_ostream2;
  m_ostreams[2] = &m_ostream3;
  m_ostreams[3] = &m_ostream4;

  m_ostreams[1]->setstate(ios_base::failbit); //disable
  m_ostreams[2]->setstate(ios_base::failbit); //disable
  m_ostreams[3]->setstate(ios_base::failbit); //disable

  m_ostream2.open("transcript.txt", ios::out);
  ASSERT_MSG(m_ostream2.is_open(), "Unable to open transcript file");
  m_ostream4.open("commands.txt", ios::out);
  ASSERT_MSG(m_ostream4.is_open(), "Unable to open command file");

  m_ostreamGame.tee(*(m_ostreams[0]));
  m_ostreamGame.tee(*(m_ostreams[1]));
  m_ostreamGame.tee(*(m_ostreams[2]));

  m_ostreamCommands.tee(*(m_ostreams[1]));
  m_ostreamCommands.tee(*(m_ostreams[3]));

  //Initialize execution state
  m_PC = memRead16( hinitialPC);
  m_stackPtr = 0;
  m_quit = false;

  //m_rndGenerator.seed(std::chrono::system_clock::now().time_since_epoch().count());
  m_rndGenerator.seed(0xABCD); //TODO: fixed seed for testing purposes, should be replaced by time based seeding

 



  m_logEnabled = false;
}

void zMachine::Error()

{
  Error("");
}

void zMachine::Error(const std::string &msg)

{
  std::cout << msg << std::endl;
  throw(std::runtime_error(msg));
}

void zMachine::Run()

{
  while (!m_quit) {
    Parse();
  }
}

void zMachine::Parse()

{ uint8_t opCode = 0;
  uint8_t opsTypes = 0;

  m_opReturnValue = 0xFFFF;
  m_opReturnVar = 0xFF;
  m_opOperandCount = 0;
  opCode =  memRead8(m_PC++);

  switch (opCode & 0xC0) { //Check two top bits
    case 0xC0: //Variable
      m_opNumber = opCode & 0x1F; //Bottom 5 bits
      opsTypes = memRead8(m_PC++);
      m_opOperandCount = 0;
      for (auto ix = 0; ix < 4; ix++) {
        if (operandEvaluate((opsTypes & (0x03 << (6 - 2 * ix))) >> (6-2*ix), ix)) {
          m_opOperandCount++;
        }
      }
      if (CheckBit(opCode, 5)) {
        evaluateVAR();
      }
      else {
        evaluate2OP();
      }
      break;
  
    case 0x80: //Short
      m_opNumber = opCode & 0x0F; //Bottom 4 bits
      if (((opCode & 0x30) >> 4) == OP_OMITTED) {
        evaluate0OP();
      } 
      else {
        m_opOperandCount = 1;
        operandEvaluate((opCode & 0x30) >> 4, 0);
        evaluate1OP();
      };
      break;
  
    default:   //Long == 2OP
      m_opNumber = opCode & 0x1F; // bottom five bits
      m_opOperandCount = 2;
      operandEvaluate(CheckBit(opCode, 6) ? OP_VARIABLE : OP_SMALL_CONSTANT, 0);
      operandEvaluate(CheckBit(opCode, 5) ? OP_VARIABLE : OP_SMALL_CONSTANT, 1);
      evaluate2OP();
      break;
  }
  debugPrintInstruction();
}

void  zMachine::memWrite32(uint16_t addr, uint32_t value, uint8_t *buffer)

{
  if (buffer == nullptr) {
    buffer = m_mem;
  }
  *(buffer + addr) = value >> 24;
  *(buffer + addr + 1) = value >>  16;
  *(buffer + addr + 2) = value >>  8;
  *(buffer + addr + 3) = (uint8_t)(value);
};

void  zMachine::memWrite16(uint16_t addr, uint16_t value, uint8_t *buffer)

{
  if (buffer == nullptr) {
    buffer = m_mem;
  }
  *(buffer + addr) = value >> 8;
  *(buffer + addr + 1) = (uint8_t)(value);
};

void  zMachine::memWrite8(uint16_t addr, uint8_t value, uint8_t *buffer)

{
  if (buffer == nullptr) {
    buffer = m_mem;
  }
  *(buffer + addr) = value;
};

uint32_t zMachine::memRead32(uint16_t addr, uint8_t *buffer)

{
  if (buffer == nullptr) {
    buffer = m_mem;
  }
  buffer += addr;
  return((buffer[0] << 24) | 
         (buffer[1] << 16) | 
         (buffer[2] << 8)  | 
         (buffer[3] << 0));
};

uint16_t zMachine::memRead16(uint16_t addr, uint8_t *buffer)

{
  if (buffer == nullptr) {
    buffer = m_mem;
  }
  buffer += addr;
  return((buffer[0] << 8) |
         (buffer[1] << 0));
};

uint8_t  zMachine::memRead8(uint16_t addr, uint8_t *buffer)

{
  if (buffer == nullptr) {
    buffer = m_mem;
  }
  return(*(buffer + addr));
};

