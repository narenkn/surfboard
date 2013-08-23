//******************************************************************************
//
// $RCSfile: creg_msg.cc,v $
//
// $Revision: 1.1 $
// $Date: 2008/02/14 13:58:11 $
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
// This is the class definitions for cRegMsg.
//******************************************************************************

// Includes

#include "creg_msg.h"

// Initialize Static Singleton Ptr

cRegMsg* cRegMsg::singletonPtr = NULL;

// Get Singleton Ptr

cRegMsg* cRegMsg::Instance() {
  if ( singletonPtr == NULL ) singletonPtr = new cRegMsg();
  return singletonPtr;
}

//------------------------------------------------------------------------------
// DisplayRaw : Display raw message of a given type
//------------------------------------------------------------------------------

void cRegMsg::DisplayRaw(eMsgType type, const char *fileName, int lineNum, 
                         FILE *fp, int debugLevel, const char *format, va_list ap) {

  ostringstream msgName;
  const char *msgFormat = NULL;
  
  debugLevel = 1;

  // *** identify and parse the message name string (always starts with ':') ***

  if (format[0]==':') {
    
    msgName << "(";
    const char *p = format;
    
    while (*p!='\0') {
      if (*p=='%') {
        if (!strncmp(p,"%%",2)) {
          msgName << '%';
          p+=2;
        }
        else if (!strncmp(p,"%d",2)) {
          int num = va_arg(ap,int);
          msgName << dec << num;
          p+=2;
        }
        else if (!strncmp(p,"%x",2)) {
          int num = va_arg(ap,int);
          msgName << hex << num;
          p+=2;
        }
        else if (!strncmp(p,"%u",2)) {
          uint num = va_arg(ap,uint);
          msgName << dec << num;
          p+=2;
        }
        else if (!strncmp(p,"%s",2)) {
          char *str = va_arg(ap,char *);
          msgName << str;
          p+=2;
        }
        else if (!strncmp(p,"%c",2)) {
          char c = va_arg(ap,int);
          msgName << c;
          p+=2;
        }
        else if (!strncmp(p,"%lld",4)) {
          unsigned long long num = va_arg(ap,unsigned long long);
          msgName << dec << num;
          p+=4;
        }
        else if (!strncmp(p,"%llx",4)) {
          unsigned long long num = va_arg(ap,unsigned long long);
          msgName << hex << num;
          p+=4;
        }
        else {
          FATAL_ERROR(":cRegMsg:BAD_FORMAT_SPECIFIER", 
                      "Messaging system did not understand specifier: %s", p);
        }
      }
      else {
        msgName << *p;
        p += 1;
      }
    }
    
    msgName << ")";
    msgFormat = va_arg(ap,char *);

  } else { 
    msgFormat = format; 
  }

  // *** Remove path from filename ***

  const char *myFileName = fileName;
  const char *myP = fileName;
  
  while(*myP != '\0') {
    if(*myP == '/') myFileName = myP+1;
    myP++;
  }
    
  // *** Generate Message ***

  static char msgStr[MAX_MESSAGE_SIZE];
  vsnprintf(msgStr, MAX_MESSAGE_SIZE, msgFormat, ap);

  // *** Output the message ***

  Output(fp, type, myFileName, lineNum, msgName.str(), msgStr);

}

//------------------------------------------------------------------------------
// Display : Display message of a given type (with debug level) to a file.
//------------------------------------------------------------------------------

void cRegMsg::Display(eMsgType type, const char *fileName, int lineNum, 
                      FILE *fp, int debugLevel, const char *format, ...) {
  
  va_list ap;
  assert(format != NULL);
  va_start(ap, format);
  DisplayRaw(type, fileName, lineNum, fp, debugLevel, format, ap);
  va_end(ap);
}

//------------------------------------------------------------------------------
// Display : Display message of a given type (without debug level) to a file.
//------------------------------------------------------------------------------

void cRegMsg::Display(eMsgType type, const char *fileName, int lineNum, 
                      FILE *fp, const char *format, ...) {
  
  // debug level is not defined in this version

  va_list ap;
  assert(format != NULL);
  va_start(ap, format);
  DisplayRaw(type, fileName, lineNum, fp, 0, format, ap);
  va_end(ap);
}

//------------------------------------------------------------------------------
// Display : Display message of a given type (with debug level) tostdout.
//------------------------------------------------------------------------------

void cRegMsg::Display(eMsgType type, const char *fileName, int lineNum, 
                      int debugLevel, const char *format, ...) {

  // file ptr is not defined in this version

  va_list ap;
  assert(format != NULL);
  va_start(ap, format);
  DisplayRaw(type, fileName, lineNum, stdout, debugLevel, format, ap);
  va_end(ap);
}

//------------------------------------------------------------------------------
// Display : Display message of a given type (without debug level) to stdout..
//------------------------------------------------------------------------------

void cRegMsg::Display(eMsgType type, const char *fileName, int lineNum, 
                      const char *format, ...) {

  // debug level and file pointer are not defined in this version

  va_list ap;
  assert(format != NULL);
  va_start(ap, format);
  DisplayRaw(type, fileName, lineNum, stdout, 0, format, ap);
  va_end(ap);

}

//------------------------------------------------------------------------------
// Output : Print the message to stdout or a file handle.
//------------------------------------------------------------------------------

void cRegMsg::Output(FILE *fp, eMsgType type, const char *fileName, int lineNum,
                     const string &msgName, const char *msgStr) {
  

  // Output message to stdout by default

  if ( fp == NULL ) {
    fp = stdout;
  }

  // Legacy output format

  string prefix = "info";

  switch (type) {
    
  case MSG_TYPE_INFO:
    prefix = "info";
    break;
    
  case MSG_TYPE_WARN:
    prefix = "warning";
    break;
    
  case MSG_TYPE_ERROR:
    prefix = "error";
    break;
    
  case MSG_TYPE_FATAL:
    prefix = "fatal error";
    break;

  }

  if (fp!=stdout) {
    fprintf(stdout,"%s: %s:%d:%s %s\n",
           prefix.c_str(), fileName, lineNum, msgName.c_str(), msgStr);
    fflush(stdout);
  }
  
  fprintf(fp,"%s: %s:%d:%s %s\n",
          prefix.c_str(), fileName, lineNum, msgName.c_str(), msgStr);
  fflush(fp);

  // Exit if error or fatal error

  if (type==MSG_TYPE_ERROR) {
    ErrorExit();
  } else if (type==MSG_TYPE_FATAL) {
    FatalExit();
  }

}

void cRegMsg::FatalError(const char *format, ...) {

  va_list ap;
  assert(format != NULL);
  va_start(ap, format);
  DisplayRaw(cRegMsg::MSG_TYPE_FATAL, "Reporting Environment", 0, stdout, 0, format, ap);
  va_end(ap);

}

void cRegMsg::Warning(const char *format, ...) {

  va_list ap;
  assert(format != NULL);
  va_start(ap, format);
  DisplayRaw(cRegMsg::MSG_TYPE_WARN, "Reporting Environment", 0, stdout, 0, format, ap);
  va_end(ap);

}

void cRegMsg::Message(const char *format, ...) {

  va_list ap;
  assert(format != NULL);
  va_start(ap, format);
  DisplayRaw(cRegMsg::MSG_TYPE_INFO, "Reporting Environment", 0, stdout, 0, format, ap);
  va_end(ap);

}

void cRegMsg::FatalExit() {
  exit(EXIT_FAILURE); // stop immediately
}

void cRegMsg::ErrorExit() {
  FatalExit();
}

