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
#include "pregen.h"

struct attack_t {
   int dn;
   int ds[8];
//   int di[2+1];
};

#define ATTACK_IN_CHECK(attack)           ((attack)->dn!=0)

extern const char  AttackTab[512];
//extern void attack_init();

extern const char ProtectTab[512];

extern bool is_attacked   ( int to, int colour);

//extern bool line_is_empty ( int from, int to);

extern bool is_pinned(int colour);
extern bool is_piece_pinned     (int square, int colour);
extern bool is_cannon_pinned(int square, int colour);
extern bool is_knight_pinned(int move, int colour);
extern bool is_filter_pinned(int move, int colour);

//extern bool attack_is_ok  (const attack_t * attack);
extern void attack_set    (attack_t * attack);//将军着法

extern bool is_in_check(int colour);//将军判断
//extern bool piece_attack_king ( int piece, int from, int king);

extern bool is_checked(int to,int colour);