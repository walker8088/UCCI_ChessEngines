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

#include "attack.h"
#include "board.h"
#include "list.h"
#include "util.h"

struct sort_t {
   int depth;
   int height;
   int trans_killer;
   int killer_1;
   int killer_2;
   int killer_3;
   int killer_4;
   int gen;
   int test;
   int pos;
   int value;
   const attack_t * attack;
   list_t list[1];
   list_t bad[1];
};

// functions

extern void sort_init    ();

extern void sort_init    (sort_t * sort,  const attack_t * attack, int depth, int height, int trans_killer);
extern int  sort_next    (sort_t * sort);

extern void sort_init_qs (sort_t * sort,  const attack_t * attack, bool check);
extern int  sort_next_qs (sort_t * sort);

extern void good_move    (int move,  int depth, int height);

extern void history_good (int move);
extern void history_bad  (int move);

extern void note_moves   (list_t * list,  int height, int trans_killer);


extern void test_gen_good_capture(list_t * list);
extern void test_sort(sort_t * sort);

//extern bool capture_is_good   (int move);
