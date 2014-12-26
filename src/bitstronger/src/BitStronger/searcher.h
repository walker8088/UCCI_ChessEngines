/*! \file searcher.h
@brief 博弈树搜索类（单件模块）
*/

#ifndef SERCHER_H
#define SERCHER_H

#include <stdio.h>
#include <time.h>
#include "../utility/bool.h"
#include "constant.h"
#include "board.h"
#include "movement.h"
#include "movemaker.h"
#include "evaluation.h"
#include "openbook.h"
//#include "hashnum.h"

///搜索层数 
static int searchDepth;

///可行着法集合 
static Movement allmoves[MAX_SEARCH_DEPTH + 1][SEARCH_WIDTH];
///静态搜索的可行着法集合 
static Movement qsmoves[MAX_QUIES_DEPTH + 1][SEARCH_WIDTH];

///当前最佳着法
static Movement best; 
///上次搜索最佳着法
static Movement lastbest; 
///当前次佳着法（保存前三位，避免循环局面）
static Movement better;
///上次搜索次佳着法（保存前三位，避免循环局面）
static Movement lastbetter;
///当前第三佳着法（保存前三位，避免循环局面）
static Movement good;
///上次搜索第三佳着法（保存前三位，避免循环局面）
static Movement lastgood;

///搜索开始时间 
static time_t startTime;

///历史着法记录
/*!记录最后四着，也就是可能出现的两个循环。 
   因为走棋出现循环反复三次者要依棋规判负或判和。 
   初始值置为不可能的着法，避免对开局的四着特殊处理。 
*/
static long history[2][4] = {{0, 1, 2, 3}, {0, 1, 2, 3}};

///哈希树历史局面记录
static HASHNUM hashlink[MAX_SEARCH_DEPTH];

///静态搜索的哈希树历史局面记录
static HASHNUM qshashlink[MAX_QUIES_DEPTH];

///得到一个最终局面的路径 
static Movement trace[MAX_SEARCH_DEPTH];
///静态搜索得到一个最终局面的路径 
static Movement qstrace[MAX_QUIES_DEPTH];

///博弈树搜索类（单件模块）
/*!
　　博弈树上的每一个节点都代表一个局面，每一层的节点都是由其父节点的所有可行着法形成的。奇数层与偶数层节点分别表示双方的局面。引擎要在众多的叶子节点上挑选一个最佳的局面，从而反推到当前的着法作为自己的选择。 

　　采用速度快而不会出现错误剪枝的Principal Variable Search。 
*/  
class Searcher
{
public:
    ///得到一个最佳着法，向界面输出 
    static void GetBestmove(Board & cur, PLAYER who);
    ///判断棋局是否决出胜负 
    inline static Bool GameOver(Board & cur);
private:
    ///用PVS搜索查找最佳着法 
    static int PVSearch(Board & cur, int depth, PLAYER who, int alpha, int beta);
    ///静态搜索（须与其它搜索配合使用，在其它搜索的叶子节点处调用）
    static int QuiesSearch(Board & cur, int depth, PLAYER who, int alpha, int beta);
};

#endif
