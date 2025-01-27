#include "zMachine.h"
#include <iostream>

using namespace std;
static char alphabet[3][27] = {
  //6     7     8    9    a     b   c    d    e    f    10  11   12    13   14   15   16   17   18   19   1a    1b   1c    1d   1e   1f
  { 'a',  'b',  'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',  't', 'u',  'v',  'w',  'x', 'y', 'z' },
{ 'A',  'B',  'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S',  'T', 'U',  'V',  'W',  'X', 'Y', 'Z' },
{ ' ',  '\n', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', ',', '!', '?', '_', '#', '\'', '\'', '/', '\\', '-', ':', '(', ')' } };


// *
//  Prints the string (zCharacters) at adress. After return, the adress will point to the byte after the terminating word.
// *
void zMachine::printString(uint16_t &adress)

{ stringState_t state = {0,0,0,0};
  bool          eos = false;

  while (!eos) {
    uint16_t data = memRead16(adress);
    uint8_t zChars[3];

    zChars[0] = (uint8_t)((data & 0x7C00) >> 10);
    zChars[1] = (uint8_t)((data & 0x03E0) >> 5);
    zChars[2] = (uint8_t)((data & 0x001F));

    for (auto ix = 0; ix < 3; ix++) {
      printZChar(zChars[ix], state);
    }

    eos = CheckBit(data, 15);
    adress += 2;
  }
}

// *
//  Prints a single zChar (based on the current state, like alphabet selection)
// *
void zMachine::printZChar(uint8_t data, stringState_t &state)

{
  switch(state.m_zASCII_state) {
    case 1:
      state.m_zASCII = data << 5;
      state.m_zASCII_state++;
      return;
      break;

    case 2:
      state.m_zASCII += data;
      printZSCII(state.m_zASCII);
      state.m_zASCII_state = 0;
      return;
      break;

    default:
      break;
  }

  if (state.m_abbreviation != 0) {
    uint16_t adress = memRead16( m_addrTableAbbreviations + (32 * (state.m_abbreviation  - 1) + data) * 2) * 2; //Word adress
    printString(adress);
    state.m_abbreviation = 0;
    return;
  }

  switch (data) {
    case 0:
      printZSCII(0x20);
      break;

    case 1:
    case 2:
    case 3:
      state.m_abbreviation = data;
      break;

    case 4:
      state.m_alphabet = 1;
      break;

    case 5:
      state.m_alphabet = 2;
      break;

    default:
      if (state.m_alphabet == 2 && data == 6) {
        state.m_zASCII_state = 1;
      }
      else {
        printZSCII(alphabet[state.m_alphabet][data - 6]);
        state.m_alphabet = 0;
      }
      break;
  }
}

// *
//  Encodes a ZSCII character (only 8 bits as those are the only valid values) into 1-3 5bit zCharacters.
// *
void ZSCII2ZChar(uint8_t c, zChar_t &zChar)

{
  zChar.clear();

  //Alphabet 0
  if (0x61 <= c && c <= 0x7A) {
    zChar.push_back(c - 0x61 + 6);
    return;
  }

  //Alphabet 1 
  if (0x41 <= c && c <= 0x5A) {
    zChar.push_back(5);
    zChar.push_back(c - 0x41 + 6);
    return;
  }

  //Alphabet 2
  //TODO: Encode alphabet 2;

  //ZSCII encoding
  zChar.push_back(6);
  zChar.push_back((c & 0xE0) << 5);
  zChar.push_back((c & 0x1F));

  return;
}

// *
//  Parses the textbuffer input into the parsebuffer based on the lexical analysis.
// *
void zMachine::stringParse(uint8_t *textBuffer, uint8_t* parseBuffer)

{ uint8_t i = 1;

  while (textBuffer[i] != 0 &&         //End of string reached
         parseBuffer[0] > parseBuffer[1]) { //Max size of parseTable (words) reached
    parseWord(textBuffer, i, parseBuffer);
  }
}

// *
//  Selects and encodes the next complete word
// *
void zMachine::parseWord(uint8_t *textBuffer, uint8_t &tIndex, uint8_t* parseBuffer)

{ uint8_t tIndexStart = tIndex;
  bool    retVal = true;
  bool    cont = true;

  if (textBuffer[tIndex] == 0x20) {
    tIndex++;
    return;
  }

  while (cont) {
    if (textBuffer[tIndex] == 0x20) {
      break;
    }

    if (textBuffer[tIndex] == 0) {
      break;
    }

    for (auto ix = 0; ix < m_dictionaryWordSeparatorsCount; ix++) {
      if (textBuffer[tIndex] == m_dictionaryWordSeparators[ix]) {
        cont = false;
      }
    }
    tIndex++;
  }

  wordEncode(textBuffer, tIndexStart, tIndex, parseBuffer);
  return;
}

// *
//  Encodes a word into the parse table
// *
void zMachine::wordEncode(uint8_t *textBuffer, uint8_t iStart, uint8_t iEnd, uint8_t* parseBuffer)

{ dictionaryKey_t dictKey = { 0x14A5,0x94A5 }; // Filled with character 5 and terminating last word
  uint8_t         encodedChars = 0;
  zChar_t         zChar;
  auto            i = iStart;

  for (auto i = 0; i < DICTIONARY_KEY_ZCHAR_SIZE && i < (iEnd - iStart); i++) {
    ZSCII2ZChar(textBuffer[iStart + i], zChar);
    for (auto j : zChar) {
      if (encodedChars < DICTIONARY_KEY_ZCHAR_SIZE) {
        dictKey[i / 3] &= ~(0x0005 << (5 * (2 - i % 3)));
        dictKey[i / 3] = dictKey[i / 3] + ((uint16_t)j << (5 * (2 - i % 3)));
        encodedChars++;
      }
    }
  }

  memWrite16(2 + parseBuffer[1] * 4, dictionaryLookup(dictKey), parseBuffer);
  memWrite8(2 + parseBuffer[1] * 4 + 2, iEnd - iStart, parseBuffer);
  memWrite8(2 + parseBuffer[1] * 4 + 2 + 1, iStart, parseBuffer);
  parseBuffer[1] ++;
}

// *
//  Performs a search of the dictionary to find a match and returning the adress if found (zero otherwise)
// *
uint16_t zMachine::dictionaryLookup(dictionaryKey_t &key)

{ //TODO: This is a brute force search. Replace with a simple map
  for (auto ix = 0; ix < m_dictionaryEntriesCount; ix++) {
    for (uint16_t iy = 0; iy < key.size(); iy++) {
      uint16_t t1 = memRead16(DICTIONARY_ENTRY(ix) + iy * 2);
      if (memRead16(DICTIONARY_ENTRY(ix) + iy * 2) != key[iy]) {
        break;
      }
      if (iy == (key.size() - 1)) {
        return(DICTIONARY_ENTRY(ix));
      }
    }
  }
  return(0);
}

// *
//  Prints a single ZSCII character to the selected streams
// *
void zMachine::printNum(uint16_t data)

{
  m_ostreamGame << (int16_t)data;
}

// *
//  Prints a single ZSCII character to the selected streams
// *
void zMachine::printZSCII(uint16_t data)

{
  //TODO: Add support for unicode characters according to default unicode table.
  ASSERT(data <= 0xFF); //Values greater are undefined in ZSCII
  m_ostreamGame << (char)data;
}
