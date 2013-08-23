//******************************************************************************
//
// $RCSfile: creg_attribute.h,v $
//
// $Revision: 1.4 $
// $Date: 2009/03/24 19:36:08 $
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
// This is the class header for cRegAttribute. This class represents a cReg
// attribute.
//******************************************************************************

#ifndef __CREG_ATTRIBUTE__
#define __CREG_ATTRIBUTE__

// Includes

#include <map>
#include <string>
#include <vector>

using std::map;
using std::string;
using std::vector;

//------------------------------------------------------------------------------
// cRegAttributeBase : Base class for a cRegAttribute
//------------------------------------------------------------------------------

class cRegAttributeBase {

public:

  cRegAttributeBase() {}
  cRegAttributeBase( const cRegAttributeBase& base ) {}
  virtual ~cRegAttributeBase() {}
  virtual cRegAttributeBase* duplicate() = 0;

};

//------------------------------------------------------------------------------
// cRegAttribute : Template class which represents an arbitrary attribute
//------------------------------------------------------------------------------

template<class T> class cRegAttribute : public cRegAttributeBase {
  
public:

  T attribute;

  // Constructor(s)

  cRegAttribute() {}

  cRegAttribute(const cRegAttribute& crAttr) {
    attribute = crAttr.attribute;
  }

  cRegAttribute(const T& attr) {
    attribute = attr;
  }

  // Destructor

  virtual ~cRegAttribute() {}

  // Make a copy of myself

  cRegAttributeBase* duplicate() {
    return( new cRegAttribute(attribute) );
  }

  // Assignment Operator

  cRegAttribute &operator= ( const cRegAttribute& crAttr ) {
    attribute = crAttr.attribute;
    return(*this);
  }

  T &operator= ( const T& attr ) {
    attribute = attr;
    return(attribute);
  }

};

 // Attribute Map

typedef map<string, cRegAttributeBase*> crAttributeMap;
typedef crAttributeMap::const_iterator  crAttributeMapConstIter;
typedef crAttributeMap::iterator        crAttributeMapIter;
typedef vector<string>                  crAttributeList;

#endif // __CREG_ATTRIBUTE__
