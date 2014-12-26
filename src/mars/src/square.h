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

********************************************************************/#pragma once

#include "colour.h"
#include "util.h"


const int FileNb = 16;
const int RankNb = 16;

const int SquareNb = FileNb * RankNb;

const int FileInc = +1;
const int RankInc = +16;

const int FileNone = 0;

const int FileA = 0x3;
const int FileB = 0x4;
const int FileC = 0x5;
const int FileD = 0x6;
const int FileE = 0x7;
const int FileF = 0x8;
const int FileG = 0x9;
const int FileH = 0xA;
const int FileI = 0xB;

const int RankNone = 0;

const int Rank0 = 0x3;
const int Rank1 = 0x4;
const int Rank2 = 0x5;
const int Rank3 = 0x6;
const int Rank4 = 0x7;
const int Rank5 = 0x8;
const int Rank6 = 0x9;
const int Rank7 = 0xA;
const int Rank8 = 0xB;
const int Rank9 = 0xC;

const int SquareNone = 0;

const int A0=0x33, B0=0x34, C0=0x35, D0=0x36, E0=0x37, F0=0x38, G0=0x39, H0=0x3A, I0=0x3B;
const int A1=0x43, B1=0x44, C1=0x45, D1=0x46, E1=0x47, F1=0x48, G1=0x49, H1=0x4A, I1=0x4B;
const int A2=0x53, B2=0x54, C2=0x55, D2=0x56, E2=0x57, F2=0x58, G2=0x59, H2=0x5A, I2=0x5B;
const int A3=0x63, B3=0x64, C3=0x65, D3=0x66, E3=0x67, F3=0x68, G3=0x69, H3=0x6A, I3=0x6B;
const int A4=0x73, B4=0x74, C4=0x75, D4=0x76, E4=0x77, F4=0x78, G4=0x79, H4=0x7A, I4=0x7B;
const int A5=0x83, B5=0x84, C5=0x85, D5=0x86, E5=0x87, F5=0x88, G5=0x89, H5=0x8A, I5=0x8B;
const int A6=0x93, B6=0x94, C6=0x95, D6=0x96, E6=0x97, F6=0x98, G6=0x99, H6=0x9A, I6=0x9B;
const int A7=0xA3, B7=0xA4, C7=0xA5, D7=0xA6, E7=0xA7, F7=0xA8, G7=0xA9, H7=0xAA, I7=0xAB;
const int A8=0xB3, B8=0xB4, C8=0xB5, D8=0xB6, E8=0xB7, F8=0xB8, G8=0xB9, H8=0xBA, I8=0xBB;
const int A9=0xC3, B9=0xC4, C9=0xC5, D9=0xC6, E9=0xC7, F9=0xC8, G9=0xC9, H9=0xCA, I9=0xCB;

typedef int sq_t;

#define SQUARE_FROM_90(square)      (SquareFrom90[square])

extern const int SquareFrom90[90];

extern int SquareTo90[SquareNb];

extern void square_init();