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
#include ".\move_do.h"
#include "pregen.h"
#include "memory.h"
#include "move.h"
#include "piece.h"
#include "pst.h"

int MoveRecords[256];
int nCurrentStep=0;
// move_do()

void move_do(int move, undo_t * undo) {

   int me, opp;
   int from, to;
   int piece, capture,pos;
   int piece_key;

   
   ASSERT(undo!=NULL);

   //ASSERT(board_is_legal(board));

   // initialise undo

   undo->capture = false;
   undo->turn = Turn;
   undo->opening = Opening;
   undo->endgame = Endgame;
   undo->key = Key;
   undo->lock = Lock;

   // init

   me = Turn;
   opp =COLOUR_OPP(me);

   from = MOVE_FROM(move);
   to = MOVE_TO(move);

   piece = Square[from];
   if(piece==0)
	   printf("Error move piece %x\n ",move);
   pos=piece & 31;
   // update key stack
   Stack[Sp++] = Lock;

   // update turn

   Turn = opp;
   Key ^= g_ZobristKeyPlayer;
   Lock ^= g_ZobristLockPlayer;

   piece_key=PieceToKey32[pos];


   if ((capture=Square[to]) != Empty) {

      undo->capture = true;
      undo->capture_square = to;
      undo->capture_piece = capture;
      
	  //吃掉的子进行清除
	  Piece[capture & 31]=0;
	  Piece_size[PIECE_COLOUR(capture)]--;

	  Number[PieceTo32[capture & 31]]--;

	  Key ^= g_ZobristKeyTable[piece_key][to];
      Lock ^= g_ZobristLockTable[piece_key][to];

   }else
   {
       //没有吃子，只更新位行、位列
	   BitRanks[to>>4] ^= g_BitRankMask[to];
       BitFiles[to & 0xf] ^= g_BitFileMask[to];
   }

   Square[from] = Empty;

   Square[to] = piece;
   
   Piece[pos] = to;

   BitRanks[from>>4] ^= g_BitRankMask[from];
   BitFiles[from & 0xf] ^= g_BitFileMask[from];

   if (piece_key < 5) {
        Opening +=g_PST[piece_key][to] - g_PST[piece_key][from];
 	    Endgame +=g_PST[piece_key+5][to] - g_PST[piece_key+5][from];
    } else {
	    Opening -=g_PST[piece_key-5][254 - to] - g_PST[piece_key-5][254 - from];
	    Endgame -=g_PST[piece_key][254 - to]   - g_PST[piece_key][254 - from];
    }
 
   Key ^= g_ZobristKeyTable[piece_key][to] ^ g_ZobristKeyTable[piece_key][from];
   Lock ^= g_ZobristLockTable[piece_key][to] ^ g_ZobristLockTable[piece_key][from];
   
   // move the piece
}

// move_undo()

void move_undo(int move, const undo_t * undo) {

   int me;
   int from, to;
   int piece,pos;

   
   ASSERT(move_is_ok(move));
   ASSERT(undo!=NULL);

   // init

   me = undo->turn;

   from = MOVE_FROM(move);
   to = MOVE_TO(move);

   piece = Square[to];
   pos=piece & 31;


   ASSERT(COLOUR_IS(piece,me));
   
   Square[from]=piece;
   Piece[pos]=from;

   BitRanks[from>>4] ^= g_BitRankMask[from];
   BitFiles[from & 0xf]^= g_BitFileMask[from];
 
  
   if(undo->capture)//吃子更新
   {
	   Square[to]=undo->capture_piece;
	   Piece[undo->capture_piece & 31]=to;
       
	   Piece_size[PIECE_COLOUR(undo->capture_piece)]++;
	   Number[PieceTo32[undo->capture_piece & 31]]++;
        
   }else
   { 
	   Square[to]=Empty;

	   BitRanks[to>>4] ^= g_BitRankMask[to];
	   BitFiles[to&0xf]^= g_BitFileMask[to];
   }
   
   Turn = undo->turn;

   Opening = undo->opening;
   Endgame = undo->endgame;

   Key = undo->key;
   Lock = undo->lock;

   Sp--;
}

// move_do_null()

void move_do_null( undo_t * undo) {

   
   ASSERT(undo!=NULL);

   //ASSERT(board_is_legal(board));

   // initialise undo

   undo->turn = Turn;
   undo->key = Key;
   undo->lock = Lock;


   // update key stack
   Stack[Sp++] = Lock;
   
   Ply_nb = 0;
   // update turn

   Turn = COLOUR_OPP(Turn);
   Key ^= g_ZobristKeyPlayer;
   Lock ^= g_ZobristLockPlayer;
}

// move_undo_null()

void move_undo_null( const undo_t * undo) {

   
   ASSERT(undo!=NULL);

   // update board info

   Turn = undo->turn;
   Key = undo->key;
   Lock= undo->lock;

   // update key stack

   ASSERT(Sp>0);
   Sp--;

   // debug

   ASSERT(board_is_ok(board));
}
