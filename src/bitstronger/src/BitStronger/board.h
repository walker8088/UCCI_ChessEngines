/*! \file board.h
@brief 棋盘表示类
*/

#ifndef BOARD_H
#define BOARD_H

#include <string.h>
#include "../utility/bool.h"
#include "constant.h"
#include "movement.h"
#include "hashnum.h"

///棋盘表示类
/*!
　　兼顾算法的效率与实现的方便，我们采用的是目前流行的另一种"16×16棋盘表示法"（在实现时为运算方便，用长度为256的一维数组表示）。这种方法的存储原理与9×10的二维数组一致，但采用16×16矩阵的中间一块表示棋盘。这样做的好处，一方面行、列坐标增量分别为0x10、0x01，可以用各类位运算表示棋盘的变化，比在9×10的二维数组中做乘除9、10的运算快得多；另一方面周围一圈"哨兵"可以避免使用大量边界条件检测，自动防止棋子走出棋盘边界。

　　棋盘表示类中只有"当面局面"和"这一局面轮到谁走棋了？"是必要信息，别的数据成员全都是为了方便其它模块的快速计算而设置。比如记录将（帅）位置可以在着法生成模块中快速得知某一着法是否造成将帅照面；记录当前局面的Zobrist数（即棋盘的哈希表示）用以在开局库中快速检索这一局面。
*/
class Board
{
public:
    /*----------成员变量----------*/
    ///这一局面轮到谁走棋了？ 
    PLAYER player;
    ///走棋一方各种棋子可行的走法数目
    /*!第一个元素无意义，后面的按棋子代码顺序排列，黑方也记绝对值1~7 
       仅在执行 MoveMaker::Make 后有意义 
    */
    int numOfMove[8];
    ///当面局面
    /*!使用16*16表示法，利于快速生成着法，同时防止棋子走出棋盘边界。 
       参考：http://www.elephantbase.net/computer/eleeye_struct.htm
    */
    CHESSMAN position[256];
    ///当前局面的 Zobrist 数 
    HASHNUM zobrist;
    ///当前局面棋子个数，用于判断是开局／中局／残局
    int numOfChess;
    ///记录将（帅）位置 
    BDPOINT kingpos[2];
    ///记录得到此局面的上一个着法 
    /*!只在执行 Board::AddMovement 之后有意义，且不可由 Board::DelMovement 回溯。
    */
    Movement lastmove;
    ///双方没有吃子的走棋步数（半回合数） 
    int non_eat_steps;
    ///当前的回合数
    int bouts;
    /*----------成员函数----------*/
    ///默认构造函数——暂不使用 
    Board();
    ///由FEN串生成棋盘的构造函数
    Board(const char * fen, int nMoveNum, long * lpdwCoordList);
    ///由FEN串生成棋盘
    /*!FEN串定义参见：http://www.elephantbase.net/protocol/cchess_fen.htm
    */ 
    void FromFen(const char * fen, int nMoveNum, long * lpdwCoordList);
    ///添加一个着法后改变局面
    void AddMovement(const Movement & move); 
    ///删除一个着法后改变局面
    void DelMovement(const Movement & move); 
    ///验证一个4字节着法字串（如“h0i2”）对于本局面是否可行
    /*!只验证起止点棋子合法性，不验证棋规合法性。
       坐标格式参见：http://www.elephantbase.net/protocol/cchess_move.htm。 
    */
    Bool CanGo(const char * str); 
private:
    /*----------私有函数----------*/
    ///清空棋盘 
    void ClearBoard();
};

#endif
