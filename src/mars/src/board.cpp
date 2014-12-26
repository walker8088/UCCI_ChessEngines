/********************************************************************

	----------------------------------------------------------------
    软件许可证 － GPL
	版权所有 (C) 2005-2008 极速人工智能研究
	----------------------------------------------------------------
	这一程序是自由软件，你可以遵照自由软件基金会出版的GNU 通用公共许
	可证条款来修改和重新发布这一程序。或者用许可证的第二版，或者（根
	据你的选择）用任何更新的版本。

    发布这一程序的目的是希望它有用，但没有任何担保。甚至没有适合特定
	目地的隐含的担保。更详细的情况请参阅GNU通用公共许可证。

    你应该已经和程序一起收到一份GNU通用公共许可证的副本（本目录
	GPL.txt文件）。如果还没有，写信给：
    The Free Software Foundation, Inc.,  675  Mass Ave,  Cambridge,
    MA02139,  USA
	----------------------------------------------------------------
	如果你在使用本软件时有什么问题或建议，请用以下地址与我们取得联系：

			http://www.jsmaster.com

	或发信到：

			jschess##163.com
	----------------------------------------------------------------
	本文件用途：	略
	
	  
	本文件编写人：	
				顾剑辉			jschess##163.com
		
	本文件版本：	03
	最后修改于：	2006-1-16
		  
	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	地址收集软件。
	----------------------------------------------------------------
	修正历史：
			
		  2006-1		第一版发布

********************************************************************/
#include ".\board.h"
#include "attack.h"
 
int piece_material[ColourNb]; // Thomas
int Square[SquareNb];         //棋子位数据
sq_t Piece[32];               //棋子类型0~15为红，16~31为黑，值为0时，表示棋子已经被吃
int Number[16];               // 前14位为棋子数统计
uint16 BitRanks[16];         // 位行数组，注意用法是"bitRanks[Squares >> 4]"
uint16 BitFiles[16];         // 位列数组，注意用法是"bitRanks[Squares & 0xf]"
int Turn;                     //0为红，1为黑
int Opening;
int Endgame;
unsigned long Key;           // Zobrist键值
uint64 Lock;                 // Zobrist校验锁
uint64 Stack[StackSize];
int Piece_size[ColourNb];
int Sp;
int Piece_nb;
int Ply_nb;

bool board_is_check() {

   return is_in_check(Turn);
}

bool board_is_mate() {

   attack_t attack[1];

   attack_set(attack);

   if (!ATTACK_IN_CHECK(attack)) return false; // not in check => not mate

   return true; // in check and no legal move => mate
}

void board_clear()
{
   int sq, sq_90;

   for (sq = 0; sq < 16; sq ++)
      BitRanks[sq] = 0;
   
   for (sq = 0; sq < 16; sq ++)
      BitFiles[sq] = 0;

   for (sq = 0; sq < SquareNb; sq++) {
      Square[sq] = Edge;
   }

   for (sq_90 = 0; sq_90 < 90; sq_90++) {
      sq = SQUARE_FROM_90(sq_90);
      Square[sq] = Empty;
   }
   
   for (sq=0; sq < 32; sq ++)
   {
	   Piece[sq]=0;
   }
   Piece_nb = 0;

   for (sq = 0;sq <14; sq ++)
	   Number[sq]=0;

   Piece_size[0]=Piece_size[1]=0;

   Opening=Endgame=0;

   Turn=0;

   Key=0;
   Lock=0;

}
/*
void board_copy(board_t * dst, const board_t * src)
{
	*dst = *src;
}
*/

void board_init_list() 
{

		
}
