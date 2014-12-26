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
#include <cstring>

#include "board.h"
#include "colour.h"
//#include "hash.h"
#include "material.h"
//#include "option.h"
#include "piece.h"
//#include "protocol.h"
#include "square.h"
#include "util.h"

static const bool UseTable = true;
static const uint32 TableSize = 256; // 4kB

static const int PawnPhase   = 0;
static const int KnightPhase = 2;
static const int CannonPhase = 2;
static const int RookPhase   = 4;
static const int TotalPhase = PawnPhase * 10 + CannonPhase * 4 + KnightPhase * 4 + RookPhase * 4;

static const int PawnOpening    = 90; 
static const int PawnEndgame    = 90; 
static const int CannonOpening  = 960;
static const int CannonEndgame  = 880;
static const int KnightOpening  = 880;
static const int KnightEndgame  = 960;
static const int RookOpening    = 2000;
static const int RookEndgame    = 2000;
static const int BishopOpening  = 400;
static const int BishopEndgame  = 400;
static const int AdvisorOpening = 400;
static const int AdvisorEndgame = 400;


typedef material_info_t entry_t;

struct material_t {
   entry_t * table;
   uint32 size;
   uint32 mask;
   uint32 used;
   sint64 read_nb;
   sint64 read_hit;
   sint64 write_nb;
   sint64 write_collision;
};

// variables

static material_t Material[1];

// prototypes

static void material_comp_info (material_info_t * info);

void material_init() {

   // UCI options

//   MaterialWeight = (option_get_int("Material") * 256 + 50) / 100;
//   OpeningExchangePenalty = option_get_int("Toga Exchange Bonus");
//   EndgameExchangePenalty = OpeningExchangePenalty; 

   // material table

   Material->size = 0;
   Material->mask = 0;
   Material->table = NULL;
}

// material_alloc()

void material_alloc() {

   ASSERT(sizeof(entry_t)==16);

   if (UseTable) {

      Material->size = TableSize;
      Material->mask = TableSize - 1;
      Material->table = (entry_t *) my_malloc(Material->size*sizeof(entry_t));

      material_clear();
   }
}

// material_clear()

void material_clear() {

   if (Material->table != NULL) {
      memset(Material->table,0,Material->size*sizeof(entry_t));
   }

   Material->used = 0;
   Material->read_nb = 0;
   Material->read_hit = 0;
   Material->write_nb = 0;
   Material->write_collision = 0;
}
void material_get_info(material_info_t * info) 
{
   uint64 key;
   entry_t * entry;

   ASSERT(info!=NULL);
   

   // probe

   if (UseTable) {

      Material->read_nb++;

      key = Lock;
      entry = &Material->table[KEY_INDEX(key)&Material->mask];

      if (entry->lock == KEY_LOCK(key)) {

         // found

         Material->read_hit++;

         *info = *entry;

         return;
      }
   }

   // calculation

   material_comp_info(info);

   // store

   if (UseTable) {

      Material->write_nb++;

      if (entry->lock == 0) { // HACK: assume free entry
         Material->used++;
      } else {
         Material->write_collision++;
      }

      *entry = *info;
      entry->lock = KEY_LOCK(key);
   }

}

static void material_comp_info(material_info_t * info) 
{
	int wp, wc, wn, wr, wb, wa;
    int bp, bc, bn, br, bb, ba;

	int phase;
    int opening, endgame;
	
	wp=Number[RedPawn14];
	wc=Number[RedCannon14];
	wn=Number[RedKnight14];
	wr=Number[RedRook14];
	wb=Number[RedBishop14];
	wa=Number[RedAdvisor14];
	
	bp=Number[BlackPawn14];
	bc=Number[BlackCannon14];
	bn=Number[BlackKnight14];
	br=Number[BlackRook14];
	bb=Number[BlackBishop14];
	ba=Number[BlackAdvisor14];

	//phase
	phase = TotalPhase;

    phase -= wp * PawnPhase;
    phase -= wn * KnightPhase;
    phase -= wc * CannonPhase;
    phase -= wr * RookPhase;

    phase -= bp * PawnPhase;
    phase -= bn * KnightPhase;
    phase -= bc * CannonPhase;
    phase -= br * RookPhase;

   if (phase < 0) phase = 0;

    ASSERT(phase>=0&&phase<=TotalPhase);
    phase = (phase * 256 + (TotalPhase / 2)) / TotalPhase;


	opening = 0;
    endgame = 0;

	opening +=wp*PawnOpening;
    opening +=wc*CannonOpening;
    opening +=wn*KnightOpening;
    opening +=wr*RookOpening;
    opening +=wb*BishopOpening;
    opening +=wa*AdvisorOpening;

	opening -=bp*PawnOpening;
    opening -=bc*CannonOpening;
    opening -=bn*KnightOpening;
    opening -=br*RookOpening;
    opening -=bb*BishopOpening;
    opening -=ba*AdvisorOpening;

    endgame +=wp*PawnEndgame;    
    endgame +=wc*CannonEndgame;   
    endgame +=wn*KnightEndgame;   
    endgame +=wr*RookEndgame;   
    endgame +=wb*BishopEndgame; 
    endgame +=wa*AdvisorEndgame;

	endgame -=bp*PawnEndgame;    
    endgame -=bc*CannonEndgame;   
    endgame -=bn*KnightEndgame;   
    endgame -=br*RookEndgame;   
    endgame -=bb*BishopEndgame; 
    endgame -=ba*AdvisorEndgame;
    
	info->phase=phase;
	info->opening=opening;
	info->endgame=endgame;

}