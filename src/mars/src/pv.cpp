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
#include ".\pv.h"

#include <cstring>

#include "board.h"
#include "move.h"
#include "move_do.h"
//#include "pv.h"
#include "util.h"

// functions

// pv_is_ok()

bool pv_is_ok(const mv_t pv[]) {

   int pos;
   int move;

   if (pv == NULL) return false;

   for (pos = 0; true; pos++) {

      if (pos >= 256) return false;
      move = pv[pos];

      if (move == MoveNone) return true;
     // if (!move_is_ok(move)) return false;
   }

   return true;
}

// pv_copy()

void pv_copy(mv_t dst[], const mv_t src[]) {

   ASSERT(pv_is_ok(src));
   ASSERT(dst!=NULL);

   while ((*dst++ = *src++) != MoveNone)
      ;
}

// pv_cat()

void pv_cat(mv_t dst[], const mv_t src[], int move) {

   ASSERT(pv_is_ok(src));
   ASSERT(dst!=NULL);

   *dst++ = move;

   while ((*dst++ = *src++) != MoveNone)
      ;
}

// pv_to_string()
/*
bool pv_to_string(const mv_t pv[], char string[], int size) {

   int pos;
   int move;

   ASSERT(pv_is_ok(pv));
   ASSERT(string!=NULL);
   ASSERT(size>=512);

   // init

   if (size < 512) return false;

   pos = 0;

   // loop

   while ((move = *pv++) != MoveNone) {

      if (pos != 0) string[pos++] = ' ';

      move_to_string(move,&string[pos],size-pos);
      pos += strlen(&string[pos]);
   }

   string[pos] = '\0';

   return true;
}
*/