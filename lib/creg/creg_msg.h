//******************************************************************************
//
// $RCSfile: creg_msg.h,v $
//
// $Revision: 1.2 $
// $Date: 2009/03/23 19:38:47 $
// $Author: grpierce $
// $State: Exp $
// $Locker:  $
//
// Copyright (c) 2008 by Advanced Micro Devices, Inc.
//
// This file is protected by Federal Copyright Law, with all rights
// reserved. No part of this file may be reproduced, stored in a
// retrieval system, translated, transcribed, or transmitted, in any
// form, or by any means manual, electric, electronic, mechanical,
// electro-magnetic, chemical, optical, or otherwise, without prior
// explicit written permission from Advanced Micro Devices, Inc.
//
//******************************************************************************
// This is the class header for cRegMsg. This class takes care of all messaging.
//******************************************************************************

#ifndef __CREG_MSG__
#define __CREG_MSG__

// Includes

#include <stdio.h>
#include <signal.h>
#include <sstream>
#include <execinfo.h>
#include <regex.h>
#include <string>
#include <cstring>
#include <stdarg.h>
#include <cmath>
#include <cassert>
#include <vector>
#include <iostream>
#include <cctype>
#include <algorithm>

using std::string;
using std::ostringstream;
using std::cout;
using std::endl;
using std::hex;
using std::dec;
using std::sort;

// Message Defines

#define MAX_MESSAGE_SIZE 10000
#define MAX_NAME_SIZE 100

#define FATAL_ERROR(ARGS...) cRegMsg::Instance()->Display(cRegMsg::MSG_TYPE_FATAL, __FILE__,__LINE__,##ARGS)
#define ERROR(ARGS...)       cRegMsg::Instance()->Display(cRegMsg::MSG_TYPE_ERROR, __FILE__,__LINE__,##ARGS)
#define WARNING(ARGS...)     cRegMsg::Instance()->Display(cRegMsg::MSG_TYPE_WARN,  __FILE__,__LINE__,##ARGS)
#define INFO(ARGS...)        cRegMsg::Instance()->Display(cRegMsg::MSG_TYPE_INFO,  __FILE__,__LINE__,##ARGS)

//------------------------------------------------------------------------------
// class cRegMsg
//------------------------------------------------------------------------------

class cRegMsg {

public:

   enum eMsgType { MSG_TYPE_ERROR    = 0x1,
                   MSG_TYPE_FATAL    = 0x3,
                   MSG_TYPE_WARN     = 0x4,
                   MSG_TYPE_INFO     = 0x8};

private:

  //-----------------//
  // Private Members //
  //-----------------//

  static cRegMsg* singletonPtr; // Singlton Pointer

  //-----------------//
  // Private Methods //
  //-----------------//

  // Constructor

  cRegMsg() {}

  // Destructor

  virtual ~cRegMsg() {}

  // Output

  void Output(FILE *fp, eMsgType type, const char *fileName, int lineNum,
              const string& msgName, const char *msgStr);
    
public:

  // Get Instance (singlton)

  static cRegMsg* Instance();

  void DisplayRaw (eMsgType type, const char *fileName, int lineNum, 
                   FILE* fp, int debugLevel, const char *format, va_list ap);
  void Display    (eMsgType type, const char *fileName, int lineNum, 
                   FILE* fp, int debugLevel, const char *format, ...);
  void Display    (eMsgType type, const char *fileName, int lineNum, 
                   FILE* fp, const char *format, ...);
  void Display    (eMsgType type, const char *fileName, int lineNum, 
                   int debugLevel, const char *format, ...);
  void Display    (eMsgType type, const char *fileName, int lineNum, 
                   const char *format, ...);

  void FatalError (const char *format, ...);
  void Warning    (const char *format, ...);
  void Message    (const char *format, ...);

  void FatalExit();
  void ErrorExit();

};

#endif // __CREG_MSG__
