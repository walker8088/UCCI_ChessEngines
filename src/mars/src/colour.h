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
// colour.h

#ifndef COLOUR_H
#define COLOUR_H

// includes

#include "util.h"

// constants

const int ColourNone = -1;
const int Red = 0;
const int Black = 1;
const int ColourNb = 2;

const int RedFlag = 1 << 5;
const int BlackFlag = 1 << 6;

// macros

//#define COLOUR_IS_OK(colour)    (((colour)&~1)==0)

#define COLOUR_IS_Red(colour) ((colour)==Red)
#define COLOUR_IS_BLACK(colour) ((colour)!=Red)

#define COLOUR_IS(piece,colour) (FLAG_IS((piece),COLOUR_FLAG(colour)))
#define FLAG_IS(piece,flag)     (((piece)&(flag))!=0)

#define COLOUR_OPP(colour)      ((colour)^(Red^Black))
#define COLOUR_FLAG(colour)     ((32<<colour))

#endif // !defined COLOUR_H

// end of colour.h

