#ifndef _IDLE_H_
#define _IDLE_H_

#ifdef _WIN32

#include <stdlib.h>

void Idle()
{
    _sleep(1);
}

#else

#include <unistd.h>

void Idle()
{
    usleep(1000);
}

#endif

#endif