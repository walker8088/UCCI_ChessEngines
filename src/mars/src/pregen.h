#pragma once
#include "random.h"
#include "util.h"

extern const char  c_InBoard[256];     // 棋盘区域表
extern const char  c_InCity[256];      // 九宫序列表
extern const char  c_HorseLegTab[512]; // 马腿表

struct slide_move_t {
  uint8 non_cap[2];      // 不吃子能走到的最大一格/最小一格
  uint8 rook_cap[2];     // 车吃子能走到的最大一格/最小一格
  uint8 cannon_cap[2];   // 炮吃子能走到的最大一格/最小一格
  uint8 super_cap[2];    // 超级炮(隔两子吃子)能走到的最大一格/最小一格
};

struct slide_mask_t {
  uint16 non_cap, rook_cap, cannon_cap, super_cap;
};

extern uint16 g_BitRankMask[256]; // 每个格子的位行的屏蔽位
extern uint16 g_BitFileMask[256]; // 每个格子的位列的屏蔽位


extern slide_move_t g_RankMoveTab[9][512];   // 36,864 字节
extern slide_move_t g_FileMoveTab[10][1024]; // 81,920 字节
extern slide_mask_t g_RankMaskTab[9][512];   // 36,864 字节
extern slide_mask_t g_FileMaskTab[10][1024]; // 81,920 字节
                                        // 共计:  237,568 字节

extern uint8 g_KingMoves[256][8];
extern uint8 g_AdvisorMoves[256][8];
extern uint8 g_BishopMoves[256][8];
extern uint8 g_ElephantEyes[256][4];
extern uint8 g_KnightMoves[256][12];
extern uint8 g_HorseLegs[256][8];
extern uint8 g_PawnMoves[2][256][4];

void pre_move_gen(void);
