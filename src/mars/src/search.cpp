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
// search.cpp

// includes

#include <csetjmp>

#include "attack.h"
#include "board.h"
//#include "book.h"
#include "colour.h"
#include "list.h"
#include "material.h"
#include "move.h"
#include "move_do.h"
#include "move_gen.h"
//#include "option.h"
//#include "pawn.h"
#include "ucci.h"
#include "pv.h"
#include "search.h"
#include "search_full.h"
#include "sort.h"
#include "trans.h"
#include "util.h"
#include "value.h"
#include "print_info.h"
#include <time.h>
// constants
extern FILE * OutFile;
static const bool UseCpuTime = false; // false
static const bool UseEvent = true; // true

static const bool UseShortSearch = true;
static const int ShortSearchDepth = 1;

static const bool DispBest = true; // true
static const bool DispDepthStart = true; // true
static const bool DispDepthEnd = true; // true
static const bool DispRoot = true; // true
static const bool DispStat = true; // true

static const bool UseEasy = true; // singular move
static const int EasyThreshold = 150;
static const double EasyRatio = 0.20;

static const bool UseEarly = true; // early iteration end
static const double EarlyRatio = 0.60;

static const bool UseBad = true;
static const int BadThreshold = 50; // 50
static const bool UseExtension = true;

uint32 start_timer, finish_timer;
long min_timer, max_timer;
int ponder=0;
int stop_thinking=0;
int batch=0;
// variables
//search_param_t SearchStack[HeightMax];
search_input_t SearchInput[1];
search_info_t SearchInfo[1];
search_root_t SearchRoot[1];
search_current_t SearchCurrent[1];
search_best_t SearchBest[1];
int node_qs;
// prototypes

//static void search_send_stat ();

// functions

// depth_is_ok()

bool depth_is_ok(int depth) {

   return depth > -128 && depth < DepthMax;
}

// height_is_ok()

bool height_is_ok(int height) {

   return height >= 0 && height < HeightMax;
}

// search_clear()

void search_clear() {

   // SearchInput

   SearchInput->infinite = false;
   SearchInput->depth_is_limited = false;
   SearchInput->depth_limit = 0;
   SearchInput->time_is_limited = false;
   SearchInput->time_limit_1 = 0.0;
   SearchInput->time_limit_2 = 0.0;

   // SearchInfo

   SearchInfo->can_stop = false;
   SearchInfo->stop = false;
   SearchInfo->check_nb = 10000; // was 100000
   SearchInfo->check_inc = 10000; // was 100000
   SearchInfo->last_time = 0.0;

   // SearchBest

   SearchBest->move = MoveNone;
   SearchBest->value = 0;
   SearchBest->flags = SearchUnknown;
   PV_CLEAR(SearchBest->pv);

   // SearchRoot

   SearchRoot->depth = 0;
   SearchRoot->move = MoveNone;
   SearchRoot->move_pos = 0;
   SearchRoot->move_nb = 0;
   SearchRoot->last_value = 0;
   SearchRoot->bad_1 = false;
   SearchRoot->bad_2 = false;
   SearchRoot->change = false;
   SearchRoot->easy = false;
   SearchRoot->flag = false;

   // SearchCurrent

   SearchCurrent->max_depth = 0;
   SearchCurrent->node_nb = 0;
   SearchCurrent->node_qs_nb = 0;
   SearchCurrent->time = 0.0;
   SearchCurrent->speed = 0.0;
   SearchCurrent->cpu = 0.0;
}

// search()

void search(int depth,long proper_timer, long limit_timer) {


   int i;
   int move_str;
   gen_legal_moves(SearchInput->list);

   // SearchRoot

   list_copy(SearchRoot->list,SearchInput->list);

   // SearchCurrent

   //board_copy(SearchCurrent->SearchInput->board);
   my_timer_reset(SearchCurrent->timer);
   my_timer_start(SearchCurrent->timer);

   	//一、分配搜索时间
   start_timer = clock();
   min_timer = start_timer + uint32(proper_timer*0.618f);
   max_timer = uint32(proper_timer*1.618f);
   if(max_timer > limit_timer)
		max_timer = limit_timer;
	max_timer += start_timer;
	stop_thinking=0;
  
	// init

   trans_inc_date(Trans);

   sort_init();
   search_full_init(SearchRoot->list);

   // iterative deepening

   for ( i= 1; i <=depth; i++) {

      if (DispDepthStart) 
	  {
		  fprintf(OutFile,"info depth %d\n",i);
		  fflush(OutFile);
	  }

      if (UseShortSearch && depth <= ShortSearchDepth) {
         search_full_root(SearchRoot->list,i,SearchShort);//alpha-beta
      } else {
         search_full_root(SearchRoot->list,i,SearchNormal);//pvs
      }

      search_update_current();
      trans_stats(Trans);printf("\n");
      if (DispDepthEnd) {
         printf("info current depth %d seldepth %d time %.0f nodes " S64_FORMAT " nps %.0f\n",i,SearchCurrent->max_depth,SearchCurrent->time*1000.0,SearchCurrent->node_nb,SearchCurrent->speed);
      }

      fprintf(OutFile,"info depth %d score %d pv",i,SearchBest->value);
	  mv_t * move;
	  move=SearchBest->pv;
	  while (*move!= MoveNone)
	  {
		  move_str=move_to_string(*move);
		  fprintf(OutFile, " %.4s", (const char *)&move_str);
	      move++;
	  }
	  fprintf(OutFile,"\n");
	  fflush(OutFile);
      
	  if(stop_thinking)
			break;
	  if(!ponder && clock()>min_timer)
	  		break;

   }
   start_timer = clock() - start_timer;

  if(SearchBest->move !=MoveNone)
  {
      move_str=move_to_string(SearchBest->pv[0]);
      fprintf(OutFile,"bestmove %.4s",(const char *)&move_str);
      move_str=move_to_string(SearchBest->pv[1]);
      fprintf(OutFile," ponder %.4s\n",(const char *)&move_str);
      fflush(OutFile);
  }
   trans_clear(Trans);
   print_list(SearchRoot->list);
   print_board();
   print_cn_move(SearchBest->pv[0]);
   printf("\n");

   save_file("info.txt");

}

void search_update_current() {

   my_timer_t *timer;
   sint64 node_nb;
   double time, speed, cpu;

   timer = SearchCurrent->timer;

   node_nb = SearchCurrent->node_nb;
   time = (UseCpuTime) ? my_timer_elapsed_cpu(timer) : my_timer_elapsed_real(timer);
   speed = (time >= 1.0) ? double(node_nb) / time : 0.0;
   cpu = my_timer_cpu_usage(timer);

   SearchCurrent->time = time;
   SearchCurrent->speed = speed;
   SearchCurrent->cpu = cpu;
}
// 中断引擎思考
int interrupt(void)
{
	if(!ponder && clock() > max_timer)
		stop_thinking = 1;
	else if(!batch) 
	{
		switch(BusyLine(0)) 
		{
			case e_CommIsReady:
				fprintf(OutFile, "readyok\n");
				fflush(OutFile);
				break;

			case e_CommPonderHit:
				if(ponder != 2) 
					ponder = 0;
				break;

			case e_CommStop:
				stop_thinking = 1;
				break;
		}
	}

	return stop_thinking;
}
// search_check()
/*
void search_check() {

   search_send_stat();

   if (UseEvent) event();

   if (SearchInput->depth_is_limited
    && SearchRoot->depth > SearchInput->depth_limit) {
      SearchRoot->flag = true;
   }

   if (SearchInput->time_is_limited
    && SearchCurrent->time >= SearchInput->time_limit_2) {
      SearchRoot->flag = true;
   }

   if (SearchInput->time_is_limited
    && SearchCurrent->time >= SearchInput->time_limit_1
    && !SearchRoot->bad_1
    && !SearchRoot->bad_2
    && (!UseExtension || SearchRoot->move_pos == 0)) {
      SearchRoot->flag = true;
   }

   if (SearchInfo->can_stop
    && (SearchInfo->stop || (SearchRoot->flag && !SearchInput->infinite))) {
      longjmp(SearchInfo->buf,1);
   }
}

// search_send_stat()

static void search_send_stat() {

   double time, speed, cpu;
   sint64 node_nb;

   search_update_current();

   if (DispStat && SearchCurrent->time >= SearchInfo->last_time + 1.0) { // at least one-second gap

      SearchInfo->last_time = SearchCurrent->time;

      time = SearchCurrent->time;
      speed = SearchCurrent->speed;
      cpu = SearchCurrent->cpu;
      node_nb = SearchCurrent->node_nb;

      send("info time %.0f nodes " S64_FORMAT " nps %.0f cpuload %.0f",time*1000.0,node_nb,speed,cpu*1000.0);

      trans_stats(Trans);
   }
}
*/
// end of search.cpp

