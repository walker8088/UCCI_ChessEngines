/*! \file movemaker.h
@brief 着法生成类（单件模块）
*/

#ifndef MOVEMAKER_H
#define MOVEMAKER_H

#include "../utility/bool.h"
#include "constant.h"
#include "board.h"
#include "movement.h"

///红仕所有可行位置
const BDPOINT R_ADVISOR_POSITION[5] = {0xa6, 0xa8, 0xb7, 0xc6, 0xc8};
///黑士所有可行位置
const BDPOINT B_ADVISOR_POSITION[5] = {0x36, 0x38, 0x47, 0x56, 0x58};
///红相所有可行位置
const BDPOINT R_BISHOP_POSITION[7] = {0x85, 0x89, 0xa3, 0xa7, 0xab, 0xc5, 0xc9};
///黑象所有可行位置
const BDPOINT B_BISHOP_POSITION[7] = {0x35, 0x39, 0x53, 0x57, 0x5b, 0x75, 0x79};
///帅（将）所有可走步路（相对坐标） 
const BDPOINT cnKINGMOVETAB[4] = {0x01, 0xff, 0x10, 0xf0};
///马所有可走步路（相对坐标）
const BDPOINT cnKnightMoveTab[8] = {0xdf, 0xe1, 0xee, 0xf2, 0x0e, 0x12, 0x1f, 0x21}; 
///以原点为起点的马腿相对坐标 
const BDPOINT cnHorseLegTab[8] = {0xf0, 0xf0, 0xff, 0x01, 0xff, 0x01, 0x10, 0x10};
///以原点为落点的马腿相对坐标 
const BDPOINT cnHorseLegBackTab[8] = {0xef, 0xf1, 0xef, 0xf1, 0x0f, 0x11, 0x0f, 0x11};
///过河红兵所有可走步路（相对坐标） 
const BDPOINT R_PawnMoveTab[3] = {0xf0,0x01,0xff};
///过河黑卒所有可走步路（相对坐标） 
const BDPOINT B_PawnMoveTab[3] = {0x10,0x01,0xff};

///着法生成类（单件模块）
/*!
　　我们需要通过一个局面，得到指定一方所有可行的着法，以便分析评估其形成的局面，得到一个最佳着法。着法的生成过程是遍历整个棋盘，对所有棋子列出其符合棋规的着法（有必要排除"主动送将"和"不应将"等违规着法），记入一个着法表示类的数组。

　　在寻找符合棋规的着法方面，我们充分利用16×16棋盘表示法在计算方面的优势，对不同的棋子，分别采用预置表、模板匹配、奇偶校验等多种位运算的方法，提高了算法效率。

　　各个棋子的可行位置参考16*16表示法模板：http://www.elephantbase.net/computer/eleeye_struct.htm
*/ 
class MoveMaker
{
public:
    ///从当前局面得到所有可行着法，返回可行的着法种数 
    static int Make(Board & cur, PLAYER who, Movement move[]);
private:
    ///对 MoveMaker::Make 得出可行的着法再次判断合理性，排除主动送将和不会应将的着法
    /*!同时判断有无将军的情况，有则置 check 为 TRUE。<-暂未做这个判断！ 
    */
    static Bool PreAdd(Board & cur, PLAYER who, BDPOINT from, BDPOINT to, Bool & check);
};

#endif
