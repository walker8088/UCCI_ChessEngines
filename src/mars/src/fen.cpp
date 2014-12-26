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
#include ".\fen.h"
#include "pregen.h"
#include "piece.h"
#include "random.h"
#include "pst.h"
#include "value.h"

void add_piece(int square,int piece)
{

	Square[square] = PieceFrom32[piece] | piece;

	Piece[piece] = square;

	Piece_size[PIECE_COLOUR(PieceFrom32[piece])]++;

	Number[PieceTo32[piece]]++;

	BitRanks[square >> 4] ^= g_BitRankMask[square];
	BitFiles[square & 0xf] ^= g_BitFileMask[square];
 
	int piece_key=PieceToKey32[piece];
	Key  ^= g_ZobristKeyTable[piece_key][square];
	Lock ^= g_ZobristLockTable[piece_key][square];

	//位置分统计
	if (piece_key < 5) {
        Opening +=g_PST[piece_key][square];
 	    Endgame +=g_PST[piece_key+5][square];
    } else {
	    Opening -=g_PST[piece_key-5][254 - square];
	    Endgame -=g_PST[piece_key][254 - square];
    }
	
    if(piece<16)
	{
		Opening+=MaterialPiece[piece];
		Endgame+=MaterialPiece[piece];
	}else
	{
		Opening-=MaterialPiece[piece];
		Endgame-=MaterialPiece[piece];
	}
}
int fen_piece(char arg) {
  switch (arg) {
  case 'K':
    return 0;
  case 'A':
    return 1;
  case 'B':
  case 'E':
    return 2;
  case 'N':
  case 'H':
    return 3;
  case 'R':
    return 4;
  case 'C':
    return 5;
  default: // 包括'P'
    return 6;
  }
}
void board_from_fen( const char fen[])
{
    int i, j, k;
    int red_piece[7];
    int black_piece[7];
    const char *char_ptr;

    red_piece[0] = 0;
	red_piece[1] = 1;
    for (i = 2; i < 7; i ++) {
        red_piece[i] = ((i-1) << 1)+1;
    }
    for (i = 0; i < 7; i ++) {
        black_piece[i] = red_piece[i] + 16;
    }
	char_ptr=fen;
	board_clear();
	if (*char_ptr == '\0') {
      return;
    }
	
	// 2. 读取棋盘上的棋子
    i = 3;
    j = 3;
    while (*char_ptr != ' ') {
      if (*char_ptr == '/') {
        j = 3;
        i ++;
        if (i > 12) {
          break;
        }
      } else if (*char_ptr >= '1' && *char_ptr <= '9') {
        for (k = 0; k < (*char_ptr - '0'); k ++) {
          if (j >= 11) {
            break;
          }
          j ++;
        }
      } else if (*char_ptr >= 'A' && *char_ptr <= 'Z') {
        k = fen_piece(*char_ptr);
        if (j <= 11) {
          if (red_piece[k] < 16) {
            if (Piece[red_piece[k]] == 0) {
              add_piece((i << 4) + j, red_piece[k]);
              red_piece[k] ++;
            }
          }
        }
        j ++;
      } else if (*char_ptr >= 'a' && *char_ptr <= 'z') {
        k = fen_piece(*char_ptr + 'A' - 'a');
        if (j <= 11) {
          if (black_piece[k] < 32) {
            if (Piece[black_piece[k]] == 0) {
              add_piece((i << 4) + j, black_piece[k]);
              black_piece[k] ++;
            }
          }
        }
        j ++;
      }
      char_ptr ++;
      if (*char_ptr == '\0') {
        return;
      }
    }
    char_ptr ++;
    
    Turn = (*char_ptr == 'b' ? 1 : 0);
	if(Turn!=0)
	{
		Key ^= g_ZobristKeyPlayer;
        Lock ^= g_ZobristLockPlayer;
	}

}
bool board_to_fen( char fen[], int size) {
	return false;
}