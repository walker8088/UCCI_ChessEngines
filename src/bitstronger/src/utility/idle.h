/*! \file idle.h
@brief CPU空转函数定义（第三方提供，遵循GPL许可）

　　作者信息：黄晨 复旦大学化学系表面化学实验室 E-mail：webmaster at elephantbase dot net 
*/

#ifndef IDLE_H
#define IDLE_H

#ifdef _WIN32
  #include <windows.h>
  inline void Idle(void) {
    Sleep(1);
  }
#else
  #include <unistd.h>
  inline void Idle(void) {
    usleep(1000);
  }
#endif

#endif
