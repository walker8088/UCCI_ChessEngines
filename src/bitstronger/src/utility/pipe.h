/*! \file pipe.h
@brief 管道通信模块（第三方提供，遵循GPL许可）

　　作者信息：黄晨 复旦大学化学系表面化学实验室 E-mail：webmaster at elephantbase dot net 
*/

#ifdef _WIN32
  #include <windows.h>
#else
  #include <unistd.h>
#endif
#include "bool.h"

#ifndef PIPE_H
#define PIPE_H

const int LINE_INPUT_MAX_CHAR = 1024;

///管道通信模块（第三方提供，遵循GPL许可） 
/*!
　　我们直接使用UCCI开发人员提供的管道通讯模块（遵循GPL许可）。

　　具体实现和功能说明参见：http://www.elephantbase.net/protocol/cchess_ucci.htm。
*/
struct PipeStruct {
#ifdef _WIN32
  HANDLE hInput, hOutput;
  BOOL bConsole;
  int nBytesLeft;
#else
  int nInput, nOutput;
#endif
  int nReadEnd;
  char szBuffer[LINE_INPUT_MAX_CHAR];

  void Open(const char *szExecFile = NULL);
  void Close(void) const;
  void ReadInput(void);
  Bool CheckInput(void);
  Bool GetBuffer(char *szLineStr);
  Bool LineInput(char *szLineStr);
  void LineOutput(const char *szLineStr) const;
}; // pipe

#endif
