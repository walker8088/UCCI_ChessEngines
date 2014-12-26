/*! \file movement.cpp
@brief 着法表示类
*/

#include "movement.h"

Movement::Movement()
{
}

Movement::Movement(BDPOINT f, BDPOINT t, CHESSMAN a, Bool c)
{
    from = f;
    to = t;
    ate = a;
    check = c;
}

char * Movement::Move2Str(char * str)
{       
    str[0] = (from & 0x0f) - 3 + 'a';
    str[1] = '9' - (from >> 4) + 3;
    str[2] = (to & 0x0f) - 3 + 'a';
    str[3] = '9' - (to >> 4) + 3;
    str[4] = '\0'; 
    return str;
}

Movement & Movement::operator = (const Movement & obj)
{
    if (this != &obj)
    {
        this->from = obj.from;
        this->to = obj.to;
        this->ate = obj.ate;
        this->check = obj.check;
    }
    return *this;
}
