#pragma once

#include "util.h"

//Ëæ»úÊý×é
extern unsigned long    g_ZobristKeyPlayer;
extern unsigned long    g_ZobristKeyTable[10][256];
extern uint64 g_ZobristLockPlayer;
extern uint64 g_ZobristLockTable[10][256];
//extern const uint64 Random64[781];
extern void zobrist_gen(void);