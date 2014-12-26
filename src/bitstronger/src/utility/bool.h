/*! \file bool.h
@brief 增强的布尔类型定义（第三方提供，遵循GPL许可）

　　作者信息：黄晨 复旦大学化学系表面化学实验室 E-mail：webmaster at elephantbase dot net 
*/

#ifndef BOOL_H
#define BOOL_H

#ifndef _WIN32
#define __int64 long long
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

typedef int Bool;             // b
typedef char BoolChar;        // bc
typedef short BoolShort;      // bw
typedef long BoolLong;        // bdw
typedef long long BoolLongLong; // bqw

inline Bool Eqv(Bool bArg1, Bool bArg2) {
  return bArg1 ? bArg2 : !bArg2;
}

inline Bool Xor(Bool bArg1, Bool bArg2) {
  return bArg1 ? !bArg2 : bArg2;
}

#endif
