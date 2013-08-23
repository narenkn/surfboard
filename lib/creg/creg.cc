//******************************************************************************
//
// $RCSfile: creg.cc,v $
//
// $Revision: 1.51 $
// $Date: 2010/02/05 21:51:45 $
// $Author: grpierce $
// $State: Exp $
// $Locker:  $
//
// Copyright (c) 2006 by Advanced Micro Devices, Inc.
//
// This file is protected by Federal Copyright Law, with all rights
// reserved. No part of this file may be reproduced, stored in a
// retrieval system, translated, transcribed, or transmitted, in any
// form, or by any means manual, electric, electronic, mechanical,
// electro-magnetic, chemical, optical, or otherwise, without prior
// explicit written permission from Advanced Micro Devices, Inc.
//
//******************************************************************************
// Class definitions for cReg
//******************************************************************************

#include "creg.h"

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------

cReg::cReg(): msb(0), lsb(0) {
  initializePointers();
  reserve();
}

//------------------------------------------------------------------------------
// Copy Constructor
//------------------------------------------------------------------------------

cReg::cReg( const cReg &reg ) {
  initializePointers();
  copy(reg);
}

//------------------------------------------------------------------------------
// Constructor : Create a register equal to a cDesignVar
//------------------------------------------------------------------------------

#ifndef NO_ENV_DKI

cReg::cReg( cDesignVar &reg ) {

  initializePointers();

  msb = reg.GetMsb();
  lsb = reg.GetLsb();

  reserve();

  if ( reg.IsAttached() ) {

    // Copy in 32 bit chunks

    if ( msb >= lsb ) {
      
      for ( uint32 wordIndex = 0; wordIndex < getWordLength(); wordIndex++ ) {
        
        uint32 _lsb = ( wordIndex * crWord::size );
        uint32 _msb = ( _lsb + crWord::size - 1 );
        
        if ( _msb > msb ) {
          _msb = msb;
        }
        
        uint32 data = reg(_msb, _lsb);
        uint32 ctrl = reg.GetControl(_msb, _lsb);
        
        setDataWord( wordIndex, data );
        setControlWord( wordIndex, ctrl );
          
      }
      
    } else {

      for ( int wordIndex = getWordLength() - 1 ; wordIndex >= 0; wordIndex-- ) {
        
        uint32 _msb = ( wordIndex * crWord::size );
        uint32 _lsb = ( _msb + crWord::size - 1 );
        
        if ( _lsb > lsb ) {
          _lsb = lsb;
        }
        
        uint32 data = reg(_msb, _lsb);
        uint32 ctrl = reg.GetControl(_msb, _lsb);
        
        setDataWord( wordIndex, data );
        setControlWord( wordIndex, ctrl );
          
      }

    }
    
  }

}

#endif // NO_ENV_DKI

//------------------------------------------------------------------------------
// Constructor : Assign register to be equal to a STL string
//------------------------------------------------------------------------------

cReg::cReg( const string &value ): lsb(0) {

  uint32 width;
  string binStr;
  parseVerilogInitialStr(value, width, binStr);

  if ( width == 0 ) {
    msb = binStr.size()-1;
  } else {
    msb = width-1;
  }

  initializePointers();
  reserve();

  setUsingBinaryString(msb, lsb, binStr);

}

//------------------------------------------------------------------------------
// Constructor : Assign register to be equal to a char
//------------------------------------------------------------------------------

cReg::cReg( const char value ): msb(0), lsb(0) {
  
  initializePointers();
  reserve();

  string s = "x";
  s[0] = value;

  set(s);

}

//------------------------------------------------------------------------------
// Constructor : Assign register to be equal to a char string
//------------------------------------------------------------------------------

cReg::cReg( const char *value ): lsb(0) {

  uint32 width;
  string binStr;
  parseVerilogInitialStr(value, width, binStr);

  if ( width == 0 ) {
    msb = binStr.size()-1;
  } else {
    msb = width-1;
  }

  initializePointers();
  reserve();

  setUsingBinaryString(msb, lsb, binStr);

}

//------------------------------------------------------------------------------
// Constructor : Assign register to be equal to a 8 bit integer
//------------------------------------------------------------------------------

cReg::cReg( const int8 value ): msb(8*sizeof(uint8)-1), lsb(0) {

  initializePointers();
  reserve();

  set(value);

}

//------------------------------------------------------------------------------
// Constructor : Assign register to be equal to a 16 bit integer
//------------------------------------------------------------------------------

cReg::cReg( const int16 value ): msb(8*sizeof(uint16)-1), lsb(0) {

  initializePointers();
  reserve();

  set(value);

}

//------------------------------------------------------------------------------
// Constructor : Assign register to be equal to a 32 bit integer
//------------------------------------------------------------------------------

cReg::cReg( const signed long value ): msb(8*sizeof(signed long)-1), lsb(0) {

  initializePointers();
  reserve();

  set(value);

}

//------------------------------------------------------------------------------
// Constructor : Assign register to be equal to a 32 bit integer
//------------------------------------------------------------------------------

cReg::cReg( const int32 value ): msb(8*sizeof(uint32)-1), lsb(0) {

  initializePointers();
  reserve();

  set(value);

}

//------------------------------------------------------------------------------
// Constructor : Assign register to be equal to a 64 bit integer
//------------------------------------------------------------------------------

cReg::cReg( const int64 value ): msb(8*sizeof(uint64)-1), lsb(0) {

  initializePointers();
  reserve();

  set(value);

}

//------------------------------------------------------------------------------
// Constructor : Assign register to be equal to a 8 bit unsigned integer
//------------------------------------------------------------------------------

cReg::cReg( const uint8 value ): msb(8*sizeof(uint8)-1), lsb(0) {

  initializePointers();
  reserve();

  set(value);

}

//------------------------------------------------------------------------------
// Constructor : Assign register to be equal to a 16 bit unsigned integer
//------------------------------------------------------------------------------

cReg::cReg( const uint16 value ): msb(8*sizeof(uint16)-1), lsb(0) {

  initializePointers();
  reserve();

  set(value);

}

//------------------------------------------------------------------------------
// Constructor : Assign register to be equal to a 32 bit unsigned integer
//------------------------------------------------------------------------------

cReg::cReg( const unsigned long value ): msb(8*sizeof(unsigned long)-1), lsb(0) {

  initializePointers();
  reserve();

  set(value);

}

//------------------------------------------------------------------------------
// Constructor : Assign register to be equal to a 32 bit unsigned integer
//------------------------------------------------------------------------------

cReg::cReg( const uint32 value ): msb(8*sizeof(uint32)-1), lsb(0) {

  initializePointers();
  reserve();

  set(value);

}

//------------------------------------------------------------------------------
// Constructor : Assign register to be equal to a 64 bit unsigned integer
//------------------------------------------------------------------------------

cReg::cReg( const uint64 value ): msb(8*sizeof(uint64)-1), lsb(0) {

  initializePointers();
  reserve();

  set(value);

}



//------------------------------------------------------------------------------
// Constructor : Create a register of a specific size
//------------------------------------------------------------------------------

cReg::cReg( const uint32 _msb, const uint32 _lsb ): msb(_msb), lsb(_lsb) {

  initializePointers();
  reserve();

}

//------------------------------------------------------------------------------
// Constructor : Create a register of a specific size equal to another register
//------------------------------------------------------------------------------

cReg::cReg( const uint32 _msb, const uint32 _lsb, const cReg &reg ): msb(_msb), lsb(_lsb) {

  initializePointers();
  reserve();

  set(_msb, _lsb, reg);

}

//------------------------------------------------------------------------------
// Constructor : Create a register of a specific size equal to a char string
//------------------------------------------------------------------------------

cReg::cReg( const uint32 _msb, const uint32 _lsb, const char *value ): msb(_msb), lsb(_lsb) {

  initializePointers();
  reserve();

  set(string(value));

}

//------------------------------------------------------------------------------
// Constructor : Create a register of a specific size equal to a STL string
//------------------------------------------------------------------------------

cReg::cReg( const uint32 _msb, const uint32 _lsb, const string &value ): msb(_msb), lsb(_lsb) {

  initializePointers();
  reserve();

  set(value);

}

//------------------------------------------------------------------------------
// Constructor : Create a register of a specific size equal to an 8 bit integer
//------------------------------------------------------------------------------

cReg::cReg( const uint32 _msb, const uint32 _lsb, const int8 value ): msb(_msb), lsb(_lsb) {

  initializePointers();
  reserve();

  set(value);

}

//------------------------------------------------------------------------------
// Constructor : Create a register of a specific size equal to an 16 bit integer
//------------------------------------------------------------------------------

cReg::cReg( const uint32 _msb, const uint32 _lsb, const int16 value ): msb(_msb), lsb(_lsb) {

  initializePointers();
  reserve();

  set(value);

}

//------------------------------------------------------------------------------
// Constructor : Create a register of a specific size equal to an 32 bit integer
//------------------------------------------------------------------------------

cReg::cReg( const uint32 _msb, const uint32 _lsb, const signed long value ): msb(_msb), lsb(_lsb) {

  initializePointers();
  reserve();

  set(value);

}

//------------------------------------------------------------------------------
// Constructor : Create a register of a specific size equal to an 32 bit integer
//------------------------------------------------------------------------------

cReg::cReg( const uint32 _msb, const uint32 _lsb, const int32 value ): msb(_msb), lsb(_lsb) {

  initializePointers();
  reserve();

  set(value);

}

//------------------------------------------------------------------------------
// Constructor : Create a register of a specific size equal to an 64 bit integer
//------------------------------------------------------------------------------

cReg::cReg( const uint32 _msb, const uint32 _lsb, const int64 value ): msb(_msb), lsb(_lsb) {

  initializePointers();
  reserve();

  set(value);

}

//------------------------------------------------------------------------------
// Constructor : Create a reg of a specific size equal to an 8 bit usign integer
//------------------------------------------------------------------------------

cReg::cReg( const uint32 _msb, const uint32 _lsb, const uint8 value ): msb(_msb), lsb(_lsb) {

  initializePointers();
  reserve();

  set(value);

}

//------------------------------------------------------------------------------
// Constructor : Create a reg of a specific size equal to an 16 bit usign integer
//------------------------------------------------------------------------------

cReg::cReg( const uint32 _msb, const uint32 _lsb, const uint16 value ): msb(_msb), lsb(_lsb) {

  initializePointers();
  reserve();

  set(value);

}

//------------------------------------------------------------------------------
// Constructor : Create a reg of a specific size equal to an 32 bit usign integer
//------------------------------------------------------------------------------

cReg::cReg( const uint32 _msb, const uint32 _lsb, const unsigned long value ): msb(_msb), lsb(_lsb) {

  initializePointers();
  reserve();

  set(value);

}

//------------------------------------------------------------------------------
// Constructor : Create a reg of a specific size equal to an 32 bit usign integer
//------------------------------------------------------------------------------

cReg::cReg( const uint32 _msb, const uint32 _lsb, const uint32 value ): msb(_msb), lsb(_lsb) {

  initializePointers();
  reserve();

  set(value);

}

//------------------------------------------------------------------------------
// Constructor : Create a reg of a specific size equal to an 64 bit usign integer
//------------------------------------------------------------------------------

cReg::cReg( const uint32 _msb, const uint32 _lsb, const uint64 value ): msb(_msb), lsb(_lsb) {

  initializePointers();
  reserve();

  set(value);

}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------

cReg::~cReg() {

  // Remove field map

  if ( fieldMap != NULL ) {

    for ( crFieldMapIter i = fieldMap->begin(); i != fieldMap->end(); ++i ) {
      delete i->second;
    }

    delete fieldMap;
    fieldMap = NULL;

  }

  // Remove attribute map

  if ( attributeMap != NULL ) {

    for ( crAttributeMapIter i = attributeMap->begin(); i != attributeMap->end(); i++ ) {
      delete i->second;
    }
    
    delete attributeMap;
    attributeMap = NULL;

  }

}

//------------------------------------------------------------------------------
// protected : mallocFieldMap
//------------------------------------------------------------------------------

void cReg::mallocFieldMap() {

  if ( fieldMap == NULL ) {
    fieldMap = new crFieldMap();
  }

}

//------------------------------------------------------------------------------
// protected : mallocAttributeMap
//------------------------------------------------------------------------------

void cReg::mallocAttributeMap() {

  if ( attributeMap == NULL ) {
    attributeMap = new crAttributeMap();
  }

}

//------------------------------------------------------------------------------
// protected copy : Copy the given cReg
//------------------------------------------------------------------------------

void cReg::copy( const cReg &reg ) {

  msb = reg.getMsb();
  lsb = reg.getLsb();

  reserve();
  initialize(ZERO);

  for ( uint32 index = 0; index < reg.getWordLength(); index++ ) {
    setDataWord(index, reg.getDataWord(index));
    setControlWord(index, reg.getControlWord(index));
  }

  // Copy field and attribute maps

  copyMaps(reg.fieldMap, reg.attributeMap);

}

//------------------------------------------------------------------------------
// protected: copyMaps
//------------------------------------------------------------------------------

void cReg::copyMaps(crFieldMap* srcFieldMap, crAttributeMap* srcAttributeMap) {

  copyFieldMap(srcFieldMap);
  copyAttributeMap(srcAttributeMap);

}

//------------------------------------------------------------------------------
// protected: copyFieldMap
//------------------------------------------------------------------------------

void cReg::copyFieldMap( crFieldMap* src ) {
  
  if ( src != NULL ) {
    
    mallocFieldMap();
    
    for ( crFieldMapIter i = src->begin(); i != src->end(); ++i ) {
      (*fieldMap)[i->first] = new cRegField(*(i->second));
    }
    
  }
  
}

//------------------------------------------------------------------------------
// protected: copyAttributeMap
//------------------------------------------------------------------------------

void cReg::copyAttributeMap( crAttributeMap* src ) {
  
  if ( src != NULL ) {
    
    mallocAttributeMap();
    
    for ( crAttributeMapIter i = src->begin(); i != src->end(); ++i ) {
      (*attributeMap)[i->first] = i->second->duplicate();
    }
    
  }
  
}

//------------------------------------------------------------------------------
// protected checkIndex : Check an index value to make sure it exists.
//------------------------------------------------------------------------------

bool cReg::checkIndex( const uint32 index, const string& msgPrefix ) const {

  if ( ! isIndexValid(index) ) {
    
    string prefix = msgPrefix;
    
    if ( prefix.empty() ) {
      prefix = "cReg";
    }
    
    FATAL_ERROR("%s index '%d' is out of the range [%d:%d].", prefix.c_str(), index, getMsb(), getLsb());

    return false;

  } else {
    return true;
  }

}

//------------------------------------------------------------------------------
// protected isIndexValid : Check an index value to make sure it's valid.
//------------------------------------------------------------------------------

bool cReg::isIndexValid( const uint32 index ) const {
  return(getActualIndex(index) < getWidth());
}

//------------------------------------------------------------------------------
// protected getActualIndex : Return the actual index
//
// NOTE: This method accounts for non-standard MSB and LSB numbers. In other words,
//       reg [23:45] or reg [67:22] are valid but not standard (e.g. reg [15:0])
//------------------------------------------------------------------------------

uint32 cReg::getActualIndex( const uint32 index ) const {

  int idx = index - lsb;

  return ( abs( idx ) );

}

//------------------------------------------------------------------------------
// protected getIndex : Return the index based on MSB and LSB (opposite of actual)
//
// NOTE: This method accounts for non-standard MSB and LSB numbers. In other words,
//       reg [23:45] or reg [67:22] are valid but not standard (e.g. reg [15:0])
//------------------------------------------------------------------------------

uint32 cReg::getIndex( const uint32 idx, const uint32 wordIdx ) const {

  if ( msb >= lsb ) {
    return ( idx + (wordIdx * crWord::size) + lsb );
  } else {
    return ( lsb - (idx + (wordIdx * crWord::size)) );
  }

}

//------------------------------------------------------------------------------
// protected isState : Check to see if a bit value is a particular state.
//------------------------------------------------------------------------------

bool cReg::isState( const uint32 index, const FourState state ) const {

  uint32 d = getDataBit(index);
  uint32 c = getControlBit(index);

  return( getState(d, c) == state );

}

//------------------------------------------------------------------------------
// protected isState : Check to see if a range of values are a particular state.
//------------------------------------------------------------------------------

bool cReg::isState( const uint32 _msb, const uint32 _lsb, const FourState state ) const {

  uint32 min      = (_msb > _lsb) ? _lsb : _msb;
  uint32 max      = (_msb < _lsb) ? _lsb : _msb;
  bool   isEqual  = true;

  // If all bits are equal to the particular state then isEqual will be true.
  // However, if just one of the bits is not equal to the state then isEqual
  // will be false.

  for ( uint32 index = min; index <= max; index++ ) {
    isEqual &= isState(index, state);
  }

  return( isEqual );

}

//------------------------------------------------------------------------------
// protected getState : Get the character state of a given FourState variable
//------------------------------------------------------------------------------

char cReg::getState( const FourState state ) const {

  switch ( state ) {
  case ZERO     : return '0';
  case ONE      : return '1';
  case TRISTATE : return 'z';
  case UNKNOWN  : return 'x';
  default       : return 'x';
  }

}

//------------------------------------------------------------------------------
// protected getState : Get the FourState for a given integer
//------------------------------------------------------------------------------

cReg::FourState cReg::getState ( const uint32 state ) const {

  if ( state == 0 ) {
    return ZERO;
  } else if ( state == 1 ) {
    return ONE;
  } else {
    return UNKNOWN;
  }

}

//------------------------------------------------------------------------------
// protected getState : Get the FourState for a given integer
//------------------------------------------------------------------------------

cReg::FourState cReg::getState ( const char state ) const {

  if ( state == '0' ) {
    return ZERO;
  } else if ( state == '1' ) {
    return ONE;
  } else if ( state == 'z' || state == 'Z' ) {
    return TRISTATE;
  } else {
    return UNKNOWN;
  }

}

//------------------------------------------------------------------------------
// protected getState : Get the FourState for a given integer
//------------------------------------------------------------------------------

cReg::FourState cReg::getState ( const string &state ) const {

  if ( state == "0" ) {
    return ZERO;
  } else if ( state == "1" ) {
    return ONE;
  } else if ( state == "z" || state == "Z" ) {
    return TRISTATE;
  } else {
    return UNKNOWN;
  }

}

//------------------------------------------------------------------------------
// protected getState : Get the state given the data and control bits.
//------------------------------------------------------------------------------

cReg::FourState cReg::getState( const uint32 d, const uint32 c ) const {

  if ( c == 0 && d == 0 ) {
    return ZERO;
  } else if ( c == 0 && d == 1 ) {
    return ONE;
  } else if ( c == 1 && d == 0 ) {
    return TRISTATE;
  } else {
    return UNKNOWN;
  }

}

//------------------------------------------------------------------------------
// protected getDataWord : Get a data word.
//------------------------------------------------------------------------------

uint32 cReg::getDataWord( const uint32 index ) const {
 return(wordVector[index].d);
}

//------------------------------------------------------------------------------
// protected getControlWord : Get a data word.
//------------------------------------------------------------------------------

uint32 cReg::getControlWord( const uint32 index ) const {
  return(wordVector[index].c);
}

//------------------------------------------------------------------------------
// protected setDataWord : Set a data word.
//------------------------------------------------------------------------------

void cReg::setDataWord( const uint32 index, uint32 word ) {
  wordVector[index].d = word;
}

//------------------------------------------------------------------------------
// protected setCtrlWord : Set a control word.
//------------------------------------------------------------------------------

void cReg::setControlWord( const uint32 index, uint32 word ) {
  wordVector[index].c = word;
}

//------------------------------------------------------------------------------
// protected getData : Given the state return what the data would be.
//------------------------------------------------------------------------------

uint32 cReg::getData( const string &state ) const {

  if ( getState(state) == ZERO || getState(state) == TRISTATE ) {
    return 0;
  } else {
    return 1;
  }

}

//------------------------------------------------------------------------------
// protected getControl : Given the state return what the control would be.
//------------------------------------------------------------------------------

uint32 cReg::getControl( const string &state ) const {

  if ( getState(state) == ZERO || getState(state) == ONE ) {
    return 0;
  } else {
    return 1;
  }

}

//------------------------------------------------------------------------------
// protected getData : Given the state return what the data would be.
//------------------------------------------------------------------------------

uint32 cReg::getData( const FourState state ) const {

  if ( state == ZERO || state == TRISTATE ) {
    return 0;
  } else {
    return 1;
  }

}

//------------------------------------------------------------------------------
// protected getControl : Given the state return what the control would be.
//------------------------------------------------------------------------------

uint32 cReg::getControl( const FourState state ) const {

  if ( state == ZERO || state == ONE ) {
    return 0;
  } else {
    return 1;
  }

}

//------------------------------------------------------------------------------
// protected getData : Given the state return what the data would be.
//------------------------------------------------------------------------------

uint32 cReg::getData( const char state ) const {

  if ( getState(state) == ZERO || getState(state) == TRISTATE ) {
    return 0;
  } else {
    return 1;
  }

}

//------------------------------------------------------------------------------
// protected getControl : Given the state return what the control would be.
//------------------------------------------------------------------------------

uint32 cReg::getControl( const char state ) const {

  if ( getState(state) == ZERO || getState(state) == ONE ) {
    return 0;
  } else {
    return 1;
  }

}

//------------------------------------------------------------------------------
// protected getData : Given the state return what the data would be.
//------------------------------------------------------------------------------

uint32 cReg::getData( const uint32 state ) const {

  if ( getState(state) == ZERO ) {
    return 0;
  } else {
    return 1;
  }

}

//------------------------------------------------------------------------------
// protected getControl : Given the state return what the control would be.
//------------------------------------------------------------------------------

uint32 cReg::getControl( const uint32 state ) const {

  if ( getState(state) == ZERO || getState(state) == ONE ) {
    return 0;
  } else {
    return 1;
  }

}

//------------------------------------------------------------------------------
// protected getDataBit: Get the data for a particular bit
//------------------------------------------------------------------------------

uint32 cReg::getDataBit( const uint32 index ) const {

  uint32 wordIndex = getWordIndex(index);
  uint32 bitIndex  = getBitIndex(index);
  uint32 word      = getDataWord(wordIndex);

  return( getIntegerBit(word, bitIndex) );

}

//------------------------------------------------------------------------------
// protected getControlBit: Get the control for a particular bit
//------------------------------------------------------------------------------

uint32 cReg::getControlBit( const uint32 index ) const {

  uint32 wordIndex = getWordIndex(index);
  uint32 bitIndex  = getBitIndex(index);
  uint32 word      = getControlWord(wordIndex);

  return( getIntegerBit(word, bitIndex) );

}

//------------------------------------------------------------------------------
// protected setIntegerBit: Set a particular bit of a uint32 integer
//------------------------------------------------------------------------------

void cReg::setIntegerBit( uint32 &_data, const uint32 index, const uint32 value ) const {

  // We want to access the actual bit within the integer.
  // To isolate the bit we create a mask and OR or AND it
  // with the integer depending on the value.

  uint32 mask = static_cast<uint32>( pow( static_cast<double>(2.0), static_cast<double>(index) ) );

  if ( value == 0 ) {
    _data &= ~mask;
  } else if ( value == 1 ) {
    _data |= mask;
  }

}

//------------------------------------------------------------------------------
// protected setIntegerBit: Set a particular bit of a uint64 integer
//------------------------------------------------------------------------------

void cReg::setIntegerBit( uint64 &_data, const uint32 index, const uint32 value ) const {

  // We want to access the actual bit within the integer.
  // To isolate the bit we create a mask and OR or AND it
  // with the integer depending on the value.

  uint64 mask = static_cast<uint64>( pow( static_cast<double>(2.0), static_cast<double>(index) ) );

  if ( value == 0 ) {
    _data &= ~mask;
  } else if ( value == 1 ) {
    _data |= mask;
  }

}

//------------------------------------------------------------------------------
// protected getIntegerBit: Get a particular bit of a uint32 integer
//------------------------------------------------------------------------------

uint32 cReg::getIntegerBit( const uint32 _data, const uint32 index ) const {

  if ( index < crWord::size ) {
    return ( _data >> index ) & 0x1;
  }

  return 0;

}

//------------------------------------------------------------------------------
// protected getIntegerBit: Get a particular bit of a uint64 integer
//------------------------------------------------------------------------------

uint64 cReg::getIntegerBit( const uint64 _data, const uint32 index ) const {

  if ( index < 2*crWord::size ) {
    return ( _data >> index ) & 0x1;
  }

  return 0;

}

//------------------------------------------------------------------------------
// protected setDataBit: Set the data for a particular bit
//------------------------------------------------------------------------------

void cReg::setDataBit( const uint32 index, const uint32 value ) {

  uint32 wordIndex = getWordIndex(index);
  uint32 bitIndex  = getBitIndex(index);
  uint32 word      = getDataWord(wordIndex);

  setIntegerBit(word, bitIndex, value);
  setDataWord(wordIndex, word);

}

//------------------------------------------------------------------------------
// protected setControlBit: Set the control for a particular bit
//------------------------------------------------------------------------------

void cReg::setControlBit( const uint32 index, const uint32 value ) {

  uint32 wordIndex = getWordIndex(index);
  uint32 bitIndex  = getBitIndex(index);
  uint32 word      = getControlWord(wordIndex);

  setIntegerBit(word, bitIndex, value);
  setControlWord(wordIndex, word);

}

//------------------------------------------------------------------------------
// protected getWordIndex: Get the word index (which word do we need)
//------------------------------------------------------------------------------

uint32 cReg::getWordIndex( const uint32 index ) const {
  return( getActualIndex(index) / crWord::size );
}

//------------------------------------------------------------------------------
// protected getBitIndex: Get the bit index (which bit in a word do we need)
//------------------------------------------------------------------------------

uint32 cReg::getBitIndex( const uint32 index ) const {
  return( getActualIndex(index) % crWord::size );
}

//------------------------------------------------------------------------------
// protected reserve: Reserve more space
//------------------------------------------------------------------------------

void cReg::reserve() {
  wordVector.resize(getWordLength());
}

//------------------------------------------------------------------------------
// protected reverse64: Bitwise reversal of a 64 bit integer (SWAR Algorithm)
//------------------------------------------------------------------------------

uint64 cReg::reverse64(register uint64 x) {

  x = (((x & 0xaaaaaaaaaaaaaaaaULL) >>  1)  | ((x & 0x5555555555555555ULL) <<  1));
  x = (((x & 0xccccccccccccccccULL) >>  2)  | ((x & 0x3333333333333333ULL) <<  2));
  x = (((x & 0xf0f0f0f0f0f0f0f0ULL) >>  4)  | ((x & 0x0f0f0f0f0f0f0f0fULL) <<  4));
  x = (((x & 0xff00ff00ff00ff00ULL) >>  8)  | ((x & 0x00ff00ff00ff00ffULL) <<  8));
  x = (((x & 0xffff0000ffff0000ULL) >> 16)  | ((x & 0x0000ffff0000ffffULL) << 16));
  return((x >> 32) | (x << 32));

}

//------------------------------------------------------------------------------
// protected removeLeadingZeros: Remove leading zeros to reduce size of register
//------------------------------------------------------------------------------

void cReg::removeLeadingZeros() {

  cReg &reg = *this;

  // Eliminate any padding of zeros. However, it assumes MSB > LSB

  while ( reg.isZero( reg.getMsb() ) && reg.getWidth() > 1 ) {
    reg.setMsb( reg.getMsb() - 1 );
  }

}

//------------------------------------------------------------------------------
// protected removeLeadingUnknowns: Remove leading X's to reduce size of register
//------------------------------------------------------------------------------

void cReg::removeLeadingUnknowns() {

  cReg &reg = *this;

  // Eliminate any leading X's. However, it assumes MSB > LSB

  while ( reg.isUnknown( reg.getMsb() ) && reg.getWidth() > 1 ) {
    reg.setMsb( reg.getMsb() - 1 );
  }

}

//------------------------------------------------------------------------------
// protected initialize: Initialize the vector array of words
//------------------------------------------------------------------------------

void cReg::initialize( uint32 d, uint32 c ) {

  // If d is ONE then we need to make all 32 bits in d equal to one

  if ( d == 1 ) {
    d = ~0;
  }

  // If c is ONE then we need to make all 32 bits in c equal to one

  if ( c == 1 ) {
    c = ~0;
  }

  for ( uint32 i = 0; i < getWordLength(); i++ ) {

    setDataWord(i, d);
    setControlWord(i, c);

  }

}

//------------------------------------------------------------------------------
// protected shift: Shift so many bits left or right.
//------------------------------------------------------------------------------

void cReg::shift( const uint32 shiftValue, bool shiftRightFlag ) {

  int wordLength = static_cast<int>(getWordLength());

  for ( int shiftCount = 1; shiftCount <= static_cast<int>(shiftValue); shiftCount++ ) {

    if ( shiftRightFlag ) {

      for ( int wordIndex = 0; wordIndex < wordLength; wordIndex++ ) {
        
        uint32 d_bit = 0;
        uint32 c_bit = 0;
        
        // Save LSB of the next data/ctrl word
        
        if ( wordIndex < ( wordLength - 1 ) ) {

          d_bit = getIntegerBit( getDataWord(wordIndex+1), 0 );
          c_bit = getIntegerBit( getControlWord(wordIndex+1), 0 );

        }
        
        uint32 d_word = getDataWord(wordIndex);
        uint32 c_word = getControlWord(wordIndex);

        // Do a right shift of both data and ctrl

        d_word >>= 1;
        c_word >>= 1;
        
        // Set the MSB of data and ctrl to what was saved
        
        setIntegerBit(d_word, (crWord::size - 1), d_bit);
        setIntegerBit(c_word, (crWord::size - 1), c_bit);
        
        setDataWord(wordIndex, d_word);
        setControlWord(wordIndex, c_word);

      }
      
    } else {

      for ( int wordIndex = ( wordLength - 1 ); wordIndex >= 0; wordIndex-- ) {

        uint32 d_bit = 0;
        uint32 c_bit = 0;
        
        // Save MSB of the previous data/ctrl word

        if ( wordIndex > 0 ) {

          d_bit = getIntegerBit( getDataWord(wordIndex-1), ( crWord::size - 1 ) );
          c_bit = getIntegerBit( getControlWord(wordIndex-1), ( crWord::size - 1 ) );
          
        }
        
        uint32 d_word = getDataWord(wordIndex);
        uint32 c_word = getControlWord(wordIndex);

        // Do a right shift of both data and ctrl

        d_word <<= 1;
        c_word <<= 1;
        
        // Set the MSB of data and ctrl to what was saved
        
        setIntegerBit(d_word, 0, d_bit);
        setIntegerBit(c_word, 0, c_bit);

        setDataWord(wordIndex, d_word);
        setControlWord(wordIndex, c_word);

      }
      
    }

  }

}


//------------------------------------------------------------------------------
// parseVerilogInitialStr : Convert a string of hex or binary digits in verilog
//                          notation to a string of binary digits.
//------------------------------------------------------------------------------

void cReg::parseVerilogInitialStr( const string& _str, uint32& width, string& result ) {

  // convert to lower-case (makes comparison easy)

  string str = toLower(_str);

  ostringstream os;
  
  string radix         = "bin";  // Radix (bin=binary, hex=hexidecimal)
  char   digit         = '0';    // Binary/Hex digit
  bool   headerFlag    = true;   // Parsing flag: the header is <num>'<radix>
  bool   sizeFlag      = false;  // Size flag: the size is the same as bit width
  bool   radixFlag     = false;  // Radix flag
  bool   fourStateFlag = false;  // Four State Flag

  width = 0;

  for ( uint32 i = 0; i < str.size(); i++ ) {

    digit = tolower(str[i]);

    // Ignore underscores

    if ( digit == '_' ) {
      continue;
    }

    if ( headerFlag && ! sizeFlag && digit == '\'' ) {

      // Parsing of the size (bit width) of this intializer is complete
      // Convert the character string to an integer and set appropriate flags.

      if ( fourStateFlag ) {
        FATAL_ERROR("cReg::set: Illegal bit width '%s' was specified in: %s", os.str().c_str(), str.c_str());
      }

      if ( ! os.str().empty() ) {
        width = atoi(os.str().c_str());
      }

      sizeFlag = true;

      os.str("");
            
    } else if ( headerFlag && ! sizeFlag ) {

      // This section is looking for the size (i.e. bit width)
      // If the tick character is never found then the default
      // behavior is to treat the whole string as binary.

      if ( ! isdigit(digit) && digit != 'x' && digit != 'z' ) {
        FATAL_ERROR("cReg::set: Illegal digit '%c' was specified at position %d in: %s", digit, i, str.c_str());
      } else if ( digit == 'x' || digit == 'z' ) {
        fourStateFlag = true;
      }

      os << digit;

    } else if ( headerFlag && sizeFlag && ! radixFlag ) {

      // Parsing of the radix is complete. It should be either
      // b for binary or h for hexidecimal.

      if ( digit == 'b' ) {
        radix = "bin";
      } else if ( digit == 'h' ) {
        radix = "hex";
      } else {
        FATAL_ERROR("cReg::set: Illegal radix '%c' was specified at position %d in : %s", digit, i, str.c_str());
      }

      radixFlag  = true;
      headerFlag = false; // done parsing header

    } else if ( ! headerFlag ) {

      if ( radix == "bin" ) {

        // Make sure digit is 0 or 1 bin

        if ( digit == '0' || digit == '1' || digit == 'x' || digit == 'z' ) {
          os << digit;
        } else {
          FATAL_ERROR("cReg::set: Illegal binary digit '%c' was specified at position %d in : %s", digit, i, str.c_str());
        }

      } else {

        // Make sure digit is 0-F hex

        if ( digit == '0' || digit == '1' || digit == '2' || digit == '3' ||
             digit == '4' || digit == '5' || digit == '6' || digit == '7' ||
             digit == '8' || digit == '9' || digit == 'a' || digit == 'b' ||
             digit == 'c' || digit == 'd' || digit == 'e' || digit == 'f' ||
             digit == 'x' || digit == 'z' ) {
          os << digit;
        } else {
          FATAL_ERROR("cReg::set: Illegal hex digit '%c' was specified at position %d in: %s", digit, i, str.c_str());
        }

      }

    }

  }

  if ( radix == "hex" ) {
    result = cReg::hex(os.str());
  } else {
    result = os.str();
  }

  // If width was not specified then the string was not a verilog
  // string but rather a binary string and the width is the size
  // of that binary string.

  if ( width == 0 ) {
    width = result.size();
  }

  if ( width > result.size() ) {

    // Pad with zeros
    
    string paddedZeros(width-result.size(), '0');
    result = paddedZeros + result;

  } else if ( width < result.size() ) {

    // Truncate

    result = result.substr(result.size()-width,result.size()-1);

  }

  width = result.size();

}

//------------------------------------------------------------------------------
// static : toLower
//------------------------------------------------------------------------------

string cReg::toLower( const string& name ) {

  string lcName;

  for ( uint32 i = 0; i < name.size(); i++ ) {
    lcName += tolower(name[i]);
  }

  return(lcName);

}

//------------------------------------------------------------------------------
// static : toUpper
//------------------------------------------------------------------------------

string cReg::toUpper( const string& name ) {

  string ucName;

  for ( uint32 i = 0; i < name.size(); i++ ) {
    ucName += toupper(name[i]);
  }

  return(ucName);

}

//------------------------------------------------------------------------------
// hex : Convert a hex string to a binary string. Used for setting registers
//       to a hex value greater than 64 bits (limit for C++).
//------------------------------------------------------------------------------

string cReg::hex( const string& str ) {
  
  string result;

  for ( uint32 i = 0; i < str.size(); i++ ) {
    
    switch ( tolower(str[i]) ) {
          
    case '0': result += "0000"; break;
    case '1': result += "0001"; break;
    case '2': result += "0010"; break;
    case '3': result += "0011"; break;
    case '4': result += "0100"; break;
    case '5': result += "0101"; break;
    case '6': result += "0110"; break;
    case '7': result += "0111"; break;
    case '8': result += "1000"; break;
    case '9': result += "1001"; break;
    case 'a': result += "1010"; break;
    case 'b': result += "1011"; break;
    case 'c': result += "1100"; break;
    case 'd': result += "1101"; break;
    case 'e': result += "1110"; break;
    case 'f': result += "1111"; break;
    case 'z': result += "zzzz"; break;
    case 'x': result += "xxxx"; break;
    default : result += "xxxx";
      
    }
 
  }

  return result;

}

//------------------------------------------------------------------------------
// copy : Copy one cReg to another cReg including it's size. The assignment
//        operator does not change the size of the resulting cReg. This
//        fuction will.
//------------------------------------------------------------------------------

void cReg::copy( const cReg& srcReg, cReg& destReg ) {

  // Resize & Initialize

  destReg.setRange(srcReg.getMsb(), srcReg.getLsb());
  destReg.initialize(ZERO);

  // Assign data

  destReg = srcReg;

  // Copy field and attribute maps

  destReg.copyMaps(srcReg.fieldMap, srcReg.attributeMap);

}

//------------------------------------------------------------------------------
// protected : findField
//------------------------------------------------------------------------------

crFieldMapIter cReg::findField( const string& name ) {

  string ucName = toUpper(name);

  // Lookup field in the field map
  
  crFieldMapIter i;

  if ( fieldMap != NULL ) {

    i = fieldMap->find(ucName);
    
    if ( i == fieldMap->end() ) {
      FATAL_ERROR("Field '%s' does not exist.", name.c_str());
    }

  } else {
    FATAL_ERROR("Field '%s' does not exist.", name.c_str());
  }

  return i;

}

//------------------------------------------------------------------------------
// hasField
//------------------------------------------------------------------------------

bool cReg::hasField( const string& name ) {

  string ucName = toUpper(name);

  // Lookup field in the field map
  
  crFieldMapIter i;

  if ( fieldMap != NULL ) {

    i = fieldMap->find(ucName);
  
    if ( i == fieldMap->end() ) {
      return(false);
    }

  } else {
    return(false);
  }

  return true;

}

//------------------------------------------------------------------------------
// protected: findAttribute
//------------------------------------------------------------------------------

crAttributeMapIter cReg::findAttribute( const string& attrName ) {
  
  // Lookup attribute in the attribute map
  
  crAttributeMapIter i;
  
  if ( attributeMap != NULL ) {
    
    i = attributeMap->find(attrName);
    
    if ( i == attributeMap->end() ) {
      FATAL_ERROR("Attribute '%s' does not exist.", attrName.c_str());
    }
    
  } else {
    FATAL_ERROR("Attribute '%s' does not exist.", attrName.c_str());
  }
  
  return i;
  
}

//------------------------------------------------------------------------------
// newAttribute
//------------------------------------------------------------------------------

void cReg::newAttribute( const string& attrName, const string& attrValue ) {
  newAttributeTyped<string>(attrName, attrValue);
}

//------------------------------------------------------------------------------
// getAttribute
//------------------------------------------------------------------------------

string cReg::getAttribute( const string& attrName ) {
  return( getAttributeTyped<string>(attrName) );
}

//------------------------------------------------------------------------------
// getAttributes
//------------------------------------------------------------------------------

crAttributeList cReg::getAttributes() {

  crAttributeList attributeList;

  if ( attributeMap != NULL ) {

    for ( crAttributeMapIter i = attributeMap->begin(); i != attributeMap->end(); ++i ) {
      attributeList.push_back(i->first);
    }

  }

  return attributeList;

}

//------------------------------------------------------------------------------
// hasAttribute
//------------------------------------------------------------------------------

bool cReg::hasAttribute( const string& attrName ) {
  
  // Lookup attribute in the attribute map
  
  crAttributeMapIter i;
    
  if ( attributeMap != NULL ) {
    
    i = attributeMap->find(attrName);
    
    if ( i == attributeMap->end() ) {
      return(false);
    }
    
  } else {
    return(false);
  }
  
  return true;
  
}

//------------------------------------------------------------------------------
// newField : Define a new field which represents a bit range of one or more
//            bits in the register.
//------------------------------------------------------------------------------

void cReg::newField(const string& fieldName, const uint32 _msb, const uint32 _lsb) {

  if ( ! isIndexValid(_msb) || ! isIndexValid(_lsb) ) {
    FATAL_ERROR("cReg::newField(name=%s, msb=%d, lsb=%d): the msb and/or lsb index is out of the range [%d:%d]",
                fieldName.c_str(), _msb, _lsb, getMsb(), getLsb());
  } else if ( hasField(fieldName) ) {
    FATAL_ERROR("cReg::newField(name=%s): field name '%s' already exists.", fieldName.c_str(), fieldName.c_str());
  } else {

    string ucFieldName = toUpper(fieldName);

    mallocFieldMap();

    cRegField* field = new cRegField(_msb, _lsb);

    // Add msb and lsb as field attributes

    fieldMap->operator[](ucFieldName) = field;

  }
  
}

//------------------------------------------------------------------------------
// newFieldAttr : Define a new field attribute.
//------------------------------------------------------------------------------

void cReg::newFieldAttr(const string& fieldName, const string& attrName, const int32& attr ) {
  newFieldAttributeTyped<int32>(fieldName, attrName, attr);
}

//------------------------------------------------------------------------------
// getField : Get a reference to the specified field.
//------------------------------------------------------------------------------

crBitRangeRef cReg::getField(const string& name) {

  crFieldMapIter i = findField(name);

  uint32 _msb = i->second->getMsb();
  uint32 _lsb = i->second->getLsb();

  return( crBitRangeRef( *this, _msb, _lsb ) );

}

//------------------------------------------------------------------------------
// getFieldMsb : Get the msb of a field
//------------------------------------------------------------------------------

uint32 cReg::getFieldMsb(const string& name) {

  crFieldMapIter i = findField(name);

  return( i->second->getMsb() );

}

//------------------------------------------------------------------------------
// getFieldLsb : Get the lsb of a field
//------------------------------------------------------------------------------

uint32 cReg::getFieldLsb(const string& name) {

  crFieldMapIter i = findField(name);

  return( i->second->getLsb() );

}

//------------------------------------------------------------------------------
// getFields
//------------------------------------------------------------------------------

crFieldList cReg::getFields() {

  crFieldList fieldList;

  if ( fieldMap != NULL ) {

    for ( crFieldMapIter i = fieldMap->begin(); i != fieldMap->end(); ++i ) {
      fieldList.push_back(i->first);
    }

  }

  return fieldList;

}

//------------------------------------------------------------------------------
// getFieldAttr : Get a field attribute
//------------------------------------------------------------------------------

int32& cReg::getFieldAttr(const string& fieldName, const string& attrName) {
  return( getFieldAttributeTyped<int32>(fieldName, attrName) );
}

//------------------------------------------------------------------------------
// resize : same as setRange
//------------------------------------------------------------------------------

void cReg::resize( const uint32 _msb, const uint32 _lsb ) {
  setRange(_msb, _lsb);
}

//------------------------------------------------------------------------------
// resizeAndCopy : same as static copy method
//------------------------------------------------------------------------------

void cReg::resizeAndCopy( const cReg& srcReg ) {
  cReg::copy(srcReg, *this);
}

//------------------------------------------------------------------------------
// concat : Concatinate (append) another register to this register
//------------------------------------------------------------------------------

void cReg::concat( const cReg& srcReg ) {

  cReg tmpReg = *this; // temporary copy of this register
  
  uint32 width = getWidth() + srcReg.getWidth();

  // Resize this register

  setMsb(width-1);
  setLsb(0);
  initialize(0);

  // Copy data

  set(width - 1, width - tmpReg.getWidth(), tmpReg);
  set(width - tmpReg.getWidth() - 1, 0, srcReg);
  
}

//------------------------------------------------------------------------------
// toHex : Create a string of hex digits which represent this register.
//------------------------------------------------------------------------------

string cReg::toHex( bool addPrefix ) const {
   
  uint32 nibbleSize       = 4;
  uint32 nibbleMask_4bit  = 0xf;
  uint32 nibbleMask_3bit  = 0x7;
  uint32 nibbleMask_2bit  = 0x3;
  uint32 nibbleMask_1bit  = 0x1;

  char   nibble     = getState(ZERO);
  string hexResult  = "";
  uint32 wordLength = getWordLength();

  for ( uint32 wordIndex = 0; wordIndex < wordLength; wordIndex++ ) {

    // Get the control word and data word

    uint32 word_c = getControlWord(wordIndex);
    uint32 word_d = getDataWord(wordIndex);

    // Calculate nibbleLength based on word size, nibble size and width of the register

    uint32 nibbleLength  = crWord::size / nibbleSize;
    uint32 remainingBits = getWidth() - wordIndex * crWord::size;

    if ( wordIndex == wordLength - 1 ) {
      nibbleLength = ( remainingBits - 1 ) / nibbleSize + 1;
    }

    for ( uint32 nibbleIndex = 0; nibbleIndex < nibbleLength; nibbleIndex++ ) {
      
      nibble        = getState(ZERO);

      // Calculate Shift

      uint32 shift  = (nibbleIndex == 0) ? 0 : nibbleSize;

      // Calculate Mask
      
      uint32 nibbleMask = nibbleMask_4bit;

      switch ( remainingBits - ( nibbleIndex * nibbleSize + 1 ) ) {
      case 0: nibbleMask = nibbleMask_1bit; break;
      case 1: nibbleMask = nibbleMask_2bit; break;
      case 2: nibbleMask = nibbleMask_3bit; break;
      case 3: nibbleMask = nibbleMask_4bit; break;
      }

      // Shift Words

      word_c >>= shift;
      word_d >>= shift;

      // Apply Mask

      uint32 nibble_c = word_c & nibbleMask;
      uint32 nibble_d = word_d & nibbleMask;

      // If any of the control bits are set for this nibble then
      // we have either TRISTATE or UNKNOWN bits and we need to
      // check for UNKNOWN bits.

      if ( nibble_c > 0 ) {

        for ( uint32 bitIndex = 0; bitIndex < nibbleSize; bitIndex++ ) {
          
          uint32 bit_c = getIntegerBit(nibble_c, bitIndex);
          uint32 bit_d = getIntegerBit(nibble_d, bitIndex);

          if ( getState( bit_d, bit_c ) == UNKNOWN ) {
            
            nibble = getState(UNKNOWN);
            break;
            
          } else if ( getState( bit_d, bit_c ) == TRISTATE ) {
            nibble = getState(TRISTATE);
          }
          
        }
        
      } else {

        // There were no control bits set so we can now get the HEX value for d

        switch (nibble_d) {

        case  0: nibble = '0'; break;
        case  1: nibble = '1'; break;
        case  2: nibble = '2'; break;
        case  3: nibble = '3'; break;
        case  4: nibble = '4'; break;
        case  5: nibble = '5'; break;
        case  6: nibble = '6'; break;
        case  7: nibble = '7'; break;
        case  8: nibble = '8'; break;
        case  9: nibble = '9'; break;
        case 10: nibble = 'a'; break;
        case 11: nibble = 'b'; break;
        case 12: nibble = 'c'; break;
        case 13: nibble = 'd'; break;
        case 14: nibble = 'e'; break;
        case 15: nibble = 'f'; break;
        default: nibble = getState(UNKNOWN);

        }

      }

      hexResult += nibble;

    }

  }

  // Reverse order of digits

  std::reverse( hexResult.begin (), hexResult.end () );

  // We need to remove any hex digits which are not part of
  // the register. For example, if this register has a width
  // of only 8 then we have 24 extra bits because the word
  // size is 32. This means there are 6 extra hex digits that
  // need to be removed.

  uint32 actualNibbleLength = getWidth() / nibbleSize;

  if ( getWidth() % nibbleSize ) {
    actualNibbleLength++;
  }

  hexResult.erase( 0, ( hexResult.size() - actualNibbleLength ) );

  // Add prefix to indicate Hexidecimal

  if ( addPrefix ) {
    char buffer[128];
    sprintf(buffer, "%d'h", getWidth());
    hexResult = buffer + hexResult;
  }

  return hexResult;

}

//------------------------------------------------------------------------------
// toBin : Create a string of binary digits which represent this register.
//------------------------------------------------------------------------------

string cReg::toBin( bool addPrefix ) const {

  string binResult;

  for ( uint32 wordIndex = 0; wordIndex < getWordLength(); wordIndex++ ) {

    // Get the control word and data word

    uint32 word_c = getControlWord(wordIndex);
    uint32 word_d = getDataWord(wordIndex);

    // Go through each bit in the 32 bit word

    for ( uint32 bitIndex = 0; bitIndex < crWord::size; bitIndex++ ) {

      uint32 bit_c = getIntegerBit(word_c, bitIndex);
      uint32 bit_d = getIntegerBit(word_d, bitIndex);
      
      binResult += getState( getState( bit_d, bit_c ) );

    }

  }

  // Reverse order of digits

  std::reverse( binResult.begin (), binResult.end () );

  // We need to remove any digits which are not part of
  // the register. For example, if this register has a width
  // of only 8 then we have 24 extra bits because the word
  // size is 32. This means there are 6 extra bin digits that
  // need to be removed.

  binResult.erase( 0, ( binResult.size() - getWidth() ) );

  // Add prefix to indicate binary
  
  if ( addPrefix ) {

    char buffer[128];
    sprintf(buffer, "%d'b", getWidth());
    binResult = buffer + binResult;

  }

  return binResult;

}

//------------------------------------------------------------------------------
// initialize: Initialize all bits of the register to a specified state
//------------------------------------------------------------------------------

void cReg::initialize( const FourState state ) {

  uint32 d = getData(state);
  uint32 c = getControl(state);

  initialize(d,c);

}

//------------------------------------------------------------------------------
// initialize: Initialize all bits of the register to a specified state
//------------------------------------------------------------------------------

void cReg::initialize( const char state ) {

  uint32 d = getData(state);
  uint32 c = getControl(state);

  initialize(d,c);

}

//------------------------------------------------------------------------------
// initialize: Initialize all bits of the register to a specified state
//------------------------------------------------------------------------------

void cReg::initialize( const string &state ) {

  uint32 d = getData(state);
  uint32 c = getControl(state);

  initialize(d,c);

}

//------------------------------------------------------------------------------
// initialize: Initialize all bits of the register to a specified state
//------------------------------------------------------------------------------

void cReg::initialize( const int32 state ) {
  initialize(static_cast<uint32>(state));
}

//------------------------------------------------------------------------------
// initialize: Initialize all bits of the register to a specified state
//------------------------------------------------------------------------------

void cReg::initialize( const uint32 state ) {

  uint32 d = getData(state);
  uint32 c = getControl(state);

  initialize(d,c);

}


//------------------------------------------------------------------------------
// setMsb: Set the MSB
//------------------------------------------------------------------------------

void cReg::setMsb( const uint32 _msb ) {

  msb = _msb;

  reserve();

}

//------------------------------------------------------------------------------
// setLsb: Set the LSB
//------------------------------------------------------------------------------

void cReg::setLsb( const uint32 _lsb ) {

  lsb = _lsb;

  reserve();

}

//------------------------------------------------------------------------------
// setRange: Set the MSB and LSB (range)
//------------------------------------------------------------------------------

void cReg::setRange( const uint32 _msb, const uint32 _lsb ) {

  lsb = _lsb;
  msb = _msb;

  reserve();

}

//------------------------------------------------------------------------------
// set: Set the register to equal the string of bit states
//------------------------------------------------------------------------------

void cReg::set( const char* value ) {
  set(string(value));
}

//------------------------------------------------------------------------------
// set: Set the register to equal the string of bit states
//------------------------------------------------------------------------------

void cReg::set( const string& value ) {

  // Pad value with zeros if value string length
  // is less than width.

  if ( value.size() < getWidth() ) {

    string s(getWidth() - value.size(), getState(ZERO));
    s += value;

    set(msb, lsb, s);

  } else {
    set(msb, lsb, value);
  }

}

//------------------------------------------------------------------------------
// set: Set the register to a 8 bit integer
//------------------------------------------------------------------------------

void cReg::set( const int8 value ) {
  set(static_cast<uint64>(value));
}

//------------------------------------------------------------------------------
// set: Set the register to a 16 bit integer
//------------------------------------------------------------------------------

void cReg::set( const int16 value ) {
  set(static_cast<uint64>(value));
}

//------------------------------------------------------------------------------
// set: Set the register to a 32 bit integer
//------------------------------------------------------------------------------

void cReg::set( const signed long value ) {
  set(static_cast<uint64>(value));
}

//------------------------------------------------------------------------------
// set: Set the register to a 32 bit integer
//------------------------------------------------------------------------------

void cReg::set( const int32 value ) {
  set(static_cast<uint64>(value));
}

//------------------------------------------------------------------------------
// set: Set the register to a 64 bit integer
//------------------------------------------------------------------------------

void cReg::set( const int64 value ) {
  set(static_cast<uint64>(value));
}

//------------------------------------------------------------------------------
// set: Set the register to a 8 bit unsigned integer
//------------------------------------------------------------------------------

void cReg::set( const uint8 value ) {
  set(static_cast<uint64>(value));
}

//------------------------------------------------------------------------------
// set: Set the register to a 16 bit unsigned integer
//------------------------------------------------------------------------------

void cReg::set( const uint16 value ) {
  set(static_cast<uint64>(value));
}

//------------------------------------------------------------------------------
// set: Set the register to a 32 bit unsigned integer
//------------------------------------------------------------------------------

void cReg::set( const unsigned long value ) {
  set(static_cast<uint64>(value));
}

//------------------------------------------------------------------------------
// set: Set the register to a 32 bit unsigned integer
//------------------------------------------------------------------------------

void cReg::set( const uint32 value ) {
  set(static_cast<uint64>(value));
}

//------------------------------------------------------------------------------
// set: Set the register to a 64 bit unsigned integer
//------------------------------------------------------------------------------

void cReg::set( const uint64 value ) {

  initialize(ZERO);
  if ( msb > lsb ) {
    set(msb, lsb, value);
  } else {
    set(lsb, msb, value);
  }

}

//------------------------------------------------------------------------------
// set: Set a bit in the register
//------------------------------------------------------------------------------

void cReg::set( const uint32 index, const FourState value ) {

  uint32 d = getData(value);
  uint32 c = getControl(value);

  setDataBit(index, d);
  setControlBit(index, c);

}

//------------------------------------------------------------------------------
// set: Set a bit in the register
//------------------------------------------------------------------------------

void cReg::set( const uint32 index, const string &value ) {

  uint32 d = getData(value);
  uint32 c = getControl(value);

  setDataBit(index, d);
  setControlBit(index, c);

}

//------------------------------------------------------------------------------
// set: Set a bit in the register
//------------------------------------------------------------------------------

void cReg::set( const uint32 index, const char value ) {

  uint32 d = getData(value);
  uint32 c = getControl(value);

  setDataBit(index, d);
  setControlBit(index, c);

}

//------------------------------------------------------------------------------
// set: Set a bit in the register
//------------------------------------------------------------------------------

void cReg::set( const uint32 index, const signed long value ) {
  set(index, static_cast<uint32>(value));
}

//------------------------------------------------------------------------------
// set: Set a bit in the register
//------------------------------------------------------------------------------

void cReg::set( const uint32 index, const unsigned long value ) {
  set(index, static_cast<uint32>(value));
}

//------------------------------------------------------------------------------
// set: Set a bit in the register
//------------------------------------------------------------------------------

void cReg::set( const uint32 index, const int32 value ) {
  set(index, static_cast<uint32>(value));
}

//------------------------------------------------------------------------------
// set: Set a bit in the register
//------------------------------------------------------------------------------

void cReg::set( const uint32 index, const uint32 value ) {

  uint32 d = getData(value);
  uint32 c = getControl(value);

  setDataBit(index, d);
  setControlBit(index, c);

}

//------------------------------------------------------------------------------
// set: Set a range of bits in the register
//------------------------------------------------------------------------------

void cReg::set( const uint32 _msb, const uint32 _lsb, const cReg &reg ) {

  // Check to make sure both msb and lsb indexes are valid

  if ( ! isIndexValid(_msb) || ! isIndexValid(_lsb) ) {
    FATAL_ERROR("cReg::set(msb=%d, lsb=%d, cReg=%s): the msb and/or lsb index is out of the range [%d:%d]",
                _msb, _lsb, reg.toHex().c_str(), getMsb(), getLsb());
  } else {

    // Set the data one bit at a time from LSB to MSB.
    
    uint32 regIndex       = reg.getLsb();
    int    regDirection   = 1;

    if ( reg.getMsb() < reg.getLsb() ) {
      regDirection = -1;
    }
    
    uint32 minIndex = _lsb;
    uint32 maxIndex = _msb;
    
    if ( _msb < _lsb ) {
      minIndex = _msb;
      maxIndex = _lsb;
    }

    // We set the bits from index _lsb to _msb, if we are out of
    // range with respect to 'reg' which was passed in as a parameter
    // then we pad the remaining bits with zero.

    for ( uint32 index = minIndex; index <= maxIndex; index++ ) {

      if ( reg.isIndexValid(regIndex) ) {
        set(index, reg.getBit(regIndex));
      } else {
        set(index, ZERO);
      }

      regIndex += regDirection;

    }
    
  }

}

//------------------------------------------------------------------------------
// set: Set a range of bits in the register
//------------------------------------------------------------------------------

void cReg::set( const uint32 _msb, const uint32 _lsb, const char* value ) {
  set(_msb, _lsb, string(value));
}

//------------------------------------------------------------------------------
// set: Set a range of bits in the register
//------------------------------------------------------------------------------

void cReg::set( const uint32 _msb, const uint32 _lsb, const string& value ) {

  uint32 width;
  string binStr;
  parseVerilogInitialStr(value, width, binStr);

  setUsingBinaryString(_msb, _lsb, binStr);

}

//------------------------------------------------------------------------------
// set: Set a range of bits in the register
//------------------------------------------------------------------------------

void cReg::setUsingBinaryString( const uint32 _msb, const uint32 _lsb, const string& binStr ) {

  uint32 width = binStr.size();

  // Check both the msb and lsb indexes to make sure they are valid

  if ( ! isIndexValid(_msb) || ! isIndexValid(_lsb) ) {
    FATAL_ERROR("cReg::set(msb=%d, lsb=%d, string=%s): the msb and/or lsb index is out of the range [%d:%d]",
                _msb, _lsb, binStr.c_str(), getMsb(), getLsb());
  } else {
    
    int    min      = (_msb > _lsb) ? _lsb : _msb;
    int    max      = (_msb < _lsb) ? _lsb : _msb;
    int    cntDir   = (_msb > _lsb) ?    1 : -1;
    int    index    = _lsb;
    int    strIndex = width-1;

    for ( int i = 0; i <= (max - min); i++ ) {

      if ( strIndex >= 0 && strIndex < static_cast<int>(width) ) {
        set(index, binStr[ strIndex ]);
      } else {
        set(index, ZERO);
      }

      index += cntDir;

      strIndex--;

    }

  }

}

//------------------------------------------------------------------------------
// set: Set a range of bits in the register
//------------------------------------------------------------------------------

void cReg::set( const uint32 _msb, const uint32 _lsb, const int8 value ) {
  set(_msb,_lsb,static_cast<uint64>(value));
}

//------------------------------------------------------------------------------
// set: Set a range of bits in the register
//------------------------------------------------------------------------------

void cReg::set( const uint32 _msb, const uint32 _lsb, const int16 value ) {
  set(_msb,_lsb,static_cast<uint64>(value));
}

//------------------------------------------------------------------------------
// set: Set a range of bits in the register
//------------------------------------------------------------------------------

void cReg::set( const uint32 _msb, const uint32 _lsb, const signed long value ) {
  set(_msb,_lsb,static_cast<uint64>(value));
}

//------------------------------------------------------------------------------
// set: Set a range of bits in the register
//------------------------------------------------------------------------------

void cReg::set( const uint32 _msb, const uint32 _lsb, const int32 value ) {
  set(_msb,_lsb,static_cast<uint64>(value));
}

//------------------------------------------------------------------------------
// set: Set a range of bits in the register
//------------------------------------------------------------------------------

void cReg::set( const uint32 _msb, const uint32 _lsb, const int64 value ) {
  set(_msb,_lsb,static_cast<uint64>(value));
}

//------------------------------------------------------------------------------
// set: Set a range of bits in the register
//------------------------------------------------------------------------------

void cReg::set( const uint32 _msb, const uint32 _lsb, const uint8 value ) {
  set(_msb,_lsb,static_cast<uint64>(value));
}

//------------------------------------------------------------------------------
// set: Set a range of bits in the register
//------------------------------------------------------------------------------

void cReg::set( const uint32 _msb, const uint32 _lsb, const uint16 value ) {
  set(_msb,_lsb,static_cast<uint64>(value));
}

//------------------------------------------------------------------------------
// set: Set a range of bits in the register
//------------------------------------------------------------------------------

void cReg::set( const uint32 _msb, const uint32 _lsb, const unsigned long value ) {
  set(_msb,_lsb,static_cast<uint64>(value));
}

//------------------------------------------------------------------------------
// set: Set a range of bits in the register
//------------------------------------------------------------------------------

void cReg::set( const uint32 _msb, const uint32 _lsb, const uint32 value ) {
  set(_msb,_lsb,static_cast<uint64>(value));
}

//------------------------------------------------------------------------------
// set: Set a range of bits in the register
//------------------------------------------------------------------------------

void cReg::set( const uint32 _msb, const uint32 _lsb, const uint64 value ) {

  // Check both the msb and lsb indexes to make sure they are valid

  if ( ! isIndexValid(_msb) || ! isIndexValid(_lsb) ) {
    FATAL_ERROR("cReg::set(msb=%d, lsb=%d, uint64=%llu): the msb and/or lsb index is out of the range [%d:%d]",
                _msb, _lsb, value, getMsb(), getLsb());
  } else {
    
    // Create temporary lsb and msb such that tmpMsb >= tmpLsb

    uint32 tmpLsb  = (_msb > _lsb) ? _lsb : _msb;
    uint32 tmpMsb  = (_msb < _lsb) ? _lsb : _msb;

    uint32 msbWordIndex = getWordIndex(tmpMsb);
    uint32 lsbWordIndex = getWordIndex(tmpLsb);
    uint64 data64       = value;
    uint32 shift        = 0;
    uint32 fullMask     = ~0;

    // NOTE: I realize that I could consolidate the following two for loops into one loop.
    //       However, I decided not too for readability.

    if ( _msb < _lsb ) {

      // NOTE: Reverse bus notation: need to do a bitwise reversal on the data

      data64 = reverse64(data64);

      for ( int32 wIdx = static_cast<int32>(msbWordIndex); wIdx >= static_cast<int32>(lsbWordIndex); --wIdx ) {
        
        uint32 wordIndex = static_cast<uint32>(wIdx);
        uint32 rightIdx  = (wordIndex == lsbWordIndex) ? getBitIndex(tmpLsb) : 0;
        uint32 leftIdx   = (wordIndex == msbWordIndex) ? getBitIndex(tmpMsb) : crWord::msb;
        uint32 leftMask  = fullMask >> ((crWord::msb) - leftIdx);
        uint32 rightMask = fullMask << rightIdx;
        uint32 mask      = (leftMask & rightMask);
        uint32 dataWord  = getDataWord(wordIndex);
        uint32 ctrlWord  = getControlWord(wordIndex);

        data64 <<= shift;
        
        uint32 data = static_cast<uint32>(data64 >> ((crWord::doubleMsb) - leftIdx));

        setDataWord(wordIndex, ((dataWord & ~mask) | (data & mask)));
        setControlWord(wordIndex, ((ctrlWord & ~mask) | (0 & mask)));

        shift = leftIdx - rightIdx + 1;

      }

    } else {

      for ( uint32 wordIndex = lsbWordIndex; wordIndex <= msbWordIndex; ++wordIndex ) {
        
        uint32 rightIdx  = (wordIndex == lsbWordIndex) ? getBitIndex(tmpLsb) : 0;
        uint32 leftIdx   = (wordIndex == msbWordIndex) ? getBitIndex(tmpMsb) : crWord::msb;
        uint32 leftMask  = fullMask >> ((crWord::msb) - leftIdx);
        uint32 rightMask = fullMask << rightIdx;
        uint32 mask      = (leftMask & rightMask);
        
        data64 >>= shift;
        
        uint32 dataWord = getDataWord(wordIndex);
        uint32 ctrlWord = getControlWord(wordIndex);
        uint32 data     = static_cast<uint32>(data64 << rightIdx);

        setDataWord(wordIndex, ((dataWord & ~mask) | (data & mask)));
        setControlWord(wordIndex, ((ctrlWord & ~mask) | (0 & mask)));

        shift = leftIdx - rightIdx + 1;
        
      }

    }

  }

}

//------------------------------------------------------------------------------
// pushMsb: Push bit value onto MSB (i.e. reg grows by one bit w/new value being MSB)
//------------------------------------------------------------------------------

void cReg::pushMsb( const FourState value ) {

  if ( getMsb() >= getLsb() ) {
    setMsb(getMsb() + 1); // increase size of reg by one
  } else {
    setLsb(getLsb() + 1); // increase size of reg by one
  }

  set(getMsb(), value); // place value at MSB

}

//------------------------------------------------------------------------------
// pushMsb: Push bit value onto MSB (i.e. reg grows by one bit w/new value being MSB)
//------------------------------------------------------------------------------

void cReg::pushMsb( const uint32 value ) {
  pushMsb(getState(value));
}

//------------------------------------------------------------------------------
// pushMsb: Push bit value onto MSB (i.e. reg grows by one bit w/new value being MSB)
//------------------------------------------------------------------------------

void cReg::pushMsb( const char value ) {
  pushMsb(getState(value));
}

//------------------------------------------------------------------------------
// pushMsb: Push bit value onto MSB (i.e. reg grows by one bit w/new value being MSB)
//------------------------------------------------------------------------------

void cReg::pushMsb( const string &value ) {
  pushMsb(getState(value));
}

//------------------------------------------------------------------------------
// pushLsb: Push bit value onto LSB (i.e. reg grows by one bit w/new value being LSB)
//------------------------------------------------------------------------------

void cReg::pushLsb( const FourState value ) {

  if ( getMsb() >= getLsb() ) {

    setMsb(getMsb() + 1); // increase size of reg by one
    shift(1, false);      // left shift by one

  } else {

    setLsb(getLsb() + 1); // increase size of reg by one
    shift(1, false);      // left shift by one

  }

  set(getLsb(), value); // place value at LSB

}

//------------------------------------------------------------------------------
// pushLsb: Push bit value onto LSB (i.e. reg grows by one bit w/new value being LSB)
//------------------------------------------------------------------------------

void cReg::pushLsb( const uint32 value ) {
  pushLsb(getState(value));
}

//------------------------------------------------------------------------------
// pushLsb: Push bit value onto LSB (i.e. reg grows by one bit w/new value being LSB)
//------------------------------------------------------------------------------

void cReg::pushLsb( const char value ) {
  pushLsb(getState(value));
}

//------------------------------------------------------------------------------
// pushLsb: Push bit value onto LSB (i.e. reg grows by one bit w/new value being LSB)
//------------------------------------------------------------------------------

void cReg::pushLsb( const string &value ) {
  pushLsb(getState(value));
}

//------------------------------------------------------------------------------
// popMsb: Pop off the MSB
//------------------------------------------------------------------------------

cReg::FourState cReg::popMsb() {

  const FourState bit = getBit(getMsb());

  if ( getMsb() != getLsb() ) {

    if ( getMsb() > getLsb() ) {
      setMsb(getMsb() - 1);
    } else {
      setMsb(getMsb() + 1);
    }
    
  }

  return bit;

}

//------------------------------------------------------------------------------
// popLsb: Pop off the LSB
//------------------------------------------------------------------------------

cReg::FourState cReg::popLsb() {

  const FourState bit = getBit(getLsb());

  shift(1, true); // right shift by one

  // Now resize the register accordingly

  if ( getMsb() != getLsb() ) {

    if ( getMsb() > getLsb() ) {
      setLsb(getLsb() + 1);
    } else {
      setLsb(getLsb() - 1);
    }

  }

  return bit;

}

//------------------------------------------------------------------------------
// getMsb: Get the most significant bit (left index)
//------------------------------------------------------------------------------

uint32 cReg::getMsb() const {
  return msb;
}

//------------------------------------------------------------------------------
// getLsb: Get the least significant bit (right index)
//------------------------------------------------------------------------------

uint32 cReg::getLsb() const {
  return lsb;
}

//------------------------------------------------------------------------------
// getWidth: Get the width of the register
//------------------------------------------------------------------------------

uint32 cReg::getWidth() const {
  return( (msb > lsb) ? ( msb - lsb + 1 ) : ( lsb - msb + 1 ) );
}

//------------------------------------------------------------------------------
// getWordLength: Get the word length (how many words)
//------------------------------------------------------------------------------

uint32 cReg::getWordLength() const {

  uint32 word_length = getWidth() / crWord::size;

  if ( getWidth() % crWord::size ) {
    word_length++;
  }

  return( word_length );

}

//------------------------------------------------------------------------------
// getBit : Get the state of a particular bit
//------------------------------------------------------------------------------

cReg::FourState cReg::getBit( const uint32 index ) const {

  uint32 d = getDataBit(index);
  uint32 c = getControlBit(index);

  return( getState(d, c) );

}

//------------------------------------------------------------------------------
// getInt32 : Get 32 bit integer value for this register (i.e. 1st word)
//------------------------------------------------------------------------------

uint32 cReg::getInt32(DataControl dc) const {

  if ( dc == DATA ) {
    return( getDataWord(0) );
  } else {
    return( getControlWord(0) );
  }

}

//------------------------------------------------------------------------------
// getInt64 : Get 64 bit integer value for this register (i.e. 1st word)
//------------------------------------------------------------------------------

uint64 cReg::getInt64(DataControl dc) const {

  // If there is only one word then return that word. If there is more than
  // one word then return the first and second words.

  uint32 wordLength = getWordLength();

  if ( dc == DATA ) {

    if ( wordLength == 1 ) {
      return( static_cast<uint64>(getDataWord(0)) );
    } else {
      return( ( static_cast<uint64>(getDataWord(1)) << crWord::size ) + getDataWord(0) );
    }

  } else {

    if ( wordLength == 1 ) {
      return( static_cast<uint64>(getControlWord(0)) );
    } else {
      return( ( static_cast<uint64>(getControlWord(1)) << crWord::size ) + getControlWord(0) );
    }

  }

}

//------------------------------------------------------------------------------
// getInt32 : Get 32 bit integer value for a range of bits
//------------------------------------------------------------------------------

uint32 cReg::getInt32( const uint32 _msb, const uint32 _lsb, DataControl dc) const {

  uint32 result = 0;
  uint32 min    = (_msb > _lsb) ? _lsb : _msb;
  uint32 max    = (_msb < _lsb) ? _lsb : _msb;
  uint32 width  = (max - min + 1);

  // Check both the msb and lsb indexes to make sure they are valid

  if ( ! isIndexValid(_msb) || ! isIndexValid(_lsb) ) {
    FATAL_ERROR("cReg::getInt32(msb=%d, lsb=%d): the msb and/or lsb index is out of the range [%d:%d]",
                _msb, _lsb, getMsb(), getLsb());
  } else {

    if ( width > crWord::size ) {
      max = min + crWord::size - 1;
    }

    // Initialize index and count direction (1=up, -1=down)

    int index = 0;
    int dir   = 1;

    if ( _msb < _lsb ) {
      index = width-1;
      dir   = -1;
    }

    for ( int i = min; i <= static_cast<int>(max); i++ ) {

      if ( dc == DATA ) {
        setIntegerBit(result, static_cast<uint32>(index), getDataBit(i));
      } else {
        setIntegerBit(result, static_cast<uint32>(index), getControlBit(i));
      }

      index += dir;

    }

  }

  return result;

}

//------------------------------------------------------------------------------
// getInt64 : Get 64 bit integer value for a range of bits
//------------------------------------------------------------------------------

uint64 cReg::getInt64( const uint32 _msb, const uint32 _lsb, DataControl dc ) const {

  uint64 result = 0;
  uint32 width  = (_msb > _lsb) ? (_msb-_lsb+1) : (_lsb-_msb+1);

  // Check both the msb and lsb indexes to make sure they are valid

  if ( ! isIndexValid(_msb) || ! isIndexValid(_lsb) ) {
    FATAL_ERROR("cReg::getInt64(msb=%d, lsb=%d): the msb and/or lsb index is out of the range [%d:%d]",
                _msb, _lsb, getMsb(), getLsb());
  } else if ( width > 2*crWord::size ) {
    FATAL_ERROR("cReg::getInt64(msb=%d, lsb=%d): the width of msb<->lsb is > %d",
                _msb, _lsb, 2*crWord::size );
  } else {

    if ( _msb >= _lsb ) {

      uint32 index = 0;
      for ( int i = _lsb; i <= static_cast<int>(_msb); i++ ) {

        if ( dc == DATA ) {
          setIntegerBit(result, index, getDataBit(i));
        } else {
          setIntegerBit(result, index, getControlBit(i));
        }

        index++;

      }

    } else {

      uint32 index = 0;
      for ( int i = _lsb; i >= static_cast<int>(_msb); i-- ) {

        if ( dc == DATA ) {
          setIntegerBit(result, index, getDataBit(i));
        } else {
          setIntegerBit(result, index, getControlBit(i));
        }

        index++;

      }

    }

  }

  return result;

}

//------------------------------------------------------------------------------
// getSlice : Get new register slice of a single bit.
//------------------------------------------------------------------------------

cReg cReg::getSlice( const uint32 index ) {
  return( getSlice( index, index ) );
}

//------------------------------------------------------------------------------
// getSlice : Get new register slice of a range of bits
//------------------------------------------------------------------------------

cReg cReg::getSlice( const uint32 _msb, const uint32 _lsb ) {

  if ( ! isIndexValid(_msb) || ! isIndexValid(_lsb) ) {
    FATAL_ERROR("cReg::getSlice(msb=%d, lsb=%d): the msb and/or lsb index is out of the range [%d:%d]",
                _msb, _lsb, getMsb(), getLsb());
  }

  uint32 min         = (_msb > _lsb) ? _lsb : _msb;
  uint32 max         = (_msb < _lsb) ? _lsb : _msb;
  uint32 width       = (max - min + 1);

  cReg slice(width-1, 0);

  for ( uint32 index = min; index <= max; index++ ) {

    int    idx        = index - _lsb;
    uint32 sliceIndex = abs( idx );

    slice.set(sliceIndex, getState(getBit(index)));

  }

  return(slice);

}

//------------------------------------------------------------------------------
// isAllZero: Is the whole register zero's
//------------------------------------------------------------------------------

bool cReg::isAllZero() const {
  return( isZero(msb, lsb) );
}

//------------------------------------------------------------------------------
// isZero: Is the single bit zero
//------------------------------------------------------------------------------

bool cReg::isZero( const uint32 index ) const {
  return( isZero(index, index) );
}

//------------------------------------------------------------------------------
// isZero: Is the range of bits zero
//------------------------------------------------------------------------------

bool cReg::isZero( const uint32 _msb, const uint32 _lsb ) const {
  return( isState(_msb, _lsb, ZERO) );
}

//------------------------------------------------------------------------------
// isOne: Is the whole register one's
//------------------------------------------------------------------------------

bool cReg::isAllOne() const {
  return( isOne(msb, lsb) );
}

//------------------------------------------------------------------------------
// isOne: Is the single bit one
//------------------------------------------------------------------------------

bool cReg::isOne( const uint32 index ) const {
  return( isOne(index, index) );
}

//------------------------------------------------------------------------------
// isOne: Is the range of bits one
//------------------------------------------------------------------------------

bool cReg::isOne( const uint32 _msb, const uint32 _lsb ) const {
  return( isState(_msb, _lsb, ONE) );
}

//------------------------------------------------------------------------------
// hasZero: Is any bit zero?
//------------------------------------------------------------------------------

bool cReg::hasZero() const {

  uint32 min = (msb>lsb) ? lsb : msb;
  uint32 max = (msb>lsb) ? msb : lsb;

  for ( uint32 i = min; i <= max; i++ ) {
    if ( isState(i, ZERO) ) {
      return true;
    }
  }

  return false;

}

//------------------------------------------------------------------------------
// hasOne: Is any bit one?
//------------------------------------------------------------------------------

bool cReg::hasOne() const {

  uint32 min = (msb>lsb) ? lsb : msb;
  uint32 max = (msb>lsb) ? msb : lsb;

  for ( uint32 i = min; i <= max; i++ ) {

    if ( isState(i, ONE) ) {
      return true;
    }

  }

  return false;

}

//------------------------------------------------------------------------------
// hasTristate: Does this register contain a tristate (Z)?
//------------------------------------------------------------------------------

bool cReg::hasTristate() const {

  uint32 min = (msb>lsb) ? lsb : msb;
  uint32 max = (msb>lsb) ? msb : lsb;

  for ( uint32 i = min; i <= max; i++ ) {
    if ( isState(i, TRISTATE) ) {
      return true;
    }
  }

  return false;

}

//------------------------------------------------------------------------------
// isTristate: Is the whole register tristate (Z)
//------------------------------------------------------------------------------

bool cReg::isTristate() const {
  return( isTristate(msb, lsb) );
}

//------------------------------------------------------------------------------
// isTristate: Is the single bit tristate (Z)
//------------------------------------------------------------------------------

bool cReg::isTristate( const uint32 index ) const {
  return( isTristate(index, index) );
}

//------------------------------------------------------------------------------
// isTristate: Is the range of bits tristate (Z)
//------------------------------------------------------------------------------

bool cReg::isTristate( const uint32 _msb, const uint32 _lsb ) const {
  return( isState(_msb, _lsb, TRISTATE) );
}

//------------------------------------------------------------------------------
// hasUnknown: Does this register contain an UNKNOWN (X)?
//------------------------------------------------------------------------------

bool cReg::hasUnknown() const {

  uint32 min = (msb>lsb) ? lsb : msb;
  uint32 max = (msb>lsb) ? msb : lsb;

  for ( uint32 i = min; i <= max; i++ ) {
    if ( isState(i, UNKNOWN) ) {
      return true;
    }
  }

  return false;

}

//------------------------------------------------------------------------------
// isUnknown: Is the whole register unknown (X)
//------------------------------------------------------------------------------

bool cReg::isUnknown() const {
  return( isUnknown(msb, lsb) );
}

//------------------------------------------------------------------------------
// isUnknown: Is the single bit unknown (X)
//------------------------------------------------------------------------------

bool cReg::isUnknown( const uint32 index ) const {
  return( isUnknown(index, index) );
}

//------------------------------------------------------------------------------
// isUnknown: Is the range of bits unknown (X)
//------------------------------------------------------------------------------

bool cReg::isUnknown( const uint32 _msb, const uint32 _lsb ) const {
  return( isState(_msb, _lsb, UNKNOWN) );
}

//------------------------------------------------------------------------------
// isTripleEqual : Is this register "triple" equal to another register.
//                 Triple equals in Verilog is where X's must match X's and
//                 Z's must match Z's.
//------------------------------------------------------------------------------

bool cReg::isTripleEqual ( const cReg &rhs ) const {

  // Start of with small and large pointers to indicate
  // what register is smaller and which one is larger.
  // NOTE: they can be equal as well.

  const cReg *sm = this;
  const cReg *lg = &rhs;

  if ( rhs.getWordLength() < getWordLength() ) {

    sm = &rhs;
    lg = this;

  }

  for ( uint32 wordIndex = 0; wordIndex < sm->getWordLength(); wordIndex++ ) {

    // Compare data and control (return false if they are not equal)
    
    if ( ( sm->getDataWord(wordIndex)    != lg->getDataWord(wordIndex)    ) ||
         ( sm->getControlWord(wordIndex) != lg->getControlWord(wordIndex) ) ) {
      return false;
    }
    
  }
  
  // Now we just need to check to see if the remaining words in larger reg are zero

  for ( uint32 wordIndex = sm->getWordLength(); wordIndex < lg->getWordLength(); wordIndex++ ) {
    
    if ( ( lg->getDataWord(wordIndex)    != 0 ) ||
         ( lg->getControlWord(wordIndex) != 0 ) ) {
      return false;
    }
    
  }

  return true;

}

//------------------------------------------------------------------------------
// hasTristateOrUnknown: Does this register contain a TRISTATE or an UNKNOWN (X)?
//------------------------------------------------------------------------------

bool cReg::hasTristateOrUnknown() const {

  // If there are any control bits set then return true.

  for ( uint32 wordIndex = 0; wordIndex < getWordLength(); wordIndex++ ) {

    if ( getControlWord(wordIndex) != 0 ) {
      return true;
    }
    
  }

  return false;

}


//------------------------------------------------------------------------------
// operator= : Assign register to be equal to another register
//------------------------------------------------------------------------------

cReg &cReg::operator=( const cReg& value ) {
  set(msb, lsb, value);
  return *this;
}

//------------------------------------------------------------------------------
// operator= : Assign register to be equal to a char string
//------------------------------------------------------------------------------

cReg &cReg::operator=( const char *value ) {
  set(msb, lsb, value);
  return *this;
}

//------------------------------------------------------------------------------
// operator= : Assign register to be equal to a STL string
//------------------------------------------------------------------------------

cReg &cReg::operator=( const string& value ) {
  set(msb, lsb, value);
  return *this;
}

//------------------------------------------------------------------------------
// operator= : Assign register to be equal to a 8 bit integer
//------------------------------------------------------------------------------

cReg &cReg::operator=( const int8 value ) {
  set(value);
  return *this;
}

//------------------------------------------------------------------------------
// operator= : Assign register to be equal to a 16 bit integer
//------------------------------------------------------------------------------

cReg &cReg::operator=( const int16 value ) {
  set(value);
  return *this;
}

//------------------------------------------------------------------------------
// operator= : Assign register to be equal to a 32 bit integer
//------------------------------------------------------------------------------

cReg &cReg::operator=( const int32 value ) {
  set(value);
  return *this;
}

//------------------------------------------------------------------------------
// operator= : Assign register to be equal to a 32 bit integer
//------------------------------------------------------------------------------

cReg &cReg::operator=( const signed long value ) {
  set(value);
  return *this;
}

//------------------------------------------------------------------------------
// operator= : Assign register to be equal to a 64 bit integer
//------------------------------------------------------------------------------

cReg &cReg::operator=( const int64 value ) {
  set(value);
  return *this;
}

//------------------------------------------------------------------------------
// operator= : Assign register to be equal to a 8 bit unsigned integer
//------------------------------------------------------------------------------

cReg &cReg::operator=( const uint8 value ) {
  set(value);
  return *this;
}

//------------------------------------------------------------------------------
// operator= : Assign register to be equal to a 16 bit unsigned integer
//------------------------------------------------------------------------------

cReg &cReg::operator=( const uint16 value ) {
  set(value);
  return *this;
}

//------------------------------------------------------------------------------
// operator= : Assign register to be equal to a 32 bit unsigned integer
//------------------------------------------------------------------------------

cReg &cReg::operator=( const unsigned long value ) {
  set(value);
  return *this;
}

//------------------------------------------------------------------------------
// operator= : Assign register to be equal to a 32 bit unsigned integer
//------------------------------------------------------------------------------

cReg &cReg::operator=( const uint32 value ) {
  set(value);
  return *this;
}

//------------------------------------------------------------------------------
// operator= : Assign register to be equal to a 64 bit unsigned integer
//------------------------------------------------------------------------------

cReg &cReg::operator=( const uint64 value ) {
  set(value);
  return *this;
}

//------------------------------------------------------------------------------
// operator+= : ADD and Assign
//------------------------------------------------------------------------------

cReg &cReg::operator+=( const cReg &rhs ) {

  *this = add( *this, rhs, 0 );

  return *this;

}

//------------------------------------------------------------------------------
// operator-= : Subtract and Assign
//------------------------------------------------------------------------------

cReg &cReg::operator-=( const cReg &rhs ) {

  *this = operator-( *this, rhs );

  return *this;

}

//------------------------------------------------------------------------------
// operator&= : AND and Assign
//------------------------------------------------------------------------------

cReg &cReg::operator&=( const cReg &rhs ) {

  cReg result = bitwiseOperation( *this, rhs, cReg::AND );

  *this = result;

  return *this;

}

//------------------------------------------------------------------------------
// operator|= : OR and Assign
//------------------------------------------------------------------------------

cReg &cReg::operator|=( const cReg &rhs ) {

  cReg result = bitwiseOperation( *this, rhs, cReg::OR );

  *this = result;

  return *this;

}

//------------------------------------------------------------------------------
// operator^= : XOR and Assign
//------------------------------------------------------------------------------

cReg &cReg::operator^=( const cReg &rhs ) {

  cReg result = bitwiseOperation( *this, rhs, cReg::XOR );

  *this = result;

  return *this;

}

//------------------------------------------------------------------------------
// operator<<= : Left shift
//------------------------------------------------------------------------------

cReg &cReg::operator<<=( const uint32 shiftValue ) {

  shift(shiftValue, false);
  return *this;

}

//------------------------------------------------------------------------------
// operator>>= : Right shift
//------------------------------------------------------------------------------

cReg &cReg::operator>>=( const uint32 shiftValue ) {

  shift(shiftValue, true);
  return *this;

}

//------------------------------------------------------------------------------
// operator<<= : Concat operator
//------------------------------------------------------------------------------

cReg &cReg::operator<<=( const cReg& rhs ) {

  this->concat(rhs);
  return *this;

}

//------------------------------------------------------------------------------
// operator>>= : Concat operator
//------------------------------------------------------------------------------

cReg &cReg::operator>>=( const cReg& rhs ) {

  cReg reg = rhs;
  reg.concat(*this);
  this->resizeAndCopy(reg);

  return *this;

}

//------------------------------------------------------------------------------
// operator== : Is this register equal to another register.
//------------------------------------------------------------------------------

bool cReg::operator== ( const cReg &rhs ) {
  return( isTripleEqual(rhs) );
}

//-----------------------------------------------------------------------------
// operator!= : Is this register not equal to another register.
//------------------------------------------------------------------------------

bool cReg::operator!= ( const cReg &rhs ) {
  return( ! ( *this == rhs ) );
}

//------------------------------------------------------------------------------
// operator< : Is this register less than another register.
//------------------------------------------------------------------------------

cReg::FourState cReg::operator< ( const cReg &rhs ) {

  // If there are any Tristate (Z) or Unknown (X) then return UNKNOWN.

  if ( hasTristateOrUnknown() || rhs.hasTristateOrUnknown() ) {
    return UNKNOWN;
  }

  // At this point there are no Z or X's so it's a simple comparison

  bool equalFlag = true;

  if ( rhs.getWordLength() <= getWordLength() ) {

    // This register is larger than rhs register.

    for ( int wordIndex = getWordLength()-1; wordIndex >= 0; wordIndex-- ) {

      // Are we looking a word that is only
      // in this register not in rhs?

      if ( wordIndex >= static_cast<int>(rhs.getWordLength()) ) {

        if ( getDataWord(wordIndex) != 0 ) {
          return ZERO;
        }

      } else {

        if ( getDataWord(wordIndex) > rhs.getDataWord(wordIndex) ) {
          return ZERO;
        } else if ( getDataWord(wordIndex) != rhs.getDataWord(wordIndex) && equalFlag ) {
          equalFlag = false;
        }

      }

    }

    if ( equalFlag ) {
      return ZERO;
    } else {
      return ONE;
    }

  } else {

    // Rhs register is larger than this register.

    for ( int wordIndex = rhs.getWordLength()-1; wordIndex >= 0; wordIndex-- ) {

      // Are we looking a word that is only
      // in this register not in rhs?

      if ( wordIndex >= static_cast<int>(getWordLength()) ) {
        
        if ( rhs.getDataWord(wordIndex) != 0 ) {
          return ONE;
        }

      } else {

        if ( getDataWord(wordIndex) > rhs.getDataWord(wordIndex) ) {
          return ZERO;
        } else if ( getDataWord(wordIndex) != rhs.getDataWord(wordIndex) && equalFlag ) {
          equalFlag = false;
        }

      }

    }

    if ( equalFlag ) {
      return ZERO;
    } else {
      return ONE;
    }

  }

}

//------------------------------------------------------------------------------
// operator> : Is this register greater than another register.
//------------------------------------------------------------------------------

cReg::FourState cReg::operator> ( const cReg &rhs ) {

  FourState lt = ( *this <  rhs );
  bool      eq = ( *this == rhs );

  // If less-than (lt) is UNKNOWN then return UNKNOWN
  // If less-than or equal then return ZERO (false) else
  // ONE (true) which means greater-than.

  if ( lt == UNKNOWN ) {
    return UNKNOWN;
  } else if ( lt == ONE || eq == true ) {
    return ZERO;
  } else {
    return ONE;
  }

}

//------------------------------------------------------------------------------
// operator<= : Is this register less than or equal another register.
//------------------------------------------------------------------------------

cReg::FourState cReg::operator<= ( const cReg &rhs ) {

  FourState lt = ( *this <  rhs );
  bool      eq = ( *this == rhs );

  if ( lt == UNKNOWN ) {
    return UNKNOWN;
  } else if ( lt == ONE || eq ) {
    return ONE;
  } else {
    return ZERO;
  }

}

//------------------------------------------------------------------------------
// operator>= : Is this register less than or equal another register.
//------------------------------------------------------------------------------

cReg::FourState cReg::operator>= ( const cReg &rhs ) {

  FourState gt = ( *this >  rhs );
  bool      eq = ( *this == rhs );

  if ( gt == UNKNOWN ) {
    return UNKNOWN;
  } else if ( gt == ONE || eq ) {
    return ONE;
  } else {
    return ZERO;
  }

}

//------------------------------------------------------------------------------
// operator<< : Return ostream for class object cReg
//------------------------------------------------------------------------------

ostream &operator<< ( ostream &os, const cReg &rhs ) {
  return os << rhs.toHex();
}

//------------------------------------------------------------------------------
// operator<< : Return ostream for class object cReg
//------------------------------------------------------------------------------

ostream &operator<< ( ostream &os, crBitRangeRef &ref ) {
  return os << ref.getSlice();
}

//------------------------------------------------------------------------------
// operator~ : One's complement
//------------------------------------------------------------------------------

cReg operator~ ( const cReg &rhs ) {

  cReg comp( rhs.getMsb(), rhs.getLsb() );

   for ( uint32 wordIndex = 0; wordIndex < comp.getWordLength(); wordIndex++ ) {

     comp.setDataWord(wordIndex, rhs.getDataWord(wordIndex));
     comp.setControlWord(wordIndex, rhs.getControlWord(wordIndex));

     comp.setDataWord(wordIndex, ~(comp.getDataWord(wordIndex)));
     comp.setDataWord(wordIndex, ( comp.getDataWord(wordIndex) | comp.getControlWord(wordIndex) ));

   }

   return comp;

}

//------------------------------------------------------------------------------
// operator++ : (prefix form) Add one to this register
//------------------------------------------------------------------------------

cReg &cReg::operator++ () {

  cReg one(1);

  *this = add( *this, one, 0 );

  return *this;

}

//------------------------------------------------------------------------------
// operator++ : (postix form) Add one to this register
//------------------------------------------------------------------------------

cReg cReg::operator++ ( const int dummy ) {

  if(dummy) {} // Make compiler happy (eliminates unused warning)

  cReg one(1);
  cReg copy = *this;

  *this = add( *this, one, 0 );

  // Eliminate any padding of zeros in the copy.

  while ( copy.isZero(copy.getMsb()) && copy.getWidth() > 1 ) {
    copy.setMsb( copy.getMsb() - 1 );
  }

  return copy;

}

//------------------------------------------------------------------------------
// operator-- : (prefix form) Subtract one from this register
//------------------------------------------------------------------------------

cReg &cReg::operator-- () {

  cReg one(getMsb(), getLsb(), 1);

  *this = operator-( *this, one );

  return *this;

}

//------------------------------------------------------------------------------
// operator-- : (postix form) Subtract one from this register
//------------------------------------------------------------------------------

cReg cReg::operator-- ( const int dummy ) {

  if(dummy) {} // Make compiler happy (eliminates unused warning)

  cReg one(getMsb(), getLsb(), 1);
  cReg copy = *this;

  *this = operator-( *this, one );

  // Eliminate any padding of zeros in the copy.

  while ( copy.isZero(copy.getMsb()) && copy.getWidth() > 1 ) {
    copy.setMsb( copy.getMsb() - 1 );
  }

  return copy;

}

//------------------------------------------------------------------------------
// operator() : Return a specific bit range for a field
//------------------------------------------------------------------------------

crBitRangeRef cReg::operator() ( const string& name ) {
  return( getField(name) );
}

//------------------------------------------------------------------------------
// operator() : Return a specific bit range for a field
//------------------------------------------------------------------------------

int32 cReg::operator() ( const string& fieldName, const string& attrName ) {
  return( getFieldAttr(fieldName, attrName) );
}

//------------------------------------------------------------------------------
// operator[] : Return a specific bit range for a field
//------------------------------------------------------------------------------

crBitRangeRef cReg::operator[] ( const string& name ) {
  return( getField(name) );
}

//------------------------------------------------------------------------------
// operator() : Return a specific bit in the register
//------------------------------------------------------------------------------

crBitRangeRef cReg::operator() ( const uint32 index ) {
  return( crBitRangeRef( *this, index, index ) );
}

//------------------------------------------------------------------------------
// operator() : Return a bit range
//------------------------------------------------------------------------------

crBitRangeRef cReg::operator() ( const uint32 _msb, const uint32 _lsb ) {
  return ( crBitRangeRef( *this, _msb, _lsb ) );
}

//------------------------------------------------------------------------------
// operator[] : Return a reference to a bit in the register
//------------------------------------------------------------------------------

crBitRangeRef cReg::operator[] ( const uint32 index ) {
  return( crBitRangeRef( *this, index, index ) );
}

//------------------------------------------------------------------------------
// operator+ : Add two cReg (registers) together
//------------------------------------------------------------------------------

cReg operator+ ( const cReg &lhs, const cReg &rhs ) {
  return( add( lhs, rhs, 0 ) );
}

//------------------------------------------------------------------------------
// add : Add two cReg (registers) together
//
// For each word we perform the following steps:
//
// Step 1: First we need to zero out any data bits which have a control bit set
//         to one.
// Step 2: Add the two words including any carry
// Step 3: Calculate carry
// Step 4: Build a control mask for lhs and rhs.
// Step 5: OR the control words (For every control bit that's a one we make the
//         data bit a one). This ensures that adding any bit to an X or a Z bit
//         will result in all remaining bits set to X.
// Step 6: If either of the ctrl masks were not zero then we want to
//         make the masks all ones so that the next word will result in all X.
//------------------------------------------------------------------------------

cReg add ( const cReg &lhs, const cReg &rhs, uint32 carry ) {

  uint32 lhs_width    = lhs.getWidth();
  uint32 rhs_width    = rhs.getWidth();
  uint32 lhs_word_len = lhs.getWordLength();
  uint32 rhs_word_len = rhs.getWordLength();
  uint32 sumMSB       = ( lhs_width > rhs_width ) ? lhs_width : rhs_width;
  uint32 sumLSB       = 0;

  // New sum register

  cReg sum(sumMSB, sumLSB);

  // According to Verilog if either operand contains an X or Z then the entire
  // result should be unknown.

  if ( lhs.hasTristateOrUnknown() || rhs.hasTristateOrUnknown() ) {
    sum.initialize(cReg::UNKNOWN);
    return(sum);
  }

  // Go through each word of the new sum register

  for ( uint32 wordIndex = 0; wordIndex < sum.getWordLength(); wordIndex++ ) {

    uint64 lhs_d_word = ( wordIndex < lhs_word_len ) ? static_cast<uint64>(lhs.getDataWord(wordIndex)) : 0ULL;
    uint64 rhs_d_word = ( wordIndex < rhs_word_len ) ? static_cast<uint64>(rhs.getDataWord(wordIndex)) : 0ULL;

    // Add the two words including any carry

    uint64 s = lhs_d_word + rhs_d_word + static_cast<uint64>(carry); 

    // See if there was a carry by checking the 33'rd bit of the 64 bit integer

    sum.setDataWord(wordIndex, static_cast<uint32>( s ));

    carry = static_cast<uint32>(( s >> crWord::size ) & 0x1ULL );

  }

  // Eliminate any padding of zeros in the sum.

  sum.removeLeadingZeros();

  return sum;

}

//------------------------------------------------------------------------------
// bitwiseOperation : Bitwise AND, OR, or XOR two cReg (registers)
//------------------------------------------------------------------------------

cReg bitwiseOperation ( const cReg &lhs, const cReg &rhs, cReg::bwOperator op ) {

  uint32 lhs_width = lhs.getWidth();
  uint32 rhs_width = rhs.getWidth();

  // New register

  cReg result;

  // Make andResult the same size as the bigger register
  // but set it's results to be the same as the smaller
  // register. This way we AND two registers of equal
  // size

  const cReg *sm = &rhs;
  const cReg *lg = &lhs;
  uint32     msb = lhs_width - 1;

  if ( lhs_width <= rhs_width ) {

    sm  = &lhs;
    lg  = &rhs;
    msb = rhs_width - 1;

  }

  result.setMsb( msb );
  
  for ( uint32 i = 0; i < sm->getWordLength(); i++ ) {

    result.setDataWord(i, sm->getDataWord(i));
    result.setControlWord(i, sm->getControlWord(i));

  }

  // Perform bitwise operation on the two registers

  for ( uint32 i = 0; i < result.getWordLength(); i++ ) {

    uint32 a = result.getDataWord(i);
    uint32 b = result.getControlWord(i);
    uint32 c = lg->getDataWord(i);
    uint32 d = lg->getControlWord(i);
    uint32 new_d = a;
    uint32 new_c = b;

    switch ( op ) {

    case cReg::AND:
      new_c = (a & d) | (b & c) | (b & d);
      new_d = (a & c) | new_c;
      break;
    case cReg::OR:
      new_c = (b & d) | (b & ~c) | (~a & d);
      new_d = a | c | new_c;
      break;
    case cReg::XOR:
      new_c = b | d;
      new_d = (a ^ c) | new_c;
      break;
      
    }

    result.setControlWord(i, new_c);
    result.setDataWord(i, new_d);

  }

  return result;

}

//------------------------------------------------------------------------------
// operator- : Subtract two cReg (registers)
//
// Step 1: Perform a one's complement of rhs.
// Step 2: Add lhs + rhs + carry (where carry is 1)
//------------------------------------------------------------------------------

cReg operator- ( const cReg &lhs, const cReg &rhs ) {

  cReg tmp_rhs = rhs;
  if ( lhs.getWidth() > rhs.getWidth() ) {
    tmp_rhs.resize(lhs.getMsb(), lhs.getLsb());
    tmp_rhs = rhs;
  }

  cReg sub = add( lhs, ( ~tmp_rhs ), 1 );

  // Remove extra MSB bit if it was added during the add operation

  if ( sub.getWidth() > lhs.getWidth() && sub.getWidth() > tmp_rhs.getWidth() ) {
    sub.setMsb( sub.getMsb() - 1 );
  }

  // Eliminate any padding of zeros.

  while ( sub.isZero(sub.getMsb()) && sub.getWidth() > 1 ) {
    sub.setMsb( sub.getMsb() - 1 );
  }

  return sub;

}

//------------------------------------------------------------------------------
// operator& : AND two cReg (registers)
//------------------------------------------------------------------------------

cReg operator& ( const cReg &lhs, const cReg &rhs ) {
  return( bitwiseOperation( lhs, rhs, cReg::AND ) );
}

//------------------------------------------------------------------------------
// operator& : OR two cReg (registers)
//------------------------------------------------------------------------------

cReg operator| ( const cReg &lhs, const cReg &rhs ) {
  return( bitwiseOperation( lhs, rhs, cReg::OR ) );
}

//------------------------------------------------------------------------------
// operator^ : XOR two cReg (registers)
//------------------------------------------------------------------------------

cReg operator^ ( const cReg &lhs, const cReg &rhs ) {
  return( bitwiseOperation( lhs, rhs, cReg::XOR ) );
}

//------------------------------------------------------------------------------
// operator>> : Shift register by shiftValue and return the results
//------------------------------------------------------------------------------

cReg  operator>> ( const cReg &lhs, const uint32 shiftValue ) {

  cReg result = lhs;

  result >>= shiftValue;

  return result;

}

//------------------------------------------------------------------------------
// operator<< : Shift register by shiftValue and return the results
//------------------------------------------------------------------------------

cReg  operator<< ( const cReg &lhs, const uint32 shiftValue ) {

  cReg result = lhs;
  result <<= shiftValue;

  return result;

}

//------------------------------------------------------------------------------
// operator>> : Concat operator
//------------------------------------------------------------------------------

cReg operator>>( const cReg& lhs, const cReg& rhs ) {
  cReg result = lhs;
  result.concat(rhs);
  return result;
}

//------------------------------------------------------------------------------
// operator<< : Concat operator
//------------------------------------------------------------------------------

cReg operator<<( const cReg& lhs, const cReg& rhs ) {
  cReg result = lhs;
  result.concat(rhs);
  return result;
}

//------------------------------------------------------------------------------
// buildControlMask: Build a mask for the control word (used in addition)
//------------------------------------------------------------------------------

uint32 buildControlMask( uint32 word ) {

  uint32 ctrlMask = ~0;
  
  for ( uint32 bitIndex = 0; bitIndex < 32; bitIndex++ ) {

    // Shift word

    if ( bitIndex > 0 ) {
      word >>= 1;
    }

    // AND word with mask of 0x1 to isolate the first bit.
    // If the first bit is zero then we want to shift a
    // zero into the ctrlMask. We want to keep doing this until
    // we find a bit that is not zero.

    if ( ( word & 0x1 ) == 0 ) {
      ctrlMask <<= 1;
    } else {
      break;
    }
    
  }
  
  return( ctrlMask );

}

//------------------------------------------------------------------------------//
//               CLASS DEFINITIONS FOR crBitRangeRef                            //
//------------------------------------------------------------------------------//

// Get slice

cReg crBitRangeRef::getSlice() const { return(reg.getSlice(msb, lsb)); }

// Conversion to strings representing binary or hex

string crBitRangeRef::toBin ( bool addPrefix ) { return getSlice().toBin( addPrefix ); }
string crBitRangeRef::toHex ( bool addPrefix ) { return getSlice().toHex( addPrefix ); }

// Assignment Operators

void crBitRangeRef::operator= ( crBitRangeRef& ref   )      { reg.set(msb, lsb, ref.getSlice());}
void crBitRangeRef::operator= ( const cReg&    value )      { reg.set(msb, lsb, value); }
void crBitRangeRef::operator= ( const char     value )      { reg.set(lsb, value);      }
void crBitRangeRef::operator= ( const char*    value )      { reg.set(msb, lsb, value); }
void crBitRangeRef::operator= ( const string&  value )      { reg.set(msb, lsb, value); }
void crBitRangeRef::operator= ( const int8     value )      { reg.set(msb, lsb, value); }
void crBitRangeRef::operator= ( const int16    value )      { reg.set(msb, lsb, value); }
void crBitRangeRef::operator= ( const int32    value )      { reg.set(msb, lsb, value); }
void crBitRangeRef::operator= ( const signed long value )   { reg.set(msb, lsb, value); }
void crBitRangeRef::operator= ( const int64    value )      { reg.set(msb, lsb, value); }
void crBitRangeRef::operator= ( const uint8    value )      { reg.set(msb, lsb, value); }
void crBitRangeRef::operator= ( const uint16   value )      { reg.set(msb, lsb, value); }
void crBitRangeRef::operator= ( const uint32   value )      { reg.set(msb, lsb, value); }
void crBitRangeRef::operator= ( const unsigned long value ) { reg.set(msb, lsb, value); }
void crBitRangeRef::operator= ( const uint64   value )      { reg.set(msb, lsb, value); }

// Compare Operators

bool crBitRangeRef::operator== ( const cReg& rhs ) {
  return (getSlice() == rhs);
}

bool crBitRangeRef::operator!= ( const cReg& rhs ) {
  return (getSlice() != rhs);
}

cReg::FourState crBitRangeRef::operator<  ( const cReg& rhs ) {
  return (getSlice() < rhs);
}

cReg::FourState crBitRangeRef::operator>  ( const cReg& rhs ) {
  return (getSlice() > rhs);
}

cReg::FourState crBitRangeRef::operator<= ( const cReg& rhs ) {
  return (getSlice() <= rhs);
}

cReg::FourState crBitRangeRef::operator>= ( const cReg& rhs ) {
  return (getSlice() >= rhs);
}

// Symmetric math operators

cReg crBitRangeRef::operator++ ( const int dummy ) {

  if(dummy) {} // Make compiler happy (eliminates unused warning)

  cReg copy = getSlice();

  reg.set(msb,lsb, copy + 1);

  return ( copy );

}

cReg crBitRangeRef::operator-- ( const int dummy ) {

  if(dummy) {} // Make compiler happy (eliminates unused warning)

  cReg copy = getSlice();

  reg.set(msb,lsb, copy - 1);

  return ( copy );

}

crBitRangeRef& crBitRangeRef::operator++() {
  reg.set(msb, lsb, ++getSlice());
  return *this;
}

crBitRangeRef& crBitRangeRef::operator--() {
  reg.set(msb, lsb, --getSlice());
  return *this;
}

crBitRangeRef& crBitRangeRef::operator+=( const cReg& rhs ) {
  reg.set(msb, lsb, getSlice() + rhs);
  return *this;
}

crBitRangeRef& crBitRangeRef::operator-=( const cReg& rhs ) {
  reg.set(msb, lsb, getSlice() - rhs);
  return *this;
}

crBitRangeRef& crBitRangeRef::operator&=( const cReg& rhs ) {
  reg.set(msb, lsb, getSlice() & rhs);
  return *this;
}

crBitRangeRef& crBitRangeRef::operator|=( const cReg& rhs ) {
  reg.set(msb, lsb, getSlice() | rhs);
  return *this;
}

crBitRangeRef& crBitRangeRef::operator^=( const cReg& rhs ) {
  reg.set(msb, lsb, getSlice() ^ rhs);
  return *this;
}

crBitRangeRef& crBitRangeRef::operator<<=( const int value ) {
  reg.set(msb, lsb, getSlice() << value);
  return *this;
}

crBitRangeRef& crBitRangeRef::operator>>=( const int value ) {
  reg.set(msb, lsb, getSlice() >> value);
  return *this;
}

//------------------------------------------------------------------------------//
//               Non-member operator functions for crBitRangeRef                //
//------------------------------------------------------------------------------//

// Math operators

//------------------------------------------------------------------------------
// operator~ : One's complement
//------------------------------------------------------------------------------

cReg operator~ ( const crBitRangeRef& rhs ) {
  return( ~(rhs.getSlice()) );
}

//------------------------------------------------------------------------------
// operator+ : Addition
//------------------------------------------------------------------------------

cReg  operator+ ( const crBitRangeRef& lhs, const crBitRangeRef& rhs ) {
  return( lhs.getSlice() + rhs.getSlice() );
}

//------------------------------------------------------------------------------
// operator- : Subtraction
//------------------------------------------------------------------------------

cReg  operator- ( const crBitRangeRef& lhs, const crBitRangeRef& rhs ) {
  return( lhs.getSlice() - rhs.getSlice() );
}

//------------------------------------------------------------------------------
// operator& : AND
//------------------------------------------------------------------------------

cReg  operator& ( const crBitRangeRef& lhs, const crBitRangeRef& rhs ) {
  return( lhs.getSlice() & rhs.getSlice() );
}

//------------------------------------------------------------------------------
// operator| : OR
//------------------------------------------------------------------------------

cReg  operator| ( const crBitRangeRef& lhs, const crBitRangeRef& rhs ) {
  return( lhs.getSlice() | rhs.getSlice() );
}

//------------------------------------------------------------------------------
// operator^ : XOR
//------------------------------------------------------------------------------

cReg  operator^ ( const crBitRangeRef& lhs, const crBitRangeRef& rhs ) {
  return( lhs.getSlice() ^ rhs.getSlice() );
}

//------------------------------------------------------------------------------
// operator>> : Right Shift
//------------------------------------------------------------------------------

cReg  operator>> ( const crBitRangeRef& lhs, const int value ) {
  return( lhs.getSlice() >> value );
}

//------------------------------------------------------------------------------
// operator<< : Left Shift
//------------------------------------------------------------------------------

cReg  operator<< ( const crBitRangeRef& lhs, const int value ) {
  return( lhs.getSlice() << value );
}
