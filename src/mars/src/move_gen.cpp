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
#include ".\move_gen.h"

#include "move.h"
#include "pregen.h"
#include "attack.h"
#include "move_legal.h"
#include "move_evasion.h"

static void add_captures            (list_t * list);
static void add_quiet_moves         (list_t * list);

void gen_legal_moves(list_t * list) {

   attack_t attack[1];

   ASSERT(list!=NULL);
   

   attack_set(attack);

   if (ATTACK_IN_CHECK(attack)) {
      gen_evasions(list,attack);
   } else 
   {
      gen_moves(list);
      list_filter(list,&pseudo_is_legal,true);
   }

   
}

void test_gen_moves(list_t *list,int from,int to1)
{
   int x,y,x1,y1,disp,piece,to;
   uint8 * ptr;
   slide_move_t *slide_move_ptr;

   //ASSERT(alists!=NULL);
   //
   
   piece=Square[from];
   if (piece != 0) {
      x = from & 0xf;
      y = from >> 4;
      x1 = to1 & 0xf;
      y1 = to1 >> 4;
      if(y==y1)
	  {
         disp = y << 4;
         slide_move_ptr = g_RankMoveTab[x - 3] + BitRanks[y];
         ptr = slide_move_ptr->rook_cap;
		 
		 if(x>x1)
		 {
            to = ptr[0] + disp;
            if (to != from) {
          
               LIST_ADD(list,MOVE_MAKE(from,to));
           
            }
		   }else
		 {
            to = ptr[1] + disp;
            if (to != from) {
           
               LIST_ADD(list,MOVE_MAKE(from,to));
           
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
             to = ptr[0] + disp;
             if (to != from) {
           
               LIST_ADD(list,MOVE_MAKE(from,to));
           
              }
		 }
		 else
		 {

              to = ptr[1] + disp;
              if (to != from) {
          
               LIST_ADD(list,MOVE_MAKE(from,to));
           
               }
		 }
	  }
	  
    }

}
void gen_moves(list_t * list) 
{
	ASSERT(list!=NULL);
    
	LIST_CLEAR(list);
	add_captures(list);
	add_quiet_moves(list);

}
void gen_captures(list_t * list) {
	
	LIST_CLEAR(list);
	add_captures(list);

}

void gen_quiet_moves(list_t * list) {

	LIST_CLEAR(list);
	add_quiet_moves(list);
}
//吃子着法
static void add_captures(list_t * list) 
{

   int me, opp;
   int opp_flag;
   int from, to;
   int piece_tag,x, y, disp,i;
   uint8 * ptr,*eye_leg_ptr;
   slide_move_t *slide_move_ptr;

   ASSERT(list!=NULL);
   

   me = Turn;
   piece_tag =me << 4;
   
   opp = COLOUR_OPP(me);
   opp_flag = COLOUR_FLAG(opp);

   // 1. 生成帅(将)的着法
   from = Piece[piece_tag];
   if (from != 0) {
       ptr = g_KingMoves[from];
       to = *ptr;
       while (to != 0) {
           if (FLAG_IS(Square[to],opp_flag)) {
               LIST_ADD(list,MOVE_MAKE(from,to));
           }
           ptr ++;
           to = *ptr;
       }
    }

  // 2. 生成仕(士)的着法
  for (i = 1; i <= 2; i ++) {
    from = Piece[piece_tag + i];
    if (from != 0) {
      ptr = g_AdvisorMoves[from];
      to = *ptr;
      while (to != 0) {
           if (FLAG_IS(Square[to],opp_flag)) {
               LIST_ADD(list,MOVE_MAKE(from,to));
           }
        ptr ++;
        to = *ptr;
      }
    }
  }

  // 3. 生成相(象)的着法
  for (i = 3; i <= 4; i ++) {
    from = Piece[piece_tag + i];
    if (from != 0) {
      ptr = g_BishopMoves[from];
      eye_leg_ptr = g_ElephantEyes[from];
      to = *ptr;
      while (to != 0) {
        if (Square[*eye_leg_ptr] == 0) {
           if (FLAG_IS(Square[to],opp_flag)) {
               LIST_ADD(list,MOVE_MAKE(from,to));
           }
        }
        ptr ++;
        to = *ptr;
        eye_leg_ptr ++;
      }
    }
  }

  // 4. 生成马的着法
  for (i = 5; i <= 6; i ++) {
    from = Piece[piece_tag + i];
    if (from != 0) {
      ptr = g_KnightMoves[from];
      eye_leg_ptr = g_HorseLegs[from];
      to = *ptr;
      while (to != 0) {
        if (Square[*eye_leg_ptr] == 0) {
           if (FLAG_IS(Square[to],opp_flag)) {
               LIST_ADD(list,MOVE_MAKE(from,to));
           }
        }
        ptr ++;
        to = *ptr;
        eye_leg_ptr ++;
      }
    }
  }

  // 5. 生成车的着法
  for (i = 7; i <= 8; i ++) {
    from = Piece[piece_tag + i];
    if (from != 0) {
      x = from & 0xf;
      y = from >> 4;
      disp = y << 4;
      slide_move_ptr = g_RankMoveTab[x - 3] + BitRanks[y];
      ptr = slide_move_ptr->rook_cap;
      to = ptr[0] + disp;
      if (to != from) {
           if (FLAG_IS(Square[to],opp_flag)) {
               LIST_ADD(list,MOVE_MAKE(from,to));
           }
      }
      to = ptr[1] + disp;
      if (to != from) {
           if (FLAG_IS(Square[to],opp_flag)) {
               LIST_ADD(list,MOVE_MAKE(from,to));
           }
      }
      disp = x;
      slide_move_ptr = g_FileMoveTab[y - 3] + BitFiles[x];
      ptr = slide_move_ptr->rook_cap;
      to = ptr[0] + disp;
      if (to != from) {
           if (FLAG_IS(Square[to],opp_flag)) {
               LIST_ADD(list,MOVE_MAKE(from,to));
           }
      }
      to = ptr[1] + disp;
      if (to != from) {
           if (FLAG_IS(Square[to],opp_flag)) {
               LIST_ADD(list,MOVE_MAKE(from,to));
           }
      }
    }
  }

  // 6. 生成炮的着法
  for (i = 9; i <= 10; i ++) {
    from = Piece[piece_tag + i];
    if (from != 0) {
      x = from & 0xf;
      y = from >> 4;
      disp = y << 4;
      slide_move_ptr = g_RankMoveTab[x - 3] + BitRanks[y];
      ptr = slide_move_ptr->cannon_cap;
      to = ptr[0] + disp;
      if (to != from) {
           if (FLAG_IS(Square[to],opp_flag)) {
               LIST_ADD(list,MOVE_MAKE(from,to));
           }
      }
      to = ptr[1] + disp;
      if (to != from) {
           if (FLAG_IS(Square[to],opp_flag)) {
               LIST_ADD(list,MOVE_MAKE(from,to));
           }
      }
      disp = x;
      slide_move_ptr = g_FileMoveTab[y - 3] + BitFiles[x];
      ptr = slide_move_ptr->cannon_cap;
      to = ptr[0] + disp;
      if (to != from) {
           if (FLAG_IS(Square[to],opp_flag)) {
               LIST_ADD(list,MOVE_MAKE(from,to));
           }
      }
      to = ptr[1] + disp;
      if (to != from) {
           if (FLAG_IS(Square[to],opp_flag)) {
               LIST_ADD(list,MOVE_MAKE(from,to));
           }
      }
    }
  }

  // 7. 生成兵(卒)的着法
  for (i = 11; i <= 15; i ++) {
    from = Piece[piece_tag + i];
    if (from != 0) {
      ptr = g_PawnMoves[Turn][from];
      to = *ptr;
      while (to != 0) {
           if (FLAG_IS(Square[to],opp_flag)) {
               LIST_ADD(list,MOVE_MAKE(from,to));
           }
        ptr ++;
        to = *ptr;
      }
    }
  }

}
//不吃子着法
static void add_quiet_moves(list_t * list)
{
   int from, to;
   int piece_tag,x, y, disp,i;
   uint8 * ptr,*eye_leg_ptr;
   slide_move_t *slide_move_ptr;

   ASSERT(list!=NULL);
   
  
   piece_tag = Turn<<4;

     // 1. 生成帅(将)的着法
  from = Piece[piece_tag];
  if (from != 0) {
    ptr = g_KingMoves[from];
    to = *ptr;
    while (to != 0) {
      // 找到一个着法后，首先判断是否吃到棋子
      if (Square[to] == 0) {
      LIST_ADD(list,MOVE_MAKE(from,to));
      }
      ptr ++;
      to = *ptr;
    }
  }

  // 2. 生成仕(士)的着法
  for (i = 1; i <= 2; i ++) {
    from = Piece[piece_tag + i];
    if (from != 0) {
      ptr = g_AdvisorMoves[from];
      to = *ptr;
      while (to != 0) {
        if (Square[to] == 0) {
          LIST_ADD(list,MOVE_MAKE(from,to));
        }
        ptr ++;
        to = *ptr;
      }
    }
  }

  // 3. 生成相(象)的着法
  for (i = 3; i <= 4; i ++) {
    from = Piece[piece_tag + i];
    if (from != 0) {
      ptr = g_BishopMoves[from];
      eye_leg_ptr = g_ElephantEyes[from];
      to = *ptr;
      while (to != 0) {
        if (Square[*eye_leg_ptr] == 0) {
          if (Square[to] == 0) {
           LIST_ADD(list,MOVE_MAKE(from,to));
          }
        }
        ptr ++;
        to = *ptr;
        eye_leg_ptr ++;
      }
    }
  }

  // 4. 生成马的着法
  for (i = 5; i <= 6; i ++) {
    from = Piece[piece_tag + i];
    if (from != 0) {
      ptr = g_KnightMoves[from];
      eye_leg_ptr = g_HorseLegs[from];
      to = *ptr;
      while (to != 0) {
        if (Square[*eye_leg_ptr] == 0) {
          if (Square[to] == 0) {
           LIST_ADD(list,MOVE_MAKE(from,to));
          }
        }
        ptr ++;
        to = *ptr;
        eye_leg_ptr ++;
      }
    }
  }

  // 5. 生成车和炮的着法，没有必要判断是否吃到本方棋子
  for (i = 7; i <= 10; i ++) {
    from = Piece[piece_tag + i];
    if (from != 0) {
      x = from & 0xf;
      y = from >> 4;
      disp = y << 4;
      slide_move_ptr = g_RankMoveTab[x - 3] + BitRanks[y];
      ptr = slide_move_ptr->non_cap;
      to = ptr[0] + disp;
      while (to != from) {
       LIST_ADD(list,MOVE_MAKE(from,to));
        to --;
      }
      to = ptr[1] + disp;
      while (to != from) {
      LIST_ADD(list,MOVE_MAKE(from,to));
        to ++;
      }
      disp = x;
      slide_move_ptr = g_FileMoveTab[y - 3] + BitFiles[x];
      ptr = slide_move_ptr->non_cap;
      to = ptr[0] + disp;
      while (to != from) {
        LIST_ADD(list,MOVE_MAKE(from,to));
        to -= 16;
      }
      to = ptr[1] + disp;
      while (to != from) {
       LIST_ADD(list,MOVE_MAKE(from,to));
        to += 16;
      }
    }
  }

  // 6. 生成兵(卒)的着法
  for (i = 11; i <= 15; i ++) {
    from = Piece[piece_tag + i];
    if (from != 0) {
      ptr = g_PawnMoves[Turn][from];
      to = *ptr;
      while (to != 0) {
        if (Square[to] == 0) {
          LIST_ADD(list,MOVE_MAKE(from,to));
        }
        ptr ++;
        to = *ptr;
      }
    }
  }

}