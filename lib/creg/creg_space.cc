//******************************************************************************
//
// $RCSfile: creg_space.cc,v $
//
// $Revision: 1.19 $
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
// Class definitions for cRegSpace
//******************************************************************************

#include "creg_space.h"

//------------------------------------------------------------------------------
// constructor(s)
//------------------------------------------------------------------------------

cRegSpace::cRegSpace() : bitWidth(32), className( "cRegSpace" ) {
}

cRegSpace::cRegSpace( uint32 bitWidth ) : bitWidth( bitWidth ), className( "cRegSpace" ) {
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------

cRegSpace::~cRegSpace() {}

//------------------------------------------------------------------------------
// protected: reserve: Reserve space for an address
//------------------------------------------------------------------------------

void cRegSpace::reserve( uint32 addr ) {
  regMap[addr].resizeAndCopy(cReg(bitWidth-1,0));
}

//------------------------------------------------------------------------------
// private : findField
//------------------------------------------------------------------------------

FieldMapIter cRegSpace::findField( const string& name ) {

  string ucName = cReg::toUpper(name);

  // Lookup field in the field map
  
  FieldMapIter i = fieldMap.find(ucName);
  
  if ( i == fieldMap.end() ) {
    FATAL_ERROR("%s: Register field '%s' does not exist.", className.c_str(), name.c_str());
  }

  return i;

}

//------------------------------------------------------------------------------
// private : getRegister
//------------------------------------------------------------------------------

cReg& cRegSpace::getRegister( const uint32 address ) {

  // Lookup address in the register map
  
  RegisterMapIter i = regMap.find(address);
  
  if ( i == regMap.end() ) {
    FATAL_ERROR("%s: Register address '%d' does not exist.", className.c_str(), address);
  }

  return i->second;

}

//------------------------------------------------------------------------------
// private : getRegister
//------------------------------------------------------------------------------

cReg& cRegSpace::getRegister( const string& name ) {

  // Lookup field in the field map
  
  FieldMapIter i = findField(name);
  
  // Get the address from the lookup table
  
  uint32 addr = getFieldAttribute(name, "ADDRESS");

  return( getRegister(addr) );

}

//------------------------------------------------------------------------------
// getFieldAttribute
//------------------------------------------------------------------------------

uint32 cRegSpace::getFieldAttribute( const string& name, const string& attribute ) {

  // Lookup field in the field map
  
  FieldMapIter     f       = findField(name);
  AttributeMap&    attrMap = (f->second).attributeMap;
  AttributeMapIter a       = attrMap.find(attribute);
  
  if ( a == attrMap.end() ) {
    FATAL_ERROR("%s: Field attribute '%s' does not exist for field '%s'.",
                className.c_str(), attribute.c_str(), name.c_str());
  }

  return a->second;

}

//------------------------------------------------------------------------------
// newField
//------------------------------------------------------------------------------

void cRegSpace::newField( const string& name, uint32 address, uint32 msb, uint32 lsb, const cReg& defaultValue ) {

  // Check MSB and LSB

  if ( msb >= bitWidth || lsb >= bitWidth ) {
    FATAL_ERROR("%s::newField : Illegal bit range [%d:%d] was specified for field %s, must be within [%d:0]",
                className.c_str(), msb, lsb, name.c_str(), bitWidth-1);
  }

  string ucName = cReg::toUpper(name);

  // Reserve space for the address, if the address does not exist.

  if ( regMap.find(address) == regMap.end() ) {
    reserve(address);
  }
  
  // Set default value

  regMap[address].set(msb, lsb, defaultValue);

  // Update field map
 
  fieldMap[ucName].attributeMap["MSB"]     = msb;
  fieldMap[ucName].attributeMap["LSB"]     = lsb;
  fieldMap[ucName].attributeMap["ADDRESS"] = address;

}

//------------------------------------------------------------------------------
// setFieldValue
//------------------------------------------------------------------------------

void cRegSpace::setFieldValue( const string& name, uint32 msb, uint32 lsb, const cReg& value ) {

  cReg& reg = getRegister(name);

  reg(msb, lsb) = value;
  
}

//------------------------------------------------------------------------------
// setFieldValue
//------------------------------------------------------------------------------

void cRegSpace::setFieldValue( const string& name, const cReg& value ) {
  
  // Get the msb and lsb from the lookup table

  uint32 msb = getFieldAttribute(name, "MSB");
  uint32 lsb = getFieldAttribute(name, "LSB");
  cReg&  reg = getRegister(name);
  
  reg(msb, lsb) = value;
  
}

//------------------------------------------------------------------------------
// getFieldValue
//------------------------------------------------------------------------------

uint32 cRegSpace::getFieldValue( const string& name, uint32 msb, uint32 lsb ) {
    
  cReg&  reg = getRegister(name);

  return reg(msb, lsb).getInt32();
  
}

//------------------------------------------------------------------------------
// getFieldRegister
//------------------------------------------------------------------------------

cReg cRegSpace::getFieldRegister( const string& name, uint32 msb, uint32 lsb ) {
  
  cReg&  reg = getRegister(name);

  return reg(msb, lsb);
  
}

//------------------------------------------------------------------------------
// getFieldValue
//------------------------------------------------------------------------------

uint32 cRegSpace::getFieldValue( const string& name ) {

  uint32 msb = getFieldAttribute(name, "MSB");
  uint32 lsb = getFieldAttribute(name, "LSB");
  cReg&  reg = getRegister(name);

  return reg(msb, lsb).getInt32();
  
}

//------------------------------------------------------------------------------
// getFieldRegister
//------------------------------------------------------------------------------

cReg cRegSpace::getFieldRegister( const string& name ) {
  
  uint32 msb = getFieldAttribute(name, "MSB");
  uint32 lsb = getFieldAttribute(name, "LSB");
  cReg&  reg = getRegister(name);

  return reg(msb, lsb);
  
}

//------------------------------------------------------------------------------
// newFieldAttribute
//------------------------------------------------------------------------------

void cRegSpace::newFieldAttribute( const string& name, const string& attribute, uint32 defaultValue ) {

  // Lookup field in the field map
  
  if ( attribute == "MSB" || attribute == "LSB" || attribute == "ADDRESS" ) {
    FATAL_ERROR("%s: Cannot define field attribute '%s' because it already exists as a built-in attribute.",
                className.c_str(), attribute.c_str());
  }

  FieldMapIter  i       = findField(name);
  AttributeMap& attrMap = (i->second).attributeMap;
  attrMap[attribute]    = defaultValue;

}

//------------------------------------------------------------------------------
// getFields
//------------------------------------------------------------------------------

vector<string> cRegSpace::getFields() {

  vector<string> strList;

  for ( FieldMapIter i = fieldMap.begin(); i != fieldMap.end(); i++ ) {
    strList.push_back(i->first);
  }

  return strList;

}

//------------------------------------------------------------------------------
// getFields
//------------------------------------------------------------------------------

vector<string> cRegSpace::getFields( uint32 address ) {

  vector<string> strList;

  for ( FieldMapIter i = fieldMap.begin(); i != fieldMap.end(); i++ ) {

    if ( (i->second).attributeMap["ADDRESS"] == address ) {
      strList.push_back(i->first);
    }

  }

  return strList;

}

//------------------------------------------------------------------------------
// getAddresses
//------------------------------------------------------------------------------

vector<uint32> cRegSpace::getAddresses() {

  vector<uint32> addrList;

  for ( RegisterMapIter i = regMap.begin(); i != regMap.end(); i++ ) {
    addrList.push_back(i->first);
  }

  sort(addrList.begin(), addrList.end());

  return addrList;

}

//------------------------------------------------------------------------------
// getClassName
//------------------------------------------------------------------------------

string cRegSpace::getClassName() {
  return(className);
}

//------------------------------------------------------------------------------
// fieldExists
//------------------------------------------------------------------------------

bool cRegSpace::fieldExists( const string& name ) {

  string ucName = cReg::toUpper(name);

  return( fieldMap.find(ucName) != fieldMap.end() );

}

//------------------------------------------------------------------------------
// str
//------------------------------------------------------------------------------

string cRegSpace::str() {

  ostringstream os;

  vector<uint32> addrList = getAddresses();

  os << "------------------------------------------------------------------------------------" << endl;
  os << "Register Space '" << className << "':" << endl;
  os << "------------------------------------------------------------------------------------" << endl;

  // Display Header

  char header[256];

  sprintf(header, "%-30s  %-4s  %-4s  %-4s  %-4s     %s", "FIELD NAME", "ADDR", "MSB", "LSB", "WIDTH", "VALUE");

  os << header << endl;
  os << "------------------------------------------------------------------------------------" << endl;

  for( uint32 i = 0; i < addrList.size(); i++ ) {    
    os << str(addrList[i]);
  }

  os << "------------------------------------------------------------------------------------" << endl;

  return os.str();

}

//------------------------------------------------------------------------------
// str(address)
//------------------------------------------------------------------------------

string cRegSpace::str( uint32 address ) {

  ostringstream os;
  
  // Display All Fields

  vector<string> fieldList = getFields(address);
  for( uint32 j = 0; j < fieldList.size(); j++ ) {

    char buffer[256];

    string name = fieldList[j];
    cReg   reg  = getFieldRegister( fieldList[j] );
    uint32 msb  = getFieldAttribute( fieldList[j], "MSB" );
    uint32 lsb  = getFieldAttribute( fieldList[j], "LSB" );

    sprintf(buffer, "%-30s  %-4d  %-4d  %-4d  %-4d      0x%s",
            name.c_str(),
            address,
            msb,
            lsb,
            reg.getWidth(),
            reg.toHex(false).c_str());

    os << buffer << endl;

  }
  
  return os.str();

}

//------------------------------------------------------------------------------
// operator[ string ]
//------------------------------------------------------------------------------

cRegSpaceRef cRegSpace::operator[]( const string& fieldName ) {

  // Lookup field in the field map
  
  FieldMapIter i = findField(fieldName);
  
  uint32 msb = (i->second).attributeMap["MSB"];
  uint32 lsb = (i->second).attributeMap["LSB"];

  return ( cRegSpaceRef( *this, fieldName, msb, lsb ) );

}

//------------------------------------------------------------------------------
// operator[ uint32 ]
//------------------------------------------------------------------------------

cReg& cRegSpace::operator[]( uint32 address ) {
  return( getRegister(address) );
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------

cRegSpace& cRegSpace::operator=( const cRegSpace& space ) {

  bitWidth   = space.bitWidth;
  regMap     = space.regMap;
  fieldMap   = space.fieldMap;
  history    = space.history;
  className  = space.className;

  return *this;

}

//------------------------------------------------------------------------------
// operator<< : Return ostream for class object cRegSpace
//------------------------------------------------------------------------------

ostream &operator<< ( ostream &os, cRegSpace &rhs ) {
  return os << rhs.str();
}

/*******************************************************************************
 *                    cRegSpaceRef Class Method Definitions                    *
 *******************************************************************************/

//------------------------------------------------------------------------------
// getWidth
//------------------------------------------------------------------------------

uint32 cRegSpaceRef::getWidth() {
  return( (msb > lsb) ? ( msb - lsb + 1 ) : ( lsb - msb + 1 ) );
}

//------------------------------------------------------------------------------
// setValue
//------------------------------------------------------------------------------

void   cRegSpaceRef::setValue( uint32 value ) {
  rs.setFieldValue(fieldName, msb, lsb, value);
}

//------------------------------------------------------------------------------
// getValue
//------------------------------------------------------------------------------

uint32 cRegSpaceRef::getValue() {
  return ( rs.getFieldValue(fieldName, msb, lsb) );
}

//------------------------------------------------------------------------------
// getAttribute
//------------------------------------------------------------------------------

uint32 cRegSpaceRef::getAttribute( const string& attribute ) {
  return( rs.getFieldAttribute(fieldName, attribute) );
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------

void   cRegSpaceRef::operator=( const cRegSpaceRef& ref ) {
  rs.setFieldValue(fieldName, msb, lsb, ref.rs.getFieldRegister(fieldName));
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------

void   cRegSpaceRef::operator=( const cReg& reg ) {
  rs.setFieldValue(fieldName, msb, lsb, reg );
}

//------------------------------------------------------------------------------
// operator[]
//------------------------------------------------------------------------------

uint32 cRegSpaceRef::operator[]( const string& attribute ) {
  return( rs.getFieldAttribute(fieldName, attribute) );
}

//------------------------------------------------------------------------------
// operator[]
//------------------------------------------------------------------------------

cRegSpaceRef cRegSpaceRef::operator[]( uint32 index ) {
  
  uint32 offset = lsb;
  
  if ( msb < lsb ) {
    offset = msb;
  }
  
  return( cRegSpaceRef(rs, fieldName, index+offset, index+offset) );
  
}

//------------------------------------------------------------------------------
// operator()
//------------------------------------------------------------------------------

cRegSpaceRef cRegSpaceRef::operator()( uint32 _msb, uint32 _lsb ) {
  
  uint32 offset = lsb;
  
  if ( msb < lsb ) {
    offset = msb;
  }
  
  return( cRegSpaceRef(rs, fieldName, _msb+offset, _lsb+offset) );
  
}
