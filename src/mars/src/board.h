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
#pragma once
#include "colour.h"
#include "piece.h"
#include "util.h"
#include "square.h"

const int Empty = 0;
const int Edge = Bishop90; // HACK: uncoloured knight

const int FlagsNone = 0;
const int StackSize = 4096;

extern int piece_material[ColourNb]; // Thomas
extern int Square[SquareNb];         //棋子位数据
extern sq_t Piece[32];               //棋子类型0~15为红，16~31为黑，值为0时，表示棋子已经被吃
extern int Number[16];               // 前14位为棋子数统计
extern uint16 BitRanks[16];         // 位行数组，注意用法是"bitRanks[Squares >> 4]"
extern uint16 BitFiles[16];         // 位列数组，注意用法是"bitRanks[Squares & 0xf]"
extern int Turn;                     //0为红，1为黑
extern int Opening;
extern int Endgame;
extern unsigned long Key;           // Zobrist键值
extern uint64 Lock;                 // Zobrist校验锁
extern uint64 Stack[StackSize];
extern int Piece_size[ColourNb];
extern int Sp;
extern int Piece_nb;
extern int Ply_nb;

extern const int BitPieceTypes[48];
extern bool board_is_check();
extern bool board_is_mate();
extern void board_clear();
extern void board_init_list();
//extern void board_copy(board_t * dst, const board_t * src);
