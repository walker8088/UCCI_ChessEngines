/*! \file board.cpp
@brief ∆Â≈Ã±Ì æ¿‡
*/

#include <stdio.h>
#include "board.h"

Board::Board()
{
}

Board::Board(const char * fen, int nMoveNum, long * lpdwCoordList)
{
    FromFen(fen, nMoveNum, lpdwCoordList);
}

void Board::AddMovement(const Movement & move)
{
    lastmove = move;
    if (position[move.to] == B_KING)
    {
        kingpos[BLACK] = 0x00;
    }
    if (position[move.to] == R_KING)
    {
        kingpos[RED] = 0x00;
    }
    if (position[move.to] != NONE)
    {
        numOfChess--;
    }
    position[move.to] = position[move.from];
    position[move.from] = NONE;
    if (position[move.to] == B_KING)
    {
        kingpos[BLACK] = move.to;
    }
    if (position[move.to] == R_KING)
    {
        kingpos[RED] = move.to;
    }
    zobrist ^= Hashnum::GetHash(move.ate, move.to);
    zobrist ^= Hashnum::GetHash(position[move.to], move.from);
    zobrist ^= Hashnum::GetHash(position[move.to], move.to);
}

void Board::DelMovement(const Movement & move)
{
    lastmove.from = 0x00; 
    lastmove.to = 0x00; 
    lastmove.ate = NONE; 
    lastmove.check = FALSE;
    if (kingpos[BLACK] == 0x00)
    {
        kingpos[BLACK] = move.to;
    }
    if (kingpos[RED] == 0x00)
    {
        kingpos[RED] = move.to;
    }
    if (move.ate != NONE)
    {
        numOfChess++;
    }
    position[move.from] = position[move.to];
    position[move.to] = move.ate;
    if (position[move.from] == B_KING)
    {
        kingpos[BLACK] = move.from;
    }
    if (position[move.from] == R_KING)
    {
        kingpos[RED] = move.from;
    }
    zobrist ^= Hashnum::GetHash(move.ate, move.to);
    zobrist ^= Hashnum::GetHash(position[move.from], move.from);
    zobrist ^= Hashnum::GetHash(position[move.from], move.to);
}

Bool Board::CanGo(const char * str)
{
    int f, t;
    f = (BDPOINT)((str[0] - 'a' + 3) + (('9' - str[1] + 3) << 4));
    t = (BDPOINT)((str[2] - 'a' + 3) + (('9' - str[3] + 3) << 4));
    if (player == RED)
    {
        return IsRed(position[f]) && IsBlackOrNone(position[t]);    
    }
    else
    {
        return IsBlack(position[f]) && IsRedOrNone(position[t]);
    }
}

void Board::ClearBoard()
{
    int i, j;
    for (i = 0x33; i <= 0xcb; i++)
    {
        position[i] = NONE;  
    }
    for (i = 0x00; i <= 0x32; i++)
    {
        position[i] = OUT;    
    }
    for (i = 0xcc; i <= 0xff; i++)
    {
        position[i] = OUT;    
    }
    for (i = 0x3c; i <= 0xbc; i += 0x10)
    {
        for (j = 0x00; j <= 0x06; j++)
        {
            position[i + j] = OUT;    
        }
    }
    kingpos[RED] = 0xc7;
    kingpos[BLACK] = 0x37;
    numOfChess = 0;
    lastmove.from = 0x00; 
    lastmove.to = 0x00; 
    lastmove.ate = NONE; 
    lastmove.check = FALSE;
    memset(&zobrist, 0, sizeof(HASHNUM)); 
} 

void Board::FromFen(const char * fen, int nMoveNum, long * lpdwCoordList)
{
    ClearBoard();
    if (!fen)
    {
        return;    
    }
    BDPOINT i = 0x33;
    while (*fen != ' ')
    {
        if ((i & 0x0f) == 0x0c)
        {
            i &= 0xf0;
            i += 0x13;
        }
        if (*fen >= '0' && *fen <= '9')
        {
            i += *fen - '0';
        }
        else if (*fen >= 'a' && *fen <= 'z' || *fen >= 'A' && *fen <= 'Z')
        {
            switch (*fen)
            {
            case 'k':
                position[i] = B_KING;
                kingpos[BLACK] = i;
                break;  
            case 'a':
                position[i] = B_ADVISOR;
                break; 
            case 'b':
                position[i] = B_BISHOP;
                break; 
            case 'n':
                position[i] = B_KNIGHT;
                break; 
            case 'r':
                position[i] = B_ROOK;
                break; 
            case 'c':
                position[i] = B_CANNON;
                break; 
            case 'p':
                position[i] = B_PAWN;
                break; 
            case 'K':
                position[i] = R_KING;
                kingpos[RED] = i;
                break; 
            case 'A':
                position[i] = R_ADVISOR;
                break; 
            case 'B':
                position[i] = R_BISHOP;
                break; 
            case 'N':
                position[i] = R_KNIGHT;
                break; 
            case 'R':
                position[i] = R_ROOK;
                break; 
            case 'C':
                position[i] = R_CANNON;
                break; 
            case 'P':
                position[i] = R_PAWN;
                break;
            }
            zobrist ^= Hashnum::GetHash(position[i], i);
            i += 0x01;
            numOfChess += 1;
        }
        fen += 1;
    }
    fen += 1;
    player = (*fen == 'b') ? BLACK : RED;
    fen += 6;
    sscanf(fen, "%d %d", &non_eat_steps, &bouts);
    player = ((CHESSMAN)nMoveNum & 0x01) ? ((PLAYER)1 - player) : player;
    int k;
    BDPOINT from, to;
    for (k = 0; k < nMoveNum; k++)
    {
        char * str = (char *)(&(lpdwCoordList[k]));
        from = (BDPOINT)((str[0] - 'a' + 3) + (('9' - str[1] + 3) << 4));
        to = (BDPOINT)((str[2] - 'a' + 3) + (('9' - str[3] + 3) << 4));
        position[to] = position[from];
        position[from] = NONE;
        if (position[to] == B_KING)
        {
            kingpos[BLACK] = to;
        }
        if (position[to] == R_KING)
        {
            kingpos[RED] = to;
        }
        zobrist ^= Hashnum::GetHash(position[to], from);
        zobrist ^= Hashnum::GetHash(position[to], to);
    }
}
