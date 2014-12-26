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
#include ".\attack.h"
#include "piece.h"
#include "move.h"

const char ProtectTab[512] = {
                       0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0
};
//if (ProtectTab[from - to + 256] == 1) {

const char AttackTab[512] = {
                          0,  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,  0,  0,-16,  0,  0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,  0,  0,-16,  0,  0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,  0,  0,-16,  0,  0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,  0,  0,-16,  0,  0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,  0,  0,-16,  0,  0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,  0,  0,-16,  0,  0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,  0,  0,-16,  0,  0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,  0,  0,-16,  0,  0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,  0,  0,-16,  0,  0, 0, 0, 0, 0, 0, 0,
  0, 0,-1, -1,-1, -1,-1,  0,  1,  1, 1, 1, 1, 0, 0, 0,
  0, 0, 0, 0, 0,  0,  0, 16,  0,  0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,  0,  0, 16,  0,  0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,  0,  0, 16,  0,  0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,  0,  0, 16,  0,  0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,  0,  0, 16,  0,  0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,  0,  0, 16,  0,  0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,  0,  0, 16,  0,  0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,  0,  0, 16,  0,  0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,  0,  0, 16,  0,  0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,  0,  0
};

/*

 0, 0, 0, 0, 0,  0,-33,-16,-31,  0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,-18,  0,-16,  0,-14, 0, 0, 0, 0, 0, 0,
  0, 0,-1, -1,-1, -1,-1,  0,  1,  1, 1, 1, 1, 0, 0, 0,
  0, 0, 0, 0, 0, 14,  0, 16,  0, 18, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,  0, 31, 16, 33,  0, 0, 0, 0, 0, 0, 0,
const int c_Rook[4] = {-16, -1, 1, 16};
const int c_Knight[8] = {-33, -31, -18, -14, 14, 18, 31, 33};

int AttackTab[512];

void attack_init()
{
	int i;
	int delta, inc;
    int piece;
    int dir, dist;
	for(i=0;i<512;++i)
	{
		AttackTab[i]=0;
	}
	for (dir = 0; dir < 8; dir++) {

      delta = c_Knight[dir];
      AttackTab[256+delta] = delta;
   }

   for (dir = 0; dir < 4; dir++) {

      inc = c_Rook[dir];

      for (dist = 1; dist < 10; dist++) {

         delta = inc*dist;
         AttackTab[256+delta] = inc;
      }
   }

}
*/

//to 是否受到colour颜色的攻击
bool is_attacked(int to, int colour) 
{
	int i, piece_tag, from, disp, x, y,pawn;
    slide_mask_t *rank_mask_ptr, *file_mask_ptr;
    // 棋子保护判断包括以下几个步骤：

    piece_tag =colour<<4;

	//pawn=PawnFlag|(1<<(5+colour));
    pawn=PawnFlag | (32 <<colour);

  if (colour == 0 ? (to & 0x80) != 0 : (to & 0x80) == 0) {
    if (c_InCity[to]) {

      // 1. 判断受到帅(将)的保护
      from = Piece[piece_tag];
      if (from != 0) {
        if (ProtectTab[from - to + 256] == 1) {
          return true;
        }
      }

      // 2. 判断受到仕(士)的保护
      for (i = 1; i <= 2; i ++) {
        from = Piece[piece_tag + i];
        if (from != 0 ) {
          if (ProtectTab[from - to + 256] == 2) {
            return true;
          }
        }
      }
    }

    // 3. 判断受到相(象)的保护
    for (i = 3; i <= 4; i ++) {
      from = Piece[piece_tag + i];
      if (from != 0 ) {
        if (ProtectTab[from - to + 256] == 3 && Square[(to + from) >> 1] == 0) {
          return true;
        }
      }
    }
  } else {

    // 4. 判断受到过河兵(卒)横向的保护
    for (from = to - 1; from <= to + 1; from += 2) {
		if((Square[from]&pawn)==pawn)
		{
			return true;
		}
    }
  }

  // 5. 判断受到过河兵(卒)纵向的保护
  from = to + (colour == 0 ? 16 : -16);
  if((Square[from]&pawn)==pawn)
  {
	  return true;
  }

  // 6. 判断受到马的保护
  for (i = 5; i <= 6; i ++) {
    from = Piece[piece_tag + i];
    if (from != 0) {
      disp = c_HorseLegTab[to - from + 256];
      if (disp != 0 && Square[from + disp]==0) {
        return true;
      }
    }
  }

  x = to & 0xf;
  y = to >> 4;
  rank_mask_ptr = g_RankMaskTab[x - 3] + BitRanks[y];
  file_mask_ptr = g_FileMaskTab[y - 3] + BitFiles[x];

  // 7. 判断受到车的保护，参阅"struct.cpp"里的"Checked()"函数
  for (i = 7; i <= 8; i ++) {
    from = Piece[piece_tag + i];
    if (from != 0 && from != to) {
      if (x == (from & 0xf)) {
        if ((file_mask_ptr->rook_cap & g_BitFileMask[from]) != 0) {
          return true;
        }
      } else if (y == (from >> 4)) {
        if ((rank_mask_ptr->rook_cap & g_BitRankMask[from]) != 0) {
          return true;
        }
      }
    }
  }

  // 8. 判断受到炮的保护，参阅"struct.cpp"里的"Checked()"函数
  for (i = 9; i <= 10; i ++) {
    from = Piece[piece_tag + i];
    if (from!=0 && from != to) {
      if (x == (from & 0xf)) {
        if ((file_mask_ptr->cannon_cap & g_BitFileMask[from]) != 0) {
          return true;
        }
      } else if (y == (from >> 4)) {
        if ((rank_mask_ptr->cannon_cap & g_BitRankMask[from]) != 0) {
          return true;
        }
      }
    }
  }
  return false;
}
//判断将是否受到牵制
bool is_pinned(int colour)
{
	int i, piece_tag, from,to, disp, x, y,me_flag;
    slide_mask_t *rank_mask_ptr, *file_mask_ptr;

    piece_tag =16-(colour<<4);
	to=Piece[(colour<<4)];
 
	me_flag=COLOUR_FLAG(colour);

    x = to & 0xf;
    y = to >> 4;
    rank_mask_ptr = g_RankMaskTab[x - 3] + BitRanks[y];
    file_mask_ptr = g_FileMaskTab[y - 3] + BitFiles[x];

	//空心炮、超级炮牵制
   for (i = 9; i <= 10; i ++) {
    from = Piece[piece_tag + i];
    if (from != 0 && from != to) {
      if ((x == (from & 0xf)) && (x == (to & 0xf))) {
        if (((file_mask_ptr->rook_cap & g_BitFileMask[from]) != 0) || ((file_mask_ptr->super_cap & g_BitFileMask[to]) != 0)) {
          return true;
        }
      } else if ((y == (from >> 4)) && (y == (to >> 4))) {
        if (((rank_mask_ptr->rook_cap & g_BitRankMask[from]) != 0) || ((rank_mask_ptr->super_cap & g_BitRankMask[to]) != 0)) {
          return true;
        }
      }
     }
   }

	//将牵制
    from=Piece[piece_tag];
    if (from != 0 && from != to) {
      if ((x == (from & 0xf)) && (x == (to & 0xf))) {
        if (((file_mask_ptr->cannon_cap & g_BitFileMask[from]) != 0)) {
          return true;
        }
      } 
    }
	// 7. 被车牵制的问题
   for (i = 7; i <= 8; i ++) {
    from = Piece[piece_tag + i];
    if (from != 0 && from != to) {
      if ((x == (from & 0xf)) && (x == (to & 0xf))) {
        if (((file_mask_ptr->cannon_cap & g_BitFileMask[from]) != 0)) {
          return true;
        }
      } else if ((y == (from >> 4)) && (y == (to >> 4))) {
        if (((rank_mask_ptr->cannon_cap & g_BitRankMask[from]) != 0)) {
          return true;
        }
      }
    }
  }
   	//马的牵制
	for (i = 5; i <= 6; i ++) {
       from = Piece[piece_tag + i];
          if (from != 0) {
              disp = c_HorseLegTab[to - from + 256];
              if (disp != 0 && FLAG_IS(Square[from + disp],me_flag)) {
                  return true;
              }
           }
     }
	return false;
}

//判断棋子是否受到牵制
bool is_piece_pinned(int square, int colour) 
{
	int i, piece_tag, from,to, disp, x, y,inc;
    slide_mask_t *rank_mask_ptr, *file_mask_ptr;
    // 棋子保护判断包括以下几个步骤：

    piece_tag =16-(colour<<4);

	to=Piece[colour<<4];

  // 被马牵制
  for (i = 5; i <= 6; i ++) {
    from = Piece[piece_tag + i];
    if (from != 0) {
      disp = c_HorseLegTab[to - from + 256];
      if (disp != 0 && Square[from + disp] && from + disp==square) {
        return true;
      }
    }
  }
  inc=AttackTab[square-to+256];
  if (inc == 0) return false;


  x = square & 0xf;
  y = square >> 4;
  rank_mask_ptr = g_RankMaskTab[x - 3] + BitRanks[y];
  file_mask_ptr = g_FileMaskTab[y - 3] + BitFiles[x];

  //双王照面牵制
  from=Piece[piece_tag];
  if (from != 0 && from != square) {
      if ((x == (from & 0xf)) && (x == (to & 0xf))) {
        if (((file_mask_ptr->rook_cap & g_BitFileMask[from]) != 0) && ((file_mask_ptr->rook_cap & g_BitFileMask[to]) != 0) ) {
          return true;
        }
      } 
    }

  // 7. 被车牵制的问题
  for (i = 7; i <= 8; i ++) {
    from = Piece[piece_tag + i];
    if (from != 0 && from != square) {
      if ((x == (from & 0xf)) && (x == (to & 0xf))) {
        if (((file_mask_ptr->rook_cap & g_BitFileMask[from]) != 0) && ((file_mask_ptr->rook_cap & g_BitFileMask[to]) != 0)) {
          return true;
        }
      } else if ((y == (from >> 4)) && (y == (to >> 4))) {
        if (((rank_mask_ptr->rook_cap & g_BitRankMask[from]) != 0) && ((rank_mask_ptr->rook_cap & g_BitRankMask[to]) != 0)) {
          return true;
        }
      }
    }
  }


  //被炮牵制
  for (i = 9; i <= 10; i ++) {
    from = Piece[piece_tag + i];
    if (from!=0 && from != square) {
      if ((x == (from & 0xf)) && (x == (to & 0xf))) {
        if ((((file_mask_ptr->cannon_cap & g_BitFileMask[from]) != 0) && ((file_mask_ptr->rook_cap & g_BitFileMask[to]) != 0))
			|| (((file_mask_ptr->cannon_cap & g_BitFileMask[to]) != 0) && ((file_mask_ptr->rook_cap & g_BitFileMask[from]) != 0))) {
          return true;
        }
      } else if ((y == (from >> 4)) && (y == (to >> 4))) {
        if ((((rank_mask_ptr->cannon_cap & g_BitRankMask[from]) != 0) && ((rank_mask_ptr->rook_cap & g_BitRankMask[to]) != 0))
			|| (((rank_mask_ptr->cannon_cap & g_BitRankMask[to]) != 0) && ((rank_mask_ptr->rook_cap & g_BitRankMask[from]) != 0))) {
          return true;
        }
      }
    }
  }
  return false;
}

//判断将是否受到牵制 空心炮
bool is_cannon_pinned(int square, int colour) 
{
    int i, piece_tag, from,to, x, y,inc;
    slide_mask_t *rank_mask_ptr, *file_mask_ptr;
    // 棋子保护判断包括以下几个步骤：

	to=Piece[colour<<4];

	inc=AttackTab[square-to+256];
	if (inc == 0) return false;

  piece_tag =16-(colour<<4);

  x = square & 0xf;
  y = square >> 4;
  rank_mask_ptr = g_RankMaskTab[x - 3] + BitRanks[y];
  file_mask_ptr = g_FileMaskTab[y - 3] + BitFiles[x];

  // 7. 空心炮牵制的问题
  for (i = 9; i <= 10; i ++) {
    from = Piece[piece_tag + i];
    if (from != 0 && from != square) {
      if ((x == (from & 0xf)) && (x == (to & 0xf))) {
        if (((file_mask_ptr->rook_cap & g_BitFileMask[from]) != 0) && ((file_mask_ptr->rook_cap & g_BitFileMask[to]) != 0)) {
          return true;
        }
      } else if ((y == (from >> 4)) && (y == (to >> 4))) {
        if (((rank_mask_ptr->rook_cap & g_BitRankMask[from]) != 0) && ((rank_mask_ptr->rook_cap & g_BitRankMask[to]) != 0)) {
          return true;
        }
      }
    }
  }

  return false;
}
//0表示没有牵制
bool is_knight_pinned(int move, int colour) 
{
	int i, piece_tag, from,to, disp,inc,square;
	int count;
	
	square=MOVE_FROM(move);

	to=Piece[colour<<4];
	inc=ProtectTab[square-to+256];
    if (inc != 2) return false;
 
	count=0;
	piece_tag =16-(colour<<4);
  // 被马牵制
  for (i = 5; i <= 6; i ++) {
    from = Piece[piece_tag + i];
    if (from != 0) {
      disp = c_HorseLegTab[to - from + 256];
      if (disp != 0 && Square[from + disp] && from + disp==square) {
		  if(from!=MOVE_TO(move)||(Square[from + disp]&RookFlag)==0)
		  {
			  return true;
		  }else
		  {
			  ++count;
		  }
      }
    }
  }
  if(count==2)
	  return true;

  return false;
}
bool is_filter_pinned(int move, int colour) 
{
	int i, piece_tag, from,to, x, y,inc,square;
	int move_to;
    slide_mask_t *rank_mask_ptr, *file_mask_ptr;    

	square=MOVE_FROM(move);
	to=Piece[colour<<4];

    inc=AttackTab[square-to+256];
    if (inc == 0) return false;

	move_to=MOVE_TO(move);

    piece_tag =16-(colour<<4);
    x = square & 0xf;
    y = square >> 4;
    rank_mask_ptr = g_RankMaskTab[x - 3] + BitRanks[y];
    file_mask_ptr = g_FileMaskTab[y - 3] + BitFiles[x];

  //双王照面牵制
  from=Piece[piece_tag];
  if (from != 0 && from != square) {
      if ((x == (from & 0xf)) && (x == (from & 0xf))) {
        if (((file_mask_ptr->rook_cap & g_BitFileMask[from]) != 0) && ((file_mask_ptr->rook_cap & g_BitFileMask[to]) != 0) ) {
			if((from & 0xf)==(move_to & 0xf))
				return false;
			return true;
        }
      } 
    }

  // 7. 被车牵制的问题
  for (i = 7; i <= 8; i ++) {
    from = Piece[piece_tag + i];
    if (from != 0 && from != square) {
      if ((x == (from & 0xf)) && (x == (to & 0xf))) {
        if (((file_mask_ptr->rook_cap & g_BitFileMask[from]) != 0) && ((file_mask_ptr->rook_cap & g_BitFileMask[to]) != 0)) {
            if(x==(move_to & 0xf))//同一列为合法
			{
				if( Square[move_to]!=Empty && from!=move_to)//吃子必须吃牵制子
					return true;
				return false;
			}
			return true;
        }
      } else if ((y == (from >> 4)) && (y == (to >> 4))) {
        if (((rank_mask_ptr->rook_cap & g_BitRankMask[from]) != 0) && ((rank_mask_ptr->rook_cap & g_BitRankMask[to]) != 0)) {
          if(y==(move_to >> 4))
		  {
			  	if(Square[move_to]!=Empty && from!=move_to)
					return true;
				return false;
		  }
			return true;
        }
      }
    }
  }


  //被炮牵制
  for (i = 9; i <= 10; i ++) {
    from = Piece[piece_tag + i];
    if (from!=0 && from != square) {
      if ((x == (from & 0xf)) && (x == (to & 0xf))) {
        if ((((file_mask_ptr->cannon_cap & g_BitFileMask[from]) != 0) && ((file_mask_ptr->rook_cap & g_BitFileMask[to]) != 0))
			|| (((file_mask_ptr->cannon_cap & g_BitFileMask[to]) != 0) && ((file_mask_ptr->rook_cap & g_BitFileMask[from]) != 0))) 
		{
			if(x==(move_to & 0xf))
			{
			   if(Square[move_to]!=0 && from!=move_to)
				   return true;
				return false;
			}
          return true;
        }
      } else if ((y == (from >> 4)) && (y == (to >> 4))) {
        if ((((rank_mask_ptr->cannon_cap & g_BitRankMask[from]) != 0) && ((rank_mask_ptr->rook_cap & g_BitRankMask[to]) != 0))
			|| (((rank_mask_ptr->cannon_cap & g_BitRankMask[to]) != 0) && ((rank_mask_ptr->rook_cap & g_BitRankMask[from]) != 0))) 
		{
			if(y==(move_to >> 4))
			{
			  if(Square[move_to]!=0 && from!=move_to)
				  return true;
				return false;
			}
          return true;
        }
      }
    }
  }
  return false;
}
//将军攻击棋子与位置
void attack_set(attack_t * attack)
{
	 int i, piece_tag, from, to, x, y, disp;
     slide_mask_t *rank_mask_ptr, *file_mask_ptr;
 
	 attack->dn=0;

	 piece_tag = 16-(Turn<<4);
     // 将军判断包括以下几部分内容：
     // 1. 判断帅(将)是否在棋盘上
     from = Piece[16-piece_tag];
     
	 if (from == 0) {
         return; 
     }
	   // 7. 判断是否被兵(卒)将军
   for (to = from - 1; to <= from + 1; to += 2) {
      if(PIECE_IS_PAWN(Square[to]))
	  {
		  attack->ds[attack->dn] = to;
          attack->dn++;
	  }
   }
   to= from - 16 + (Turn << 5);
   if(PIECE_IS_PAWN(Square[to]))
   {
		  attack->ds[attack->dn] = to;
          attack->dn++;
    }
     // 2. 获得帅(将)所在格子的位行和位列
     x = from & 0xf;
     y = from >> 4;
     rank_mask_ptr = g_RankMaskTab[x - 3] + BitRanks[y];
     file_mask_ptr = g_FileMaskTab[y - 3] + BitFiles[x];
     // 3. 判断是否将帅对脸
     /*
	 to = Piece[piece_tag];
     if (to != 0) {
        if (x == (to & 0xf) && (file_mask_ptr->rook_cap & g_BitFileMask[to]) != 0) {
              attack->ds[attack->dn] = to;
              attack->di[attack->dn] = 0;
              attack->dn++;
        }
     }
	 */
	 
  // 4. 判断是否被马将军
  for (i = 5; i <= 6; i ++) {
    to = Piece[piece_tag + i];
    if (to != 0) {
      disp = c_HorseLegTab[from - to + 256];
      if (disp != 0 && Square[to + disp] == 0) {
              attack->ds[attack->dn] = to;
              attack->dn++;
      }
    }
  }
 
  // 5. 判断是否被车将军或将帅对脸
  for (i = 7; i <= 8; i ++) {
    to = Piece[piece_tag + i];
    if (to != 0) {
      if (x == (to & 0xf)) {
        if ((file_mask_ptr->rook_cap & g_BitFileMask[to]) != 0) {
              attack->ds[attack->dn] = to;
              attack->dn++; 
        }
      } else if (y == (to >> 4)) {
        if ((rank_mask_ptr->rook_cap & g_BitRankMask[to]) != 0) {
              attack->ds[attack->dn] = to;
              attack->dn++; 
        }
      }
    }
  }

 // 6. 判断是否被炮将军
  for (i = 9; i <= 10; i ++) {
    to = Piece[piece_tag + i];
    if (to != 0) {
      if (x == (to & 0xf)) {
        if ((file_mask_ptr->cannon_cap & g_BitFileMask[to]) != 0) {
              attack->ds[attack->dn] = to;
              attack->dn++;
        }
      } else if (y == (to >> 4)) {
        if ((rank_mask_ptr->cannon_cap & g_BitRankMask[to]) != 0) {
              attack->ds[attack->dn] = to;
              attack->dn++; 
        }
      }
    }
  }
}

//是否被将军
bool is_in_check(int colour)
{
     int i, piece_tag, from, to, x, y, disp;
     slide_mask_t *rank_mask_ptr, *file_mask_ptr;
 
	 piece_tag = 16-(colour<<4);
     // 将军判断包括以下几部分内容：
     // 1. 判断帅(将)是否在棋盘上
     from = Piece[colour<<4];
     
	 if (from == 0) {
         return true; 
     }
     // 2. 获得帅(将)所在格子的位行和位列
     x = from & 0xf;
     y = from >> 4;
     rank_mask_ptr = g_RankMaskTab[x - 3] + BitRanks[y];
     file_mask_ptr = g_FileMaskTab[y - 3] + BitFiles[x];
     // 3. 判断是否将帅对脸
     to = Piece[piece_tag];
     if (to != 0) {
        if (x == (to & 0xf) && (file_mask_ptr->rook_cap & g_BitFileMask[to]) != 0) {
               return true; 
        }
     }
  // 4. 判断是否被马将军
  for (i = 5; i <= 6; i ++) {
    to = Piece[piece_tag + i];
    if (to != 0) {
      disp = c_HorseLegTab[from - to + 256];
      if (disp != 0 && Square[to + disp] == 0) {
               return true; 
      }
    }
  }
  // 5. 判断是否被车将军或将帅对脸
  for (i = 7; i <= 8; i ++) {
    to = Piece[piece_tag + i];
    if (to != 0) {
      if (x == (to & 0xf)) {
        if ((file_mask_ptr->rook_cap & g_BitFileMask[to]) != 0) {
              return true; 
        }
      } else if (y == (to >> 4)) {
        if ((rank_mask_ptr->rook_cap & g_BitRankMask[to]) != 0) {
              return true; 
        }
      }
    }
  }
  // 6. 判断是否被炮将军
  for (i = 9; i <= 10; i ++) {
    to = Piece[piece_tag + i];
    if (to != 0) {
      if (x == (to & 0xf)) {
        if ((file_mask_ptr->cannon_cap & g_BitFileMask[to]) != 0) {
             return true; 
        }
      } else if (y == (to >> 4)) {
        if ((rank_mask_ptr->cannon_cap & g_BitRankMask[to]) != 0) {
             return true; 
        }
      }
    }
  }
  // 7. 判断是否被兵(卒)将军
  for (to = from - 1; to <= from + 1; to += 2) {
	if(PIECE_IS_PAWN(Square[to]))
	{
		return true;
	}
  }
  to= from - 16 + (colour << 5);
  if(PIECE_IS_PAWN(Square[to]))
  {
		return true;
  }
    return false;
}

//是否被将军
//移动后 是否受到colour颜色的将军 ，在移动将军时候使用
bool is_checked(int to, int colour) 
{
	int i, piece_tag, from, disp, x, y;
	int pawn,king;
    slide_mask_t *rank_mask_ptr, *file_mask_ptr;
    
    piece_tag =colour<<4;
    pawn=PawnFlag | (32 <<colour);

	king=Piece[16-piece_tag];

  if (colour == 0 ? (to & 0x80) != 0 : (to & 0x80) == 0) {
      ;
  } else {

    // 4. 判断受到过河兵(卒)横向的保护
    for (from = to - 1; from <= to + 1; from += 2) {
		if((Square[from]&pawn)==pawn)
		{
			return true;
		}
    }
  }

  // 5. 判断受到过河兵(卒)纵向的保护
  from = to + (colour == 0 ? 16 : -16);
  if((Square[from]&pawn)==pawn)
  {
	  return true;
  }

  // 6. 判断受到马的保护
  for (i = 5; i <= 6; i ++) {
    from = Piece[piece_tag + i];
    if (from != 0) {
      disp = c_HorseLegTab[to - from + 256];
      if (disp != 0 && !Square[from + disp]) {
        return true;
      }
    }
  }

  x = to & 0xf;
  y = to >> 4;
  rank_mask_ptr = g_RankMaskTab[x - 3] + BitRanks[y];
  file_mask_ptr = g_FileMaskTab[y - 3] + BitFiles[x];

  // 7. 判断受到车的保护，参阅"struct.cpp"里的"Checked()"函数
  for (i = 7; i <= 8; i ++) {
    from = Piece[piece_tag + i];
    if (from != 0 && from != to) {
      if (x == (from & 0xf)) {
        if ((file_mask_ptr->rook_cap & g_BitFileMask[from]) != 0) {
          return true;
        }
      } else if (y == (from >> 4)) {
        if ((rank_mask_ptr->rook_cap & g_BitRankMask[from]) != 0) {
          return true;
        }
      }
    }
  }

  //是否有双王照面的危险
  from=Piece[piece_tag];
  if (from != 0 && from != to) {
      if (x == (from & 0xf)) {
        if ((file_mask_ptr->rook_cap & g_BitFileMask[from]) != 0) {
          return true;
        }
      } 
    }

  // 8. 判断受到炮的保护
  for (i = 9; i <= 10; i ++) {
    from = Piece[piece_tag + i];
    if (from!=0 && from != to) {
      if (x == (from & 0xf)) {
        if ((file_mask_ptr->cannon_cap & g_BitFileMask[from]) != 0) {
           if(to+AttackTab[from-to+256]==king)
			   return false;
           return true;
        }
      } else if (y == (from >> 4)) {
        if ((rank_mask_ptr->cannon_cap & g_BitRankMask[from]) != 0) {
		   if(to+AttackTab[from-to+256]==king)
			   return false;
           return true;
        }
      }
    }
  }
  return false;
}