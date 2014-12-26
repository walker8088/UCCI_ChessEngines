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

#include "board.h"
#include "util.h"

struct undo_t {

   bool capture;

   int capture_square;
   int capture_piece;
   //int capture_pos;

   //int pawn_pos;
   //int from;

   int turn;
   int flags;
   int ep_square;
   int ply_nb;

   int cap_sq;

   int opening;
   int endgame;

   //uint16 bit_ranks[16];         // 位行数组，注意用法是"bitRanks[Squares >> 4]"
   //uint16 bit_files[16];         // 位列数组，注意用法是"bitRanks[Squares & 0xf]"

   unsigned long key;      // Zobrist键值
   uint64 lock;            // Zobrist校验锁
};


extern void move_do        ( int move, undo_t * undo);
extern void move_undo      ( int move, const undo_t * undo);

extern void move_do_null   ( undo_t * undo);
extern void move_undo_null ( const undo_t * undo);
