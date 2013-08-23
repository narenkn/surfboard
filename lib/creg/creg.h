//******************************************************************************
//
// $RCSfile: creg.h,v $
//
// $Revision: 1.39 $
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
// This is the class header for cReg. This class represents a hardware register.
// This class has many easy to use access methods to set and get information.
// Many of the operators have been overriden to help make this class flexable.
//******************************************************************************

#ifndef __CREG__
#define __CREG__

// Local Includes

#include "creg_attribute.h"
#include "creg_field.h"

#ifndef NO_ENV_DKI
#include "env_dki.h"
#else

#include "creg_msg.h"

// Integer data typedefs

typedef signed   char           int8;
typedef signed   short          int16;
typedef signed   int            int32;
typedef signed   long long      int64;

typedef unsigned char           uint8;
typedef unsigned short          uint16;
typedef unsigned int            uint32;
typedef unsigned long long      uint64;

#endif

// Includes

#include <cmath>
#include <stdio.h>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <ctype.h>
#include <typeinfo>

using std::pair;
using std::map;
using std::vector;
using std::string;
using std::ostream;
using std::ostringstream;
using std::cout;
using std::endl;
using std::hex;
using std::sort;
using std::type_info;
using std::bad_cast;

// Prototype

class crBitRangeRef; // cReg Bit Range Referance Class

//------------------------------------------------------------------------------
// class crWord
//------------------------------------------------------------------------------

class crWord {

public:

  static const unsigned int size       = 8 * sizeof(unsigned int);
  static const unsigned int doubleSize = 2 * size;
  static const unsigned int msb        = size - 1;
  static const unsigned int doubleMsb  = doubleSize - 1;
  static const unsigned int mask       = ~0;

  unsigned int d;
  unsigned int c;

};

typedef vector<crWord> crWordVector;

//------------------------------------------------------------------------------
// class cReg
//------------------------------------------------------------------------------

class cReg {

public:

  // FourState Enumeration

  typedef enum {ZERO=0, ONE=1, TRISTATE=2, UNKNOWN=3} FourState;
  typedef enum {DATA=0, CONTROL=1}                    DataControl;
  typedef enum {AND, OR, XOR}                         bwOperator;

protected:

  friend class crBitRangeRef;

  //-------------------//
  // Protected Members //
  //-------------------//

  crWordVector     wordVector;      // Vector of crWord's
  uint32           msb;             // most significant bit
  uint32           lsb;             // least significant bit
  crFieldMap*      fieldMap;        // Field Map Ptr
  crAttributeMap*  attributeMap;    // Attribute Map Ptr

  //-------------------//
  // Protected Methods //
  //-------------------//

  // Initialize Map Pointers

  void   initializePointers() {

    // Initialize field and attribute Maps
    
    fieldMap     = NULL; // Field Map Ptr
    attributeMap = NULL; // Attribute Map Ptr

  }

  // Copy

  void   copy( const cReg &reg );

  // Parse Verilog style string

  void   parseVerilogInitialStr( const string& _str, uint32& width, string& result );
  void   setUsingBinaryString  ( const uint32 _msb, const uint32 _lsb, const string& binStr );

  // Index related methods


  bool   checkIndex     ( const uint32 index, const string& msgPrefix = "" ) const;
  bool   isIndexValid   ( const uint32 index ) const;
  uint32 getActualIndex ( const uint32 index ) const;
  uint32 getWordIndex   ( const uint32 index ) const;
  uint32 getBitIndex    ( const uint32 index ) const;
  uint32 getIndex       ( const uint32 idx, const uint32 wordIdx ) const;

  // State related methods

  bool            isState  ( const uint32 index, const FourState state ) const;
  bool            isState  ( const uint32 _msb, const uint32 _lsb, const FourState state ) const;

  char      getState ( const FourState state ) const;
  FourState getState ( const char      state ) const;
  FourState getState ( const string   &state ) const;
  FourState getState ( const uint32    state ) const;
  FourState getState ( const uint32 d, const uint32 c ) const;

  // Bit related methods

  uint32 getDataBit     ( const uint32 index ) const;
  uint32 getControlBit  ( const uint32 index ) const;
  uint32 getIntegerBit  ( const uint32 data, const uint32 index ) const;
  uint64 getIntegerBit  ( const uint64 data, const uint32 index ) const;

  // Getting Data and Control

  uint32 getDataWord    ( const uint32 index ) const;
  uint32 getControlWord ( const uint32 index ) const;

  uint32 getData        ( const string    &state ) const;
  uint32 getData        ( const char       state ) const;
  uint32 getData        ( const FourState  state ) const;
  uint32 getData        ( const uint32     state ) const;

  uint32 getControl     ( const string    &state ) const;
  uint32 getControl     ( const char       state ) const;
  uint32 getControl     ( const FourState  state ) const;
  uint32 getControl     ( const uint32     state ) const;

  // Setting Data and Control

  void setDataWord    ( const uint32 index, uint32 word );
  void setControlWord ( const uint32 index, uint32 word );

  void setDataBit     ( const uint32 index, const uint32 value );
  void setControlBit  ( const uint32 index, const uint32 value );
  void setIntegerBit  ( uint32 &data, const uint32 index, const uint32 value ) const;
  void setIntegerBit  ( uint64 &data, const uint32 index, const uint32 value ) const;

  // Misc methods

  void   initialize            ( uint32 d, uint32 c );
  void   reserve               ();
  void   removeLeadingZeros    ();
  void   removeLeadingUnknowns ();
  void   mallocFieldMap        ();
  void   mallocAttributeMap    ();
  uint64 reverse64             ( register uint64 val );

  // Field & Attribute specific methods

  crFieldMapIter     findField     ( const string& fieldName );
  crAttributeMapIter findAttribute ( const string& attrName );

  void copyMaps        ( crFieldMap* srcFieldMap, crAttributeMap* srcAttributeMap );
  void copyFieldMap    ( crFieldMap* src );
  void copyAttributeMap( crAttributeMap* src );

  // Math

  void shift ( const uint32 shiftValue, bool shiftRightFlag );

public:

  // Constructors

  cReg();
  cReg( const cReg   &reg   );
  cReg( const char   *value );
  cReg( const string &value );
  cReg( const char   value  );
  cReg( const int8   value  );
  cReg( const int16  value  );
  cReg( const int32  value  );
  cReg( const signed long value );
  cReg( const int64  value  );
  cReg( const uint8  value  );
  cReg( const uint16 value  );
  cReg( const uint32 value  );
  cReg( const unsigned long value );
  cReg( const uint64 value  );


#ifndef NO_ENV_DKI
explicit cReg( cDesignVar &reg );
#endif

  cReg( const uint32 _msb, const uint32 _lsb );
  cReg( const uint32 _msb, const uint32 _lsb, const cReg   &reg   );
  cReg( const uint32 _msb, const uint32 _lsb, const char   *value );
  cReg( const uint32 _msb, const uint32 _lsb, const string &value );
  cReg( const uint32 _msb, const uint32 _lsb, const int8   value  );
  cReg( const uint32 _msb, const uint32 _lsb, const int16  value  );
  cReg( const uint32 _msb, const uint32 _lsb, const signed long  value  );
  cReg( const uint32 _msb, const uint32 _lsb, const int32  value  );
  cReg( const uint32 _msb, const uint32 _lsb, const int64  value  );
  cReg( const uint32 _msb, const uint32 _lsb, const uint8  value  );
  cReg( const uint32 _msb, const uint32 _lsb, const uint16 value  );
  cReg( const uint32 _msb, const uint32 _lsb, const uint32 value  );
  cReg( const uint32 _msb, const uint32 _lsb, const unsigned long value  );
  cReg( const uint32 _msb, const uint32 _lsb, const uint64 value  );

  // Destructor

  ~cReg();

  //----------------//
  // Static Methods //
  //----------------//

  static string hex     ( const string& str );
  static void   copy    ( const cReg& srcReg, cReg& destReg );
  static string toLower ( const string& name );
  static string toUpper ( const string& name );

  //---------------//
  // Field Methods //
  //---------------//

  // New/Get Fields

  void           newField  ( const string& name, const uint32 _msb, const uint32 _lsb );
  crBitRangeRef  getField  ( const string& name );
  crFieldList    getFields (); // Return all fields.
  bool           hasField  ( const string& fieldName );

  // newFieldAttributeTyped

  template <class T>
  void newFieldAttributeTyped ( const string& fieldName, const string& attrName, const T& attr ) {

    if ( ! hasField(fieldName) ) {
      FATAL_ERROR("cReg::newFieldAttributeTyped: Field '%s' does not exist.", fieldName.c_str());
    }

    crFieldMapIter i     = findField(fieldName);
    cRegField*     field = i->second;

    field->template newAttributeTyped<T>(attrName, attr);

  }

  // getFieldAttributeTyped

  template <class T>
  T& getFieldAttributeTyped ( const string& fieldName, const string& attrName ) {

    if ( ! hasField(fieldName) ) {
      FATAL_ERROR("cReg::getFieldAttributeTyped: Field '%s' does not exist.", fieldName.c_str());
    }

    crFieldMapIter i     = findField(fieldName);
    cRegField*     field = i->second;

    return( field->template getAttributeTyped<T>(attrName) );

  }

  // New/Get Field Attributes (kept for backwards compatability)

  void   newFieldAttr ( const string& fieldName, const string& attrName, const int32& attr );
  int32& getFieldAttr ( const string& fieldName, const string& attrName );
  uint32 getFieldMsb  ( const string& fieldName );
  uint32 getFieldLsb  ( const string& fieldName );


  //-------------------//
  // Attribute Methods //
  //-------------------//

  // New/Get Fields

  void             newAttribute  ( const string& attrName, const string& attrValue );  // Create new string attribute
  bool             hasAttribute  ( const string& attrName );                           // Does an attribute exist?
  string           getAttribute  ( const string& attrName );                           // Get attribute value
  crAttributeList  getAttributes ();                                                   // Return all attributes.
  
  // newAttribute

  template <class T>
  void newAttributeTyped ( const string& attrName, const T& attr ) {
    
    mallocAttributeMap();
    attributeMap->operator[](attrName) = new cRegAttribute<T>(attr);

  }

  // getAttribute

  template <class T>
  T& getAttributeTyped ( const string& attrName ) {

    crAttributeMapIter i = findAttribute(attrName);
    cRegAttributeBase* b = i->second;
    cRegAttribute<T>*  a = dynamic_cast<cRegAttribute<T>*>(b);
    
    if ( a == NULL ) {
      FATAL_ERROR("getAttributeTyped: Unable to cast value for attribute '%s', value has a different type.", attrName.c_str());
    }
    
    return( a->attribute );
    
  }

  //-------------//
  // Get Methods //
  //-------------//

  uint32 getMsb()        const;
  uint32 getLsb()        const;
  uint32 getWidth()      const;
  uint32 getWordLength() const;

  // Get the state of a bit

  FourState getBit( const uint32 index ) const;

  // Get integer values

  uint32 getInt32(DataControl dc=DATA) const; // Return a 32 bit integer (first word) - DATA or CONTROL
  uint64 getInt64(DataControl dc=DATA) const; // Return a 64 bit integer (first and second words) - DATA or CONTROL

  uint32 getInt32( const uint32 _msb, const uint32 _lsb, DataControl dc=DATA ) const; // Return a 32 bit integer for specified range - DATA or CONTROL
  uint64 getInt64( const uint32 _msb, const uint32 _lsb, DataControl dc=DATA ) const; // Return a 64 bit integer for specified range - DATA or CONTROL

  // Get a slice

  cReg getSlice ( const uint32 index );                    // Return new single bit register slice
  cReg getSlice ( const uint32 _msb, const uint32 _lsb );  // Return new register slice > one bit

  //--------------------//
  // Initialize Methods //
  //--------------------//

  // Initialize all bits in the register to a given state

  void initialize( const char       state );
  void initialize( const string    &state );
  void initialize( const int32      state );
  void initialize( const uint32     state );
  void initialize( const FourState  state );

  // Resize & Copy

  void resize       ( const uint32 _msb, const uint32 _lsb );
  void resizeAndCopy( const cReg& srcReg );

  // Concat

  void concat( const cReg& srcReg );

  //-------------//
  // Set Methods //
  //-------------//

  // Set Msb and Lsb

  void setRange( const uint32 _msb, const uint32 _lsb );
  void setMsb( const uint32 _msb );
  void setLsb( const uint32 _lsb );

  // Set the register to a string of 4-state values

  void set( const char*   value );
  void set( const string& value );

  // Set the register to a 64 bit integer.

  void set( const int8   value );
  void set( const int16  value );
  void set( const int32  value );
  void set( const signed long  value );
  void set( const int64  value );
  void set( const uint8  value );
  void set( const uint16 value );
  void set( const uint32 value );
  void set( const unsigned long value );
  void set( const uint64 value );

  // Set a single bit in the register

  void set( const uint32 index, const FourState     value ); // Bit value is (FourState)
  void set( const uint32 index, const string&       value ); // Bit value is (string)
  void set( const uint32 index, const char          value ); // Bit value is (char)
  void set( const uint32 index, const int32         value ); // Bit value is (int : non-negative only)
  void set( const uint32 index, const uint32        value ); // Bit value is (unsigned int)
  void set( const uint32 index, const signed long   value ); // Bit value is (int : non-negative only)
  void set( const uint32 index, const unsigned long value ); // Bit value is (unsigned int)

  // Set a range of bits in the register

  void set( const uint32 _msb, const uint32 _lsb, const cReg    &reg  );         // Value is (cReg)
  void set( const uint32 _msb, const uint32 _lsb, const char*   value );         // Value is (char*)
  void set( const uint32 _msb, const uint32 _lsb, const string &value );         // Value is (string)
  void set( const uint32 _msb, const uint32 _lsb, const int8    value );         // Value is (8  bit int)
  void set( const uint32 _msb, const uint32 _lsb, const int16   value );         // Value is (16 bit int)
  void set( const uint32 _msb, const uint32 _lsb, const int32   value );         // Value is (32 bit int)
  void set( const uint32 _msb, const uint32 _lsb, const signed long value );     // Value is (32 bit int)
  void set( const uint32 _msb, const uint32 _lsb, const int64   value );         // Value is (64 bit int)
  void set( const uint32 _msb, const uint32 _lsb, const uint8   value );         // Value is (8  bit unsigned int)
  void set( const uint32 _msb, const uint32 _lsb, const uint16  value );         // Value is (16 bit unsigned int)
  void set( const uint32 _msb, const uint32 _lsb, const uint32  value );         // Value is (32 bit unsigned int)
  void set( const uint32 _msb, const uint32 _lsb, const unsigned long  value );  // Value is (32 bit unsigned int)
  void set( const uint32 _msb, const uint32 _lsb, const uint64  value );         // Value is (64 bit unsigned int)

  //------------//
  // Is Methods //
  //------------//

  bool isTripleEqual( const cReg &rhs ) const;            // Are two registers equal bit for bit (Z == Z and X == X)
  bool isTripleEqual( const cReg &rhs, string& result );  // Are two registers equal bit for bit (Z == Z and X == X)

  // Zero

  bool isAllZero() const;                                        // Is the whole register zero
  bool isZero   ( const uint32 index ) const;                    // Is a single bit zero
  bool isZero   ( const uint32 _msb, const uint32 _lsb) const;   // Is a range of bits zero

  // One

  bool isAllOne() const;                                         // Is the whole register one
  bool isOne   ( const uint32 index ) const;                     // Is a single bit one
  bool isOne   ( const uint32 _msb, const uint32 _lsb ) const;   // Is a range of bits one

  // Tristate

  bool isTristate() const;                                       // Is the whole register tristate
  bool isTristate( const uint32 index ) const;                   // Is a single bit tristate
  bool isTristate( const uint32 _msb, const uint32 _lsb) const;  // Is a range of bits tristate

  // Unknown

  bool isUnknown() const;                                        // Is the whole register unknown
  bool isUnknown( const uint32 index ) const;                    // Is a single bit unknown
  bool isUnknown( const uint32 _msb, const uint32 _lsb ) const;  // Is a range of bits unknown

  //-------------//
  // Has Methods //
  //-------------//

  bool hasZero() const;                          // Register contains one or more bits that are Zero     (0)
  bool hasOne() const;                           // Register contains one or more bits that are One      (1)
  bool hasTristate() const;                      // Register contains one or more bits that are Tristate (Z)
  bool hasUnknown() const;                       // Register contains one or more bits that are Unknown  (X)
  bool hasTristateOrUnknown() const;             // Register contains one or more bits that are Z or X

  //----------------//
  // Output Formats //
  //----------------//

  string toBin ( bool addPrefix = true ) const;  // Convert to string representing a binary number
  string toHex ( bool addPrefix = true ) const;  // Convert to string representing a hexidecimal number

  //------------------//
  // Push/Pop Methods //
  //------------------//

  void pushMsb ( const uint32 value );           // Push bit value onto MSB (i.e. reg grows by one bit w/new value being MSB)
  void pushMsb ( const char value );             // Push bit value onto MSB (i.e. reg grows by one bit w/new value being MSB)
  void pushMsb ( const string &value );          // Push bit value onto MSB (i.e. reg grows by one bit w/new value being MSB)
  void pushMsb ( const FourState value );        // Push bit value onto MSB (i.e. reg grows by one bit w/new value being MSB)
  void pushLsb ( const uint32 value );           // Push bit value onto LSB (i.e. reg grows by one bit w/new value being LSB)
  void pushLsb ( const char value );             // Push bit value onto LSB (i.e. reg grows by one bit w/new value being LSB)
  void pushLsb ( const string &value );          // Push bit value onto LSB (i.e. reg grows by one bit w/new value being LSB)
  void pushLsb ( const FourState value );        // Push bit value onto LSB (i.e. reg grows by one bit w/new value being LSB)

  FourState popMsb();                      // Pop MSB bit (i.e. reg width decrease by one bit)
  FourState popLsb();                      // Pop LSB bit (i.e. reg width decrease by one bit)

  //---------------------------//
  // Member operator functions //
  //---------------------------//

  // NOTE: The following operator conflicts with the == operator
  // operator unsigned int () { return(getInt32()); }

  // Assignment Operator

  cReg &operator= ( const cReg&   value );       // Assignment to another register
  cReg &operator= ( const char*   value );       // Assignment to a char string
  cReg &operator= ( const string& value );       // Assignment to a STL string
  cReg &operator= ( const int8    value );       // Assignment to a  8-bit integer
  cReg &operator= ( const int16   value );       // Assignment to a 16-bit integer
  cReg &operator= ( const int32   value );       // Assignment to a 32-bit integer
  cReg &operator= ( const signed long value );   // Assignment to a 32-bit integer
  cReg &operator= ( const int64   value );       // Assignment to a 64-bit integer
  cReg &operator= ( const uint8   value );       // Assignment to a  8-bit unsigned integer
  cReg &operator= ( const uint16  value );       // Assignment to a 16-bit unsigned integer
  cReg &operator= ( const uint32  value );       // Assignment to a 32-bit unsigned integer
  cReg &operator= ( const unsigned long value ); // Assignment to a 32-bit unsigned integer
  cReg &operator= ( const uint64  value );       // Assignment to a 64-bit unsigned integer
 
  // Symmetric Math operators

  cReg &operator++  ();                          // Increment by one (prefix)
  cReg &operator--  ();                          // Decrement by one (prefix)
  cReg  operator++  ( const int dummy );         // Increment by one (postfix)
  cReg  operator--  ( const int dummy );         // Decrement by one (postfix)
  cReg &operator+=  ( const cReg &rhs );         // Addition and Assign
  cReg &operator-=  ( const cReg &rhs );         // Subtraction and Assign
  cReg &operator&=  ( const cReg &rhs );         // AND and Assign
  cReg &operator|=  ( const cReg &rhs );         // OR and Assign
  cReg &operator^=  ( const cReg &rhs );         // OR and Assign
  cReg &operator<<= ( const uint32 shiftValue ); // Left Shift and Assign
  cReg &operator>>= ( const uint32 shiftValue ); // Right Shift and Assign
  cReg &operator<<= ( const cReg &rhs );         // Concat (onto lsb, left of register)  Operator
  cReg &operator>>= ( const cReg &rhs );         // Concat (onto msb, right of register) Operator

  // Logic operators

  bool        operator== ( const cReg &rhs ); // Equal
  bool        operator!= ( const cReg &rhs ); // Not Equal
  FourState   operator<  ( const cReg &rhs ); // Less than
  FourState   operator>  ( const cReg &rhs ); // Greater than
  FourState   operator<= ( const cReg &rhs ); // Less than and equal
  FourState   operator>= ( const cReg &rhs ); // Greater than and equal
  
  // Math Operators

  friend cReg add              ( const cReg &lhs, const cReg &rhs, uint32 carry );        // Addition with carry
  friend cReg bitwiseOperation ( const cReg &lhs, const cReg &rhs, cReg::bwOperator op ); // Bitwise operation

  friend cReg operator~  ( const cReg &rhs );                  // Complement
  friend cReg operator+  ( const cReg &lhs, const cReg &rhs ); // Addition
  friend cReg operator-  ( const cReg &lhs, const cReg &rhs ); // Subtraction
  friend cReg operator&  ( const cReg &lhs, const cReg &rhs ); // AND
  friend cReg operator|  ( const cReg &lhs, const cReg &rhs ); // OR
  friend cReg operator^  ( const cReg &lhs, const cReg &rhs ); // XOR

  // Access operators

  crBitRangeRef operator() ( const uint32 index );                    // return bit range of one bit
  crBitRangeRef operator() ( const uint32 _msb, const uint32 _lsb);   // return bit range
  crBitRangeRef operator[] ( const uint32 index );                    // return bit range of one bit
  crBitRangeRef operator() ( const string& name );                    // return bit range for a field
  crBitRangeRef operator[] ( const string& name );                    // return bit range for a field

  int32         operator() ( const string& fieldName, const string& attributeName ); // return field attribute for a field


};

//-----------------------------------------------//
// Non-member utility functions for cReg //
//-----------------------------------------------//

uint32 buildControlMask ( uint32 word );
cReg   add              ( const cReg &lhs, const cReg &rhs, uint32 carry );        // Addition with carry
cReg   bitwiseOperation ( const cReg &lhs, const cReg &rhs, cReg::bwOperator op ); // Bitwise operation

//----------------------------------------//
// Non-member operator functions for cReg //
//----------------------------------------//

// Math operators

cReg  operator~  ( const cReg &rhs );                          // Complement
cReg  operator+  ( const cReg &lhs, const cReg &rhs );         // Addition
cReg  operator-  ( const cReg &lhs, const cReg &rhs );         // Subtraction
cReg  operator&  ( const cReg &lhs, const cReg &rhs );         // AND
cReg  operator|  ( const cReg &lhs, const cReg &rhs );         // OR
cReg  operator^  ( const cReg &lhs, const cReg &rhs );         // XOR
cReg  operator>> ( const cReg &lhs, const uint32 shiftValue ); // Right Shift
cReg  operator<< ( const cReg &lhs, const uint32 shiftValue ); // Left Shift

// Concat operators

cReg operator>> ( const cReg& lhs, const cReg& rhs ); // Concat this register onto the rhs register
cReg operator<< ( const cReg& lhs, const cReg& rhs ); // Concat the rhs register onto this register

// Output stream operator

ostream &operator<< ( ostream &os, const cReg &rhs );

//------------------------------------------------------------------------------
// class crBitRangeRef
//------------------------------------------------------------------------------

class crBitRangeRef {

  friend class cReg;

  cReg    &reg;
  uint32  msb;
  uint32  lsb;

public:

  // Constructors

  crBitRangeRef( cReg& reg, uint32 msb, uint32 lsb ) : reg( reg ), msb( msb ), lsb( lsb ) {}
  crBitRangeRef( const crBitRangeRef& ref ) : reg( ref.reg ), msb( ref.msb ), lsb( ref.lsb ) {}
  crBitRangeRef();

  // Get Slice

  cReg getSlice() const;

  // Conversion to strings representing binary or hex

  string toBin ( bool addPrefix = true );
  string toHex ( bool addPrefix = true );

  // Conversion to integers

  uint32 getInt32() { return(reg.getInt32(msb,lsb)); }
  uint64 getInt64() { return(reg.getInt64(msb,lsb)); }

  // Is Operators

  bool isZero()     { return(reg.isZero(msb,lsb)); }
  bool isOne()      { return(reg.isOne(msb,lsb)); }
  bool isTristate() { return(reg.isTristate(msb,lsb)); }
  bool isUnknown()  { return(reg.isUnknown(msb,lsb)); }

  // Has Operators

  bool hasZero()     { return(getSlice().hasZero()); }
  bool hasOne()      { return(getSlice().hasOne()); }
  bool hasTristate() { return(getSlice().hasTristate()); }
  bool hasUnknown()  { return(getSlice().hasUnknown()); }

  //----------------------//
  // Overloaded operators //
  //----------------------//

  // Casting Operator

  operator const cReg () {
    return(reg.getSlice(msb,lsb));
  }

  // Assignement Operators

  void operator= ( crBitRangeRef&       ref   );
  void operator= ( const cReg&          value );
  void operator= ( const char           value );
  void operator= ( const char*          value );
  void operator= ( const string&        value );
  void operator= ( const int8           value );
  void operator= ( const int16          value );
  void operator= ( const int32          value );
  void operator= ( const signed long    value );
  void operator= ( const int64          value );
  void operator= ( const uint8          value );
  void operator= ( const uint16         value );
  void operator= ( const uint32         value );
  void operator= ( const unsigned long  value );
  void operator= ( const uint64         value );

  // Compare Operators

  bool            operator== ( const cReg& rhs );
  bool            operator!= ( const cReg& rhs );
  cReg::FourState operator<  ( const cReg& rhs );
  cReg::FourState operator>  ( const cReg& rhs );
  cReg::FourState operator<= ( const cReg& rhs );
  cReg::FourState operator>= ( const cReg& rhs );

  // Symmetric math operators

  cReg           operator++  ( const int dummy );    // Increment by one (postfix)
  cReg           operator--  ( const int dummy );    // Decrement by one (postfix)
  crBitRangeRef& operator++  ();                     // Increment by one (prefix)
  crBitRangeRef& operator--  ();                     // Decrement by one (prefix)
  crBitRangeRef& operator+=  ( const cReg&  rhs   ); // Addition and Assign
  crBitRangeRef& operator-=  ( const cReg&  rhs   ); // Subtraction and Assign
  crBitRangeRef& operator&=  ( const cReg&  rhs   ); // AND and Assign
  crBitRangeRef& operator|=  ( const cReg&  rhs   ); // OR and Assign
  crBitRangeRef& operator^=  ( const cReg&  rhs   ); // OR and Assign
  crBitRangeRef& operator<<= ( const int    value ); // Left Shift and Assign
  crBitRangeRef& operator>>= ( const int    value ); // Right Shift and Assign

  // Math operators

  friend cReg operator~  ( const crBitRangeRef& rhs );                           // Complement
  friend cReg operator+  ( const crBitRangeRef& lhs, const crBitRangeRef& rhs ); // Addition
  friend cReg operator-  ( const crBitRangeRef& lhs, const crBitRangeRef& rhs ); // Subtraction
  friend cReg operator&  ( const crBitRangeRef& lhs, const crBitRangeRef& rhs ); // AND
  friend cReg operator|  ( const crBitRangeRef& lhs, const crBitRangeRef& rhs ); // OR
  friend cReg operator^  ( const crBitRangeRef& lhs, const crBitRangeRef& rhs ); // XOR

};

//-------------------------------------------------//
// Non-member operator functions for crBitRangeRef //
//-------------------------------------------------//

// Math operators

cReg  operator~  ( const crBitRangeRef& rhs );                           // Complement
cReg  operator+  ( const crBitRangeRef& lhs, const crBitRangeRef& rhs ); // Addition
cReg  operator-  ( const crBitRangeRef& lhs, const crBitRangeRef& rhs ); // Subtraction
cReg  operator&  ( const crBitRangeRef& lhs, const crBitRangeRef& rhs ); // AND
cReg  operator|  ( const crBitRangeRef& lhs, const crBitRangeRef& rhs ); // OR
cReg  operator^  ( const crBitRangeRef& lhs, const crBitRangeRef& rhs ); // XOR
cReg  operator>> ( const crBitRangeRef& lhs, const int value );          // Right Shift
cReg  operator<< ( const crBitRangeRef& lhs, const int value );          // Left Shift

// Ostream operator

ostream &operator<< ( ostream &os, crBitRangeRef &ref );

//------------------------------------------------------------------------------
// Template class cReg_t (inherits cReg)
//------------------------------------------------------------------------------

template<int _msb, int _lsb> class cReg_t : public cReg {

public:

  cReg_t()                            : cReg( _msb, _lsb)         {}
  cReg_t( const int8    value )       : cReg( _msb, _lsb, value ) {}
  cReg_t( const int16   value )       : cReg( _msb, _lsb, value ) {}
  cReg_t( const int32   value )       : cReg( _msb, _lsb, value ) {}
  cReg_t( const signed long value )   : cReg( _msb, _lsb, value ) {}
  cReg_t( const int64   value )       : cReg( _msb, _lsb, value ) {}
  cReg_t( const uint8   value )       : cReg( _msb, _lsb, value ) {}
  cReg_t( const uint16  value )       : cReg( _msb, _lsb, value ) {}
  cReg_t( const uint32  value )       : cReg( _msb, _lsb, value ) {}
  cReg_t( const unsigned long value ) : cReg( _msb, _lsb, value ) {}
  cReg_t( const uint64  value )       : cReg( _msb, _lsb, value ) {}
  cReg_t( const string& value )       : cReg( _msb, _lsb, value ) {}
  cReg_t( const char*   value )       : cReg( _msb, _lsb, value ) {}
  cReg_t( const cReg&   value )       : cReg( _msb, _lsb, value ) {}

};

#endif // __CREG__
