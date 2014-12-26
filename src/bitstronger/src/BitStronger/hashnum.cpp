/*! \file hashnum.cpp
@brief 哈希数操作类（单件模块）
*/

#include <string.h>
#include "hashnum.h"

void Hashnum::InitHash()
{
    if (loadedhash)
    {
        return;    
    }
    
    BDPOINT i, j;
    CHESSMAN c;
    
    doc = fopen(hashpath, "rb");
    for (i = 0x30; i <= 0xc0; i += 0x10)
    {
        for (j = 0x03; j <= 0x0b; j += 0x01)
        {
            for (c = 0; c <= 6; c += 1)
            {
                fread(&hash[c][i + j], sizeof(HASHNUM), 1, doc);
            }
            for (c = 13; c >= 7; c -= 1)
            {
                fread(&hash[c][i + j], sizeof(HASHNUM), 1, doc);
            }   
        } 
    }
    
    fclose(doc);
        
    TTList[RED] = new TTItem[1024 * 1024];
    TTList[BLACK] = new TTItem[1024 * 1024];
    int k;
    for (k = 0; k < 1024 * 1024; k++)
    {
        TTList[RED][k].depth = TTList[BLACK][k].depth = -1;        
    }
    
    loadedhash = TRUE;
}

void Hashnum::ClearHash()
{
    delete[] TTList[RED]; 
    delete[] TTList[BLACK]; 
}

HASHNUM Hashnum::GetHash(CHESSMAN chess, BDPOINT point)
{
    if (chess == NONE)
    {
        return (HASHNUM)0;  
    }
    else
    {  
        return IsRed(chess) ? hash[chess - (CHESSMAN)1][point] : hash[(CHESSMAN)7 + (CHESSMAN)~chess][point];
    }
}

int Hashnum::SearchTT(HASHNUM zobrist, int max_depth, int depth, PLAYER who, int alpha, int beta)
{
    int num = (int)(zobrist & (HASHNUM)0xFFFFF);
    if (TTList[who][num].zobrist == zobrist && TTList[who][num].max_depth == max_depth && TTList[who][num].depth == depth)
    {
        switch (TTList[who][num].entry)   
        {
        case EXACT:
            return TTList[who][num].value;
            break;
        case L_BOUND:
            if (TTList[who][num].value >= beta)
            {
                return TTList[who][num].value;
            }
            break;
        case U_BOUND:
            if (TTList[who][num].value <= alpha)
            {
                return TTList[who][num].value;
            }
            break;  
        }
    }
    return NOT_IN_TT;
}

void Hashnum::SaveTT(HASHNUM zobrist, int max_depth, int depth, PLAYER who, EntryType entry, int value)
{
    int num = (int)(zobrist & (HASHNUM)0xFFFFF);
    TTList[who][num].zobrist = zobrist;
    TTList[who][num].max_depth = max_depth;
    TTList[who][num].depth = depth;
    TTList[who][num].entry = entry;
    TTList[who][num].value = value;
}

