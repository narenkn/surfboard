//******************************************************************************
//
// $RCSfile: creg_space.h,v $
//
// $Revision: 1.18 $
// $Date: 2009/02/26 03:27:21 $
// $Author: grpierce $
// $State: Exp $
// $Locker:  $
//
// Copyright (c) 2007 by Advanced Micro Devices, Inc.
//
// This file is protected by Federal Copyright Law, with all rights
// reserved. No part of this file may be reproduced, stored in a
// retrieval system, translated, transcribed, or transmitted, in any
// form, or by any means manual, electric, electronic, mechanical,
// electro-magnetic, chemical, optical, or otherwise, without prior
// explicit written permission from Advanced Micro Devices, Inc.
//
//******************************************************************************
//
//******************************************************************************

#ifndef __CREG_SPACE__
#define __CREG_SPACE__

// Local Includes

#include "creg.h"

// Includes

#include <map>
#include <sstream>
#include <algorithm>

using std::map;
using std::ostringstream;

// Class Prototypes

class cRegSpaceRef;

// Attribute Map

typedef map<string, uint32>          AttributeMap;
typedef AttributeMap::const_iterator AttributeMapConstIter;
typedef AttributeMap::iterator       AttributeMapIter;

// RegField Struct

typedef struct {
  AttributeMap attributeMap;
} RegField;

// Register Map

typedef map<uint32, cReg>               RegisterMap;
typedef RegisterMap::const_iterator     RegisterMapConstIter;
typedef RegisterMap::iterator           RegisterMapIter;

// Field Map

typedef map<string, RegField>           FieldMap;
typedef FieldMap::const_iterator        FieldMapConstIter;
typedef FieldMap::iterator              FieldMapIter;

//------------------------------------------------------------------------------
// class cRegSpace
//------------------------------------------------------------------------------

class cRegSpace {

private:

  friend class cRegSpaceRef;
  friend class crBitRangeRef;

  //-----------------//
  // Private Members //
  //-----------------//

  uint32         bitWidth;
  RegisterMap    regMap;
  FieldMap       fieldMap;
  vector<uint32> history;

  //-----------------//
  // Private Methods //
  //-----------------//

  // Find a field in the field map

  FieldMapIter     findField( const string& name );
  cReg&            getRegister( const uint32 address );
  cReg&            getRegister( const string& name );

protected:

  string className;

  void newField          ( const string& name, uint32 address, uint32 msb, uint32 lsb, const cReg& defaultValue );
  void newFieldAttribute ( const string& name, const string& attribute, uint32 defaultValue );
  void reserve           ( uint32 addr ); // Reserve space for address

public:

  // Constructor

  cRegSpace ();
  cRegSpace ( uint32 bitWidth );

  // Destructor

  virtual ~cRegSpace();

  // Field Methods (set and get)

  void   setFieldValue     ( const string& name, const cReg& value );
  void   setFieldValue     ( const string& name, uint32 msb, uint32 lsb, const cReg& value );
  uint32 getFieldValue     ( const string& name );
  uint32 getFieldValue     ( const string& name, uint32 msb, uint32 lsb );
  uint32 getFieldAttribute ( const string& name, const string& attribute );
  cReg   getFieldRegister  ( const string& name );
  cReg   getFieldRegister  ( const string& name, uint32 msb, uint32 lsb );

  // Get Methods

  vector<string> getFields           ();                 // Return all fields.
  vector<string> getFields           ( uint32 address ); // Return fields for a specific address.
  vector<uint32> getAddresses        ();                 // Return a sorted list of addresses.
  string         getClassName        ();                 // Return the className.

  // Test Methods

  bool fieldExists( const string& name );

  // Overloaded Operators

  cRegSpaceRef  operator[]( const string& fieldName );
  cReg&         operator[]( uint32 address );
  cRegSpace&    operator= ( const cRegSpace& space );

  // Print

  string str();
  string str( uint32 address );

};

// Output stream operator

ostream &operator<< ( ostream &os, cRegSpace &rhs );

//------------------------------------------------------------------------------
// Template class cRegSpace_t (inherits cRegSpace)
//------------------------------------------------------------------------------

template<int _bitWidth> class cRegSpace_t : public cRegSpace {

public:
  
  cRegSpace_t() : cRegSpace(_bitWidth) {}

};

//------------------------------------------------------------------------------
// class cRegSpaceRef
//------------------------------------------------------------------------------

class cRegSpaceRef {

  friend class cRegSpace;

  //-----------------//
  // Private Members //
  //-----------------//

  cRegSpace &rs;
  string    fieldName;
  uint32    msb;
  uint32    lsb;

  //-----------------//
  // Private Methods //
  //-----------------//

  cRegSpaceRef( cRegSpace& rs, const string& fieldName, uint32 msb, uint32 lsb ) : rs(rs), fieldName(fieldName), msb(msb), lsb(lsb) {}
  cRegSpaceRef( const cRegSpaceRef& ref ) : rs(ref.rs), fieldName(ref.fieldName), msb(ref.msb), lsb(ref.lsb) {}
  cRegSpaceRef();

public:

  //----------------//
  // Public Methods //
  //----------------//

  // Get Methods

  uint32 getWidth     ();
  uint32 getValue     ();
  cReg   getRegister  () { return(rs.getFieldRegister(fieldName,msb,lsb)); }
  uint32 getAttribute ( const string& attribute );

  // Set Methods

  void   setValue     ( uint32 value );

  // Overloaded Operators

  void         operator= ( const cRegSpaceRef& ref );
  void         operator= ( const cReg& reg );
  uint32       operator[]( const string& attribute );
  cRegSpaceRef operator[]( uint32 index );
  cRegSpaceRef operator()( uint32 _msb, uint32 _lsb );

  // Casting Operator

  operator uint32 () {
    return(rs.getFieldValue(fieldName, msb, lsb));
  }

};

#endif // __CREG_SPACE__
