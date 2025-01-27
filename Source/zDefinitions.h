
//Header positions
#define hVersion                0x00   // uint8_t
#define hflags1                 0x01   // uint8_t
#define hbaseMemHigh            0x04   // uint16_t
#define hinitialPC              0x06   // uint16_t
#define hlocDictionary          0x08   // uint16_t
#define hlocTableObject         0x0A   // uint16_t
#define hlocTableGlobalVar      0x0C   // uint16_t
#define hbaseMemStatic          0x0E   // uint16_t
#define hflags2                 0x10   // uint8_t
#define hlocTableAbbreviations  0x18   // uint16_t
#define hlength                 0x1A   // uint16_t
#define hcheckSum               0x1C   // uint16_t
#define hiptNumber              0x1E   // uint8_t
#define hiptVersion             0x1F   // uint8_t
#define hstandardRevisionNumber 0x32   // uint16_t

//Operand types
#define OP_LARGE_CONSTANT       0x00
#define OP_SMALL_CONSTANT       0x01
#define OP_VARIABLE             0x02
#define OP_OMITTED              0x03

//Objects
#define OBJ_PROP_DEFAULT(x)     (m_addrTableObject + ((x)-1)*2)       //  words numbered 1-31
#define OBJ_ATTRIBUTES(x)       (m_addrTableObject + 62+((x)-1)*9)
#define OBJ_PARENT(x)           (m_addrTableObject + 62+((x)-1)*9 +4)
#define OBJ_SIBLING(x)          (m_addrTableObject + 62+((x)-1)*9 +5)  
#define OBJ_CHILD(x)            (m_addrTableObject + 62+((x)-1)*9 +6)  
#define OBJ_PROPERTY(x)         (m_addrTableObject + 62+((x)-1)*9 +7) 

//DICTIONARY
#define DICTIONARY_ENTRY(x)       (m_dictionaryEntriesAddr + ((x)*m_dictionaryEntryLength)) 
#define DICTIONARY_KEY_ZCHAR_SIZE 6 //i.e. 6 characters at most in version 3
#define DICTIONARY_KEY_WORD_SIZE  2 //i.e. 6 characters at most in version 3