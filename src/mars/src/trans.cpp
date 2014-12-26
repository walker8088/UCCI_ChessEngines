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
#include "move.h"
#include ".\trans.h"
#include "util.h"
#include "value.h"


// macros

#define MIN(a,b) ((a)<=(b)?(a):(b))
#define MAX(a,b) ((a)>=(b)?(a):(b))

//#define KEY_INDEX(key) (uint32(key))
//#define KEY_LOCK(key)  (uint32((key) & 0xffffffff))
// constants

// constants

static const bool UseModulo = false;

static const int DateSize = 16;

static const int ClusterSize = 4; // TODO: unsigned?

static const int DepthNone = -128;

// types

struct entry_t {
   uint32 lock;
   uint16 move;
   sint8 depth;
   uint8 date;
   sint8 move_depth;
   uint8 flags;
   sint8 min_depth;
   sint8 max_depth;
   sint16 min_value;
   sint16 max_value;
};

struct trans { // HACK: typedef'ed in trans.h
   entry_t * table;
   uint32 size;
   uint32 mask;
   int date;
   int age[DateSize];
   uint32 used;
   sint64 read_nb;
   sint64 read_hit;
   sint64 write_nb;
   sint64 write_hit;
   sint64 write_collision;
};

// variables

trans_t Trans[1];

// prototypes

static void      trans_set_date (trans_t * trans, int date);
static int       trans_age      (const trans_t * trans, int date);

static entry_t * trans_entry    (trans_t * trans, uint64 key);

static bool      entry_is_ok    (const entry_t * entry);

// functions

// trans_is_ok()

bool trans_is_ok(const trans_t * trans) {

   int date;

   if (trans == NULL) return false;

   if (trans->table == NULL) return false;
   if (trans->size == 0) return false;
   if (trans->mask == 0 || trans->mask >= trans->size) return false;
   if (trans->date >= DateSize) return false;

   for (date = 0; date < DateSize; date++) {
      if (trans->age[date] != trans_age(trans,date)) return false;
   }

   return true;
}

// trans_init()

void trans_init(trans_t * trans) {

   ASSERT(trans!=NULL);

   ASSERT(sizeof(entry_t)==16);

   trans->size = 0;
   trans->mask = 0;
   trans->table = NULL;

   trans_set_date(trans,0);

   trans_clear(trans);

   // ASSERT(trans_is_ok(trans));
}

// trans_alloc()

void trans_alloc(trans_t * trans) {

   uint32 size, target;

   ASSERT(trans!=NULL);

   // calculate size

   target=16;// = option_get_int("Hash");
   if (target < 4) target = 16;
   target *= 1024 * 1024;

   for (size = 1; size != 0 && size <= target; size *= 2)
      ;

   size /= 2;
   ASSERT(size>0&&size<=target);

   // allocate table

   size /= sizeof(entry_t);
   ASSERT(size!=0&&(size&(size-1))==0); // power of 2

   trans->size = size + (ClusterSize - 1); // HACK to avoid testing for end of table
   trans->mask = size - 1;

   trans->table = (entry_t *) my_malloc(trans->size*sizeof(entry_t));

   trans_clear(trans);

   ASSERT(trans_is_ok(trans));
}

// trans_free()

void trans_free(trans_t * trans) {

   ASSERT(trans_is_ok(trans));

   my_free(trans->table);

   trans->table = NULL;
   trans->size = 0;
   trans->mask = 0;
}

// trans_clear()

void trans_clear(trans_t * trans) {

   entry_t clear_entry[1];
   entry_t * entry;
   uint32 index;

   ASSERT(trans!=NULL);

   trans_set_date(trans,0);

   clear_entry->lock = 0;
   clear_entry->move = MoveNone;
   clear_entry->depth = DepthNone;
   clear_entry->date = trans->date;
   clear_entry->move_depth = DepthNone;
   clear_entry->flags = 0;
   clear_entry->min_depth = DepthNone;
   clear_entry->max_depth = DepthNone;
   clear_entry->min_value = -ValueInf;
   clear_entry->max_value = +ValueInf;

   ASSERT(entry_is_ok(clear_entry));

   entry = trans->table;

   for (index = 0; index < trans->size; index++) {
      *entry++ = *clear_entry;
   }
}

// trans_inc_date()

void trans_inc_date(trans_t * trans) {

   ASSERT(trans!=NULL);

   trans_set_date(trans,(trans->date+1)%DateSize);
}

// trans_set_date()

static void trans_set_date(trans_t * trans, int date) {

   ASSERT(trans!=NULL);
   ASSERT(date>=0&&date<DateSize);

   trans->date = date;

   for (date = 0; date < DateSize; date++) {
      trans->age[date] = trans_age(trans,date);
   }

   trans->used = 0;
   trans->read_nb = 0;
   trans->read_hit = 0;
   trans->write_nb = 0;
   trans->write_hit = 0;
   trans->write_collision = 0;
}

// trans_age()

static int trans_age(const trans_t * trans, int date) {

   int age;

   ASSERT(trans!=NULL);
   ASSERT(date>=0&&date<DateSize);

   age = trans->date - date;
   if (age < 0) age += DateSize;

   ASSERT(age>=0&&age<DateSize);

   return age;
}

// trans_store()存储

void trans_store(trans_t * trans, uint64 key, int move, int depth, int min_value, int max_value) {

   entry_t * entry, * best_entry;
   int score, best_score;
   int i;

   ASSERT(trans_is_ok(trans));
   ASSERT(move>=0&&move<65536);
   ASSERT(depth>=-127&&depth<=+127);
   ASSERT(min_value>=-ValueInf&&min_value<=+ValueInf);
   ASSERT(max_value>=-ValueInf&&max_value<=+ValueInf);
   ASSERT(min_value<=max_value);

   // init

   trans->write_nb++;

   // probe

   best_entry = NULL;
   best_score = -32767;

   entry = trans_entry(trans,key);

   for (i = 0; i < ClusterSize; i++, entry++) {

      if (entry->lock == KEY_LOCK(key)) {

         // hash hit => update existing entry

         trans->write_hit++;
         if (entry->date != trans->date) trans->used++;

         entry->date = trans->date;

         if (depth > entry->depth) entry->depth = depth; // for replacement scheme

         if (move != MoveNone && depth >= entry->move_depth) {
            entry->move_depth = depth;
            entry->move = move;
         }

         if (min_value > -ValueInf && depth >= entry->min_depth) {
            entry->min_depth = depth;
            entry->min_value = min_value;
         }

         if (max_value < +ValueInf && depth >= entry->max_depth) {
            entry->max_depth = depth;
            entry->max_value = max_value;
         }

         ASSERT(entry_is_ok(entry));

         return;
      }

      // evaluate replacement score

      score = trans->age[entry->date] * 256 - entry->depth;
      ASSERT(score>-32767);

      if (score > best_score) {
         best_entry = entry;
         best_score = score;
      }
   }

   // "best" entry found

   entry = best_entry;
   ASSERT(entry!=NULL);
   ASSERT(entry->lock!=KEY_LOCK(key));

   if (entry->date == trans->date) {
      trans->write_collision++;
   } else {
      trans->used++;
   }

   // store

   ASSERT(entry!=NULL);

   entry->lock = KEY_LOCK(key);
   entry->date = trans->date;

   entry->depth = depth;

   entry->move_depth = (move != MoveNone) ? depth : DepthNone;
   entry->move = move;

   entry->min_depth = (min_value > -ValueInf) ? depth : DepthNone;
   entry->max_depth = (max_value < +ValueInf) ? depth : DepthNone;
   entry->min_value = min_value;
   entry->max_value = max_value;

   ASSERT(entry_is_ok(entry));
}

// trans_retrieve() 找回

bool trans_retrieve(trans_t * trans, uint64 key, int * move, int * min_depth, int * max_depth, int * min_value, int * max_value) {

   entry_t * entry;
   int i;

   ASSERT(trans_is_ok(trans));
   ASSERT(move!=NULL);
   ASSERT(min_depth!=NULL);
   ASSERT(max_depth!=NULL);
   ASSERT(min_value!=NULL);
   ASSERT(max_value!=NULL);

   // init

   trans->read_nb++;

   // probe

   entry = trans_entry(trans,key);

   for (i = 0; i < ClusterSize; i++, entry++) {

      if (entry->lock == KEY_LOCK(key)) {

         // found

         trans->read_hit++;
         if (entry->date != trans->date) entry->date = trans->date;

         *move = entry->move;

         *min_depth = entry->min_depth;
         *max_depth = entry->max_depth;
         *min_value = entry->min_value;
         *max_value = entry->max_value;

         return true;
      }
   }

   // not found

   return false;
}

// trans_stats()

void trans_stats(const trans_t * trans) {

   double full;
   // double hit, collision;

   ASSERT(trans_is_ok(trans));

   full = double(trans->used) / double(trans->size);
   // hit = double(trans->read_hit) / double(trans->read_nb);
   // collision = double(trans->write_collision) / double(trans->write_nb);

   printf("info hashfull %.0f ",full*1000.0);
}

// trans_entry()

static entry_t * trans_entry(trans_t * trans, uint64 key) {

   uint32 index;

   ASSERT(trans_is_ok(trans));

   if (UseModulo) {
      index = KEY_INDEX(key) % (trans->mask + 1);
   } else {
      index = KEY_INDEX(key) & trans->mask;
   }

   ASSERT(index<=trans->mask);

   return &trans->table[index];
}

// entry_is_ok()

static bool entry_is_ok(const entry_t * entry) {

   if (entry == NULL) return false;

   if (entry->date >= DateSize) return false;

   if (entry->move == MoveNone && entry->move_depth != DepthNone) return false;
   if (entry->move != MoveNone && entry->move_depth == DepthNone) return false;

   if (entry->min_value == -ValueInf && entry->min_depth != DepthNone) return false;
   if (entry->min_value >  -ValueInf && entry->min_depth == DepthNone) return false;

   if (entry->max_value == +ValueInf && entry->max_depth != DepthNone) return false;
   if (entry->max_value <  +ValueInf && entry->max_depth == DepthNone) return false;

   return true;
}

// end of trans.cpp

