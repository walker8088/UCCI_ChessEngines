/*! \file constant.h
@brief 基本常量定义 
*/

#ifndef CONSTANT_H
#define CONSTANT_H

#include "../utility/bool.h"

///是否为红棋 
#define IsRed(x) (!((x) & 0xf0) && (x))
///是否为黑棋 
#define IsBlack(x) (!(~(x) & 0xf0))
///是否为红棋或无子（用于黑方判断可行着法） 
#define IsRedOrNone(x) (!((x) & 0xf0))
///是否为黑棋或无子（用于红方判断可行着法） 
#define IsBlackOrNone(x) (!(~(x) & 0xf0) || !(x))
///判断是红棋还是黑棋
/*!注意这条只在有棋子时能返回正确结果，NONE和OUT时返回结果无意义。 
*/ 
#define GetColor(x) (!(~(x) & 0xf0))

///棋手（红方／黑方） 
typedef unsigned char PLAYER;
///棋子 
typedef unsigned char CHESSMAN;
///棋盘坐标（16*16表示法） 
typedef unsigned char BDPOINT;
///哈希数（64位） 
typedef long long HASHNUM;

///搜索棋局深度（采用迭代深化搜索后此常量暂不使用）
const int SEARCH_DEPTH = 4; 
///最大搜索棋局深度（用于迭代深化搜索） 
const int MAX_SEARCH_DEPTH = 16;
///静态搜索时最大延伸搜索棋局深度
const int MAX_QUIES_DEPTH = 16;
///每步最长搜索时间（以秒计，用于迭代深化搜索） 
const int LONGEST_SEARCH_TIME = 5; 

///搜索时每层最多保存棋局个数 
const int SEARCH_WIDTH = 80;
///表示当前局面是死局面的评估值，此值应小于我方失去所有棋子的分数
const int NO_BEST_MOVE = -20000;
///表示每步搜索时间超时的评估值（用于迭代深化搜索），此值应小于我方失去所有棋子的分数
const int TIME_OVER = -65432;
///表示当前局面在置换表中不存在的评估值，此值应小于我方失去所有棋子的分数
const int NOT_IN_TT = -60000;
///局面正常情况（不失将帅）一方可能的最高评估值
const int MAX_VALUE_OF_BOARD = 2000;

///棋手编码 
enum Player
{
    RED = 0, BLACK = 1    
};

///棋子编码。注意：用 unsigned char 类型的 position 数组存储棋子编码，
/*!负数将存为其8位二进制补码，如：-2 -> 254。 
   NONE 表示无棋子，OUT 表示棋盘之外的位置。 
*/
enum Chessman
{
    NONE = 0, OUT = 99,
    R_KING = 1, R_ADVISOR = 2, R_BISHOP = 3, R_KNIGHT = 4, R_ROOK = 5, R_CANNON = 6, R_PAWN = 7, 
    B_KING = 255, B_ADVISOR = 254, B_BISHOP = 253, B_KNIGHT = 252, B_ROOK = 251, B_CANNON = 250, B_PAWN = 249
};

///局势编码（开局／中局／残局） 
enum BoardState
{
    START_STATE = 0, MID_STATE = 1, END_STATE = 2    
};

#endif
