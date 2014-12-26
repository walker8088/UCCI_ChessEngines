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
// search_full.cpp

// includes

#include "attack.h"
#include "board.h"
#include "colour.h"
#include "eval.h"
#include "list.h"
#include "move.h"
#include "move_check.h"
#include "move_do.h"
//#include "option.h"
#include "piece.h"
//#include "pst.h"
#include "pv.h"
//#include "recog.h"
#include "search.h"
#include "search_full.h"
#include "see.h"
#include "sort.h"
#include "trans.h"
#include "util.h"
#include "value.h"
#include "print_info.h"

// constants and variables

// main search
#define MIN(X, Y)  ((X) < (Y) ? (X) : (Y))

static const bool UseDistancePruning = true;

// transposition table

static const bool UseTrans = true;
static const int TransDepth = 1;

static const bool UseMateValues = true; // use mate values from shallower searches?

// null move

static /* const */ bool UseNull = true;
static /* const */ bool UseNullEval = true; // true
static const int NullDepth = 2;
static /* const */ int NullReduction = 3;

static /* const */ bool UseVer = true;
static /* const */ bool UseVerEndgame = true; // true
static /* const */ int VerReduction = 5; // was 3

// move ordering

static const bool UseIID = true;
static const int IIDDepth = 3;
static const int IIDReduction = 2;

// extensions

static const bool ExtendSingleReply = true; // true

// history pruning

static /* const */ bool UseHistory = true;
static const int HistoryDepth = 3;
static const int HistoryMoveNb = 3;
static /* const */ int HistoryValue = 9830; // 60%
static /* const */ int HistoryBound = 2458; // * 16384 + 50) / 100 10%=1638 15%=2458 20%=3277
static /* const */ bool UseExtendedHistory = true;
static const bool HistoryReSearch = true;

// futility pruning

static /* const */ bool UseFutility = false; // false
static /* const */ int FutilityMargin = 100;

static /* const */ int FutilityMargin1 = 100;
static /* const */ int FutilityMargin2 = 300;
static /* const */ int FutilityMargin3 = 950;

// quiescence search

static /* const */ bool UseDelta = false; // false
static /* const */ int DeltaMargin = 50;

static /* const */ int CheckNb = 1;
static /* const */ int CheckDepth = 0; // 1 - CheckNb

// misc

static const int NodeAll = -1;
static const int NodePV  =  0;
static const int NodeCut = +1;

// macros

#define NODE_OPP(type)     (-(type))
#define DEPTH_MATCH(d1,d2) ((d1)>=(d2))

// prototypes

static int  full_root            (list_t * list,  int alpha, int beta, int depth, int height, int search_type);

static int  full_search          ( int alpha, int beta, int depth, int height, mv_t pv[], int node_type);
static int  full_no_null         ( int alpha, int beta, int depth, int height, mv_t pv[], int node_type, int trans_move, int * best_move);

static int  full_quiescence      ( int alpha, int beta, int depth, int height, mv_t pv[]);

static int  full_new_depth       (int depth, int move,  bool single_reply, bool in_pv);

static bool do_null              ();
static bool do_ver               ();

static void pv_fill              (const mv_t pv[]);

static bool move_is_dangerous    (int move);
static bool capture_is_dangerous (int move);

static bool simple_stalemate     ();

// functions

// search_full_init()

void search_full_init(list_t * list) {

//   const char * string;
   int trans_move, trans_min_depth, trans_max_depth, trans_min_value, trans_max_value;

   
   ASSERT(board_is_ok(board));


   list_note(list);
   list_sort(list);

   // basic sort

   trans_move = MoveNone;
   if (UseTrans) trans_retrieve(Trans,Lock,&trans_move,&trans_min_depth,&trans_max_depth,&trans_min_value,&trans_max_value);

   note_moves(list,0,trans_move);
   list_sort(list);

   //print_list(list);
}

// search_full_root()

int search_full_root(list_t * list,  int depth, int search_type) {

   int value;

   value = full_root(list,-ValueInf,+ValueInf,depth,0,search_type);

   return value;
}

// full_root()

static int full_root(list_t * list,  int alpha, int beta, int depth, int height, int search_type) {

   int old_alpha;
   int value, best_value;
   int i, move;
   int new_depth;
   undo_t undo[1];
   mv_t new_pv[HeightMax];

   // init

   //SearchStack[height].best_move = MoveNone;
   //SearchStack[height].move = MoveNone;
   //SearchStack[height].threat_move = MoveNone;
   //SearchStack[height].reduced = false;

   SearchCurrent->node_nb++;
   SearchInfo->check_nb--;

   for (i = 0; i < LIST_SIZE(list); i++) list->value[i] = ValueNone;

   old_alpha = alpha;
   best_value = ValueNone;

   // move loop

   for (i = 0; i < LIST_SIZE(list); i++) {
	 //  SearchCurrent->node_nb++;
      
      move = LIST_MOVE(list,i);
      
	 // printf("\nMOVE(full_root)=");print_move(move);printf("\n");
      //SearchStack[height].move = move;
      SearchRoot->depth = depth;
      SearchRoot->move = move;
      SearchRoot->move_pos = i;
      SearchRoot->move_nb = LIST_SIZE(list);

      //search_update_root();

      new_depth = full_new_depth(depth,move,board_is_check()&&LIST_SIZE(list)==1,true);

      move_do(move,undo);

      if (search_type == SearchShort || best_value == ValueNone) { // first move
         value = -full_search(-beta,-alpha,new_depth,height+1,new_pv,NodePV);
      } else { // other moves
         value = -full_search(-alpha-1,-alpha,new_depth,height+1,new_pv,NodeCut);
         if (value > alpha) { // && value < beta
            SearchRoot->change = true;
            SearchRoot->easy = false;
            SearchRoot->flag = false;
            //search_update_root();
            value = -full_search(-beta,-alpha,new_depth,height+1,new_pv,NodePV);
         }
      }
      move_undo(move,undo);
      if (value <= alpha) { // upper bound
         list->value[i] = old_alpha;
      } else if (value >= beta) { // lower bound
         list->value[i] = beta;
      } else { // alpha < value < beta => exact value
         list->value[i] = value;
      }
     
      if (value > best_value && (best_value == ValueNone || value > alpha)) {

         SearchBest->move = move;
		 //SearchStack[height].best_move = move;
         SearchBest->value = value;
         if (value <= alpha) { // upper bound
            SearchBest->flags = SearchUpper;
         } else if (value >= beta) { // lower bound
            SearchBest->flags = SearchLower;
         } else { // alpha < value < beta => exact value
            SearchBest->flags = SearchExact;
         }
         SearchBest->depth = depth;
         pv_cat(SearchBest->pv,new_pv,move);

		 //search_update_best();
      }
      
      if (value > best_value) {
         best_value = value;
         if (value > alpha) {
            if (search_type == SearchNormal) alpha = value;
            if (value >= beta) break;
         }
      }
   }

   ASSERT(value_is_ok(best_value));

   list_sort(list);

   ASSERT(SearchBest->move==LIST_MOVE(list,0));
   ASSERT(SearchBest->value==best_value);
   if (UseTrans && best_value > old_alpha && best_value < beta) {
      pv_fill(SearchBest->pv);
   }

   return best_value;
}

// full_search()

static int full_search( int alpha, int beta, int depth, int height, mv_t pv[], int node_type) {

   bool in_check;
   bool single_reply;
   int trans_move, trans_depth, trans_min_depth, trans_max_depth, trans_min_value, trans_max_value;
   int min_value, max_value;
   int old_alpha;
   int value, best_value;
   int move, best_move;
   int new_depth;
   int played_nb;
   int i;
   int opt_value;
   bool reduced;
   int FutilityMargin;
   attack_t attack[1];
   sort_t sort[1];
   undo_t undo[1];
   mv_t new_pv[HeightMax];
   mv_t played[256];

   // horizon?

   if (depth <= 0) return full_quiescence(alpha,beta,0,height,pv);

   // init

   SearchCurrent->node_nb++;
   SearchInfo->check_nb--;
   PV_CLEAR(pv);
   PV_CLEAR(new_pv);
   PV_CLEAR(played);
   
    if (UseDistancePruning) {

      // lower bound

      value = VALUE_MATE(height+2); // does not work if the current position is mate
      if (value > alpha && board_is_mate()) value = VALUE_MATE(height);

      if (value > alpha) {
         alpha = value;
         if (value >= beta) return value;
      }

      // upper bound

      value = -VALUE_MATE(height+1);

      if (value < beta) {
         beta = value;
         if (value <= alpha) return value;
      }
   }
   
   trans_move = MoveNone;
  
   if (UseTrans && depth >= TransDepth) {

       if (trans_retrieve(Trans,Lock,&trans_move,&trans_min_depth,&trans_max_depth,&trans_min_value,&trans_max_value)) {

         // trans_move is now updated
	  
        if (node_type != NodePV) {

            if (UseMateValues) {

               if (trans_min_value > +ValueEvalInf && trans_min_depth < depth) {
                  trans_min_depth = depth;
               }

               if (trans_max_value < -ValueEvalInf && trans_max_depth < depth) {
                  trans_max_depth = depth;
               }
            }

            min_value = -ValueInf;

            if (DEPTH_MATCH(trans_min_depth,depth)) {
               min_value = value_from_trans(trans_min_value,height);
               if (min_value >= beta) return min_value;
            }

            max_value = +ValueInf;

            if (DEPTH_MATCH(trans_max_depth,depth)) {
               max_value = value_from_trans(trans_max_value,height);
               if (max_value <= alpha) return max_value;
            }

            if (min_value == max_value) return min_value; // exact match
         }
      }
   }
   
   if (height >= HeightMax-1) return eval(0,0);

   if(stop_thinking)
	   return ValueNone;
   if( interrupt())
	   return ValueNone;

   old_alpha = alpha;
   best_value = ValueNone;
   best_move = MoveNone;
   played_nb = 0;

   attack_set(attack);
   in_check = ATTACK_IN_CHECK(attack);
   
   if (UseNull && depth >= NullDepth && node_type != NodePV) {

      if (!in_check
       && !value_is_mate(beta)
       && do_null()
       && (!UseNullEval || depth <= NullReduction+1 || eval(0,0) >= beta)) {

         // null-move search

         new_depth = depth - NullReduction - 1;

         move_do_null(undo);
         value = -full_search(-beta,-beta+1,new_depth,height+1,new_pv,NODE_OPP(node_type));
         move_undo_null(undo);

         // verification search

         if (UseVer && depth > VerReduction) {

            if (value >= beta && (!UseVerEndgame || do_ver())) {

               new_depth = depth - VerReduction;
               ASSERT(new_depth>0);

               value = full_no_null(alpha,beta,new_depth,height,new_pv,NodeCut,trans_move,&move);

               if (value >= beta) {
                  ASSERT(move==new_pv[0]);
                  played[played_nb++] = move;
                  best_move = move;
				  //SearchStack[height].move = move;
				  //SearchStack[height].best_move = move;
                  best_value = value;
                  pv_copy(pv,new_pv);
                  goto cut;
               }
			   //SearchStack[height].threat_move = //SearchStack[height+1].best_move;
		       /*if (//SearchStack[height-1].reduced){ // Idea by Tord Romstad 
				if ( MOVE_FROM(//SearchStack[height+1].best_move) == MOVE_TO(//SearchStack[height-1].move))
					return alpha-1;
		       }*/
            }
         }

         // pruning

         if (value >= beta) {

            if (value > +ValueEvalInf) value = +ValueEvalInf; // do not return unproven mates

            // pv_cat(pv,new_pv,MoveNull);

            best_move = MoveNone;
            best_value = value;
            goto cut;
         }

      }
   }


   
   if (UseIID && depth >= IIDDepth && node_type == NodePV && trans_move == MoveNone) {

      //new_depth = depth - IIDReduction;
	  new_depth = MIN(depth - IIDReduction,depth/2);

      value = full_search(alpha,beta,new_depth,height,new_pv,node_type);
      if (value <= alpha) value = full_search(-ValueInf,beta,new_depth,height,new_pv,node_type);

      trans_move = new_pv[0];
   }

   sort_init(sort,attack,depth,height,trans_move);

   single_reply = false;
   if (in_check && LIST_SIZE(sort->list) == 1) single_reply = true; // HACK

   // move loop

   opt_value = +ValueInf;

   while ((move=sort_next(sort)) != MoveNone) {

	  //SearchStack[height].move = move;

      new_depth = full_new_depth(depth,move,single_reply,node_type==NodePV);

      // history pruning
      
      reduced = false;
      
	  if (UseHistory && depth >= HistoryDepth && node_type != NodePV) {
         if (!in_check && played_nb >= HistoryMoveNb && new_depth < depth) {
            value = sort->value; // history score
            if (value < HistoryValue) {
               new_depth--;
               reduced = true;
            }
         }
      }
/*      
      if (UseFutility && depth == 1 && node_type != NodePV) {

         if (!in_check && new_depth == 0 && !move_is_tactical(move)) {

            ASSERT(!move_is_check(move,board));

            // optimistic evaluation

            if (opt_value == +ValueInf) {
               opt_value = eval(0,0) + FutilityMargin;
               ASSERT(opt_value<+ValueInf);
            }

            value = opt_value;

            // pruning

            if (value <= alpha) {

               if (value > best_value) {
                  best_value = value;
                  PV_CLEAR(pv);
               }

               continue;
            }
         }
      }
*/
	  
	  if (UseFutility && depth == 1 && node_type != NodePV) {

         if (!in_check && new_depth == 0 && !move_is_tactical(move)) {

            ASSERT(!move_is_check(move));

            // optimistic evaluation

            if (opt_value == +ValueInf) {
				if (depth==2){
					FutilityMargin = FutilityMargin2;
				}
				else{
					FutilityMargin = FutilityMargin1;
				}
               opt_value = eval(0,0) + FutilityMargin;
               ASSERT(opt_value<+ValueInf);
            }

            value = opt_value;

            // pruning

            if (value <= alpha) {

               if (value > best_value) {
                  best_value = value;
                  PV_CLEAR(pv);
               }

               continue;
            }
         }
      }
      /*
	  reduced = false;

      if (UseHistory && depth >= HistoryDepth && node_type != NodePV) {
         if (!in_check && played_nb >= HistoryMoveNb && new_depth < depth) {
            value = sort->value; // history score
            if (value < HistoryValue) {
               new_depth--;
               reduced = true;
			   if (UseExtendedHistory && value < HistoryBound && new_depth >= 1){
				   new_depth--;
				   
			   }
            }
         }
      }
      */
      //SearchStack[height].reduced = reduced;
      move_do(move,undo);

      if (node_type != NodePV || best_value == ValueNone) { // first move
         value = -full_search(-beta,-alpha,new_depth,height+1,new_pv,NODE_OPP(node_type));
      } else { // other moves
         value = -full_search(-alpha-1,-alpha,new_depth,height+1,new_pv,NodeCut);
         if (value > alpha) { // && value < beta
            value = -full_search(-beta,-alpha,new_depth,height+1,new_pv,NodePV);
         }
      }

	  if (HistoryReSearch && reduced && value >= beta) {

         ASSERT(node_type!=NodePV);
         //SearchStack[height].reduced = false;
         new_depth++;
         value = -full_search(-beta,-alpha,new_depth,height+1,new_pv,NODE_OPP(node_type));
      }

      move_undo(move,undo);

      played[played_nb++] = move;

      if (value > best_value) {
         best_value = value;
         pv_cat(pv,new_pv,move);
         if (value > alpha) {
            alpha = value;
            best_move = move;
			//SearchStack[height].best_move = move;
            if (value >= beta) goto cut;
         }
      }

      if (node_type == NodeCut) node_type = NodeAll;
   }

   // ALL node

   if (best_value == ValueNone) { // no legal move
      if (in_check) {
         return VALUE_MATE(height);
      } else {
         return ValueDraw;
      }
   }

cut:
     
if (best_move !=MoveNone) {
      good_move(best_move,depth,height);
     if (best_value >= beta && !move_is_tactical(best_move)) {
 
         for (i = 0; i < played_nb-1; i++) {
            move = played[i];
            history_bad(move);
         }

         history_good(best_move);
      }
   }


if (UseTrans && depth >= TransDepth) {

      trans_move = best_move;
      trans_depth = depth;
      trans_min_value = (best_value > old_alpha) ? value_to_trans(best_value,height) : -ValueInf;
      trans_max_value = (best_value < beta)      ? value_to_trans(best_value,height) : +ValueInf;

      trans_store(Trans,Lock,trans_move,trans_depth,trans_min_value,trans_max_value);
   }

   return best_value;
}

// full_no_null()

static int full_no_null( int alpha, int beta, int depth, int height, mv_t pv[], int node_type, int trans_move, int * best_move) {

   int value, best_value;
   int move;
   int new_depth;
   attack_t attack[1];
   sort_t sort[1];
   undo_t undo[1];
   mv_t new_pv[HeightMax];

   // init
   //SearchStack[height].best_move = MoveNone;
   //SearchStack[height].move = MoveNone;
   //SearchStack[height].threat_move = MoveNone;
   //SearchStack[height].reduced = false;


   SearchCurrent->node_nb++;
   SearchInfo->check_nb--;
   PV_CLEAR(pv);
   PV_CLEAR(new_pv);

   if (height > SearchCurrent->max_depth) SearchCurrent->max_depth = height;
   /*
   if (SearchInfo->check_nb <= 0) {
      SearchInfo->check_nb += SearchInfo->check_inc;
      search_check();
   }
   */
   attack_set(attack);
   ASSERT(!ATTACK_IN_CHECK(attack));

   *best_move = MoveNone;
   best_value = ValueNone;

   // move loop

   sort_init(sort,attack,depth,height,trans_move);

   while ((move=sort_next(sort)) != MoveNone) {
      
	  //SearchStack[height].move = move;
      new_depth = full_new_depth(depth,move,false,false);

      move_do(move,undo);
      value = -full_search(-beta,-alpha,new_depth,height+1,new_pv,NODE_OPP(node_type));
      move_undo(move,undo);

      if (value > best_value) {
         best_value = value;
         pv_cat(pv,new_pv,move);
         if (value > alpha) {
            alpha = value;
            *best_move = move;
			//SearchStack[height].best_move = move;
            if (value >= beta) goto cut;
         }
      }
   }

   // ALL node

   if (best_value == ValueNone) { // no legal move => stalemate
      ASSERT(board_is_stalemate(board));
      best_value = ValueDraw;
   }

cut:

   ASSERT(value_is_ok(best_value));

   return best_value;
}

// full_quiescence()

static int full_quiescence( int alpha, int beta, int depth, int height, mv_t pv[]) {

   bool in_check;
   int old_alpha;
   int value, best_value;
   int best_move;
   int move;
   int opt_value;
   attack_t attack[1];
   sort_t sort[1];
   undo_t undo[1];
   mv_t new_pv[HeightMax];

   
   ASSERT(range_is_ok(alpha,beta));
   ASSERT(depth_is_ok(depth));
   ASSERT(height_is_ok(height));
   ASSERT(pv!=NULL);

   ASSERT(board_is_legal(board));
   ASSERT(depth<=0);

   // init
   //SearchStack[height].best_move = MoveNone;
   //SearchStack[height].move = MoveNone;
   //SearchStack[height].threat_move = MoveNone;
   //SearchStack[height].reduced = false;

   SearchCurrent->node_nb++;
   SearchCurrent->node_qs_nb++;
   SearchInfo->check_nb--;
   PV_CLEAR(pv);
   PV_CLEAR(new_pv);

   if (height > SearchCurrent->max_depth) SearchCurrent->max_depth = height;

   // draw?

   //if (board_is_repetition(board) || recog_draw(board)) return ValueDraw;

   // mate-distance pruning
   
   if (UseDistancePruning) {

      // lower bound

      value = VALUE_MATE(height+2); // does not work if the current position is mate
      if (value > alpha && board_is_mate()) value = VALUE_MATE(height);

      if (value > alpha) {
         alpha = value;
         if (value >= beta) return value;
      }

      // upper bound

      value = -VALUE_MATE(height+1);

      if (value < beta) {
         beta = value;
         if (value <= alpha) return value;
      }
   }
   
   // more init

   attack_set(attack);
   in_check = ATTACK_IN_CHECK(attack);

   if (in_check) {
      ASSERT(depth<0);
      depth++; // in-check extension
   }

   // height limit

   if (height >= HeightMax-1) return eval(0,0);

   // more init

   old_alpha = alpha;
   best_value = ValueNone;
   best_move = MoveNone;

    /*if (UseDelta)*/  opt_value = +ValueInf;

   if (!in_check) {

      // lone-king stalemate?

     // if (simple_stalemate(board)) return ValueDraw;

      // stand pat

      value = eval(0,0);

      ASSERT(value>best_value);
      best_value = value;
      if (value > alpha) {
         alpha = value;
         if (value >= beta) goto cut;
      }

      //if (UseDelta) {
      //   opt_value = value + DeltaMargin;
      //   ASSERT(opt_value<+ValueInf);
      //}
   }

   // move loop

   sort_init_qs(sort,attack,depth>=CheckDepth);

   while ((move=sort_next_qs(sort)) != MoveNone) {

	   //SearchStack[height].move = move;
      // delta pruning
/*
      if (UseDelta && beta == old_alpha+1) {

         if (!in_check && !move_is_check(move) /*&& !capture_is_dangerous(move)) {

            ASSERT(move_is_tactical(move));

            // optimistic evaluation

            value = opt_value;

            int to = MOVE_TO(move);
            int capture = Square[to];

            if (capture != Empty) {
               value += VALUE_PIECE(capture);
            } 
            // pruning

            if (value <= alpha) {

               if (value > best_value) {
                  best_value = value;
                  PV_CLEAR(pv);
               }

               continue;
            }
         }
      }
*/
      move_do(move,undo);
      value = -full_quiescence(-beta,-alpha,depth-1,height+1,new_pv);
      move_undo(move,undo);

      if (value > best_value) {
         best_value = value;
         pv_cat(pv,new_pv,move);
         if (value > alpha) {
            alpha = value;
            best_move = move;
			//SearchStack[height].best_move = move;
            if (value >= beta) goto cut;
         }
      }
   }

   // ALL node

   if (best_value == ValueNone) { // no legal move
      ASSERT(board_is_mate(board));
      return VALUE_MATE(height);
   }

cut:

   ASSERT(value_is_ok(best_value));

   return best_value;
}

// full_new_depth()

static int full_new_depth(int depth, int move,  bool single_reply, bool in_pv) {

   int new_depth;

   new_depth = depth - 1;
   
  // if ((single_reply && ExtendSingleReply)
    /*|| (in_pv && MOVE_TO(move) == board->cap_sq // recapture
              && see_move(move) > 0)
    || (in_pv && PIECE_IS_PAWN(MOVE_PIECE(move))
              && PAWN_RANK(MOVE_TO(move),Turn) == Rank7
              && see_move(move) >= 0)*/
   // || 
   if( (single_reply && ExtendSingleReply)) {
     new_depth++;
   }
   
   ASSERT(new_depth>=0&&new_depth<=depth);

   return new_depth;
}

// do_null()

static bool do_null() {

   

   // use null move if the side-to-move has at least one piece

   return Piece_size[Turn] >= 2; // king + one piece
}

// do_ver()

static bool do_ver() {

   

   // use verification if the side-to-move has at most one piece

   return Piece_size[Turn] <= 2; // king + one piece
}

// pv_fill()

static void pv_fill(const mv_t pv[]) {

   int move;
   int trans_move, trans_depth, trans_min_value, trans_max_value;
   undo_t undo[1];

   ASSERT(pv!=NULL);
   

   ASSERT(UseTrans);

   move = *pv;

   if (move != MoveNone && move != MoveNull) {

      move_do(move,undo);
      pv_fill(pv+1);
      move_undo(move,undo);

      trans_move = move;
      trans_depth = -127; // HACK
      trans_min_value = -ValueInf;
      trans_max_value = +ValueInf;

      trans_store(Trans,Lock,trans_move,trans_depth,trans_min_value,trans_max_value);
   }
}

// move_is_dangerous()
/*
static bool move_is_dangerous(int move) {

   int piece;

   ASSERT(move_is_ok(move));
   

   ASSERT(!move_is_tactical(move));

   piece = MOVE_PIECE(move);

   if (PIECE_IS_PAWN(piece)
    && PAWN_RANK(MOVE_TO(move),Turn) >= Rank7) {
      return true;
   }

   return false;
}

// capture_is_dangerous()

static bool capture_is_dangerous(int move) {

   int piece, capture;

   ASSERT(move_is_ok(move));
   

   ASSERT(move_is_tactical(move));

   piece = MOVE_PIECE(move);

   if (PIECE_IS_PAWN(piece)
    && PAWN_RANK(MOVE_TO(move),Turn) >= Rank7) {
      return true;
   }

   capture = move_capture(move);

   if (PIECE_IS_QUEEN(capture)) return true;

   if (PIECE_IS_PAWN(capture)
    && PAWN_RANK(MOVE_TO(move),Turn) <= Rank2) {
      return true;
   }

   return false;
}

// simple_stalemate()

static bool simple_stalemate() {

   int me, opp;
   int king;
   int opp_flag;
   int from, to;
   int capture;
   const inc_t * inc_ptr;
   int inc;

   

   ASSERT(board_is_legal(board));
   ASSERT(!board_is_check(board));

   // lone king?

   me = Turn;
   if (Piece_size[me] != 1 || Pawn_size[me] != 0) return false; // no

   // king in a corner?

   king = KING_POS(me);
   if (king != A1 && king != H1 && king != A8 && king != H8) return false; // no

   // init

   opp = COLOUR_OPP(me);
   opp_flag = COLOUR_FLAG(opp);

   // king can move?

   from = king;

   for (inc_ptr = KingInc; (inc=*inc_ptr) != IncNone; inc_ptr++) {
      to = from + inc;
      capture = Square[to];
      if (capture == Empty || FLAG_IS(capture,opp_flag)) {
         if (!is_attacked(to,opp)) return false; // legal king move
      }
   }

   // no legal move

   ASSERT(board_is_stalemate((board_t*)board));

   return true;
}
*/
// end of search_full.cpp

