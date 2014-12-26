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
#include ".\see.h"
#include "pregen.h"
#include "attack.h"
#include "value.h"
#include "move.h"
#include <stdio.h>

//SEE 是一种寻找好的吃子着法的算法
//

// macros

#define ALIST_CLEAR(alist) ((alist)->size=0)

// types

struct alist_t {
   int size;
   int square[15];
};

struct alists_t {
   alist_t alist[ColourNb][1];
};


static int  see_rec       (alists_t * alists,  int colour, int to, int piece_value);

static void alist_build   (alist_t * alist,  int square,int to, int colour);
static void alists_hidden (alists_t * alists,  int from, int to);

static void alist_clear   (alist_t * alist);
static void alist_add     (alist_t * alist, int square);
static void alist_remove  (alist_t * alist, int pos);
static int  alist_pop     (alist_t * alist);

// see_move()

int see_move(int move) {

   int att, def;
   int from, to;
   alists_t alists[1];
   int value, piece_value;
   int piece, capture;
   alist_t * alist;
   int pos;

   ASSERT(move_is_ok(move));
   

  // printf("start see_move %x %d,%d\n",move,Turn,Square[move>>8] & 31);
   // init

   from = MOVE_FROM(move);
   to = MOVE_TO(move);

   // move the piece

   piece_value = 0;

   piece = Square[from] & 31;
   ASSERT(piece_is_ok(piece));

   att = PIECE_COLOUR(Square[from]);
   def = COLOUR_OPP(att);


   piece_value += VALUE_PIECE(piece);

   // clear attacker lists

   ALIST_CLEAR(alists->alist[Black]);
   ALIST_CLEAR(alists->alist[Red]);

   // find hidden attackers隐藏的

   alists_hidden(alists,from,to);

  // for(i=0;i<alists->alist[0]->size;++i)
  // {
	//   printf("%x\n",alists->alist[0]->square[i]);
 //  }
   // capture the piece

   value = 0;

   capture = Square[to];

   if (capture != Empty) {

      ASSERT(piece_is_ok(capture));
      ASSERT(COLOUR_IS(capture,def));

      value += VALUE_PIECE(Square[to] & 31);
   }
  // printf("\n");

   // build defender list防卫者

   alist = alists->alist[def];
   //printf("***\n");
   alist_build(alist,from,to,def);
   // printf("***1\n");
   if (alist->size == 0)
   {
	//   printf("stop see\n\n");
	   return value; // no defender => stop SEE
   }
  
   //for(i=0;i<alist->size;++i)
   //{
	//   printf("(%x),",alist->square[i]);
   //}
   // build attacker list攻击者
   alist = alists->alist[att];
//printf("***8\n");
   alist_build(alist,from,to,att);

   //for(i=0;i<alist->size;++i)
   //{
//	   printf("[%x],",alist->square[i]);
   //}

   // remove the moved piece (if it's an attacker)

   for (pos = 0; pos < alist->size && alist->square[pos] != from; pos++)
      ;

   if (pos < alist->size) alist_remove(alist,pos);

   // SEE suuearch
//printf("***ii\n");
   value -= see_rec(alists,def,to,piece_value);
//   printf("\nend see move %x value=%d \n\n",move,value);
   return value;
}

// see_square()

int see_square( int to, int colour) {

   int att, def;
   alists_t alists[1];
   alist_t * alist;
   int piece_value;
   int piece;

   
   //ASSERT(SQUARE_IS_OK(to));
   //ASSERT(COLOUR_IS_OK(colour));

   ASSERT(COLOUR_IS(Square[to],COLOUR_OPP(colour)));

   // build attacker list

   att = colour;
   alist = alists->alist[att];

   ALIST_CLEAR(alist);
   alist_build(alist,0,to,att);

   if (alist->size == 0) return 0; // no attacker => stop SEE

   // build defender list

   def = COLOUR_OPP(att);
   alist = alists->alist[def];

   ALIST_CLEAR(alist);
   alist_build(alist,0,to,def);

   // captured piece

   piece = Square[to] & 31;
   //ASSERT(piece_is_ok(piece));
   ASSERT(COLOUR_IS(piece,def));

   piece_value = VALUE_PIECE(piece);

   // SEE search

   return see_rec(alists,att,to,piece_value);
}

// see_rec()

static int see_rec(alists_t * alists,  int colour, int to, int piece_value) {

   int from, piece;
   int value;

   ASSERT(alists!=NULL);
   
   ASSERT(COLOUR_IS_OK(colour));
   //ASSERT(SQUARE_IS_OK(to));
   ASSERT(piece_value>0);

   // find the least valuable attacker

   from = alist_pop(alists->alist[colour]);
   if (from == SquareNone) return 0; // no more attackers

   // find hidden attackers

   alists_hidden(alists,from,to);

   // calculate the capture value

   value = +piece_value; // captured piece
   if (value == ValueKing) return value; // do not allow an answer to a king capture

   piece = Square[from] & 31;

   piece_value = VALUE_PIECE(piece);

   value -= see_rec(alists,COLOUR_OPP(colour),to,piece_value);

   if (value < 0) value = 0;

   return value;
}

// alist_build()

static void alist_build(alist_t * alist,  int square,int to, int colour) {

	int i, piece_tag, from, disp, x, y,pawn;
    slide_mask_t *rank_mask_ptr, *file_mask_ptr;
    // 棋子保护判断包括以下几个步骤：
    //if(Square[square]==0 ||Square[square]==Edge)
	//   return ;

    piece_tag =colour<<4;

	//pawn=PawnFlag|(1<<(5+colour));
    pawn=PawnFlag | (32 <<colour);
  if (colour == 0 ? (to & 0x80) != 0 : (to & 0x80) == 0) {
    if (c_InCity[to]) {

      // 1. 判断受到帅(将)的保护
      from = Piece[piece_tag];
      if (from != 0) {
        if (ProtectTab[from - to + 256] == 1) {
          alist_add(alist,from);
        }
      }

      // 2. 判断受到仕(士)的保护
      for (i = 1; i <= 2; i ++) {
        from = Piece[piece_tag + i];
        if (from != 0 ) {
          if (ProtectTab[from - to + 256] == 2) {
            alist_add(alist,from);
          }
        }
      }
    }

    // 3. 判断受到相(象)的保护
    for (i = 3; i <= 4; i ++) {
      from = Piece[piece_tag + i];
      if (from != 0 ) {
        if (ProtectTab[from - to + 256] == 3 && Square[(to + from) >> 1] == 0) {
          alist_add(alist,from);
        }
      }
    }
  } else {

    // 4. 判断受到过河兵(卒)横向的保护
    for (from = to - 1; from <= to + 1; from += 2) {
		if((Square[from] & pawn) ==pawn)
		{
			alist_add(alist,from);
		}
    }
  }

  // 5. 判断受到过河兵(卒)纵向的保护
  from = to + (colour == 0 ? 16 : -16);
  if((Square[from] & pawn) ==pawn)
  {
	  alist_add(alist,from);
  }

  // 6. 判断受到马的保护
  for (i = 5; i <= 6; i ++) {
    from = Piece[piece_tag + i];
    if (from != 0) {
      disp = c_HorseLegTab[to - from + 256];
      if (disp != 0 && !Square[from + disp]) {
        alist_add(alist,from);
      }
    }
  }

  x = to & 0xf;
  y = to >> 4;
  rank_mask_ptr = g_RankMaskTab[x - 3] + BitRanks[y];
  file_mask_ptr = g_FileMaskTab[y - 3] + BitFiles[x];

// 8. 判断受到炮的保护，参阅"struct.cpp"里的"Checked()"函数
  for (i = 9; i <= 10; i ++) {
    from = Piece[piece_tag + i];
    if (from!=0 && from != to) {
      if (x == (from & 0xf)) {
        if ((file_mask_ptr->cannon_cap & g_BitFileMask[from]) != 0) {
		  if(x==(square & 0xf) && (from>square && square>to) || (from<square && square<to))
		  ;
		  else
          alist_add(alist,from);
        }
      } else if (y == (from >> 4)) {
        if ((rank_mask_ptr->cannon_cap & g_BitRankMask[from]) != 0) {
		  if(y==(square >> 4) && (from>square && square>to) || (from<square && square<to))
		  ;
		  else
          alist_add(alist,from);
        }
      }
    }
  }

  // 7. 判断受到车的保护，参阅"struct.cpp"里的"Checked()"函数
  for (i = 7; i <= 8; i ++) {
    from = Piece[piece_tag + i];
    if (from != 0 && from != to) {
      if (x == (from & 0xf)) {
        if ((file_mask_ptr->rook_cap & g_BitFileMask[from]) != 0) {
          alist_add(alist,from);
        }
      } else if (y == (from >> 4)) {
        if ((rank_mask_ptr->rook_cap & g_BitRankMask[from]) != 0) {
          alist_add(alist,from);
        }
      }
    }
  }

  
 
}

// alists_hidden()
//滑子后方的棋子还是同一类型的滑子
static void alists_hidden(alists_t * alists,  int from, int to) 
{
   int i,x,y,x1,y1,disp,piece,to1;
   uint8 * ptr;
   slide_move_t *slide_move_ptr;

   ASSERT(alists!=NULL);
   
   
   piece=Square[from];
   if (PIECE_IS_SLIDER(piece)) {//车---车
      x = from & 0xf;           //炮---炮
      y = from >> 4;
      x1 = to & 0xf;
      y1 = to >> 4;
      if(y==y1)
	  {
         disp = y << 4;
         slide_move_ptr = g_RankMoveTab[x - 3] + BitRanks[y];
         ptr = slide_move_ptr->rook_cap;
		 
		 if(x>x1)
		 {
            to1 = ptr[0] + disp;
            if (to1 != from) {

				if((Square[from]&PIECE_TYPE(Square[to1]))!=0)
                    alist_add(alists->alist[PIECE_COLOUR(Square[to1])],to1);          
             }
		  }else
		 {
            to1 = ptr[1] + disp;
            if (to1 != from) {
           
               	   if((Square[from]&PIECE_TYPE(Square[to1]))!=0)
                    alist_add(alists->alist[PIECE_COLOUR(Square[to1])],to1); 
            }
		  }
	    }
	  
	  if(x==x1)
	  {
		 
         disp = x;
         slide_move_ptr = g_FileMoveTab[y - 3] + BitFiles[x];
         ptr = slide_move_ptr->rook_cap;
         
		 if(y>y1)
		 {
             to1 = ptr[0] + disp;
             if (to1 != from) {
           
               		if((Square[from]&PIECE_TYPE(Square[to1]))!=0)
                       alist_add(alists->alist[PIECE_COLOUR(Square[to1])],to1);
           
             }
		 }
		 else
		 {

              to1 = ptr[1] + disp;
              if (to1 != from) {
          
               		if((Square[from]&PIECE_TYPE(Square[to1]))!=0)
                         alist_add(alists->alist[PIECE_COLOUR(Square[to1])],to1);
           
               }
		 }
	  }
	  
    }
   if(PIECE_IS_ROOKPAWN(piece)) //车兵--X-炮
   {
	  x = from & 0xf;
      y = from >> 4;
      x1 = to & 0xf;
      y1 = to >> 4;
      if(y==y1)
	  {
         disp = y << 4;
         slide_move_ptr = g_RankMoveTab[x - 3] + BitRanks[y];
         ptr = slide_move_ptr->cannon_cap;
		 
		 if(x>x1)
		 {
            to1 = ptr[0] + disp;
            if (to1 != from) {

				if(PIECE_IS_CANNON(Square[to1]))
                    alist_add(alists->alist[PIECE_COLOUR(Square[to1])],to1);          
             }
		  }else
		 {
            to1 = ptr[1] + disp;
            if (to1 != from) {
               	if(PIECE_IS_CANNON(Square[to1]))
                    alist_add(alists->alist[PIECE_COLOUR(Square[to1])],to1); 
            }
		  }
	    }
	  
	  if(x==x1)
	  {
		 
         disp = x;
         slide_move_ptr = g_FileMoveTab[y - 3] + BitFiles[x];
         ptr = slide_move_ptr->cannon_cap;
         
		 if(y>y1)
		 {
             to1 = ptr[0] + disp;
             if (to1 != from) {
               		if(PIECE_IS_CANNON(Square[to1]))
                       alist_add(alists->alist[PIECE_COLOUR(Square[to1])],to1);
           
             }
		 }
		 else
		 {

              to1 = ptr[1] + disp;
              if (to1 != from) {
          
               		if(PIECE_IS_CANNON(Square[to1]))
                         alist_add(alists->alist[PIECE_COLOUR(Square[to1])],to1);
           
               }
		 }
	  }
   }
   if(PIECE_IS_ADVISOR(piece))//士-----马
   {
        for (i = 5; i <= 6; i ++) {
            to1 = Piece[i];
            if (to1 != 0) {
                 disp = c_HorseLegTab[to - to1 + 256];
                 if (disp != 0 && to1 + disp==from) {
                     alist_add(alists->alist[0],to1);
                 }
             }
         }
                for (i = 21; i <= 22; i ++) {
            to1 = Piece[i];
            if (to1 != 0) {
                 disp = c_HorseLegTab[to - to1 + 256];
                 if (disp != 0 && to1 + disp==from) {
                     alist_add(alists->alist[1],to1);
                 }
             }
       }

   }
}

// alist_clear()

static void alist_clear(alist_t * alist) {

   ASSERT(alist!=NULL);

   alist->size = 0;
}

// alist_add()

static void alist_add(alist_t * alist, int square) {

   int mvv;
   int size, pos;

   ASSERT(alist!=NULL);
   ASSERT(SQUARE_IS_OK(square));
   

   // insert in MV order

   //piece = Square[square];
   //将其转化为MV值
   
   //printf("vv%d,%x,",alist->size,square);
   mvv=MvvValues[Square[square] & 31];

   size = ++alist->size; // HACK
   ASSERT(size>0&&size<16);

   for (pos = size-1; pos > 0 && mvv > MvvValues[Square[alist->square[pos-1]]& 31]; pos--) { // HACK
      ASSERT(pos>0&&pos<size);
      alist->square[pos] = alist->square[pos-1];
   }
   ASSERT(pos>=0&&pos<size);
   alist->square[pos] = square;
}

// alist_remove()

static void alist_remove(alist_t * alist, int pos) {

   int size, i;

   ASSERT(alist!=NULL);
   ASSERT(pos>=0&&pos<alist->size);

   size = alist->size--; // HACK
   ASSERT(size>=1);

   ASSERT(pos>=0&&pos<size);

   for (i = pos; i < size-1; i++) {
      ASSERT(i>=0&&i<size-1);
      alist->square[i] = alist->square[i+1];
   }
}

// alist_pop()

static int alist_pop(alist_t * alist) {

   int sq;
   int size;

   ASSERT(alist!=NULL);
   

   sq = SquareNone;

   size = alist->size;

   if (size != 0) {
      size--;
      ASSERT(size>=0);
      sq = alist->square[size];
      alist->size = size;
   }

   return sq;
}

// end of see.cpp


