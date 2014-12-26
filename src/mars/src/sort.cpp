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
#include "attack.h"
#include "board.h"
#include "colour.h"
#include "list.h"
#include "move.h"
#include "move_check.h"
#include "move_evasion.h"
#include "move_gen.h"
#include "move_legal.h"
#include "piece.h"
#include "search.h"
#include "see.h"
#include "sort.h"
#include "util.h"
#include "value.h"

static const int KillerNb = 2;

static const int HistorySize = 14 * 90;
static const int HistoryMax = 16384;

static const int TransScore   = +32766;
static const int GoodScore    =  +4000;
static const int KillerScore  =     +4;
static const int HistoryScore = -24000;
static const int BadScore     = -28000;

static const int CODE_SIZE = 256;

// macros

#define HISTORY_INC(depth) ((depth)*(depth))

// types

enum gen_t {
   GEN_ERROR,
   GEN_LEGAL_EVASION,
   GEN_TRANS,
   GEN_GOOD_CAPTURE,
   GEN_BAD_CAPTURE,
   GEN_KILLER,
   GEN_QUIET,
   GEN_EVASION_QS,
   GEN_CAPTURE_QS,
   GEN_CHECK_QS,
   GEN_END
};

enum test_t {
   TEST_ERROR,
   TEST_NONE,
   TEST_LEGAL,
   TEST_TRANS_KILLER,
   TEST_GOOD_CAPTURE,
   TEST_BAD_CAPTURE,
   TEST_KILLER,
   TEST_QUIET,
   TEST_CAPTURE_QS,
   TEST_CHECK_QS
};

// variables

static int PosLegalEvasion;
static int PosSEE;

static int PosEvasionQS;
static int PosCheckQS;
static int PosCaptureQS;

static int Code[CODE_SIZE];

static uint16 Killer[HeightMax][KillerNb];

static uint16 History[HistorySize];
static uint16 HistHit[HistorySize];
static uint16 HistTot[HistorySize];

// prototypes

static void note_captures     (list_t * list);
static void note_quiet_moves  (list_t * list);
static void note_moves_simple (list_t * list);
static void note_mvv_lva      (list_t * list);

static int  move_value        (int move,  int height, int trans_killer);
static int  capture_value     (int move);
static int  quiet_move_value  (int move);
static int  move_value_simple (int move);

static int  history_prob      (int move);

static bool capture_is_good   (int move);

static int  mvv_lva           (int move);

static int  history_index     (int move);

void test_sort(sort_t * sort)
{
	attack_t attack[1];
 
	attack_set(attack);

	//test_gen_good_capture(sort->list);
	//sort->gen=
	

	sort_init(sort,attack,0,0,0);

	sort_next(sort);
}

void test_gen_good_capture(list_t * list)
{
	int count;
	list_t temp[1];
	LIST_CLEAR(temp);	
	LIST_CLEAR(list);
	gen_captures(temp);
    note_mvv_lva(temp);
    list_sort(temp);
	count=0;
	for(int i=0;i<temp->size;++i)
	{//
		 //capture_is_good(temp->move[i]);
         if(capture_is_good(temp->move[i]))
	     {
		   LIST_ADD(list,temp->move[i]);
		   list->value[count]=see_move(temp->move[i]);//temp->value[i];
		   count++;
	     }
	}
	list_sort(list);
	
}

void sort_init() {

   int i, height;
   int pos;

   // killer

   for (height = 0; height < HeightMax; height++) {
      for (i = 0; i < KillerNb; i++) Killer[height][i] = MoveNone;
   }

   // history

   for (i = 0; i < HistorySize; i++) History[i] = 0;

   for (i = 0; i < HistorySize; i++) {
      HistHit[i] = 1;
      HistTot[i] = 1;
   }

   // Code[]

   for (pos = 0; pos < CODE_SIZE; pos++) Code[pos] = GEN_ERROR;

   pos = 0;

   // main search

   PosLegalEvasion = pos;
   Code[pos++] = GEN_LEGAL_EVASION;
   Code[pos++] = GEN_END;

   PosSEE = pos;
   Code[pos++] = GEN_TRANS;
   Code[pos++] = GEN_GOOD_CAPTURE;
   Code[pos++] = GEN_KILLER;
   Code[pos++] = GEN_QUIET;
   Code[pos++] = GEN_BAD_CAPTURE;
   Code[pos++] = GEN_END;

   // quiescence search

   PosEvasionQS = pos;
   Code[pos++] = GEN_EVASION_QS;
   Code[pos++] = GEN_END;

   PosCheckQS = pos;
   Code[pos++] = GEN_CAPTURE_QS;
   Code[pos++] = GEN_CHECK_QS;
   Code[pos++] = GEN_END;

   PosCaptureQS = pos;
   Code[pos++] = GEN_CAPTURE_QS;
   Code[pos++] = GEN_END;

   ASSERT(pos<CODE_SIZE);
}

// sort_init()

void sort_init(sort_t * sort,  const attack_t * attack, int depth, int height, int trans_killer) {

   ASSERT(sort!=NULL);
   
   ASSERT(attack!=NULL);
   ASSERT(depth_is_ok(depth));
   ASSERT(height_is_ok(height));
   ASSERT(trans_killer==MoveNone||move_is_ok(trans_killer));

   sort->attack = attack;

   sort->depth = depth;
   sort->height = height;

   sort->trans_killer = trans_killer;
   sort->killer_1 = Killer[sort->height][0];
   sort->killer_2 = Killer[sort->height][1];
   if (sort->height > 2){
	  sort->killer_3 = Killer[sort->height-2][0];
      sort->killer_4 = Killer[sort->height-2][1]; 
   }
   else{
      sort->killer_3 = MoveNone;
      sort->killer_4 = MoveNone;
   }

   if (ATTACK_IN_CHECK(sort->attack)) {

      gen_evasions(sort->list,sort->attack);
      note_moves(sort->list,sort->height,sort->trans_killer);
      list_sort(sort->list);

      sort->gen = PosLegalEvasion + 1;
      sort->test = TEST_NONE;

   } else { // not in check

      LIST_CLEAR(sort->list);
      sort->gen = PosSEE;
   }

   sort->pos = 0;
}

// sort_next()

int sort_next(sort_t * sort) {

   int move;
   int gen;

   ASSERT(sort!=NULL);

   while (true) {

      while (sort->pos < LIST_SIZE(sort->list)) {

         // next move

         move = LIST_MOVE(sort->list,sort->pos);
         sort->value = 16384; // default score
         sort->pos++;

         ASSERT(move!=MoveNone);

         // test

         if (false) {

         } else if (sort->test == TEST_NONE) {

            // no-op

         } else if (sort->test == TEST_TRANS_KILLER) {

            if (!move_is_pseudo(move)) continue;
            if (!pseudo_is_legal(move)) continue;

         } else if (sort->test == TEST_GOOD_CAPTURE) {

            ASSERT(move_is_tactical(move));

            if (move == sort->trans_killer) continue;

            if (!capture_is_good(move)) {
               LIST_ADD(sort->bad,move);
               continue;
            }

            if (!pseudo_is_legal(move)) continue;

         } else if (sort->test == TEST_BAD_CAPTURE) {

            ASSERT(move_is_tactical(move));
            ASSERT(!capture_is_good(move));

            ASSERT(move!=sort->trans_killer);
            if (!pseudo_is_legal(move)) continue;

         } else if (sort->test == TEST_KILLER) {

            if (move == sort->trans_killer) continue;
            if (!quiet_is_pseudo(move)) continue;
            if (!pseudo_is_legal(move)) continue;

            ASSERT(!move_is_tactical(move));

         } else if (sort->test == TEST_QUIET) {

            ASSERT(!move_is_tactical(move));

            if (move == sort->trans_killer) continue;
            if (move == sort->killer_1) continue;
            if (move == sort->killer_2) continue;
			if (move == sort->killer_3) continue;
            if (move == sort->killer_4) continue;
            if (!pseudo_is_legal(move)) continue;

            sort->value = history_prob(move);

         } else {

            ASSERT(false);

            return MoveNone;
         }

         ASSERT(pseudo_is_legal(move));

         return move;
      }

      // next stage

      gen = Code[sort->gen++];

      if (false) {

      } else if (gen == GEN_TRANS) {

         LIST_CLEAR(sort->list);
         if (sort->trans_killer != MoveNone) LIST_ADD(sort->list,sort->trans_killer);

         sort->test = TEST_TRANS_KILLER;

      } else if (gen == GEN_GOOD_CAPTURE) {

         gen_captures(sort->list);
         note_mvv_lva(sort->list);
         list_sort(sort->list);

         LIST_CLEAR(sort->bad);

         sort->test = TEST_GOOD_CAPTURE;

      } else if (gen == GEN_BAD_CAPTURE) {

         list_copy(sort->list,sort->bad);

         sort->test = TEST_BAD_CAPTURE;

      } else if (gen == GEN_KILLER) {

         LIST_CLEAR(sort->list);
         if (sort->killer_1 != MoveNone) LIST_ADD(sort->list,sort->killer_1);
         if (sort->killer_2 != MoveNone) LIST_ADD(sort->list,sort->killer_2);
		 if (sort->killer_3 != MoveNone) LIST_ADD(sort->list,sort->killer_3);
         if (sort->killer_4 != MoveNone) LIST_ADD(sort->list,sort->killer_4);

         sort->test = TEST_KILLER;

      } else if (gen == GEN_QUIET) {

         gen_quiet_moves(sort->list);
         note_quiet_moves(sort->list);
         list_sort(sort->list);

         sort->test = TEST_QUIET;

      } else {

         ASSERT(gen==GEN_END);

         return MoveNone;
      }

      sort->pos = 0;
   }
}

// sort_init_qs()

void sort_init_qs(sort_t * sort,  const attack_t * attack, bool check) {

   ASSERT(sort!=NULL);
   
   ASSERT(attack!=NULL);
   ASSERT(check==true||check==false);

   sort->attack = attack;

   if (ATTACK_IN_CHECK(sort->attack)) {
      sort->gen = PosEvasionQS;
   } else if (check) {
      sort->gen = PosCheckQS;
   } else {
      sort->gen = PosCaptureQS;
   }

   LIST_CLEAR(sort->list);
   sort->pos = 0;
}

// sort_next_qs()

int sort_next_qs(sort_t * sort) {

   int move;
   int gen;

   ASSERT(sort!=NULL);

   while (true) {

      while (sort->pos < LIST_SIZE(sort->list)) {

         // next move

         move = LIST_MOVE(sort->list,sort->pos);
         sort->pos++;

         ASSERT(move!=MoveNone);

         // test

         if (false) {

         } else if (sort->test == TEST_LEGAL) {

            if (!pseudo_is_legal(move)) continue;

         } else if (sort->test == TEST_CAPTURE_QS) {

            ASSERT(move_is_tactical(move));

            if (!capture_is_good(move)) continue;
            if (!pseudo_is_legal(move)) continue;

         } else if (sort->test == TEST_CHECK_QS) {

            ASSERT(!move_is_tactical(move));
            ASSERT(move_is_check(move));

            if (see_move(move) < 0) continue;
            if (!pseudo_is_legal(move)) continue;

         } else {

            ASSERT(false);

            return MoveNone;
         }

         ASSERT(pseudo_is_legal(move));

         return move;
      }

      // next stage

      gen = Code[sort->gen++];

      if (false) {

      } else if (gen == GEN_EVASION_QS) {

         gen_evasions(sort->list,sort->attack);
         note_moves_simple(sort->list);
         list_sort(sort->list);

         sort->test = TEST_LEGAL;

      } else if (gen == GEN_CAPTURE_QS) {

         gen_captures(sort->list);
         note_mvv_lva(sort->list);
         list_sort(sort->list);

         sort->test = TEST_CAPTURE_QS;

      } else if (gen == GEN_CHECK_QS) {

         gen_quiet_checks(sort->list);

         sort->test = TEST_CHECK_QS;

      } else {

         ASSERT(gen==GEN_END);

         return MoveNone;
      }

      sort->pos = 0;
   }
}

// good_move()

void good_move(int move,  int depth, int height) {

   int index;
   int i;

   ASSERT(move_is_ok(move));
   
   ASSERT(depth_is_ok(depth));
   ASSERT(height_is_ok(height));

   if (move_is_tactical(move)) return;

   // killer

   if (Killer[height][0] != move) {
      Killer[height][1] = Killer[height][0];
      Killer[height][0] = move;
   }

   ASSERT(Killer[height][0]==move);
   ASSERT(Killer[height][1]!=move);

   // history

   index = history_index(move);

   //printf("index=%d ",index);

   History[index] += HISTORY_INC(depth);

   if (History[index] >= HistoryMax) {
      for (i = 0; i < HistorySize; i++) {
         History[i] = (History[i] + 1) / 2;
      }
   }
}

// history_good()

void history_good(int move) {

   int index;

   ASSERT(move_is_ok(move));
   

   if (move_is_tactical(move)) return;

   // history

   index = history_index(move);

   HistHit[index]++;
   HistTot[index]++;

   if (HistTot[index] >= HistoryMax) {
      HistHit[index] = (HistHit[index] + 1) / 2;
      HistTot[index] = (HistTot[index] + 1) / 2;
   }

   ASSERT(HistHit[index]<=HistTot[index]);
   ASSERT(HistTot[index]<HistoryMax);
}

// history_bad()

void history_bad(int move) {

   int index;

   ASSERT(move_is_ok(move));
   

   if (move_is_tactical(move)) return;

   // history

   index = history_index(move);

   HistTot[index]++;

   if (HistTot[index] >= HistoryMax) {
      HistHit[index] = (HistHit[index] + 1) / 2;
      HistTot[index] = (HistTot[index] + 1) / 2;
   }

   ASSERT(HistHit[index]<=HistTot[index]);
   ASSERT(HistTot[index]<HistoryMax);
}

// note_moves()

void note_moves(list_t * list,  int height, int trans_killer) {

   int size;
   int i, move;

   
   
   ASSERT(height_is_ok(height));
   ASSERT(trans_killer==MoveNone||move_is_ok(trans_killer));

   size = LIST_SIZE(list);

   if (size >= 2) {
      for (i = 0; i < size; i++) {
         move = LIST_MOVE(list,i);
         list->value[i] = move_value(move,height,trans_killer);
      }
   }
}

// note_captures()

static void note_captures(list_t * list) {

   int size;
   int i, move;

   size = LIST_SIZE(list);

   if (size >= 2) {
      for (i = 0; i < size; i++) {
         move = LIST_MOVE(list,i);
         list->value[i] = capture_value(move);
      }
   }
}

// note_quiet_moves()

static void note_quiet_moves(list_t * list) {

   int size;
   int i, move;

   
   

   size = LIST_SIZE(list);

   if (size >= 2) {
      for (i = 0; i < size; i++) {
         move = LIST_MOVE(list,i);
         list->value[i] = quiet_move_value(move);
      }
   }
}

// note_moves_simple()

static void note_moves_simple(list_t * list) {

   int size;
   int i, move;

   size = LIST_SIZE(list);

   if (size >= 2) {
      for (i = 0; i < size; i++) {
         move = LIST_MOVE(list,i);
         list->value[i] = move_value_simple(move);
      }
   }
}

// note_mvv_lva()

static void note_mvv_lva(list_t * list) {

   int size;
   int i, move;

   
   

   size = LIST_SIZE(list);

   if (size >= 2) {
      for (i = 0; i < size; i++) {
         move = LIST_MOVE(list,i);
         list->value[i] = mvv_lva(move);
      }
   }
}

// move_value()

static int move_value(int move,  int height, int trans_killer) {

   int value;

   ASSERT(move_is_ok(move));
   
   ASSERT(height_is_ok(height));
   ASSERT(trans_killer==MoveNone||move_is_ok(trans_killer));

   if (false) {
   } else if (move == trans_killer) { // transposition table killer
      value = TransScore;
   } else if (move_is_tactical(move)) { // capture or promote
      value = capture_value(move);
   } else if (move == Killer[height][0]) { // killer 1
      value = KillerScore;
   } else if (move == Killer[height][1]) { // killer 2
      value = KillerScore - 1;
   } else { // quiet move
      value = quiet_move_value(move);
   }

   return value;
}
// capture_value()

static int capture_value(int move) {

   int value;

   ASSERT(move_is_ok(move));
   

   ASSERT(move_is_tactical(move));

   value = mvv_lva(move);

   if (capture_is_good(move)) {
      value += GoodScore;
   } else {
      value += BadScore;
   }

   ASSERT(value>=-30000&&value<=+30000);

   return value;
}

// quiet_move_value()

static int quiet_move_value(int move) {

   int value;
   int index;

   ASSERT(move_is_ok(move));
   

   ASSERT(!move_is_tactical(move));

   index = history_index(move);

   value = HistoryScore + History[index];
   ASSERT(value>=HistoryScore&&value<=KillerScore-4);

   return value;
}

// move_value_simple()

static int move_value_simple(int move) {

   int value;

   ASSERT(move_is_ok(move));
   

   value = HistoryScore;
   if (move_is_tactical(move)) value = mvv_lva(move);

   return value;
}

// history_prob()

static int history_prob(int move) {

   int value;
   int index;

   ASSERT(move_is_ok(move));
   

   ASSERT(!move_is_tactical(move));

   index = history_index(move);

   ASSERT(HistHit[index]<=HistTot[index]);
   ASSERT(HistTot[index]<HistoryMax);

   value = (HistHit[index] * 16384) / HistTot[index];
   ASSERT(value>=0&&value<=16384);

   return value;
}


static bool capture_is_good(int move) {

   int piece, capture;

   capture = Square[MOVE_TO(move)];

   if (capture != Empty) {

      piece = Square[MOVE_FROM(move)];

      if (VALUE_PIECE(capture & 31) >= VALUE_PIECE(piece & 31)) return true;
   }

   return see_move(move) >= 0;
}
static int mvv_lva(int move) {

   int piece, capture;
   int value;

   if ((capture = Square[MOVE_TO(move)]) != Empty) { // normal capture

      piece = Square[MOVE_FROM(move)];

      value = MvvValues[capture & 31] * 6 - MvvValues[piece & 31];// + 5;
	  //value = MvvValues[capture & 31]  - MvvValues[piece & 31];
      //ASSERT(value>=0&&value<30);

   }

   return value;
}


static int history_index(int move) {

   int index;
   
   ASSERT(move_is_ok(move));
   

   ASSERT(!move_is_tactical(move));

   //index = PIECE_TO_12(Square[MOVE_FROM(move)]) * 64 + SQUARE_TO_64(MOVE_TO(move));
   index=PieceTo32[Square[MOVE_FROM(move)] & 31]*90+SquareTo90[MOVE_TO(move)];
   ASSERT(index>=0&&index<HistorySize);

   return index;
}

