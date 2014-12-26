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



#include ".\move_evasion.h"
#include "attack.h"
#include "list.h"
#include "move.h"
#include "move_check.h"
#include "board.h"
#include "move_do.h"

int nCheck;
//挡

//杀
static void gen_attack(list_t * list,int to,int colour);
//移将
static void gen_move_king(list_t * list,const attack_t * attack);
//挡 闪
static void gen_block(list_t * list,const attack_t * attack);

static void add_block(list_t * list,int square,int to,int colour);
//将军躲避完全算法
void gen_evasions(list_t * list,const attack_t * attack) 
{
    int dn,move,size;
	list_t temp[1];
    LIST_CLEAR(list);
	LIST_CLEAR(temp);
    dn=attack->dn;
    if(dn==1)
	{   
		nCheck++;
		gen_move_king(list,attack); //移将
		gen_attack(list,attack->ds[0],Turn);              //杀
	    gen_block(list, attack);                          //挡 闪
	}else if(dn>1)
	{
		gen_move_king(temp,attack); //移将
		gen_attack(temp,attack->ds[0],Turn);              //杀
	    gen_block(temp, attack);  //挡 闪
		size=LIST_SIZE(temp);
        for(int i=0;i<size;++i)
		{
			move=LIST_MOVE(temp,i);
			if(!move_is_check(move))
			{
				LIST_ADD(list,move);
			}
		}


	}
}
static void gen_attack(list_t * list,int to,int colour)
{
	int i, piece_tag, from, disp, x, y,pawn,opp;
    slide_mask_t *rank_mask_ptr, *file_mask_ptr;

    piece_tag =colour<<4;

	//pawn=PawnFlag|(1<<(5+colour));
	pawn=PawnFlag | (32 <<colour);
	opp = COLOUR_OPP(colour);

  if (colour == 0 ? (to & 0x80) != 0 : (to & 0x80) == 0) {
    if (c_InCity[to]) {

      // 1. 用帅(将)杀
		
      from = Piece[piece_tag];
      if (from != 0 && !is_checked(to,opp)) {
        if (ProtectTab[from - to + 256] == 1) {
          LIST_ADD(list,MOVE_MAKE(from,to));
        }
      }

      // 2. 用仕(士)杀
      for (i = 1; i <= 2; i ++) {
        from = Piece[piece_tag + i];
        if (from != 0 && !is_piece_pinned(from,colour)) {
          if (ProtectTab[from - to + 256] == 2) {
            LIST_ADD(list,MOVE_MAKE(from,to));
          }
        }
      }
    }

    // 3. 用相(象)杀
    for (i = 3; i <= 4; i ++) {
      from = Piece[piece_tag + i];
      if (from != 0 && !is_piece_pinned(from,colour)) {
        if (ProtectTab[from - to + 256] == 3 && Square[(to + from) >> 1] == 0) {
          LIST_ADD(list,MOVE_MAKE(from,to));
        }
      }
    }
  } else {

    // 4. 用兵(卒)横向杀
    for (from = to - 1; from <= to + 1; from += 2) {
		if((Square[from]&pawn)==pawn && !is_piece_pinned(from,colour))
		{
			LIST_ADD(list,MOVE_MAKE(from,to));
		}
    }
  }

  // 5. 用兵(卒)纵向杀
  from = to + (colour == 0 ? 16 : -16);
  if((Square[from]&pawn)==pawn && !is_piece_pinned(from,colour))
  {
	  LIST_ADD(list,MOVE_MAKE(from,to));
  }

  // 6. 用马杀
  for (i = 5; i <= 6; i ++) {
    from = Piece[piece_tag + i];
    if (from != 0 && !is_piece_pinned(from,colour)) {
      disp = c_HorseLegTab[to - from + 256];
      if (disp != 0 && Square[from + disp]==0) {
        LIST_ADD(list,MOVE_MAKE(from,to));
      }
    }
  }

  x = to & 0xf;
  y = to >> 4;
  rank_mask_ptr = g_RankMaskTab[x - 3] + BitRanks[y];
  file_mask_ptr = g_FileMaskTab[y - 3] + BitFiles[x];

  // 7. 用车杀
  for (i = 7; i <= 8; i ++) {
    from = Piece[piece_tag + i];

    if (from != 0 && from != to && !is_piece_pinned(from,colour)) {
      if (x == (from & 0xf)) {
        if ((file_mask_ptr->rook_cap & g_BitFileMask[from]) != 0) {
          LIST_ADD(list,MOVE_MAKE(from,to));
        }
      } else if (y == (from >> 4)) {
        if ((rank_mask_ptr->rook_cap & g_BitRankMask[from]) != 0) {
          LIST_ADD(list,MOVE_MAKE(from,to));
        }
      }
    }
  }

  // 8. 用炮杀
  
  for (i = 9; i <= 10; i ++) {
    from = Piece[piece_tag + i];
    if (from!=0 && from != to && !is_piece_pinned(from,colour)) {
      if (x == (from & 0xf)) {
        if ((file_mask_ptr->cannon_cap & g_BitFileMask[from]) != 0) {
          LIST_ADD(list,MOVE_MAKE(from,to));
        }
      } else if (y == (from >> 4)) {
        if ((rank_mask_ptr->cannon_cap & g_BitRankMask[from]) != 0) {
          LIST_ADD(list,MOVE_MAKE(from,to));
        }
      }
    }
  }
  
}
static void gen_move_king(list_t * list,const attack_t* attack)
{
   int me, opp,dn;
   int opp_flag;
   int from, to;
   int piece_tag;
   uint8 * ptr;
   
   dn=attack->dn;
   me = Turn;
   piece_tag =me << 4;
   
   opp = COLOUR_OPP(me);
   opp_flag = COLOUR_FLAG(opp);
 
   // 帅(将)的移动着法
   from = Piece[piece_tag];
   if (from != 0) {
       ptr = g_KingMoves[from];
       to = *ptr;
       while (to != 0) {
           if (Square[to]==Empty ) {	
               if(!is_checked(to,opp))
			   {
				   
				   for(int i=0;i<dn;++i)
				   {
					   if(PIECE_IS_SLIDER(Square[attack->ds[i]]))   //过滤滑子将军的同行或同列移动
					   {
					      if((attack->ds[i]&0xf) ==(to&0xf)|| (attack->ds[i]>>4) == (to>>4))
                                goto I;  
					   }
				   }

				   LIST_ADD(list,MOVE_MAKE(from,to));

I:                 ;
			   }
		   
           }else if(FLAG_IS(Square[to],opp_flag))
		   {
			    if(!is_checked(to,opp))
			   {
				   
				   for(int i=0;i<dn;++i)
				   {
					   if(PIECE_IS_SLIDER(Square[attack->ds[i]]))   //过滤滑子将军的同行或同列移动
					   {
					       //if((attack->ds[i]&0xf) ==(to&0xf)|| (attack->ds[i]>>4) == (to>>4))
						   if(AttackTab[to-from + 256]==AttackTab[attack->ds[i]-from + 256]) 
						   LIST_ADD(list,MOVE_MAKE(from,to));  
					   }
				   }

			   }
		   }
           ptr ++;
           to = *ptr;
       }
    }
}
//挡 闪
static void gen_block(list_t * list,  const attack_t * attack)
{
	int from,to,paojiazi,sq,temp,leg;
	int piece,opp,opp_flag;
	int inc;
	int me;

	me=Turn;
	opp = COLOUR_OPP(me);
    opp_flag = COLOUR_FLAG(opp);

	if(attack->dn>0)
	{
		from=attack->ds[0];
		piece=Square[from];
		
        to=Piece[me<<4];

		if(PIECE_IS_CANNON(piece))               //敌方炮将
		{
			inc=AttackTab[to-from + 256];
            sq = from;
		    do
			{
			    sq += inc; 	  
			}while (Square[sq]==Empty);    //统计炮架子 （注：也可以用位行位列来统计）
			paojiazi=sq;  
			
			sq = from;
			temp=to-inc;
           // printf("sq=%d,temp=%d\n",sq,temp);
            do
			{
			     sq += inc; 
				 if(sq!=paojiazi)
				 {
					 //printf("sq=%d,paojiazi=%d\n",sq,paojiazi);
			         add_block(list,paojiazi,sq,me);  //挡
				 }else
				 {
					 if(PIECE_IS_CANNON(Square[paojiazi]) && FLAG_IS(Square[paojiazi],opp_flag)) //炮架子为敌方炮，构成双炮将，放弃炮到将线路的挡
						 break;
				 }

			 }while (sq!=temp);
            
		    /*sq = from;			
			temp=to-inc;

            if(PIECE_IS_ROOK(Square[paojiazi]) && FLAG_IS(Square[paojiazi],opp_flag))
			{
				sq=paojiazi;  //炮车将，跳过跑架子-炮的解将
			}else if(PIECE_IS_CANNON(Square[paojiazi]) && FLAG_IS(Square[paojiazi],opp_flag))
			{
                temp=paojiazi-inc;
			}
            do
			{
			     sq += inc; 
			     add_block(list,paojiazi,sq,me);  //挡

			 }while (sq!=temp);
			 */
            //炮架子为本方棋子
			if(FLAG_IS(Square[paojiazi],(32<<me)))  //闪
			{
			    if((inc==16)||(inc==-16))
			       add_flee(list,paojiazi,true); //炮 车 兵 闪
			    else
			       add_flee(list,paojiazi,false);//其他 闪
			}


		}else if(PIECE_IS_ROOK(piece)) //敌方车将
		{
			inc=AttackTab[to-from + 256];
			sq = from;
			sq += inc; 	  
			while (Square[sq]==Empty)
			{
				add_block(list,0,sq,me);    //挡
				sq += inc;
			}

		}else if(PIECE_IS_KNIGHT(piece))          //敌方马将
		{
			 leg = c_HorseLegTab[to - from + 256];
			 add_block(list,0,from+leg,me); //挡马腿
		}
	}
}
/*
static void gen_multi_evasion(list_t * list, const attack_t * attack)
{
	int from1,from2,to;
	from1=attack->dn[0];
	from2=attack->dn[1];
	if((PIECE_IS_PAWN(Square[from1]) && (PIECE_IS_CANNON(Square[from2])))
	{
	}
}*/
/*
//挡 闪
static void gen_multi_block(list_t * list,  const attack_t * attack)
{
	int from,to,paojiazi,sq,temp,leg;
	int piece,opp,opp_flag;
	int inc;
	int me;

	me=Turn;
	opp = COLOUR_OPP(me);
    opp_flag = COLOUR_FLAG(opp);

	if(attack->dn>1)
	{
		from=attack->ds[0];
		piece=Square[from];
		
        to=Piece[me<<4];

		if(PIECE_IS_CANNON(piece))               //敌方炮将
		{
			inc=AttackTab[to-from + 256];
            sq = from;
		    do
			{
			    sq += inc; 	  
			}while (Square[sq]==Empty);    //统计炮架子 （注：也可以用位行位列来统计）
			paojiazi=sq;  
			
			sq = from;
			temp=to-inc;
            do
			{
			     sq += inc; 
				 if(sq!=paojiazi)
				 {
			         add_block(list,paojiazi,sq,me);  //挡
				 }else
				 {
					 if(PIECE_IS_CANNON(Square[paojiazi]) && FLAG_IS(Square[paojiazi],opp_flag)) //炮架子为敌方炮，构成双炮将，放弃炮到将线路的挡
						 break;
				 }

			 }while (sq!=temp);
            
            //炮架子为本方棋子
			if(FLAG_IS(Square[paojiazi],(32<<me)))  //闪
			{
			    if((inc==16)||(inc==-16))
			       add_flee(list,paojiazi,true); //炮 车 兵 闪
			    else
			       add_flee(list,paojiazi,false);//其他 闪
			}


		}else if(PIECE_IS_ROOK(piece)) //敌方车将
		{
			inc=AttackTab[to-from + 256];
			sq = from;
			sq += inc; 	  
			while (Square[sq]==Empty)
			{
				add_block(list,0,sq,me);    //挡
				sq += inc;
			}

		}else if(PIECE_IS_KNIGHT(piece))          //敌方马将
		{
			 leg = c_HorseLegTab[to - from + 256];
			 add_block(list,0,from+leg,me); //挡马腿
		}
	}
}
*/
static void add_block(list_t * list, int square, int to,int colour)
{	
	int i, piece_tag, from, disp, x, y;
    
    slide_mask_t *rank_mask_ptr, *file_mask_ptr;
    int pawn;

    piece_tag =colour<<4;

	//pawn=PawnFlag|(1<<(5+colour));
    pawn=PawnFlag | (32 <<colour);

  if (colour == 0 ? (to & 0x80) != 0 : (to & 0x80) == 0) {
    if (c_InCity[to]) {

      // 1. 用帅(将)挡，不合法

      // 2. 用仕(士)挡
      for (i = 1; i <= 2; i ++) {
        from = Piece[piece_tag + i];
        if (from != 0 && !is_piece_pinned(from,colour)) {
          if (ProtectTab[from - to + 256] == 2 ) {
            LIST_ADD(list,MOVE_MAKE(from,to));
          }
        }
      }
	}

    // 3. 用相(象)挡
    for (i = 3; i <= 4; i ++) {
      from = Piece[piece_tag + i];
      if (from != 0 && !is_piece_pinned(from,colour)) {
        if (ProtectTab[from - to + 256] == 3 && Square[(to + from) >> 1] == 0 ) {
          LIST_ADD(list,MOVE_MAKE(from,to));
        }
      }
    }
  } else {
    
    // 4. 用兵(卒)横向挡
    for (from = to - 1; from <= to + 1; from += 2) {

		if(from!=square && (Square[from]&pawn)==pawn && !is_piece_pinned(from,colour))
		{
			LIST_ADD(list,MOVE_MAKE(from,to));
		}

    }
  }

  // 5. 用兵(卒)纵向挡
  from = to + (colour == 0 ? 16 : -16);
   if(from!=square && (Square[from] & pawn)==pawn && !is_piece_pinned(from,colour))
  {
	  LIST_ADD(list,MOVE_MAKE(from,to));
  }
  // 6. 用马挡
  for (i = 5; i <= 6; i ++) {
    from = Piece[piece_tag + i];
    if (from != 0 && !is_piece_pinned(from,colour)) {
      disp = c_HorseLegTab[to - from + 256];
      if (disp != 0 && Square[from + disp]==0 ) {
        LIST_ADD(list,MOVE_MAKE(from,to));
      }
    }
  }
  
  x = to & 0xf;
  y = to >> 4;
  rank_mask_ptr = g_RankMaskTab[x - 3] + BitRanks[y];
  file_mask_ptr = g_FileMaskTab[y - 3] + BitFiles[x];

  // 7. 用车、炮挡
  for (i = 7; i <= 10; i ++) {
    from = Piece[piece_tag + i];

    if (square !=from && from != 0 && from != to) {
      if (x == (from & 0xf) && !is_piece_pinned(from,colour)) {
        if ((file_mask_ptr->rook_cap & g_BitFileMask[from]) != 0) {
          LIST_ADD(list,MOVE_MAKE(from,to));
        }
      } else if (y == (from >> 4) && !is_piece_pinned(from,colour)) {
        if ((rank_mask_ptr->rook_cap & g_BitRankMask[from]) != 0) {
          LIST_ADD(list,MOVE_MAKE(from,to));
        }
      }
    }
  }
  
}

void add_flee(list_t * list,int from,bool up)
{
    int me, opp;
    int opp_flag;
    int to;
    int x, y, disp;
    uint8 * ptr,*eye_leg_ptr;
    slide_move_t *slide_move_ptr;
	int piece;

	piece=Square[from];

    me = Turn;
  
    opp = COLOUR_OPP(me);
    opp_flag = COLOUR_FLAG(opp);

	if(PIECE_IS_ADVISOR(piece)) //士闪
	{
		ptr = g_AdvisorMoves[from];
        to = *ptr;
        while (to != 0) {
           if (FLAG_IS(Square[to],opp_flag)||Square[to]==Empty) {
               LIST_ADD(list,MOVE_MAKE(from,to));
           }
           ptr ++;
           to = *ptr;
        }
	
	}else if (PIECE_IS_BISHOP(piece))//象闪
	{
		ptr = g_BishopMoves[from];
        eye_leg_ptr = g_ElephantEyes[from];
        to = *ptr;
        while (to != 0) {
           if (Square[*eye_leg_ptr] == 0) {
              if (FLAG_IS(Square[to],opp_flag)||Square[to]==Empty) {
                  LIST_ADD(list,MOVE_MAKE(from,to));
              }
           }
           ptr ++;
           to = *ptr;
           eye_leg_ptr ++;
         }
	
	}else if (PIECE_IS_KNIGHT(piece))//马闪
	{
		ptr = g_KnightMoves[from];
        eye_leg_ptr = g_HorseLegs[from];
        to = *ptr;
        while (to != 0) {
           if (Square[*eye_leg_ptr] == 0) {
              if (FLAG_IS(Square[to],opp_flag)||Square[to]==Empty) {
                  LIST_ADD(list,MOVE_MAKE(from,to));
              }
           }
           ptr ++;
           to = *ptr;
           eye_leg_ptr ++;
         }
	
	}else if (PIECE_IS_ROOK(piece))//车闪
	{	
		x = from & 0xf;    
	    y = from >> 4;		
		if(up)
		{      
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
		}else
		{
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
	
	}else if (PIECE_IS_CANNON(piece)) //炮闪
	{
		x = from & 0xf;
        y = from >> 4;
	    if(up)
		{
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
		}else
		{
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
	
	}else if (PIECE_IS_PAWN(piece)) //兵闪
	{
		int inc;
		ptr = g_PawnMoves[Turn][from];
        to = *ptr;
        while (to != 0) {
			
			inc=AttackTab[to-from + 256];
			if(up)
			{  //兵行走法
			   if(inc==-1 || inc==1)
               if (FLAG_IS(Square[to],opp_flag)||Square[to]==Empty) {
                   LIST_ADD(list,MOVE_MAKE(from,to));
               }
			}else
			{
			   //兵列走法
			   if(inc==-16 || inc==16)
			   if (FLAG_IS(Square[to],opp_flag)||Square[to]==Empty) {
                   LIST_ADD(list,MOVE_MAKE(from,to));
               }

			}
           ptr ++;
           to = *ptr;
         }
	
	}
}