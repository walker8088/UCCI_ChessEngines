/*! \file movement.h
@brief 着法表示类
*/

#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "../utility/bool.h"
#include "constant.h"

///着法表示类
/*!
　　记录了一种着法的起点、终点、吃了什么子、是否构成将军。其中有必要记录被吃的子，因为在博弈树搜索中涉及着法回溯，必须使一个局面能够完整地还原回上一局面。
*/ 
class Movement
{
public:
    /*----------成员变量----------*/
    ///起始位置（用16*16表示法） 
    BDPOINT from;
    ///落子位置（用16*16表示法） 
    BDPOINT to;
    ///被吃的子（用 enum Chessman 表示） 
    CHESSMAN ate;
    ///是否构成将军 
    Bool check;
    /*----------成员函数----------*/
    ///默认构造函数——暂不使用 
    Movement();
    ///使用已知信息的构造函数
    Movement(BDPOINT f, BDPOINT t, CHESSMAN a, Bool c); 
    ///把着法转换成界面接收的字符串
    char * Move2Str(char * str);
    ///重载赋值运算符
    Movement & operator = (const Movement & obj); 
};

#endif
