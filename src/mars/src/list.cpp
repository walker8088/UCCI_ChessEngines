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
#include ".\list.h"
#include "move.h"

bool list_is_ok(const list_t * list) {

   if (list == NULL) return false;

   if (list->size < 0 || list->size >= ListSize) return false;

   return true;
}


void list_remove(list_t * list, int pos) {

   int i;

   
   ASSERT(pos>=0&&pos<list->size);

   for (i = pos; i < list->size-1; i++) {
      list->move[i] = list->move[i+1];
      list->value[i] = list->value[i+1];
   }

   list->size--;
}

// list_copy()

void list_copy(list_t * dst, const list_t * src) {

   int i;

   ASSERT(dst!=NULL);
   ASSERT(list_is_ok(src));

   dst->size = src->size;

   for (i = 0; i < src->size; i++) {
      dst->move[i] = src->move[i];
      dst->value[i] = src->value[i];
   }
}

// list_sort()

void list_sort(list_t * list) {

   int size;
   int i, j;
   int move, value;

   

   // init

   size = list->size;
   list->value[size] = -32768; // HACK: sentinel

   // insert sort (stable)

   for (i = size-2; i >= 0; i--) {

      move = list->move[i];
      value = list->value[i];

      for (j = i; value < list->value[j+1]; j++) {
         list->move[j] = list->move[j+1];
         list->value[j] = list->value[j+1];
      }

      ASSERT(j<size);

      list->move[j] = move;
      list->value[j] = value;
   }

   // debug

   if (DEBUG) {
      for (i = 0; i < size-1; i++) {
         ASSERT(list->value[i]>=list->value[i+1]);
      }
   }
}

// list_contain()

bool list_contain(const list_t * list, int move) {

   int i;

   
   ASSERT(move_is_ok(move));

   for (i = 0; i < list->size; i++) {
      if (list->move[i] == move) return true;
   }

   return false;
}

// list_note()

void list_note(list_t * list) {

   int i, move;

   

   for (i = 0; i < list->size; i++) {
      move = list->move[i];
      ASSERT(move_is_ok(move));
      list->value[i] = -move_order(move);
   }
}

// list_filter()

void list_filter(list_t * list, move_test_t test, bool keep) {

   int pos;
   int i, move, value;

   ASSERT(list!=NULL);
   
   ASSERT(test!=NULL);
   ASSERT(keep==true||keep==false);

   pos = 0;

   for (i = 0; i < LIST_SIZE(list); i++) {

      ASSERT(pos>=0&&pos<=i);

      move = LIST_MOVE(list,i);
      value = LIST_VALUE(list,i);

      if ((*test)(move) == keep) {
         list->move[pos] = move;
         list->value[pos] = value;
         pos++;
      }
   }

   ASSERT(pos>=0&&pos<=LIST_SIZE(list));
   list->size = pos;

   // debug

   
}

// end of list.cpp

