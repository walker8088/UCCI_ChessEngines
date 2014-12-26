/*! \file hashnum.h
@brief 哈希数操作类（单件模块）
*/

#ifndef HASHNUM_H
#define HASHNUM_H

#include <stdio.h>
#include <stdlib.h>
#include "../utility/bool.h"
#include "constant.h"

///引擎工作路径 
extern char workpath[256];
///保存 Zobrist 数的文件所在路径 
extern char hashpath[256];

///是否已经加载？不重复加载 
static Bool loadedhash = FALSE;

///保存 Zobrist 数的文件 
static FILE * doc;

///Zobrist 数组
static HASHNUM hash[14][256];

///置换表项目类型 
enum EntryType
{
    EXACT, L_BOUND, U_BOUND 
};

///置换表项目类
/*!将已经搜索过的局面加入置换表，以便在后续的搜索中遇到相同的局面直接调用其评估值 
*/ 
class TTItem
{
public:
    ///该局面的哈希数
    /*!尽管用哈希数的后20位作为存储下标，但为防止冲突情况，仍应记录哈希数并检验之。 
    */ 
    HASHNUM zobrist;
    ///该局面所在的博弈树最大搜索层数 
    int max_depth; 
    ///该局面所在的博弈树深度 
    int depth; 
    ///置换表项目类型
    EntryType entry;
    ///局面评估值 
    int value; 
};

///置换表
/*!分别记录红黑双方；下标为局面 Zobrist 数的末20位。 
*/ 
static TTItem * TTList[2];

///哈希数操作类（单件模块） 
/*!
　　我们采用博弈算法中经典的Zobrist键值表示法，对每个棋子位于每个位置的情况（共32个棋子×90个位置，不排除不可能的位置）用一个64位的数（8个字节）表示。一个局面上所有棋子的Zobrist键值之异或就定义为这种局面的哈希表示。理论表明这种哈希表示法冲突概率极低，不用考虑冲突处理。

　　Zobrist键值可以通过编写小程序随机生成，将其保存在一个文件（Zobrist.bin）中，每8个字节为一个Zobrist键值。键值文件在引擎加载时读入内存，以后使用时直接调用。
*/
class Hashnum
{
public:
    ///初始化 Zobrist 数组和置换表 
    static void InitHash();
    ///清除置换表
    static void ClearHash(); 
    ///得到某个棋子在某个位置对应的 Zobrist 数 
    static HASHNUM GetHash(CHESSMAN chess, BDPOINT point);
    ///在置换表中查找已存项目
    static int SearchTT(HASHNUM zobrist, int max_depth, int depth, PLAYER who, int alpha, int beta);
    ///将项目保存到置换表
    static void SaveTT(HASHNUM zobrist, int max_depth, int depth, PLAYER who, EntryType entry, int value);
};

#endif
