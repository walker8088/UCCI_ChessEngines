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
#include ".\move_legal.h"
#include "piece.h"
#include "attack.h"
#include "board.h"
#include "move.h"

bool move_is_pseudo(int move) {

   int me, opp;
   int from, to;
   int piece, capture;
   int disp,x,y;

   // init

   me = Turn;
   opp = COLOUR_OPP(Turn);

   // from

   from = MOVE_FROM(move);

   piece = Square[from];
   if (!COLOUR_IS(piece,me)) return false;

   // to

   to = MOVE_TO(move);
   ASSERT(SQUARE_IS_OK(to));

   capture = Square[to];
   if (COLOUR_IS(capture,me)) return false;

   if(PIECE_IS_KING(piece))
   {
	   return c_InCity[to] && ProtectTab[to - from + 256]== 1;
   }else if(PIECE_IS_ADVISOR(piece))
   {
	   return c_InCity[to] && ProtectTab[to - from + 256]== 2;
   }else if(PIECE_IS_BISHOP(piece))
   {
        return ((from ^ to) & 0x80) == 0 && ProtectTab[to - from + 256] == 3 && Square[(from + to) >> 1] == 0;
   }else if(PIECE_IS_KNIGHT(piece))
   {
	    disp = c_HorseLegTab[to - from + 256];
        return disp != 0 && Square[from + disp] == 0;
   }else if(PIECE_IS_ROOK(piece))
   {
	   x = from & 0xf;
       y = from >> 4;
       if (x == (to & 0xf)) {
          if (capture == 0) {
              return (g_FileMaskTab[y - 3][BitFiles[x]].non_cap & g_BitFileMask[to]) != 0;
          } else {
              return (g_FileMaskTab[y - 3][BitFiles[x]].rook_cap & g_BitFileMask[to]) != 0;
          }
        } else if (y == (to >> 4)) {
          if (capture == 0) {
              return (g_RankMaskTab[x - 3][BitRanks[y]].non_cap & g_BitRankMask[to]) != 0;
          } else {
              return (g_RankMaskTab[x - 3][BitRanks[y]].rook_cap & g_BitRankMask[to]) != 0;
          }
        } else {
         return false;
        }
   }else if(PIECE_IS_CANNON(piece))
   {
	    x = from & 0xf;
        y = from >> 4;
        if (x == (to & 0xf)) {
            if (capture == 0) {
                return (g_FileMaskTab[y - 3][BitFiles[x]].non_cap & g_BitFileMask[to]) != 0;
            } else {
                return (g_FileMaskTab[y - 3][BitFiles[x]].cannon_cap & g_BitFileMask[to]) != 0;
            }
        } else if (y == (to >> 4)) {
            if (capture == 0) {
                return (g_RankMaskTab[x - 3][BitRanks[y]].non_cap & g_BitRankMask[to]) != 0;
            } else {
                return (g_RankMaskTab[x - 3][BitRanks[y]].cannon_cap & g_BitRankMask[to]) != 0;
            }
        } else {
            return false;
        }

   }else if(PIECE_IS_PAWN(piece))
   {
	   if(COLOUR_IS(piece,0))
	   {
	       return to == from - 16 || ((to & 0x80) == 0 && (to == from - 1 || to == from + 1));
       } else {
           return to == from + 16 || ((to & 0x80) != 0 && (to == from - 1 || to == from + 1));
       }
	  
   }

   return false;
}
bool quiet_is_pseudo(int move) {

   int me, opp;
   int from, to;
   int piece;
   int disp,x,y;

   // init

   me = Turn;
   opp = COLOUR_OPP(Turn);

   // from

   from = MOVE_FROM(move);

   piece = Square[from];
   if (!COLOUR_IS(piece,me)) return false;

   ASSERT(piece_is_ok(piece));

   // to

   to = MOVE_TO(move);


   if (Square[to] != Empty) return false; // capture

  if(PIECE_IS_KING(piece))
   {
	   return c_InCity[to] && ProtectTab[to - from + 256]== 1;
   }else if(PIECE_IS_ADVISOR(piece))
   {
	   return c_InCity[to] && ProtectTab[to - from + 256]== 2;
   }else if(PIECE_IS_BISHOP(piece))
   {
        return ((from ^ to) & 0x80) == 0 && ProtectTab[to - from + 256] == 3 && Square[(from + to) >> 1] == 0;
   }else if(PIECE_IS_KNIGHT(piece))
   {
	    disp = c_HorseLegTab[to - from + 256];
        return disp != 0 && Square[from + disp] == 0;
   }else if(PIECE_IS_ROOK(piece) || PIECE_IS_CANNON(piece))
   {
	   x = from & 0xf;
       y = from >> 4;
       if (x == (to & 0xf)) {
              return (g_FileMaskTab[y - 3][BitFiles[x]].non_cap & g_BitFileMask[to]) != 0;        
        } else if (y == (to >> 4)) {
              return (g_RankMaskTab[x - 3][BitRanks[y]].non_cap & g_BitRankMask[to]) != 0;     
        } else {
         return false;
        }
   }else if(PIECE_IS_PAWN(piece))
   {
	   if(COLOUR_IS(piece,0))
	   {
	       return to == from - 16 || ((to & 0x80) == 0 && (to == from - 1 || to == from + 1));
       } else {
           return to == from + 16 || ((to & 0x80) != 0 && (to == from - 1 || to == from + 1));
       }  
   }
   return false;
}


bool pseudo_is_legal(int move) {
   int me, opp;
   int from, to;
   int piece;
   bool legal;

   // init
   
   me = Turn;
   opp = COLOUR_OPP(me);

   from = MOVE_FROM(move);
   to = MOVE_TO(move);

   piece = Square[from];

   if (PIECE_IS_KING(piece)) {

      legal = !is_checked(to,opp);

      return legal;
   }
   
   if(is_cannon_pinned(to,me))//空心炮
   {
	   return false;
   }
   if(is_knight_pinned(move,me))
   {
	   return false;
   }
   if (is_filter_pinned(move,me)) {
      return false;
   }
   return true;
}