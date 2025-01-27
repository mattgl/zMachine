#include "zMachine.h"

// *
//  Sets an attribute of the given object
// *
void  zMachine::objAttrSet(uint16_t obj, uint16_t attr)

{ uint32_t result = memRead32( OBJ_ATTRIBUTES(obj));

  result |= (0x80000000 >> attr);
  memWrite32( OBJ_ATTRIBUTES(obj),result);
};

// *
//  Clears an attribute of the given object
// *
void  zMachine::objAttrClear(uint16_t obj, uint16_t attr)

{ uint32_t result = memRead32( OBJ_ATTRIBUTES(obj));

  result &= ~(0x80000000 >> attr);
  memWrite32( OBJ_ATTRIBUTES(obj), result);
};

// *
//  Checks if an object has a given attribute
// *
bool  zMachine::objAttrGet(uint16_t obj, uint16_t attr)

{ uint32_t result = memRead32( OBJ_ATTRIBUTES(obj));

  result = (result) & (0x80000000 >> attr);
  return (result != (uint32_t)0);
};

// *
//  Gets the if of the next property
// *
uint16_t zMachine::objPropertyGetNext(uint16_t obj, uint16_t propId)

{ uint16_t       adress;
  propertyInfo_t propertyInfo;

  if (propId == 0) {
    objPropertyFirst(obj,adress);
    objPropertyGet(adress, propertyInfo);
  }
  else {
    objPropertyAddrGet(obj,propId, propertyInfo);
    adress = propertyInfo.dataAdress + propertyInfo.size;
    objPropertyGet(adress, propertyInfo);
  }
  return(propertyInfo.propId);
}

// *
//  Gets the adress of the first property of an object
// *
void zMachine::objPropertyFirst(uint16_t obj, uint16_t &adress)

{ uint16_t propertyTableAddr = memRead16(OBJ_PROPERTY(obj));

  ASSERT(obj < 0xFF);
  adress = propertyTableAddr + 1 + memRead8(propertyTableAddr) * 2;
}

// *
//  Reads the property data at a given adress and if successful, updates adress to the next property
// *
bool zMachine::objPropertyGet(uint16_t &adress, propertyInfo_t &propertyInfo)

{ uint8_t sizeByte;

  sizeByte = memRead8(adress);
  if (sizeByte == 0) {
    propertyInfo.propId = 0;
    propertyInfo.size = 0;
    propertyInfo.dataAdress = 0;
    return(false);
  }
  propertyInfo.propId = sizeByte & 0x1F;
  propertyInfo.size = ((sizeByte & 0xE0) >> 5) + 1;
  propertyInfo.dataAdress = adress+1;
  adress += propertyInfo.size + 1;
  return(true);
}

// *
//  Gets the size and (data) adress of a given property (possibly default table)
//  Returns false if the object does not have it
// *
bool zMachine::objPropertyAddrGet(uint16_t obj, uint16_t property, propertyInfo_t &propertyInfo)

{ uint16_t       adress;
  bool           retVal=false;
  
  objPropertyFirst(obj,adress);
  while (objPropertyGet(adress, propertyInfo)) {
    if (propertyInfo.propId < property) {
      break;
    }
    if (propertyInfo.propId == property) {
      retVal = true;
      break;
    }
  }

  if (!retVal) {
    propertyInfo.size = 2;
    propertyInfo.dataAdress = OBJ_PROP_DEFAULT(property);
  }
  return(retVal);
}

// *
//  Returns the address to an objects shortname.
// *
uint16_t zMachine::objShortName(uint16_t obj)

{
  ASSERT(obj < 0xFF);
  return(memRead16( OBJ_PROPERTY(obj)) + 1);
};
