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
#include ".\move.h"

int move_order(int move) {

  // ASSERT(move_is_ok(move));
  return move;
  // return ((move & 07777) << 2) | ((move >> 12) & 3);
}

bool move_is_tactical(int move) {

   ASSERT(move_is_ok(move));
   

  // return (move & (1 << 15)) != 0 || Square[MOVE_TO(move)] != Empty; // HACK
   return Square[MOVE_TO(move)] != Empty;
}

// move_capture()

int move_capture(int move) {

   ASSERT(move_is_ok(move));
   

   return Square[MOVE_TO(move)];
}

const long move_to_string(int move)
{
	int from,to;
    char ret_val[4];

	from=MOVE_FROM(move);
	to=MOVE_TO(move);

    ret_val[0] = (from & 0xf) + 'a' - 3;
    ret_val[1] = '9' + 3-(from>>4);
    ret_val[2] = (to & 0xf) + 'a' - 3;
    ret_val[3] = '9' + 3-(to>>4);
    return *(long *) ret_val;
  }
int move_from_string(const unsigned int move_str) 
{
	unsigned char *argptr = (unsigned char *) &move_str;
	unsigned int src = ((12-argptr[1]+'0')<<4) + argptr[0]-'a'+3;	// y0x0
	unsigned int dst = ((12-argptr[3]+'0')<<4) + argptr[2]-'a'+3;	// y1x1
	return ( src << 8 ) | dst;										// y0x0y1x1
}