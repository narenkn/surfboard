//******************************************************************************
//
// $RCSfile: creg_field.h,v $
//
// $Revision: 1.7 $
// $Date: 2009/05/30 02:10:57 $
// $Author: grpierce $
// $State: Exp $
// $Locker:  $
//
// Copyright (c) 2009 by Advanced Micro Devices, Inc.
//
// This file is protected by Federal Copyright Law, with all rights
// reserved. No part of this file may be reproduced, stored in a
// retrieval system, translated, transcribed, or transmitted, in any
// form, or by any means manual, electric, electronic, mechanical,
// electro-magnetic, chemical, optical, or otherwise, without prior
// explicit written permission from Advanced Micro Devices, Inc.
//
//******************************************************************************
// This is the class header for cRegField. This class represents a field for a
// cReg class.
//******************************************************************************

#ifndef __CREG_FIELD__
#define __CREG_FIELD__

#ifndef NO_ENV_DKI
#include "env_dki.h"
#else
#include "creg_msg.h"
#endif

// Includes

#include <vector>
#include <map>
#include <string>

// Local Includes

#include "creg_attribute.h"

// Using

using std::map;
using std::vector;
using std::string;

//------------------------------------------------------------------------------
// class cRegField
//------------------------------------------------------------------------------

class cRegField {

private:

  unsigned int        msb;           // Field Msb
  unsigned int        lsb;           // Field Lsb
  crAttributeMap*     attributeMap;  // Field Attribute Map Pointer

  //------------------------------------------------------------------------------
  // mallocAttributeMap
  //------------------------------------------------------------------------------

  void mallocAttributeMap() {
    
    if ( attributeMap == NULL ) {
      attributeMap = new crAttributeMap();
    }
    
  }

  //------------------------------------------------------------------------------
  // findAttribute
  //------------------------------------------------------------------------------

  crAttributeMapIter findAttribute( const string& attrName ) {
        
    // Lookup attribute in the attribute map
    
    crAttributeMapIter i;
    
    if ( attributeMap != NULL ) {
      
      i = attributeMap->find(attrName);
      
      if ( i == attributeMap->end() ) {
        FATAL_ERROR("Field attribute '%s' does not exist.", attrName.c_str());
      }
      
    } else {
      FATAL_ERROR("Field attribute '%s' does not exist.", attrName.c_str());
    }
    
    return i;
    
  }

  //------------------------------------------------------------------------------
  // copyAttributeMap
  //------------------------------------------------------------------------------

  void copyAttributeMap( crAttributeMap* src ) {

    if ( src != NULL ) {
      
      mallocAttributeMap();
      
      for ( crAttributeMapIter i = src->begin(); i != src->end(); ++i ) {

        cRegAttributeBase* base = i->second->duplicate();
        (*attributeMap)[i->first] = base;

      }
      
    }

  }

  //------------------------------------------------------------------------------
  // Copy cRegField
  //------------------------------------------------------------------------------

  cRegField& copy( const cRegField& field ) {

    this->msb          = field.msb;
    this->lsb          = field.lsb;
    this->attributeMap = NULL;

    copyAttributeMap(field.attributeMap);

    return(*this);

  }

public:

  // Constructor(s)

  cRegField() {

    lsb          = 0;
    msb          = 0;
    attributeMap = NULL;

  }

  cRegField(unsigned int msb, unsigned int lsb) {

    this->msb    = msb;
    this->lsb    = lsb;
    attributeMap = NULL;

  }

  // Copy Constructor

  cRegField(const cRegField& field) {
    copy(field);
  }

  // Destructor

  virtual ~cRegField() {

    if ( attributeMap != NULL ) {

      for ( crAttributeMapIter i = attributeMap->begin(); i != attributeMap->end(); ++i ) {
        delete i->second;
      }
      
      delete attributeMap;
      attributeMap = NULL;

    }

  }
  
  // Get Methods

  unsigned int  getMsb()  { return msb; }
  unsigned int  getLsb()  { return lsb; }

  //------------------------------------------------------------------------------
  // hasAttribute
  //------------------------------------------------------------------------------

  bool hasAttribute( const string& attrName ) {
    
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
  // newAttribute
  //------------------------------------------------------------------------------

  template <class T>
  void newAttributeTyped ( const string& attrName, const T& attr ) {

    mallocAttributeMap();
    (*attributeMap)[attrName] = new cRegAttribute<T>(attr);

  }

  //------------------------------------------------------------------------------
  // getAttributeTyped
  //------------------------------------------------------------------------------

  template <class T>
  T& getAttributeTyped ( const string& attrName ) {

    crAttributeMapIter i = findAttribute(attrName);
    cRegAttributeBase* b = i->second;
    cRegAttribute<T>*  a = dynamic_cast<cRegAttribute<T>*>(b);
    
    if ( a == NULL ) {
      FATAL_ERROR("cRegField::getAttribute: Unable to cast value for field attribute '%s', value has a different type.", attrName.c_str());
    }
    
    return( a->attribute );
    
  }

  //------------------------------------------------------------------------------
  // Assignment operator to another cRegField
  //------------------------------------------------------------------------------

  cRegField& operator= ( const cRegField& field ) {

    copy(field);
    return(*this);

  }

};

// Field Map

typedef map<string, cRegField*>      crFieldMap;
typedef crFieldMap::const_iterator   crFieldMapConstIter;
typedef crFieldMap::iterator         crFieldMapIter;
typedef vector<string>               crFieldList;
typedef crFieldList                  FieldList;

#endif // __CREG_FIELD__
