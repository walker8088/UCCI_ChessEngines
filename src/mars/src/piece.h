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

// 将、车、炮、马、象、士、兵
const int KingFlag    =1<<7;
const int AdvisorFlag =1<<8;
const int BishopFlag  =1<<9;
const int KnightFlag  =1<<10;
const int RookFlag    =1<<11;
const int CannonFlag  =1<<12;
const int PawnFlag    =1<<13;

const int SliderFlag  =RookFlag|CannonFlag;
const int RookPawnFlag=RookFlag|PawnFlag;

const int King90   =KingFlag;
const int Advisor90=AdvisorFlag;
const int Bishop90 =BishopFlag;
const int Knight90 =KnightFlag;
const int Rook90   =RookFlag;
const int Cannon90 =CannonFlag;
const int Pawn90   =PawnFlag;

//红方棋子表示 帅仕相马车炮兵
const int RedKing    =King90   | RedFlag;
const int RedAdvisor =Advisor90| RedFlag;
const int RedBishop  =Bishop90 | RedFlag;
const int RedKnight  =Knight90 | RedFlag;
const int RedRook    =Rook90   | RedFlag;
const int RedCannon  =Cannon90 | RedFlag;
const int RedPawn    =Pawn90   | RedFlag;

//黑方棋子表示 将士象马车炮卒
const int BlackKing    =King90   | BlackFlag;
const int BlackAdvisor =Advisor90| BlackFlag;
const int BlackBishop  =Bishop90 | BlackFlag;
const int BlackKnight  =Knight90 | BlackFlag;
const int BlackRook    =Rook90   | BlackFlag;
const int BlackCannon  =Cannon90 | BlackFlag;
const int BlackPawn    =Pawn90   | BlackFlag;

const int PieceNb        = 256;

const int RedKing14     =0;
const int RedAdvisor14  =1;
const int RedBishop14   =2;
const int RedKnight14   =3;
const int RedRook14     =4;
const int RedCannon14   =5;
const int RedPawn14     =6;

const int BlackKing14   =7;
const int BlackAdvisor14=8;
const int BlackBishop14 =9;
const int BlackKnight14 =10;
const int BlackRook14   =11;
const int BlackCannon14 =12;
const int BlackPawn14   =13;

#define PIECE_COLOUR(piece)      (((127 & piece)>>5)-1)
#define PIECE_TYPE(piece)        ((piece)&~127)
/*
#define PIECE_IS_KING(piece)     (((piece)&KingFlag)!=0)
#define PIECE_IS_ADVISOR(piece)  (((piece)&AdvisorFlag)!=0) 
#define PIECE_IS_BISHOP(piece)   (((piece)&BishopFlag)!=0)
#define PIECE_IS_KNIGHT(piece)   (((piece)&KnightFlag)!=0)
#define PIECE_IS_ROOK(piece)     (((piece)&RookFlag)!=0)
#define PIECE_IS_CANNON(piece)   (((piece)&CannonFlag)!=0)
#define PIECE_IS_PAWN(piece)     (((piece)&PawnFlag)!=0)
#define PIECE_IS_SLIDER(piece)   (((piece)&SliderFlag)!=0)  //是否为滑子
#define PIECE_IS_ROOKPAWN(piece) (((piece)&RookPawnFlag)!=0)
*/

#define PIECE_IS_KING(piece)     (((piece)&KingFlag))
#define PIECE_IS_ADVISOR(piece)  (((piece)&AdvisorFlag)) 
#define PIECE_IS_BISHOP(piece)   (((piece)&BishopFlag))
#define PIECE_IS_KNIGHT(piece)   (((piece)&KnightFlag))
#define PIECE_IS_ROOK(piece)     (((piece)&RookFlag))
#define PIECE_IS_CANNON(piece)   (((piece)&CannonFlag))
#define PIECE_IS_PAWN(piece)     (((piece)&PawnFlag))
#define PIECE_IS_SLIDER(piece)   (((piece)&SliderFlag))  //是否为滑子
#define PIECE_IS_ROOKPAWN(piece) (((piece)&RookPawnFlag))

#define PAWN_MAKE(colour)        (PawnMake[colour])

typedef int inc_t;

extern const int PawnMake[ColourNb];
extern const int PieceFrom32[32];
extern const int PieceTo32[32];
extern const int PieceToKey32[32];
extern const int MvvValues[32];