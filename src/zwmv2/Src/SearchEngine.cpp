
/* ============================================================================================ *\
 *	Author		:	ahei																		*
 *	Email		:	ahei0802@126.com															*
 *	Msn			:	ahei080210114@hotmail.com													*
 *	QQ			:	8261525																		*
 *	Version		:	0.2 																		*
 *	Created		:	2005/03/01 9:10:13															*
 *	Modified	:	2005/01/01 17:15:46															*
 *	FileName	: 	SearchEngine.cpp															*
 *	Description	:	This file is using for declaring the class CSearchEngine,and this class is	*
 *					using for generating moves,evaluating and searching.						*
 * 	Warning     : 	Copyright (c) ahei 2004 All rights reserved.  								*
\* ============================================================================================ */

#ifdef _WIN32
//#include <mmsystem.h>
#endif
#include <iostream>
#include <new>
#include <algorithm>
#include <iomanip>
#include <vector>
#include <time.h>
#include <crtdbg.h>

#include "Idle.h"
#include "DispMsg.h"
#include "SearchEngine.h"

using namespace std;

// 棋子基本价值
FSINT g_nBasicValues[8] = 
{
	BV_NOCHESS,				// 无
	BV_KING,				// 将
	BV_ADVISOR,				// 士
	BV_BISHOP,				// 象
	BV_ROOK,				// 车
	BV_KNIGHT_OPEN_GAME,	// 马
	BV_CANNON_OPEN_GAME,	// 炮
	BV_PAWN_OPEN_GAME,		// 卒
};

// 用于CHESSMOVE的排序
FSbool UDgreater(CHESSMOVE cm1, CHESSMOVE cm2);

/*******************************************************************************************************
函数名 : CSearchEngine
描  述 : 
参  数 : 无
返回值 : 无
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年9月28日 22:24:53
*******************************************************************************************************/
CSearchEngine::CSearchEngine()
{
	// 此处的变量不能放到InitVar函数里面,他们要提前初始化
	m_bAllocateMem = FALSE;
	m_EgnProtocol = EgnPUCCI;
	m_nHashTabMask = 0x7ffff;	// hash表屏蔽位

	FSINT nLen;

	GetCurrentDirectory(MAXBUFFERLEN, m_szDir);
	m_szDir[nLen = strlen(m_szDir)] = '\\';
	_ASSERT(nLen <= MAXBUFFERLEN - 1);
	m_szDir[nLen + 1] = '\0';
}

/*******************************************************************************************************
函数名 : ~CSearchEngine
描  述 : 
参  数 : 无
返回值 : 无
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年9月28日 22:25:03
*******************************************************************************************************/
CSearchEngine::~CSearchEngine()
{
	DeleteCriticalSection(&g_CriticalSection);
}

/*******************************************************************************************************
函数名 : AllocateMem
描  述 : 分配内存
参  数 : 无
返回值 : 无
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年8月7日 12:45:07
*******************************************************************************************************/
FSVOID CSearchEngine::AllocateMem()
{
	if (CanMemMeet() == FALSE)
	{
		DISPMSG1_ENDL(cout,	"内存不足,程序退出!");
		exit(EXIT_SUCCESS);
	}

	InitPointer();

	// 走法预产生数组
	// 车
	TRYNEW(m_pcmRookFilePreMoves = new PRECHESSMOVE[91][ROOKFILECHECKSUMMAXCNT][4];);
	TRYNEW(m_pcmRookRankPreMoves = new PRECHESSMOVE[91][ROOKRANKCHECKSUMMAXCNT][4];);

	TRYNEW(m_pcmRookFileCapPreMoves = new PRECHESSMOVE[91][ROOKFILECHECKSUMMAXCNT][4];);
	TRYNEW(m_pcmRookRankCapPreMoves = new PRECHESSMOVE[91][ROOKRANKCHECKSUMMAXCNT][4];);

	TRYNEW(m_pcmRookFileNoCapPreMoves = new PRECHESSMOVE[91][ROOKFILECHECKSUMMAXCNT];);
	TRYNEW(m_pcmRookRankNoCapPreMoves = new PRECHESSMOVE[91][ROOKRANKCHECKSUMMAXCNT];);

	TRYNEW(m_nRookFoeFileRankCheckNum = new FEATCHAR[10][ROOKFRCHECKSUMMAXCNT][1024];);

	// 马
	// 不吃子走法
	TRYNEW(m_pcmKnightNoCapPreMoves = new PRECHESSMOVE[91][256][16];);
	// 吃子走法
	TRYNEW(m_pcmKnightCapPreMoves = new PRECHESSMOVE[91][256][16];);
	
	// 炮
	TRYNEW(m_pcmCannonFilePreMoves = new PRECHESSMOVE[91][CANNONFILECHECKSUMMAXCNT][4];);
	TRYNEW(m_pcmCannonRankPreMoves = new PRECHESSMOVE[91][CANNONRANKCHECKSUMMAXCNT][4];);

	// 不吃子走法[2005/10/23 15:23:58]
	TRYNEW(m_pcmCannonFileNoCapPreMoves = new PRECHESSMOVE[91][CANNONFILECHECKSUMMAXCNT];);
	TRYNEW(m_pcmCannonRankNoCapPreMoves = new PRECHESSMOVE[91][CANNONRANKCHECKSUMMAXCNT];);

	// 吃子走法[2005/10/23 15:24:09]
	TRYNEW(m_pcmCannonFileCapPreMoves = new PRECHESSMOVE[91][CANNONFILECHECKSUMMAXCNT][4];);
	TRYNEW(m_pcmCannonRankCapPreMoves = new PRECHESSMOVE[91][CANNONRANKCHECKSUMMAXCNT][4];);	

	TRYNEW(m_nCannonFoeFileRankCheckNum = new FEATCHAR[10][CANNONFRCHECKSUMMAXCNT][1024];);

	// 将
	TRYNEW(m_pcmKingPreMoves = new PRECHESSMOVE[18][16][16];);

	// 士
	TRYNEW(m_pcmAdvisorPreMoves = new PRECHESSMOVE[11][16][16];);

	// 象
	TRYNEW(m_pcmBishopPreMoves = new PRECHESSMOVE[15][16][16][16];);
	TRYNEW(m_pcmBishopNoCapPreMoves = new PRECHESSMOVE[15][16][16];);
	TRYNEW(m_pcmBishopCapPreMoves = new PRECHESSMOVE[15][16][16];);

	// 卒
	TRYNEW(m_PawnPreMoves = new PRECHESSMOVE[2][56][8][8];);
	TRYNEW(m_PawnNoCapPreMoves = new PRECHESSMOVE[2][56][8];);
	TRYNEW(m_PawnCapPreMoves = new PRECHESSMOVE[2][56][8];);

	InitPreMoveListPointer();

	// 双将照面判断数组
	TRYNEW(m_bIsTwoKingMeet= new FEATBOOL[10][10][1024];);

	// 将军预判断数组
	// 车
	TRYNEW(m_bIsRookFilePreCheck = new FEATBOOL[2][91][9][512];);
	TRYNEW(m_bIsRookRankPreCheck = new FEATBOOL[2][91][9][1024];);

	// 马
	TRYNEW(m_bIsKnightPreCheck = new FEATBOOL[2][91][9][16];);

	// 炮
	TRYNEW(m_bIsCannonFilePreCheck = new FEATBOOL[2][91][9][512];);
	TRYNEW(m_bIsCannonRankPreCheck = new FEATBOOL[2][91][9][1024];);

	// 兵
	TRYNEW(m_bIsPawnPreCheck = new FEATBOOL[2][56][9];);

	// 走法有效性预判断数组
	// 车
//	TRYNEW(m_bIsRookFilePreValidMove = new FEATBOOL[90][90][512];);
//	TRYNEW(m_bIsRookRankPreValidMove = new FEATBOOL[90][90][1024];);
//
//	// 炮
//	TRYNEW(m_bIsCannonFilePreValidMove = new FEATBOOL[90][90][512][2];);
//	TRYNEW(m_bIsCannonRankPreValidMove = new FEATBOOL[90][90][1024][2];);

	// 车
	TRYNEW(m_bIsRookPreValidMove = new FEATBOOL[10][10][1024];);

	// 炮
	TRYNEW(m_bIsCannonPreValidMove = new FEATBOOL[10][10][1024][2];);

	// hash表
	m_nRepHashTabMask = 0x3ff;	// 重复局面检测的屏蔽位

	TRYNEW(m_pHashTab[BLACK] = new HASHITEM[m_nHashTabMask + 1];);	// 6M
	TRYNEW(m_pHashTab[RED] = new HASHITEM[m_nHashTabMask + 1];);	// 6M

//	TRYNEW(m_pHashTabByDepth[BLACK] = new HASHITEM[m_nHashTabMask + 1];);	//6M
//	TRYNEW(m_pHashTabByDepth[RED] = new HASHITEM[m_nHashTabMask + 1];);	// 6M

	TRYNEW(m_pQSHashTab[BLACK] = new QSHASHITEM[m_nHashTabMask + 1];);	// 6M
	TRYNEW(m_pQSHashTab[RED] = new QSHASHITEM[m_nHashTabMask + 1];);	// 6M

	m_nRepHashTab = new FSINT[m_nRepHashTabMask + 1];
}

/*******************************************************************************************************
函数名 : FreeMem
描  述 : 释放内存
参  数 : 无
返回值 : 无
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年8月7日 12:45:30
*******************************************************************************************************/
FSVOID CSearchEngine::FreeMem()
{
	// =============================走法产生=============================
	// 走法预产生数组中的走法链表
	FSINT w, p, i, j, k;
	FSINT nFoeFileCheckNum;
	FSINT nFoeRankCheckNum;

	FSINT tAll = clock();
	
	cout << endl << "正在释放车的预走法内存..." << endl;
	// 车
	for (p = 0; p < 90; p++)
	{
		i = m_CGN2Pos[p].x;
		j = m_CGN2Pos[p].y;
		
		// 行走法
		for (k = 0; k < ROOKFILECHECKSUMMAXCNT; k++)
		{
			// 必须!!!
			if (m_nRookFileRankCheckSum[i][k] == INVALIDCHECKSUM)
			{
				break;
			}
			
			// 释放不吃子走法[2005/10/23 14:49:01]
			VALIDDELETE(m_pcmRookFileNoCapPreMoves[p][k].pMoveList, &m_InvalidMovePointer);

			for (nFoeFileCheckNum = 0; nFoeFileCheckNum < 4; nFoeFileCheckNum++)
			{
				//必须!!!
				if (m_nRookFoeFileRankCheckSum[i][k][nFoeFileCheckNum] == INVALIDCHECKSUM)
				{
					break;
				}
				
				VALIDDELETE(m_pcmRookFilePreMoves[p][k][nFoeFileCheckNum].pMoveList, &m_InvalidMovePointer);
				VALIDDELETE(m_pcmRookFileCapPreMoves[p][k][nFoeFileCheckNum].pMoveList, &m_InvalidMovePointer);
			}
		}
		
		//列走法
		for (k = 0; k < ROOKRANKCHECKSUMMAXCNT; k++)
		{
			//必须!!!
			if (m_nRookFileRankCheckSum[j][k] == INVALIDCHECKSUM)
			{
				break;
			}
			
			VALIDDELETE(m_pcmRookRankNoCapPreMoves[p][k].pMoveList, &m_InvalidMovePointer);
			
			for (nFoeRankCheckNum = 0; nFoeRankCheckNum < 4; nFoeRankCheckNum++)
			{
				//必须!!!
				if (m_nRookFoeFileRankCheckSum[j][k][nFoeRankCheckNum] == INVALIDCHECKSUM)
				{
					break;
				}
				
				VALIDDELETE(m_pcmRookRankPreMoves[p][k][nFoeRankCheckNum].pMoveList, &m_InvalidMovePointer);
				VALIDDELETE(m_pcmRookRankCapPreMoves[p][k][nFoeRankCheckNum].pMoveList, &m_InvalidMovePointer);
			}
		}
	}
	
	cout << "正在释放马的预走法内存..." << endl;
	FSINT t = clock();
	//马
	//不吃子走法
	for (p = 0; p < 90; p++)
	{
		for (i = 0; i < 256; i++)
		{
			if (m_bIsKnightInvalidArroundCheckSum[p][i] == FALSE)
			{
				continue;
			}

			for (j = 0; j < 16; j++)
			{
				if (m_bIsKnightInvalidLegCheckSum[p][i][j] == FALSE)
				{
					continue;
				}

				VALIDDELETE(m_pcmKnightNoCapPreMoves[p][i][j].pMoveList, &m_InvalidMovePointer);
			}
		}
	}
	//吃子走法
	for (p = 0; p < 90; p++)
	{
		for (i = 0; i < 256; i++)
		{
			if (m_bIsKnightInvalidArroundCheckSum[p][i] == FALSE)
			{
				continue;
			}
			
			for (j = 0; j < 16; j++)
			{
				if (m_bIsKnightInvalidLegCheckSum[p][i][j] == FALSE)
				{
					continue;
				}
				
				VALIDDELETE(m_pcmKnightCapPreMoves[p][i][j].pMoveList, &m_InvalidMovePointer);
			}
		}
	}
	cout << "释放马预走法内存时间:" << clock() - t << "ms" << endl;

	cout << "正在释放炮的预走法内存..." << endl;
	//炮
	for (p = 0; p < 90; p++)
	{
		i = m_CGN2Pos[p].x;
		j = m_CGN2Pos[p].y;
		
		//行走法
		for (k = 0; k < CANNONFILECHECKSUMMAXCNT; k++)
		{
			//必须!!!
			if (m_nCannonFileRankCheckSum[i][k] == INVALIDCHECKSUM)
			{
				break;
			}
			
			VALIDDELETE(m_pcmCannonFileNoCapPreMoves[p][k].pMoveList, &m_InvalidMovePointer);

			for (nFoeFileCheckNum = 0; nFoeFileCheckNum < 4; nFoeFileCheckNum++)
			{
				//必须!!!
				if (m_nCannonFoeFileRankCheckSum[i][k][nFoeFileCheckNum] == INVALIDCHECKSUM)
				{
					break ;
				}
				
				VALIDDELETE(m_pcmCannonFilePreMoves[p][k][nFoeFileCheckNum].pMoveList, &m_InvalidMovePointer);
				VALIDDELETE(m_pcmCannonFileCapPreMoves[p][k][nFoeFileCheckNum].pMoveList, &m_InvalidMovePointer);
			}
		}
		
		//列走法
		for (k = 0; k < CANNONRANKCHECKSUMMAXCNT; k++)
		{
			//必须!!!
			if (m_nCannonFileRankCheckSum[j][k] == INVALIDCHECKSUM)
			{
				break;
			}
			
			VALIDDELETE(m_pcmCannonRankNoCapPreMoves[p][k].pMoveList, &m_InvalidMovePointer);

			for (nFoeRankCheckNum = 0; nFoeRankCheckNum < 4; nFoeRankCheckNum++)
			{
				//必须!!!
				if (m_nCannonFoeFileRankCheckSum[j][k][nFoeRankCheckNum] == INVALIDCHECKSUM)
				{
					break;
				}
				
				VALIDDELETE(m_pcmCannonRankPreMoves[p][k][nFoeRankCheckNum].pMoveList, &m_InvalidMovePointer);
				VALIDDELETE(m_pcmCannonRankCapPreMoves[p][k][nFoeRankCheckNum].pMoveList, &m_InvalidMovePointer);
			}
		}
	}

	cout << "正在释放将的预走法内存..." << endl;
	//将
	for (i = 0; i < 18; i++)
	{
		for (j = 0;j < 16; j++)
		{
			for (k = 0; k < 16; k++)
			{
				VALIDDELETE(m_pcmKingPreMoves[i][j][k].pMoveList, &m_InvalidMovePointer);
			}
		}
	}
	//吃子走法
	for (i = 0; i < 18; i++)
	{
		for (k = 0; k < 16; k++)
		{
			VALIDDELETE(m_pcmKingNoCapPreMoves[i][k].pMoveList, &m_InvalidMovePointer);
			VALIDDELETE(m_pcmKingCapPreMoves[i][k].pMoveList, &m_InvalidMovePointer);
		}
	}

	cout << "正在释放士的预走法内存..." << endl;
	//士
	for (i = 0; i < 11; i++)
	{
		for (j = 0; j < 16; j++)
		{
			for (k = 0; k < 16; k++)
			{
				if (m_pcmAdvisorPreMoves[i][j][k].pMoveList != &m_InvalidMovePointer)
				{
					TRYDELETE(SAFEDELETE(m_pcmAdvisorPreMoves[i][j][k].pMoveList));
				}
			}
		}
	}
	for (i = 0; i < 11; i++)
	{
		for (k = 0; k < 16; k++)
		{
			VALIDDELETE(m_pcmAdvisorNoCapPreMoves[i][k].pMoveList, &m_InvalidMovePointer);
			VALIDDELETE(m_pcmAdvisorCapPreMoves[i][k].pMoveList, &m_InvalidMovePointer);
		}
	}

	cout << "正在释放象的预走法内存..." << endl;
	//象
	for (p = 0; p < 15; p++)
	{
		for (i = 0; i < 16; i++)
		{
			for (j = 0; j < 16; j++)
			{
				for (k = 0; k < 16; k++)
				{
					VALIDDELETE(m_pcmBishopPreMoves[p][i][j][k].pMoveList, &m_InvalidMovePointer);
				}
			}
		}
	}
	for (p = 0; p < 15; p++)
	{
		for (j = 0; j < 16; j++)
		{
			for (k = 0; k < 16; k++)
			{
				VALIDDELETE(m_pcmBishopNoCapPreMoves[p][j][k].pMoveList, &m_InvalidMovePointer);
				VALIDDELETE(m_pcmBishopCapPreMoves[p][j][k].pMoveList, &m_InvalidMovePointer);
			}
		}
	}

	cout << "正在释放兵的预走法内存..." << endl;
	//兵
	for (w = 0; w < 2; w++)
	{
		for (p = 0; p < 56; p++)
		{
			for (i = 0; i < 8; i++)
			{
				for (j = 0; j < 8; j++)
				{
					VALIDDELETE(m_PawnPreMoves[w][p][i][j].pMoveList, &m_InvalidMovePointer);
				}
			}
		}
	}
	for (w = 0; w < 2; w++)
	{
		for (p = 0; p < 56; p++)
		{
			for (j = 0; j < 8; j++)
			{
				VALIDDELETE(m_PawnNoCapPreMoves[w][p][j].pMoveList, &m_InvalidMovePointer);
				VALIDDELETE(m_PawnCapPreMoves[w][p][j].pMoveList, &m_InvalidMovePointer);
			}
		}
	}

	cout << "正在释放其他内存..." << endl;

	//走法预产生数组
	//车
	TRYDELETE(SAFEDELETE(m_pcmRookFilePreMoves));
	TRYDELETE(SAFEDELETE(m_pcmRookRankPreMoves));

	TRYDELETE(SAFEDELETE(m_pcmRookFileCapPreMoves));
	TRYDELETE(SAFEDELETE(m_pcmRookRankCapPreMoves));

	TRYDELETE(SAFEDELETE(m_pcmRookFileNoCapPreMoves));
	TRYDELETE(SAFEDELETE(m_pcmRookRankNoCapPreMoves));

	TRYDELETE(SAFEDELETE(m_nRookFoeFileRankCheckNum));

	//马
	TRYDELETE(SAFEDELETE(m_pcmKnightNoCapPreMoves));
	TRYDELETE(SAFEDELETE(m_pcmKnightCapPreMoves));

	//炮
	TRYDELETE(SAFEDELETE(m_pcmCannonFilePreMoves));
	TRYDELETE(SAFEDELETE(m_pcmCannonRankPreMoves));

	// 不吃子走法[2005/10/23 15:44:30]
	TRYDELETE(SAFEDELETE(m_pcmCannonFileNoCapPreMoves));
	TRYDELETE(SAFEDELETE(m_pcmCannonRankNoCapPreMoves));

	TRYDELETE(SAFEDELETE(m_pcmCannonFileCapPreMoves));
	TRYDELETE(SAFEDELETE(m_pcmCannonRankCapPreMoves));

	TRYDELETE(SAFEDELETE(m_nCannonFoeFileRankCheckNum));

	//将
	TRYDELETE(SAFEDELETE(m_pcmKingPreMoves));

	//士
	TRYDELETE(SAFEDELETE(m_pcmAdvisorPreMoves));
	
	//象
	TRYDELETE(SAFEDELETE(m_pcmBishopPreMoves));
	TRYDELETE(SAFEDELETE(m_pcmBishopNoCapPreMoves));
	TRYDELETE(SAFEDELETE(m_pcmBishopCapPreMoves));
	
	//兵
	TRYDELETE(SAFEDELETE(m_PawnPreMoves));
	TRYDELETE(SAFEDELETE(m_PawnNoCapPreMoves));
	TRYDELETE(SAFEDELETE(m_PawnCapPreMoves));

	//双将照面判断数组
	TRYDELETE(SAFEDELETE(m_bIsTwoKingMeet));
	
	//=============================局面评估=============================
	
	//=============================搜索引擎=============================
	//将军判断数组
	//车
	TRYDELETE(SAFEDELETE(m_bIsRookFilePreCheck));
	TRYDELETE(SAFEDELETE(m_bIsRookRankPreCheck));
	
	//马
	TRYDELETE(SAFEDELETE(m_bIsKnightPreCheck));
	
	//炮
	TRYDELETE(SAFEDELETE(m_bIsCannonFilePreCheck));
	TRYDELETE(SAFEDELETE(m_bIsCannonRankPreCheck));
	
	//兵
	TRYDELETE(SAFEDELETE(m_bIsPawnPreCheck));

	//hash表
	TRYDELETE(SAFEDELETE(m_pHashTab[BLACK]));
	TRYDELETE(SAFEDELETE(m_pHashTab[RED]));

	TRYDELETE(SAFEDELETE(m_pHashTabByDepth[BLACK]));
	TRYDELETE(SAFEDELETE(m_pHashTabByDepth[RED]));

	TRYDELETE(SAFEDELETE(m_pQSHashTab[BLACK]));
	TRYDELETE(SAFEDELETE(m_pQSHashTab[RED]));

	TRYDELETE(SAFEDELETE(m_nRepHashTab));

	//走法有效性预判断数组
	//车
//	TRYDELETE(SAFEDELETE(m_bIsRookFilePreValidMove));
//	TRYDELETE(SAFEDELETE(m_bIsRookRankPreValidMove));
//
//	//炮
//	TRYDELETE(SAFEDELETE(m_bIsCannonFilePreValidMove));
//	TRYDELETE(SAFEDELETE(m_bIsCannonRankPreValidMove));

	//车
	TRYDELETE(SAFEDELETE(m_bIsRookPreValidMove));

	//炮
	TRYDELETE(SAFEDELETE(m_bIsCannonPreValidMove));

	//=============================其他方面=============================

	DISPMSG3_ENDL(cout,		"释放时间:", clock() - t, "ms");

	DISPMSG2_ENDL(cout,		"成功退出引擎", g_szEngineName);
}

/*******************************************************************************************************
函数名 : AllocateMemErrProc
描  述 : 
参  数 : 无
返回值 : 无
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年8月7日 12:48:16
*******************************************************************************************************/
FSVOID CSearchEngine::AllocateMemErrProc()
{
	cout << "内存不足!" << endl;

	FreeMem();
	exit(EXIT_FAILURE);
}

/*******************************************************************************************************
函数名 : FreeMemErrProc
描  述 : 
参  数 : 无
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年10月3日 22:07:25
*******************************************************************************************************/
FSVOID CSearchEngine::FreeMemErrProc()
{
	cout << "释放内存时产生错误!" << endl;
}

/*******************************************************************************************************
函数名 : InitVar
描  述 : 初始化变量
参  数 : 无
返回值 : 无
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年8月7日 12:37:15
*******************************************************************************************************/
FSVOID CSearchEngine::InitVar()
{
	//=============================走法产生=============================
	FSINT p;
	FSINT i, j;
	FSINT m, n;
	FSINT k;
	FSINT c;
	FSINT nFoeBit;

	//m_nMask
	c = 1;
	for (i = 0; i < 10; i++)
	{
		m_nMask[i] = c;
		c <<= 1;

		m_nReverseMask[i] = (~m_nMask[i]);
	}

	//m_CGN2Pos
	for (p = 0; p < 91; p++)
	{
		m_CGN2Pos[p].x = (i = p / 10);
		m_CGN2Pos[p].y = (j = p % 10);

		m_nPos2CGN[i][j] = p;
	}

	//注意:bug易发区
	//m_nFileRankAroundCheckSum
	i = 0;
	for (k = 0; k < 1024; k++)
	{
		m_nFileRankAroundCheckSum[i][k] = 0;

		if (m_nMask[i + 1] & k)
		{
			m_nFileRankAroundCheckSum[i][k] += 2;
		}
	}
	for (i = 1; i < 9; i++)
	{
		for (k = 0; k < 1024; k++)
		{
			m_nFileRankAroundCheckSum[i][k] = 0;

			if (m_nMask[i-1] & k)
			{
				m_nFileRankAroundCheckSum[i][k]++;
			}
			if (m_nMask[i+1] & k)
			{
				m_nFileRankAroundCheckSum[i][k] += 2;
			}
		}
	}
	i = 9;
	for (k = 0; k < 1024; k++)
	{
		m_nFileRankAroundCheckSum[i][k] = 0;

		if ( m_nMask[i - 1] & k )
		{
			m_nFileRankAroundCheckSum[i][k] ++ ;
		}
	}

	//TODO:bug易发区
	//m_nFileRankAroundCheckSum2
	for (i = 0; i < 2; i++)
	{
		for (k = 0; k < 1024; k++)
		{
			m_nFileRankAroundCheckSum2[i][k] = 0;

			if (m_nMask[i + 2] & k)
			{
				m_nFileRankAroundCheckSum2[i][k] += 2;
			}
		}
	}
	for ( i = 2 ; i < 8 ; i ++ )
	{
		for (k = 0; k < 1024; k++)
		{
			m_nFileRankAroundCheckSum2[i][k] = 0;

			if (m_nMask[i-2] & k)
			{
				m_nFileRankAroundCheckSum2[i][k]++;
			}
			if (m_nMask[i+2] & k)
			{
				m_nFileRankAroundCheckSum2[i][k] += 2;
			}
		}
	}
	for (i = 8; i < 10; i++)
	{
		for (k = 0; k < 1024; k++)
		{
			m_nFileRankAroundCheckSum2[i][k] = 0;

			if (m_nMask[i - 2] & k)
			{
				m_nFileRankAroundCheckSum2[i][k]++;
			}
		}
	}

	//m_nAroundCheckSum
	for (i = 0;i<4;i++)
	{
		for (j = 0;j<4;j++)
		{
			m_nAroundCheckSum[i][j] = (j << 2) | i;
		}
	}

	//求车的位行和位列的校验序号
	FSINT nNum;
	FSINT nFlag;
	FSINT nRookCheckSum;

	//注意:必须!!!
	for (i = 0; i < 10; i++)
	{
		for ( nNum = 0 ; nNum < ROOKFRCHECKSUMMAXCNT ; nNum ++ )
		{
			m_nRookFileRankCheckSum[i][nNum] = INVALIDCHECKSUM;
		}
	}

	//注意:bug易发区
	for ( i = 0 ; i < 10 ; i ++ )
	{
        nNum = 0;

		for ( k = 0 ; k < 1024 ; k ++)
		{
			nRookCheckSum = 0;

			for (m = i-1;m >= 0; m--)
			{
				if (m_nMask[m] & k)
				{
					nRookCheckSum |= m_nMask[m];
					break;
				}
			}

			for (n = i + 1; n < 10; n++)
			{
				if (m_nMask[n] & k)
				{
					nRookCheckSum |= m_nMask[n];
					break;
				}
			}

			nFlag = 0;
			for ( j = 0;j < nNum;j ++ )
			{
				if (m_nRookFileRankCheckSum[i][j] == nRookCheckSum)
				{
					nFlag = 1;
					break;
				}
			}

			if (nFlag)
			{
				m_nRookFileRankCheckNum[i][k] = j;
			}
			else
			{
				m_nRookFileRankCheckNum[i][k] = nNum ;
				m_nRookFileRankCheckSum[i][nNum] = nRookCheckSum ;

				nNum++;
			}
		}
	}

	//m_nRookFoeFileRankCheckNum
	FSINT nRookFoeFileRankCheckSum;

	//注意:必须!!!
	for (i = 0; i < 10; i++)
	{
		for (k = 0; k < ROOKFRCHECKSUMMAXCNT; k++)
		{
			//注意:必须!!!
			if (m_nRookFileRankCheckSum[i][k] == INVALIDCHECKSUM)
			{
				break ;
			}

			for (nNum = 0; nNum < 4; nNum++)
			{
				m_nRookFoeFileRankCheckSum[i][k][nNum] = INVALIDCHECKSUM;
			}
		}
	}

	//注意:bug易发区
	for ( i = 0 ; i < 10 ; i ++ )
	{
		for ( k = 0 ; k < ROOKFRCHECKSUMMAXCNT ; k ++ )
		{
			if (m_nRookFileRankCheckSum[i][k] == INVALIDCHECKSUM)
			{
				break ;
			}

            nNum = 0;
            
			for ( nFoeBit = 0 ; nFoeBit < 1024 ; nFoeBit ++ )
			{
				nRookFoeFileRankCheckSum = nFoeBit & m_nRookFileRankCheckSum[i][k];
				
                nFlag = 0;
				for (j = 0;j<nNum;j++)
				{
					if (m_nRookFoeFileRankCheckSum[i][k][j] ==nRookFoeFileRankCheckSum)
					{
						nFlag = 1;
						break;
					}
				}
				if (nFlag)
				{
					m_nRookFoeFileRankCheckNum[i][k][nFoeBit] = j ;
				}
				else
				{
					m_nRookFoeFileRankCheckNum[i][k][nFoeBit] = nNum;
					m_nRookFoeFileRankCheckSum[i][k][nNum] = nRookFoeFileRankCheckSum;

					nNum++;
				}
			}
		}
	}

	//m_nKnightAroundCheckSum
	FSINT nBit1, nBit2;

	for (nBit1 = 0; nBit1 < 16; nBit1++)
	{
		for (nBit2 = 0; nBit2 < 16; nBit2++)
		{
			m_nKnightAroundCheckSum[nBit1][nBit2] = nBit1 | (nBit2 << 4);
		}
	}

	//m_bIsKnightInvalidArroundCheckSum
	FSINT nAroundCheckSum;

	for (p = 0; p < 90; p++)
	{
		i = m_CGN2Pos[p].x;
		j = m_CGN2Pos[p].y;
		
		for (nAroundCheckSum = 0; nAroundCheckSum < 256; nAroundCheckSum++)
		{
			//左
			if (i - 2 < 0)
			{
				//下
				if (((nAroundCheckSum & m_nMask[2]) != 0) && (j - 1 < 0))
				{
					m_bIsKnightInvalidArroundCheckSum[p][nAroundCheckSum] = FALSE;
					continue;
				}
				//上
				if (((nAroundCheckSum & m_nMask[4]) != 0) && (j + 1 >= 10))
				{
					m_bIsKnightInvalidArroundCheckSum[p][nAroundCheckSum] = FALSE;
					continue;
				}
			}
			
			//右
			if (i + 2 >= 9)
			{
				//下
				if (((nAroundCheckSum & m_nMask[3]) != 0) && (j - 1 < 0))
				{
					m_bIsKnightInvalidArroundCheckSum[p][nAroundCheckSum] = FALSE;
					continue;
				}
				//上
				if (((nAroundCheckSum & m_nMask[5]) != 0) && (j + 1 >= 10))
				{
					m_bIsKnightInvalidArroundCheckSum[p][nAroundCheckSum] = FALSE;
					continue;
				}
			}
			
			//下
			if (j - 2 < 0)
			{
				//左
				if (((nAroundCheckSum & m_nMask[0]) != 0) && (i - 1 < 0))
				{
					m_bIsKnightInvalidArroundCheckSum[p][nAroundCheckSum] = FALSE;
					continue;
				}
				//右
				if (((nAroundCheckSum & m_nMask[1]) != 0) && (i + 1 >= 9))
				{
					m_bIsKnightInvalidArroundCheckSum[p][nAroundCheckSum] = FALSE;
					continue;
				}
			}
			
			//上
			if (j + 2 >= 10)
			{
				//左
				if (((nAroundCheckSum & m_nMask[6]) != 0) && (i - 1 < 0))
				{
					m_bIsKnightInvalidArroundCheckSum[p][nAroundCheckSum] = FALSE;
					continue;
				}
				//右
				if (((nAroundCheckSum & m_nMask[7]) != 0) && (i + 1 >= 9))
				{
					m_bIsKnightInvalidArroundCheckSum[p][nAroundCheckSum] = FALSE;
					continue;
				}
			}

			m_bIsKnightInvalidArroundCheckSum[p][nAroundCheckSum] = TRUE;
		}
	}

	//m_bIsKnightInvalidLegCheckSum
	FSINT nKnightLeg;

	for (p = 0; p < 90; p++)
	{
		i = m_CGN2Pos[p].x;
		j = m_CGN2Pos[p].y;
		
		for (nAroundCheckSum = 0; nAroundCheckSum < 256; nAroundCheckSum++)
		{
			if (m_bIsKnightInvalidArroundCheckSum[p][nAroundCheckSum] == FALSE)
			{
				continue;
			}

			for (nKnightLeg = 0; nKnightLeg < 16; nKnightLeg++)
			{
				//左
				if (i - 1 < 0 && (nKnightLeg & m_nMask[0]))
				{
					m_bIsKnightInvalidLegCheckSum[p][nAroundCheckSum][nKnightLeg] = FALSE;
					continue;
				}

				//右
				if (i + 1 >= 9 && (nKnightLeg & m_nMask[1]))
				{
					m_bIsKnightInvalidLegCheckSum[p][nAroundCheckSum][nKnightLeg] = FALSE;
					continue;
				}

				//下
				if (j - 1 < 0 && (nKnightLeg & m_nMask[2]))
				{
					m_bIsKnightInvalidLegCheckSum[p][nAroundCheckSum][nKnightLeg] = FALSE;
					continue;
				}

				//上
				if (j + 1 >= 10 && (nKnightLeg & m_nMask[3]))
				{
					m_bIsKnightInvalidLegCheckSum[p][nAroundCheckSum][nKnightLeg] = FALSE;
					continue;
				}

				m_bIsKnightInvalidLegCheckSum[p][nAroundCheckSum][nKnightLeg] = TRUE;
			}
		}
	}

	//求炮的位行和位列的校验序号
	FSINT nCannonCheckSum;

	for (i = 0; i < 10; i++)
	{
		for (nNum = 0 ; nNum < CANNONFRCHECKSUMMAXCNT; nNum++)
		{
			m_nCannonFileRankCheckSum[i][nNum] = INVALIDCHECKSUM;
		}
	}

	//注意:bug易发区
	for (i = 0; i < 10; i++)
	{
        nNum = 0 ;

		for (k = 0; k < 1024; k++)
		{
			nCannonCheckSum = 0 ;

			nFlag = 0;
			for (m = i - 1; m >= 0; m--)
			{
				if (m_nMask[m] & k)
				{
					//if语句体不能颠倒
					if (nFlag == 1)
					{
						nCannonCheckSum |= m_nMask[m];
						break;
					}
					else
					{
						nCannonCheckSum |= m_nMask[m] ;
						nFlag = 1;
					}
				}
			}

			nFlag = 0;
			for (n = i + 1; n < 10; n++)
			{
				if (m_nMask[n] & k)
				{
					//if语句体不能颠倒
					if (nFlag == 1)
					{
						nCannonCheckSum |= m_nMask[n];
						break;
					}
					else
					{
						nCannonCheckSum |= m_nMask[n];
						nFlag = TRUE;
					}
				}
			}

			nFlag = 0;
			for (j = 0; j < nNum; j++)
			{
				if (m_nCannonFileRankCheckSum[i][j] == nCannonCheckSum)
				{
					nFlag = 1;
					break;
				}
			}

			if (nFlag)
			{
				m_nCannonFileRankCheckNum[i][k] = j;
			}
			else
			{
				m_nCannonFileRankCheckNum[i][k] = nNum;
				m_nCannonFileRankCheckSum[i][nNum] = nCannonCheckSum;

				nNum++;
			}
		}
	}

	//m_nCannonFoeFileRankCheckNum
	FSINT nCannonFoeFileRankCheckSum;
	
	for (i = 0; i < 10; i++)
	{
		for (k = 0; k < CANNONFRCHECKSUMMAXCNT; k++)
		{
			if ( m_nCannonFileRankCheckSum[i][k] == INVALIDCHECKSUM )
			{
				break ;
			}

			for (nNum = 0; nNum < 4; nNum++)
			{
				m_nCannonFoeFileRankCheckSum[i][k][nNum] = INVALIDCHECKSUM;
			}
		}
	}

	for (i = 0; i < 10; i++)
	{
		for (k = 0; k < CANNONFRCHECKSUMMAXCNT; k++)
		{
			if (m_nCannonFileRankCheckSum[i][k] == INVALIDCHECKSUM)
			{
				break;
			}

            nNum = 0;
            
			for (nFoeBit = 0;nFoeBit<1024;nFoeBit++)
			{
				//TODO:~的使用
				nCannonFoeFileRankCheckSum = nFoeBit & m_nCannonFileRankCheckSum[i][k] & 
											 (~m_nRookFileRankCheckSum[i][m_nRookFileRankCheckNum[i][m_nCannonFileRankCheckSum[i][k]]]);
				
                nFlag = 0;
				for (j = 0;j<nNum;j++)
				{
					if (m_nCannonFoeFileRankCheckSum[i][k][j] == nCannonFoeFileRankCheckSum)
					{
						nFlag = 1;
						break;
					}
				}
				if (nFlag)
				{
					m_nCannonFoeFileRankCheckNum[i][k][nFoeBit] = j;
				}
				else
				{
					m_nCannonFoeFileRankCheckNum[i][k][nFoeBit] = nNum ;
					m_nCannonFoeFileRankCheckSum[i][k][nNum] = nCannonFoeFileRankCheckSum ;

					nNum ++;
				}
			}
		}
	}

	//m_nKingPosCheckNum
    nNum = 0;
    for (i = 3; i < 6; i++)
    {
        for (j = 0;j<3;j++)
        {
            p = 10 * i + j;

            m_nKingPosCheckNum[p] = nNum;
            m_nKingPosCheckSum[nNum] = p;

            nNum++;
        }

        for (j = 7; j < 10; j++)
        {
            p = 10 * i + j;
            
            m_nKingPosCheckNum[p] = nNum;
            m_nKingPosCheckSum[nNum] = p;
            
            nNum++;
        }
    }

    //m_nAdvisorPosCheckNum
    m_nAdvisorPosCheckNum[30] = 0;
    m_nAdvisorPosCheckNum[50] = 1;
    m_nAdvisorPosCheckNum[41] = 2;
    m_nAdvisorPosCheckNum[32] = 3;
    m_nAdvisorPosCheckNum[52] = 4;

    m_nAdvisorPosCheckNum[37] = 5;
    m_nAdvisorPosCheckNum[57] = 6;
    m_nAdvisorPosCheckNum[48] = 7;
    m_nAdvisorPosCheckNum[39] = 8;
    m_nAdvisorPosCheckNum[59] = 9;

	m_nAdvisorPosCheckNum[90] = 10;
	
    m_nAdvisorPosCheckSum[0] = 30;
    m_nAdvisorPosCheckSum[1] = 50;
    m_nAdvisorPosCheckSum[2] = 41;
    m_nAdvisorPosCheckSum[3] = 32;
    m_nAdvisorPosCheckSum[4] = 52;

    m_nAdvisorPosCheckSum[5] = 37;
    m_nAdvisorPosCheckSum[6] = 57;
    m_nAdvisorPosCheckSum[7] = 48;
    m_nAdvisorPosCheckSum[8] = 39;
    m_nAdvisorPosCheckSum[9] = 59;

	m_nAdvisorPosCheckSum[10] = 90;
	
	//m_nBishopPosCheckNum
	m_nBishopPosCheckNum[20] = 0;
    m_nBishopPosCheckNum[60] = 1;
    m_nBishopPosCheckNum[2] = 2;
    m_nBishopPosCheckNum[42] = 3;
    m_nBishopPosCheckNum[82] = 4;
    m_nBishopPosCheckNum[24] = 5;
    m_nBishopPosCheckNum[64] = 6;
    m_nBishopPosCheckNum[25] = 7;
    m_nBishopPosCheckNum[65] = 8;
    m_nBishopPosCheckNum[7] = 9;
	m_nBishopPosCheckNum[47] = 10;
	m_nBishopPosCheckNum[87] = 11;
    m_nBishopPosCheckNum[29] = 12;
	m_nBishopPosCheckNum[69] = 13;
	m_nBishopPosCheckNum[90] = 14;

    m_nBishopPosCheckSum[0] = 20;
    m_nBishopPosCheckSum[1] = 60;
    m_nBishopPosCheckSum[2] = 2;
    m_nBishopPosCheckSum[3] = 42;
    m_nBishopPosCheckSum[4] = 82;
    m_nBishopPosCheckSum[5] = 24;
    m_nBishopPosCheckSum[6] = 64;
    m_nBishopPosCheckSum[7] = 25;
    m_nBishopPosCheckSum[8] = 65;
    m_nBishopPosCheckSum[9] = 7;
	m_nBishopPosCheckSum[10] = 47;
	m_nBishopPosCheckSum[11] = 87;
    m_nBishopPosCheckSum[12] = 29;
	m_nBishopPosCheckSum[13] = 69;
	m_nBishopPosCheckSum[14] = 90;

	//m_nPawnRankAroundCheckSum
	for (j = 0; j < 10; j++)
	{
		for (k = 0; k < 1024; k++)
		{
			m_nPawnRankAroundCheckSum[0][j][k] = 0;

			if ((j > 0) && (k & m_nMask[j - 1]))
			{
				m_nPawnRankAroundCheckSum[0][j][k] = 1;
			}
		}
	}

	for (j = 0; j < 10; j++)
	{
		for (k = 0; k < 1024; k++)
		{
			m_nPawnRankAroundCheckSum[1][j][k] = 0;

			if ((j < 9) && (k & m_nMask[j + 1]))
			{
				m_nPawnRankAroundCheckSum[1][j][k] = 1;
			}
		}
	}

	//m_nPawnPosCheckNum
	nNum = 0;
	for (i = 0; i < 9; i++)
	{
		for (j = 5; j < 7; j++)
		{
			if ((i % 2) == 0)
			{
				p = 10 * i + j;

				m_nPawnPosCheckNum[BLACK][p] = nNum;
				m_nPawnPosCheckSum[BLACK][nNum] = p;

				nNum++;
			}
		}
	}
	for (i = 0; i < 9; i++)
	{
		for (j = 0; j < 5; j++)
		{
			p = 10 * i + j;

			m_nPawnPosCheckNum[BLACK][p] = nNum;
			m_nPawnPosCheckSum[BLACK][nNum] = p;

			nNum++;
		}
	}
	m_nPawnPosCheckNum[BLACK][90] = 55;
	m_nPawnPosCheckSum[BLACK][55] = 90;

	nNum = 0;
	for (i = 0; i < 9; i++)
	{
		for (j = 3; j < 5; j++)
		{
			if ((i % 2) == 0)
			{
				p = 10 * i + j;

				m_nPawnPosCheckNum[RED][p] = nNum;
				m_nPawnPosCheckSum[RED][nNum] = p;

				nNum++;
			}
		}
	}
	for (i = 0; i < 9; i++)
	{
		for (j = 5; j < 10; j++)
		{
			p = 10 * i + j;

			m_nPawnPosCheckNum[RED][p] = nNum;
			m_nPawnPosCheckSum[RED][nNum] = p;

			nNum++;
		}
	}
	m_nPawnPosCheckNum[RED][90] = 55;
	m_nPawnPosCheckSum[RED][55] = 90;
	
	//m_nBRKingPosCheckNum[BLACK]
    nNum = 0;
    for (i = 3; i < 6; i++)
    {
        for (j = 7; j < 10; j++)
        {
            p = 10 * i + j;
            
            m_nBRKingPosCheckNum[BLACK][p] = nNum;
            m_nBRKingPosCheckSum[BLACK][nNum] = p;
            
            nNum++;
        }
    }
	m_nBRKingPosCheckNum[BLACK][MAXCHESSGRIDNUM] = nNum;

	//m_nBRKingPosCheckNum[RED]
    nNum = 0;
    for (i = 3; i < 6; i++)
    {
        for (j = 0;j<3;j++)
        {
            p = 10 * i + j;

            m_nBRKingPosCheckNum[RED][p] = nNum;
            m_nBRKingPosCheckSum[RED][nNum] = p;

            nNum++;
        }
    }
	m_nBRKingPosCheckNum[RED][MAXCHESSGRIDNUM] = nNum;
	
	//双将照面判断数组m_bIsTwoKingMeet[3][3][1024]
	FSINT y1, y2;
	FSINT nBKingPosCheckNum;
	FSINT nBKingPos;
	FSINT nRKingPosCheckNum;
	FSINT nRKingPos;
	FSBOOL bFlag;

	for (nBKingPosCheckNum = 0; nBKingPosCheckNum < 9; nBKingPosCheckNum++)
	{
		for (nRKingPosCheckNum = 0; nRKingPosCheckNum < 9; nRKingPosCheckNum++)
		{
			for (k = 0; k < 1024; k++)
			{
				m_bIsTwoKingMeet[nBKingPosCheckNum][nRKingPosCheckNum][k] = FALSE;

				nBKingPos = m_nBRKingPosCheckSum[BLACK][nBKingPosCheckNum];
				nRKingPos = m_nBRKingPosCheckSum[RED][nRKingPosCheckNum];
				
				if (m_CGN2Pos[nBKingPos].x != m_CGN2Pos[nRKingPos].x)
				{
					continue;
				}

				bFlag = TRUE;
				y1 = m_CGN2Pos[nBKingPos].y;
				y2 = m_CGN2Pos[nRKingPos].y;
				for (m = y2 + 1; m < y1; m++)
				{
					if (k & m_nMask[m])
					{
						bFlag = FALSE;
						break;
					}
				}
				m_bIsTwoKingMeet[nBKingPosCheckNum][nRKingPosCheckNum][k] = bFlag;
			}
		}
	}
	for (nBKingPosCheckNum = 0; nBKingPosCheckNum <= 9; nBKingPosCheckNum++)
	{
		for (k = 0; k < 1024; k++)
		{
			m_bIsTwoKingMeet[nBKingPosCheckNum][9][k] = FALSE;
		}
	}
	for (nRKingPosCheckNum = 0; nRKingPosCheckNum < 9; nRKingPosCheckNum++)
	{
		for (k = 0; k < 1024; k++)
		{
			m_bIsTwoKingMeet[9][nRKingPosCheckNum][k] = FALSE;
		}
	}

	//=============================局面评估=============================
	m_nHave[BLACK] = 0xFFFF;
	m_nHave[RED] = 0;

	m_nSign[BLACK] = 1;
	m_nSign[RED] = -1;

	for (i = 0; i < 17; i++)
	{
		m_nRookFlexValue[i] = (i + 1) * FV_ROOK;
	}
	for (i = 0; i < 8; i++)
	{
		m_nKnightFlexValue[i] = (i + 1) * FV_KNIGHT;
	}
	for (i = 0; i < 17; i++)
	{
		m_nCannonFlexValue[i] = (i + 1) * FV_CANNON;
	}

	m_nEmptyHeartCannonValue[0] = 0;
	m_nEmptyHeartCannonValue[1] = EMPTYHEARTVALUE;

	m_nKnightBasicValue[OPEN_GAME] = BV_KNIGHT_OPEN_GAME;
	m_nKnightBasicValue[MID_GAME] = BV_KNIGHT_MID_GAME;
	m_nKnightBasicValue[END_GAME] = BV_KNIGHT_END_GAME;

	m_nCannonBasicValue[OPEN_GAME] = BV_CANNON_OPEN_GAME;
	m_nCannonBasicValue[MID_GAME] = BV_CANNON_MID_GAME;
	m_nCannonBasicValue[END_GAME] = BV_CANNON_END_GAME;

	m_nPawnBasicValue[OPEN_GAME] = BV_PAWN_OPEN_GAME;
	m_nPawnBasicValue[MID_GAME] = BV_PAWN_MID_GAME;
	m_nPawnBasicValue[END_GAME] = BV_PAWN_END_GAME;

	//=============================搜索引擎=============================
	//填充随机数组
	srand(time(0));

	for (i = 0; i < 15; i++)
	{
		for (j = 0; j < 9; j++)
		{
			for (k = 0; k < 10; k++)
			{
				m_nHashKey32[i][j][k] = Rand32();
				m_nHashKey64[i][j][k] = Rand64();
			}
		}
	}
	m_nHashKeyPlayer32 = Rand32();
	m_nHashKeyPlayer64 = Rand64();

	//hash表初始化
	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < m_nHashTabMask + 1; j++)
		{
			m_pHashTab[i][j].nDepth = INVALIDPLY;
			m_pHashTab[i][j].nEval = 0;
			m_pHashTab[i][j].nCBCheckSum = 0;
		}
	}

	m_nDRF_NM = 2;	//深度减少因子-Null Move(DepthDecreaseFactor)
//	m_nDRF_NM = 6;	//深度减少因子-Null Move(DepthDecreaseFactor) 速度不错

	m_nHistoryShift = 2;
	
	//初始化m_MoveList每层最后一个走法,历史得分表中用到
	//FSINT CSearchEngine::Search(FSINT nDepth, FSINT alpha, FSINT beta)中
	//cm = m_MoveList[nDepth][nBestMoveNum];
	//DEBUGASSERT(m_nHistoryScore[cm.nFrom][cm.nTo] >= 0 && m_nHistoryScore[cm.nFrom][cm.nTo] + (1 << nDepth) <= UINT_MAX);
	//m_nHistoryScore[cm.nFrom][cm.nTo] += (1 << nDepth);
	FSINT nDepth;

	for (nDepth = 0; nDepth < MAXSEARCHDEPTH; nDepth++)
	{
		m_MoveList[nDepth][MAXALLMOVESCNT - 1].nFrom = MAXCHESSGRIDNUM;
		m_MoveList[nDepth][MAXALLMOVESCNT - 1].nTo = MAXCHESSGRIDNUM;
	}

	m_nSearchCnt = 0;
	m_nTotalSearchTime = 0;

	m_cmFight[0].nKind = NORMAL;

	m_cmNullMove.nFrom = INVALIDPIECEPOS;
	m_cmNullMove.nTo = INVALIDPIECEPOS;
	m_cmNullMove.nScore = 0;
	m_cmNullMove.nKind = NORMAL;
	m_cmNullMove.nEatChess = NOCHESS;

	m_bNullMoveOn = TRUE;

	m_bOpenBookOn = TRUE;
	m_bEndBookOn = TRUE;

	m_bOnlyMoveOn = TRUE;

	m_nGetOpenBookMoveErrCnt = 0;

	m_nContemptFactor = g_nContemptFactor_Normal;
	m_ChessStyle = Normal; //风格均衡

	m_nDrawMoves = 120;

	m_nCheck[FALSE][NORMAL] = NORMAL;
	m_nCheck[FALSE][ESCAPE] = ESCAPE;
	m_nCheck[FALSE][CAP] = CAP;
	m_nCheck[FALSE][EAT] = EAT;
	m_nCheck[FALSE][CHECK] = CHECK;

	m_nCheck[TRUE][NORMAL] = CHECK;
	m_nCheck[TRUE][ESCAPE] = CHECK;
	m_nCheck[TRUE][CAP] = CHECK;
	m_nCheck[TRUE][EAT] = CHECK;
	m_nCheck[TRUE][CHECK] = CHECK;

//	m_GenMovesFun[ROOK] = GenRookMoves;
//	m_GenMovesFun[KNIGHT] = GenKnightMoves;
//	m_GenMovesFun[CANNON] = GenCannonMoves;
//	m_GenMovesFun[KING] = GenKingMoves;
//	m_GenMovesFun[ADVISOR] = GenAdvisorMoves;
//	m_GenMovesFun[BISHOP] = GenBishopMoves;
//	m_GenMovesFun[PAWN] = GenPawnMoves;
//
//	m_GenMovesFun2[ROOK] = GenRookMoves;
//	m_GenMovesFun2[KNIGHT] = GenKnightMoves;
//	m_GenMovesFun2[CANNON] = GenCannonMoves;
//	m_GenMovesFun2[KING] = GenKingMoves;
//	m_GenMovesFun2[ADVISOR] = GenAdvisorMoves;
//	m_GenMovesFun2[BISHOP] = GenBishopMoves;
//	m_GenMovesFun2[PAWN] = GenPawnMoves;
//
//	m_GenCapMovesFun[ROOK] = GenRookCapMoves;
//	m_GenCapMovesFun[KNIGHT] = GenKnightCapMoves;
//	m_GenCapMovesFun[CANNON] = GenCannonCapMoves;
//	m_GenCapMovesFun[KING] = GenKingCapMoves;
//	m_GenCapMovesFun[ADVISOR] = GenAdvisorCapMoves;
//	m_GenCapMovesFun[BISHOP] = GenBishopCapMoves;
//	m_GenCapMovesFun[PAWN] = GenPawnCapMoves;
	
	m_nCheckExtensionPlies = 1;

	m_bIsHaveCheckMateMove = FALSE;
	m_bIsHaveGoodPosition = FALSE;

	//=============================UCCI协议=============================
#ifdef _WIN32
	m_hThinkThread = NULL;

	InitializeCriticalSection(&g_CriticalSection);
#endif

	m_bPonder = FALSE;
	m_bPonderHit = FALSE;

	m_nNoEatMoveCnt_BS = 0;
	m_nNoEatMoveCnt_IS = 0;
	m_nNoEatMoveCnt_IS_Backup = 0;

	m_nBouts = 0;
	m_nCurBout = 0;

	//=============================浅红协议=============================
	m_nQHLevel = 4;
	m_nSearchDepth = g_nQHSchDptByLvl[m_nQHLevel - 1];

	//=============================其他方面=============================
	//m_nBytes
	for (i = 0; i < MAXSEARCHDEPTH; i++)
	{
		m_nBytes[i] = i * sizeof(CHESSMOVE);
	}

	m_esUCCIEgnStatus = EgnSIdle;

	//m_nPieceID2ChessID
	//黑
	m_nPieceID2ChessID[23] = BROOK;
	m_nPieceID2ChessID[31] = BROOK;

	m_nPieceID2ChessID[24] = BKNIGHT;
	m_nPieceID2ChessID[30] = BKNIGHT;

	m_nPieceID2ChessID[21] = BCANNON;
	m_nPieceID2ChessID[22] = BCANNON;

	m_nPieceID2ChessID[27] = BKING;

	m_nPieceID2ChessID[26] = BADVISOR;
	m_nPieceID2ChessID[28] = BADVISOR;

	m_nPieceID2ChessID[25] = BBISHOP;
	m_nPieceID2ChessID[29] = BBISHOP;

	m_nPieceID2ChessID[16] = BPAWN;
	m_nPieceID2ChessID[17] = BPAWN;
	m_nPieceID2ChessID[18] = BPAWN;
	m_nPieceID2ChessID[19] = BPAWN;
	m_nPieceID2ChessID[20] = BPAWN;

	//红
	m_nPieceID2ChessID[0]  = RROOK;
	m_nPieceID2ChessID[8]  = RROOK;

	m_nPieceID2ChessID[1]  = RKNIGHT;
	m_nPieceID2ChessID[7]  = RKNIGHT;

	m_nPieceID2ChessID[9]  = RCANNON;
	m_nPieceID2ChessID[10] = RCANNON;

	m_nPieceID2ChessID[4]  = RKING;

	m_nPieceID2ChessID[3]  = RADVISOR;
	m_nPieceID2ChessID[5]  = RADVISOR;

	m_nPieceID2ChessID[2]  = RBISHOP;
	m_nPieceID2ChessID[6]  = RBISHOP;

	m_nPieceID2ChessID[11] = RPAWN;
	m_nPieceID2ChessID[12] = RPAWN;
	m_nPieceID2ChessID[13] = RPAWN;
	m_nPieceID2ChessID[14] = RPAWN;
	m_nPieceID2ChessID[15] = RPAWN;	

	//棋子方判断数组
	for (i = BBEGIN; i <= REND; i++)
	{
		if (i >= BBEGIN && i <= BEND)
		{
			m_bIsBlack_ChessID[i] = TRUE;
			m_bIsRed_ChessID[i] = FALSE;
		}
		else
		{
			m_bIsBlack_ChessID[i] = FALSE;
			m_bIsRed_ChessID[i] = TRUE;
		}
	}
	for (i = 0; i < 32; i++)
	{
		m_bIsBlack_PieceID[i] = m_bIsBlack_ChessID[m_nPieceID2ChessID[i]];
		m_bIsRed_PieceID[i] = m_bIsRed_ChessID[m_nPieceID2ChessID[i]];
	}
	//MakeMove和UnMakeMove用到
	m_bIsBlack_PieceID[INVALIDPIECEID] = 2;
	m_bIsRed_PieceID[INVALIDPIECEID] = 2;

	//m_nChessID2PieceID
	//黑
	m_nChessID2PieceID[BKING][0] = 27;

	m_nChessID2PieceID[BADVISOR][0] = 26;
	m_nChessID2PieceID[BADVISOR][1] = 28;

	m_nChessID2PieceID[BBISHOP][0] = 25;
	m_nChessID2PieceID[BBISHOP][1] = 29;

	m_nChessID2PieceID[BROOK][0] = 23;
	m_nChessID2PieceID[BROOK][1] = 31;

	m_nChessID2PieceID[BKNIGHT][0] = 24;
	m_nChessID2PieceID[BKNIGHT][1] = 30;

	m_nChessID2PieceID[BCANNON][0] = 21;
	m_nChessID2PieceID[BCANNON][1] = 22;

	m_nChessID2PieceID[BPAWN][0] = 16;
	m_nChessID2PieceID[BPAWN][1] = 17;
	m_nChessID2PieceID[BPAWN][2] = 18;
	m_nChessID2PieceID[BPAWN][3] = 19;
	m_nChessID2PieceID[BPAWN][4] = 20;

	//红
	m_nChessID2PieceID[RKING][0] = 4;

	m_nChessID2PieceID[RADVISOR][0] = 3;
	m_nChessID2PieceID[RADVISOR][1] = 5;

	m_nChessID2PieceID[RBISHOP][0] = 2;
	m_nChessID2PieceID[RBISHOP][1] = 6;

	m_nChessID2PieceID[RROOK][0] = 0;
	m_nChessID2PieceID[RROOK][1] = 8;

	m_nChessID2PieceID[RKNIGHT][0] = 1;
	m_nChessID2PieceID[RKNIGHT][1] = 7;

	m_nChessID2PieceID[RCANNON][0] = 9;
	m_nChessID2PieceID[RCANNON][1] = 10;

	m_nChessID2PieceID[RPAWN][0] = 11;
	m_nChessID2PieceID[RPAWN][1] = 12;
	m_nChessID2PieceID[RPAWN][2] = 13;
	m_nChessID2PieceID[RPAWN][3] = 14;
	m_nChessID2PieceID[RPAWN][4] = 15;

	//黑
	m_nInitChessCnt[BKING] = 1;
	for (i=2;i<7;i++)
	{
		m_nInitChessCnt[i] = 2;
	}
	m_nInitChessCnt[BPAWN] = 5;

	//红
	m_nInitChessCnt[RKING] = 1;
	for (i=9;i<14;i++)
	{
		m_nInitChessCnt[i] = 2;
	}
	m_nInitChessCnt[RPAWN] = 5;

	for (i = 0; i < 10; i++)
	{
		for (j = 0; j < 3; j++)
		{
			m_nAbs[i][j] = (i - j) < 0 ? 11 : (i - j);
		}
	}

	m_nRook[0] = BROOK;
	m_nRook[1] = RROOK;

	m_nKnight[0] = BKNIGHT;
	m_nKnight[1] = RKNIGHT;

	m_nCannon[0] = BCANNON;
	m_nCannon[1] = RCANNON;

	m_nKing[0] = BKING;
	m_nKing[1] = RKING;

	m_nAdvisor[0] = BADVISOR;
	m_nAdvisor[1] = RADVISOR;

	m_nBishop[0] = BBISHOP;
	m_nBishop[1] = RBISHOP;

	m_nPawn[0] = BPAWN;
	m_nPawn[1] = RPAWN;

	m_nKingID2PieceID[BLACK] = m_nChessID2PieceID[BKING][0];
	m_nKingID2PieceID[RED] = m_nChessID2PieceID[RKING][0];

	//棋盘特征初始化(扩展)
	for (i = 10; i < 12; i++)
	{
		m_nBitFile[i] = 0;

		m_nBRBitFile[0][i] = 0;
		m_nBRBitFile[1][i] = 0;
	}
	for (i = 9; i < 12; i++)
	{
		m_nBitRank[i] = 0;

		m_nBRBitRank[0][i] = 0;
		m_nBRBitRank[1][i] = 0;
	}

	m_nPiecePos[MAXPIECEID] = INVALIDPIECEPOS;

	memcpy(m_szWhoN2WhoStr[0], "黑", 3);
	memcpy(m_szWhoN2WhoStr[1], "红", 3);

	for (i = 0; i < 32; i++)
	{
		m_nPieceID2Bit[i] = 1;
	}
	m_nPieceID2Bit[INVALIDPIECEID] = 0;

	m_nMaxSearchTime = 0; // 最长搜索时间[2005/10/22 15:46:05]

	memcpy(m_szBookFile, BOOKFILE, MAXBUFFERLEN);

	m_nConvertChessID[NOCHESS] = NOCHESS;
	for (i = BBEGIN; i <= BEND; i++)
	{
		m_nConvertChessID[i] = i + 7;
	}
	for (i = RBEGIN; i <= REND; i++)
	{
		m_nConvertChessID[i] = i - 7;
	}
}

/*******************************************************************************************************
函数名 : GenPreMoves
描  述 : 走法预产生
参  数 : 无
返回值 : 无
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年8月7日 12:42:31
*******************************************************************************************************/
FSVOID CSearchEngine::GenPreMoves()
{
//	cout << "正在产生车预走法..." << endl;
	GenRookPreMoves   ();	//车

	FSINT t;

	switch(m_EgnProtocol)
	{
	case EgnPUCCI:
	case EgnPAhei:
		cout << "正在产生马预走法..." << endl;
		break;

	case EgnPQianHong:
		STATEMENT_TESTQHP(cout << "正在产生马预走法..." << endl;);
		break;

	default:
		break;
	}
	t = clock();
	GenKnightPreMoves ();	//马
	switch(m_EgnProtocol)
	{
	case EgnPUCCI:
	case EgnPAhei:
		cout << "产生马预走法时间:" << clock() - t << "ms" << endl;
		break;
					
	case EgnPQianHong:
		STATEMENT_TESTQHP(cout << "产生马预走法时间:" << clock() - t << "ms" << endl;);
		break;
					
	default:
		break;
	}

//	cout << "正在产生炮预走法..." << endl;
	t = clock();
	GenCannonPreMoves ();	//炮
//	cout << "产生炮预走法时间:" << clock() - t << "ms" << endl;

//	cout << "正在产生将预走法..." << endl;
	GenKingPreMoves   ();	//将

//	cout << "正在产生士预走法..." << endl;
	GenAdvisorPreMoves();	//士

//	cout << "正在产生象预走法..." << endl;
	GenBishopPreMoves ();	//象

//	cout << "正在产生兵预走法..." << endl;
	GenPawnPreMoves   ();	//兵
}

/*******************************************************************************************************
函数名 : GenRookPreMoves
描  述 : 车的走法生成
参  数 : 无
返回值 : 无
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年7月25日 1:04:29
*******************************************************************************************************/
FSVOID CSearchEngine::GenRookPreMoves()
{
	FSINT p;
	FSINT m, n;
	FSINT i, j, k;
	FSINT nAllCount;
	FSINT nNoCapCount;
	FSINT nCapCount;
	FSINT nCount1, nCount2;
	FSINT nBitFile, nBitRank;//位行,位列
	FSINT nFoeBitFile, nFoeBitRank;//对方子位行,位列
	FSINT nFoeFileCheckNum, nFoeRankCheckNum;//对方子位行和位列校验序号
	
#ifdef MEMCALC
	
	FSINT nRookMovesSize = 0;
	FSINT nRookCapMovesSize = 0;
	
#endif
	
	//注意:bug易发区,注意File和Rank
	for (p = 0; p < 90; p++)
	{
		i = m_CGN2Pos[p].x;
		j = m_CGN2Pos[p].y;
		
		//行走法
		for (k = 0; k < ROOKFILECHECKSUMMAXCNT; k++)
		{
			//必须!!!
			if ((nBitFile = m_nRookFileRankCheckSum[i][k]) == INVALIDCHECKSUM)
			{
				break;
			}
			
			nCount1 = 0;
			for (m = i - 1; m >= 0; m--)
			{
				if (m_nMask[m] & nBitFile)
				{
					break;
				}
				
				nCount1++;
			}
			
			nCount2 = 0;
			for (n = i + 1; n < 9; n++)
			{
				if (m_nMask[n] & nBitFile)
				{
					break;
				}
				
				nCount2++;
			}
			
			nNoCapCount = nCount1 + nCount2;

			if (nNoCapCount > 0)
			{
				DEBUGASSERT(nAllCount - nCapCount <= MAXROOKFILENOCAPMOVECNT);
				TRYNEW(m_pcmRookFileNoCapPreMoves[p][k].pMoveList = 
					new CHESSMOVE[nNoCapCount];);

				//行不吃子走法
				for (m = i - 1; m > i - 1 - nCount1; m--)
				{
					DEBUGASSERT(m >= 0 && i - 1 - m >= 0 && i - 1 - m < nNoCapCount);
					
					FILLCHESSMOVE(m_pcmRookFileNoCapPreMoves[p][k].pMoveList[i - 1 - m], 
						p, 10 * m + j, BV_ROOK, NOCHESS, NORMAL);
				}
				for (n = i + 1; n < i + 1 + nCount2; n++)
				{
					FILLCHESSMOVE(m_pcmRookFileNoCapPreMoves[p][k].pMoveList[n - i - 1 + nCount1], 
						p, POS2CGN(n, j), BV_ROOK, NOCHESS, NORMAL);
				}
			}

#ifdef MEMCALC
			
			nRookCapMovesSize += m_nBytes[nNoCapCount];
			
#endif
			
			for (nFoeFileCheckNum = 0; nFoeFileCheckNum < 4; nFoeFileCheckNum++)
			{
				//必须!!!
				if ((nFoeBitFile = m_nRookFoeFileRankCheckSum[i][k][nFoeFileCheckNum]) == INVALIDCHECKSUM)
				{
					break;
				}
				
				nAllCount = nNoCapCount;
				
				if (nCount1 < i)
				{
					if (m_nMask[i - nCount1 - 1] & nFoeBitFile)//说明i - nCount1 - 1位对方有子
					{
						nAllCount++;
					}
				}
				if ((i + nCount2 + 1 ) < 9)
				{
					if (m_nMask[i + nCount2 + 1] & nFoeBitFile)//说明i + nCount1 + 1位对方有子
					{
						nAllCount++;
					}
				}
				DEBUGASSERT(nAllCount <= MAXROOKFILEMOVECNT);
				
#ifdef MEMCALC
				
				nRookMovesSize += m_nBytes[nAllCount];
				
#endif

				nCapCount = nAllCount - nNoCapCount;

				m_pcmRookFilePreMoves[p][k][nFoeFileCheckNum].nCount = nAllCount;
				m_pcmRookFileNoCapPreMoves[p][k].nCount = nNoCapCount;
				m_pcmRookFileCapPreMoves[p][k][nFoeFileCheckNum].nCount = nCapCount;
				if (nAllCount)
				{
					TRYNEW(m_pcmRookFilePreMoves[p][k][nFoeFileCheckNum].pMoveList = 
						new CHESSMOVE[nAllCount];);

					//行不吃子走法
					if (nNoCapCount > 0)
					{
						for (m = i - 1; m > i - 1 - nCount1; m--)
						{
							DEBUGASSERT(m >= 0 && i - 1 - m >= 0 && i - 1 - m < nAllCount);
							
							FILLCHESSMOVE(m_pcmRookFilePreMoves[p][k][nFoeFileCheckNum].pMoveList[i - 1 - m], 
								p, 10 * m + j, BV_ROOK, NOCHESS, NORMAL);
						}
						for (n = i + 1; n < i + 1 + nCount2; n++)
						{
							FILLCHESSMOVE(m_pcmRookFilePreMoves[p][k][nFoeFileCheckNum].pMoveList[n - i - 1 + nCount1], 
								p, POS2CGN(n, j), BV_ROOK, NOCHESS, NORMAL);
						}
					}

					//行吃子走法
					if (nCapCount > 0)
					{
						DEBUGASSERT(nCapCount <= MAXROOKFILECAPMOVECNT);
						TRYNEW(m_pcmRookFileCapPreMoves[p][k][nFoeFileCheckNum].pMoveList = 
							new CHESSMOVE[nCapCount];);

						if (nCount1 < i)
						{
							if (m_nMask[i - nCount1 - 1] & nFoeBitFile)//说明i-Num1-1位对方有子
							{
								DEBUGASSERT(i - nCount1 - 1 >= 0 && i - nCount1 - 1 < 9);							
								DEBUGASSERT(nNoCapCount < nAllCount);
								
								FILLCHESSMOVE(m_pcmRookFilePreMoves[p][k][nFoeFileCheckNum].pMoveList[nCount1 + nCount2], 
									p, POS2CGN(i - nCount1 - 1, j), BV_ROOK, NOCHESS, EAT);
								
								FILLCHESSMOVE(m_pcmRookFileCapPreMoves[p][k][nFoeFileCheckNum].pMoveList[0], 
									p, POS2CGN(i - nCount1 - 1, j), BV_ROOK, NOCHESS, EAT);
							}
						}
						if (i + nCount2 + 1 < 9)
						{
							if (m_nMask[i + nCount2 + 1] & nFoeBitFile)//说明i+Num2+1位对方有子
							{
								DEBUGASSERT(i + nCount2 + 1 >= 0 && i + nCount2 + 1 < 9);							
								DEBUGASSERT(nAllCount - 1 >= 0);
								
								FILLCHESSMOVE(m_pcmRookFilePreMoves[p][k][nFoeFileCheckNum].pMoveList[nAllCount - 1], 
									p, POS2CGN(i + nCount2 + 1, j), BV_ROOK, NOCHESS, EAT);
								
								FILLCHESSMOVE(m_pcmRookFileCapPreMoves[p][k][nFoeFileCheckNum].pMoveList[nCapCount - 1], 
									p, POS2CGN(i + nCount2 + 1, j), BV_ROOK, NOCHESS, EAT);
							}
						}
					}
				}
			}
		}

		//列走法
		for (k = 0; k < ROOKRANKCHECKSUMMAXCNT; k++)
		{
			//必须!!!
			if ((nBitRank = m_nRookFileRankCheckSum[j][k]) == INVALIDCHECKSUM)
			{
				break ;
			}
			
			nCount1 = 0;
			for (m = j - 1; m >= 0; m--)
			{
				if (m_nMask[m] & nBitRank)
				{
					break;
				}
				
				nCount1++;
			}
			
			nCount2 = 0;
			for (n = j + 1; n < 10; n++)
			{
				if (m_nMask[n] & nBitRank)
				{
					break;
				}
				
				nCount2++;
			}
			
			nNoCapCount = nCount1 + nCount2;
			if (nNoCapCount > 0)
			{
				DEBUGASSERT(nCapCount <= MAXROOKRANKNOCAPMOVECNT);

				TRYNEW(m_pcmRookRankNoCapPreMoves[p][k].pMoveList = 
					new CHESSMOVE[nNoCapCount];);
				
				//列不吃子走法
				for (m = j - 1; m > j - 1 - nCount1; m--)
				{
					DEBUGASSERT(j - 1 - m >= 0 && j - 1 - m < nNoCapCount);
					
					FILLCHESSMOVE(m_pcmRookRankNoCapPreMoves[p][k].pMoveList[j - 1 - m], 
						p, POS2CGN(i, m), BV_ROOK, NOCHESS, NORMAL);
				}
				
				for (n = j + 1; n < j + 1 + nCount2; n++)
				{
					DEBUGASSERT(n - j - 1 + nCount1 >= 0 && n - j - 1 + nCount1 < nNoCapCount);

					FILLCHESSMOVE(m_pcmRookRankNoCapPreMoves[p][k].pMoveList[n - j - 1 + nCount1], 
						p, POS2CGN(i, n), BV_ROOK, NOCHESS, NORMAL);
				}
			}

#ifdef MEMCALC
			
			nRookCapMovesSize += m_nBytes[nNoCapCount];
			
#endif
			
			for (nFoeRankCheckNum = 0; nFoeRankCheckNum < 4; nFoeRankCheckNum++)
			{
				//必须!!!
				if ((nFoeBitRank = m_nRookFoeFileRankCheckSum[j][k][nFoeRankCheckNum]) == INVALIDCHECKSUM)
				{
					break;
				}
				
				nAllCount = nNoCapCount;
				
				if (nCount1 < j)
				{
					if (m_nMask[j - nCount1 - 1] & nFoeBitRank)//说明j - nCount1 - 1位对方有子
					{
						nAllCount++;
					}
				}
				if ((j + nCount2 + 1) < 10)
				{
					if (m_nMask[j + nCount2 + 1] & nFoeBitRank)//说明j + nCount2 + 1位对方有子
					{
						nAllCount++;
					}
				}
				DEBUGASSERT(nAllCount <= MAXROOKRANKMOVECNT);
				
#ifdef MEMCALC
				
				nRookMovesSize += m_nBytes[nAllCount];
				
#endif

				nCapCount = nAllCount - nNoCapCount;
				m_pcmRookRankPreMoves[p][k][nFoeRankCheckNum].nCount = nAllCount;
				m_pcmRookRankNoCapPreMoves[p][k].nCount = nNoCapCount;
				m_pcmRookRankCapPreMoves[p][k][nFoeRankCheckNum].nCount = nCapCount;
				if (nAllCount)
				{
					TRYNEW(m_pcmRookRankPreMoves[p][k][nFoeRankCheckNum].pMoveList = 
						new CHESSMOVE[nAllCount];);

					//列不吃子走法
					if (nNoCapCount > 0)
					{
						DEBUGASSERT(nCapCount <= MAXROOKRANKNOCAPMOVECNT);

						for (m = j - 1; m > j - 1 - nCount1; m--)
						{
							DEBUGASSERT(j - 1 - m >= 0 && j - 1 - m < nAllCount);
							
							FILLCHESSMOVE(m_pcmRookRankPreMoves[p][k][nFoeRankCheckNum].pMoveList[j - 1 - m], 
								p, POS2CGN(i, m), BV_ROOK, NOCHESS, NORMAL);
						}
						
						for (n = j + 1; n < j + 1 + nCount2; n++)
						{
							DEBUGASSERT(n - j - 1 + nCount1 >= 0 && n - j - 1 + nCount1 < nAllCount);
							
							FILLCHESSMOVE(m_pcmRookRankPreMoves[p][k][nFoeRankCheckNum].pMoveList[n - j - 1 + nCount1], 
								p, POS2CGN(i, n), BV_ROOK, NOCHESS, NORMAL);
						}
					}

					//列吃子走法
					if (nCapCount > 0)
					{
						DEBUGASSERT(nCapCount <= MAXROOKRANKCAPMOVECNT);
						TRYNEW(m_pcmRookRankCapPreMoves[p][k][nFoeRankCheckNum].pMoveList = 
							new CHESSMOVE[nCapCount];);
						
						if (nCount1 < j)
						{
							if (m_nMask[j - nCount1 - 1] & nFoeBitRank)//说明j-Num1-1位对方有子
							{
								FILLCHESSMOVE(m_pcmRookRankPreMoves[p][k][nFoeRankCheckNum].pMoveList[nCount1 + nCount2], 
									p, POS2CGN(i, j - nCount1 - 1), BV_ROOK, NOCHESS, EAT);
								
								FILLCHESSMOVE(m_pcmRookRankCapPreMoves[p][k][nFoeRankCheckNum].pMoveList[0], 
									p, POS2CGN(i, j - nCount1 - 1), BV_ROOK, NOCHESS, EAT);
							}
						}
						if ((j + nCount2 + 1) < 10)
						{
							if (m_nMask[j+nCount2+1] & nFoeBitRank)//说明i+Num2+1位对方有子
							{
								FILLCHESSMOVE(m_pcmRookRankPreMoves[p][k][nFoeRankCheckNum].pMoveList[nAllCount - 1], 
									p, POS2CGN(i, j + nCount2 + 1), BV_ROOK, NOCHESS, EAT);
								
								FILLCHESSMOVE(m_pcmRookRankCapPreMoves[p][k][nFoeRankCheckNum].pMoveList[nCapCount - 1], 
									p, POS2CGN(i, j + nCount2 + 1), BV_ROOK, NOCHESS, EAT);
							}
						}
					}
				}
			}
		}
	}

#ifdef MEMCALC
		
		cout << "车所有走法内存:" << setprecision(3) << B2M(nRookMovesSize) << "M" << endl;
		cout << "车所有吃子走法内存:" << setprecision(3) << B2M(nRookMovesSize - nRookCapMovesSize) << "M" << endl;
		
#endif
}

/*******************************************************************************************************
函数名 : GenKnightPreMoves
描  述 : 马走法预产生
参  数 : 无
返回值 : 无
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年8月7日 12:37:41
*******************************************************************************************************/
FSVOID CSearchEngine::GenKnightPreMoves()
{
	FSINT p;
	FSINT i, j;
	FSINT nCount;
	FSINT nCapCount;
	FSINT nKnightLeg;
	FSINT nAroundCheckSum;//马可走点的校验和

#ifdef MEMCALC

	FSINT nKnightMovesSize = 0;
	FSINT nKnightCapMovesSize = 0;

#endif

	// 加载马预走法成功[2005/10/21 22:31:51]
//	if (LoadKnightPreMove() == TRUE)
//	{
//		return;
//	}

	//不吃子走法
	for (p = 0; p < 90; p++)
	{
		i = m_CGN2Pos[p].x;
		j = m_CGN2Pos[p].y;

		//马可走点的校验和
		for (nAroundCheckSum = 0; nAroundCheckSum < 256; nAroundCheckSum++)
		{
			DEBUGASSERT(m_bIsKnightInvalidArroundCheckSum[p][nAroundCheckSum] == TRUE || 
				   m_bIsKnightInvalidArroundCheckSum[p][nAroundCheckSum] == FALSE);

			//判断此校验和是否有效
			if (m_bIsKnightInvalidArroundCheckSum[p][nAroundCheckSum] == FALSE)
			{
				continue;
			}
			
			//马腿校验和
			for (nKnightLeg = 0; nKnightLeg < 16; nKnightLeg++)
			{
				DEBUGASSERT(m_bIsKnightInvalidLegCheckSum[p][nAroundCheckSum][nKnightLeg] == TRUE ||
					   m_bIsKnightInvalidLegCheckSum[p][nAroundCheckSum][nKnightLeg] == FALSE);

				//判断此马腿校验和是否有效
				if (m_bIsKnightInvalidLegCheckSum[p][nAroundCheckSum][nKnightLeg] == FALSE)
				{
					continue;
				}

				nCount = 0;

				//左
				if (((nKnightLeg & m_nMask[0]) == 0) && (i - 2 >= 0))
				{
					//下
					if (((nAroundCheckSum & m_nMask[2]) == 0) && (j - 1 >= 0))
					{
						nCount++;
					}
					//上
					if (((nAroundCheckSum & m_nMask[4]) == 0) && (j + 1 < 10))
					{
						nCount++;
					}
				}
				
				//右
				if (((nKnightLeg & m_nMask[1]) == 0) && (i + 2 < 9))
				{
					//下
					if (((nAroundCheckSum & m_nMask[3]) == 0) && (j - 1 >= 0))
					{
						nCount++;
					}
					//上
					if (((nAroundCheckSum & m_nMask[5]) == 0) && (j + 1 < 10))
					{
						nCount++;
					}
				}
				
				//下
				if (((nKnightLeg & m_nMask[2]) == 0) && (j - 2 >= 0))
				{
					//左
					if (((nAroundCheckSum & m_nMask[0]) == 0) && (i - 1 >= 0))
					{
						nCount++;
					}
					//右
					if (((nAroundCheckSum & m_nMask[1]) == 0) && (i + 1 < 9))
					{
						nCount++;
					}
				}

				//上
				if (((nKnightLeg & m_nMask[3]) == 0) && j + 2 < 10)
				{
					//左
					if (((nAroundCheckSum & m_nMask[6]) == 0) && (i - 1 >= 0))
					{
						nCount++;
					}
					//右
					if (((nAroundCheckSum & m_nMask[7]) == 0) && (i + 1 < 9))
					{
						nCount++;
					}
				}
				DEBUGASSERT(nCount >= 0 && nCount <= 8);

				m_pcmKnightNoCapPreMoves[p][nAroundCheckSum][nKnightLeg].nCount = nCount;
				if (nCount)
				{
					TRYNEW(m_pcmKnightNoCapPreMoves[p][nAroundCheckSum][nKnightLeg].pMoveList = new CHESSMOVE[nCount];);

#ifdef MEMCALC

					nKnightMovesSize += m_nBytes[nCount];

#endif

					nCount = 0;

					//左
					if (((nKnightLeg & m_nMask[0]) == 0) && (i - 2 >= 0))
					{
						//下
						if (((nAroundCheckSum & m_nMask[2]) == 0) && (j - 1 >= 0))
						{
//							m_pcmKnightNoCapPreMoves[p][nAroundCheckSum][nKnightLeg].pMoveList[nCount].nFrom = p;
//							m_pcmKnightNoCapPreMoves[p][nAroundCheckSum][nKnightLeg].pMoveList[nCount].nTo = p - 21;
//							m_pcmKnightNoCapPreMoves[p][nAroundCheckSum][nKnightLeg].pMoveList[nCount].nKind = NORMAL;

							FILLCHESSMOVE(m_pcmKnightNoCapPreMoves[p][nAroundCheckSum][nKnightLeg].pMoveList[nCount], 
								p, p - 21, BV_KNIGHT_OPEN_GAME, NOCHESS, NORMAL);

							nCount++;
						}
						
						//上
						if (((nAroundCheckSum & m_nMask[4]) == 0) && (j + 1 < 10))
						{
//							m_pcmKnightNoCapPreMoves[p][nAroundCheckSum][nKnightLeg].pMoveList[nCount].nFrom = p;
//							m_pcmKnightNoCapPreMoves[p][nAroundCheckSum][nKnightLeg].pMoveList[nCount].nTo = p - 19;
//							m_pcmKnightNoCapPreMoves[p][nAroundCheckSum][nKnightLeg].pMoveList[nCount].nKind = NORMAL;
							
							FILLCHESSMOVE(m_pcmKnightNoCapPreMoves[p][nAroundCheckSum][nKnightLeg].pMoveList[nCount], 
								p, p - 19, BV_KNIGHT_OPEN_GAME, NOCHESS, NORMAL);

							nCount++;
						}
					}
					
					//右
					if (((nKnightLeg & m_nMask[1]) == 0) && (i + 2 < 9))
					{
						//下
						if (((nAroundCheckSum & m_nMask[3]) == 0) && (j - 1 >= 0))
						{
//							m_pcmKnightNoCapPreMoves[p][nAroundCheckSum][nKnightLeg].pMoveList[nCount].nFrom = p;
//							m_pcmKnightNoCapPreMoves[p][nAroundCheckSum][nKnightLeg].pMoveList[nCount].nTo = p + 19;
//							m_pcmKnightNoCapPreMoves[p][nAroundCheckSum][nKnightLeg].pMoveList[nCount].nKind = NORMAL;
							
							FILLCHESSMOVE(m_pcmKnightNoCapPreMoves[p][nAroundCheckSum][nKnightLeg].pMoveList[nCount], 
								p, p + 19, BV_KNIGHT_OPEN_GAME, NOCHESS, NORMAL);

							nCount++;
						}
						
						//上
						if (((nAroundCheckSum & m_nMask[5]) == 0) && (j + 1 < 10))
						{
//							m_pcmKnightNoCapPreMoves[p][nAroundCheckSum][nKnightLeg].pMoveList[nCount].nFrom = p;
//							m_pcmKnightNoCapPreMoves[p][nAroundCheckSum][nKnightLeg].pMoveList[nCount].nTo = p + 21;
//							m_pcmKnightNoCapPreMoves[p][nAroundCheckSum][nKnightLeg].pMoveList[nCount].nKind = NORMAL;
							
							FILLCHESSMOVE(m_pcmKnightNoCapPreMoves[p][nAroundCheckSum][nKnightLeg].pMoveList[nCount], 
								p, p + 21, BV_KNIGHT_OPEN_GAME, NOCHESS, NORMAL);

							nCount++;
						}
					}
					
					//下
					if (((nKnightLeg & m_nMask[2]) == 0) && (j - 2 >= 0))
					{
						//左
						if (((nAroundCheckSum & m_nMask[0]) == 0) && (i - 1 >= 0))
						{
//							m_pcmKnightNoCapPreMoves[p][nAroundCheckSum][nKnightLeg].pMoveList[nCount].nFrom = p;
//							m_pcmKnightNoCapPreMoves[p][nAroundCheckSum][nKnightLeg].pMoveList[nCount].nTo = p - 12;
//							m_pcmKnightNoCapPreMoves[p][nAroundCheckSum][nKnightLeg].pMoveList[nCount].nKind = NORMAL;
							
							FILLCHESSMOVE(m_pcmKnightNoCapPreMoves[p][nAroundCheckSum][nKnightLeg].pMoveList[nCount], 
								p, p - 12, BV_KNIGHT_OPEN_GAME, NOCHESS, NORMAL);

							nCount++;
						}

						//右
						if (((nAroundCheckSum & m_nMask[1]) == 0) && (i + 1 < 9))
						{
//							m_pcmKnightNoCapPreMoves[p][nAroundCheckSum][nKnightLeg].pMoveList[nCount].nFrom = p;
//							m_pcmKnightNoCapPreMoves[p][nAroundCheckSum][nKnightLeg].pMoveList[nCount].nTo = p + 8;
//							m_pcmKnightNoCapPreMoves[p][nAroundCheckSum][nKnightLeg].pMoveList[nCount].nKind = NORMAL;
							
							FILLCHESSMOVE(m_pcmKnightNoCapPreMoves[p][nAroundCheckSum][nKnightLeg].pMoveList[nCount], 
								p, p + 8, BV_KNIGHT_OPEN_GAME, NOCHESS, NORMAL);

							nCount++;
						}
					}

					//上
					if (((nKnightLeg & m_nMask[3]) == 0) && j + 2 < 10)
					{
						//左
						if (((nAroundCheckSum & m_nMask[6]) == 0) && (i - 1 >= 0))
						{
//							m_pcmKnightNoCapPreMoves[p][nAroundCheckSum][nKnightLeg].pMoveList[nCount].nFrom = p;
//							m_pcmKnightNoCapPreMoves[p][nAroundCheckSum][nKnightLeg].pMoveList[nCount].nTo = p - 8;
//							m_pcmKnightNoCapPreMoves[p][nAroundCheckSum][nKnightLeg].pMoveList[nCount].nKind = NORMAL;
//							
							FILLCHESSMOVE(m_pcmKnightNoCapPreMoves[p][nAroundCheckSum][nKnightLeg].pMoveList[nCount], 
								p, p - 8, BV_KNIGHT_OPEN_GAME, NOCHESS, NORMAL);

							nCount++;
						}

						//右
						if (((nAroundCheckSum & m_nMask[7]) == 0) && (i + 1 < 9))
						{
//							m_pcmKnightNoCapPreMoves[p][nAroundCheckSum][nKnightLeg].pMoveList[nCount].nFrom = p;
//							m_pcmKnightNoCapPreMoves[p][nAroundCheckSum][nKnightLeg].pMoveList[nCount].nTo = p + 12;
//							m_pcmKnightNoCapPreMoves[p][nAroundCheckSum][nKnightLeg].pMoveList[nCount].nKind = NORMAL;
							
							FILLCHESSMOVE(m_pcmKnightNoCapPreMoves[p][nAroundCheckSum][nKnightLeg].pMoveList[nCount], 
								p, p + 12, BV_KNIGHT_OPEN_GAME, NOCHESS, NORMAL);

							nCount++;
						}
					}
				}
			}
		}
	}

	//吃子走法
	FSINT nFoeBitBoardCheckSum;//对方子位棋盘校验和

	for (p = 0; p < 90; p++)
	{
		i = m_CGN2Pos[p].x;
		j = m_CGN2Pos[p].y;
		
		//马可走点为对方子的校验和
		for (nFoeBitBoardCheckSum = 0; nFoeBitBoardCheckSum < 256; nFoeBitBoardCheckSum++)
		{
			DEBUGASSERT(m_bIsKnightInvalidArroundCheckSum[p][nFoeBitBoardCheckSum] == TRUE || 
				m_bIsKnightInvalidArroundCheckSum[p][nFoeBitBoardCheckSum] == FALSE);

			//检查是否为有效的校验和
			if (m_bIsKnightInvalidArroundCheckSum[p][nFoeBitBoardCheckSum] == FALSE)
			{
				continue;
			}
			
			//马腿校验和
			for (nKnightLeg = 0; nKnightLeg < 16; nKnightLeg++)
			{
				DEBUGASSERT(m_bIsKnightInvalidLegCheckSum[p][nFoeBitBoardCheckSum][nKnightLeg] == TRUE || 
					m_bIsKnightInvalidLegCheckSum[p][nFoeBitBoardCheckSum][nKnightLeg] == FALSE);

				//检查是否为有效的马腿校验和
				if (m_bIsKnightInvalidLegCheckSum[p][nFoeBitBoardCheckSum][nKnightLeg] == FALSE)
				{
					continue;
				}
				
				nCapCount = 0;
				
				//左
				if (((nKnightLeg & m_nMask[0]) == 0) && (i - 2 >= 0))
				{
					//下
					if ((nFoeBitBoardCheckSum & m_nMask[2]) && (j - 1 >= 0))
					{
						nCapCount++;
					}
					//上
					if ((nFoeBitBoardCheckSum & m_nMask[4]) && (j + 1 < 10))
					{
						nCapCount++;
					}
				}
				
				//右
				if (((nKnightLeg & m_nMask[1]) == 0) && (i + 2 < 9))
				{
					//下
					if ((nFoeBitBoardCheckSum & m_nMask[3]) && (j - 1 >= 0))
					{
						nCapCount++;
					}
					//上
					if ((nFoeBitBoardCheckSum & m_nMask[5]) && (j + 1 < 10))
					{
						nCapCount++;
					}
				}
				
				//下
				if (((nKnightLeg & m_nMask[2]) == 0) && (j - 2 >= 0))
				{
					//左
					if ((nFoeBitBoardCheckSum & m_nMask[0]) && (i - 1 >= 0))
					{
						nCapCount++;
					}
					//右
					if ((nFoeBitBoardCheckSum & m_nMask[1]) && (i + 1 < 9))
					{
						nCapCount++;
					}
				}
				
				//上
				if (((nKnightLeg & m_nMask[3]) == 0) && j + 2 < 10)
				{
					//左
					if ((nFoeBitBoardCheckSum & m_nMask[6]) && (i - 1 >= 0))
					{
						nCapCount++;
					}
					//右
					if ((nFoeBitBoardCheckSum & m_nMask[7]) && (i + 1 < 9))
					{
						nCapCount++;
					}
				}
				DEBUGASSERT(nCapCount <= 8);
				
				m_pcmKnightCapPreMoves[p][nFoeBitBoardCheckSum][nKnightLeg].nCount = nCapCount;
				if (nCapCount)
				{
					TRYNEW(m_pcmKnightCapPreMoves[p][nFoeBitBoardCheckSum][nKnightLeg].pMoveList = new CHESSMOVE[nCapCount];);
					
#ifdef MEMCALC

					nKnightMovesSize += m_nBytes[nCapCount];
					nKnightCapMovesSize += m_nBytes[nCapCount];
					
#endif
					
					nCapCount = 0;
					
					//左
					if (((nKnightLeg & m_nMask[0]) == 0) && (i - 2 >= 0))
					{
						//下
						if ((nFoeBitBoardCheckSum & m_nMask[2]) && (j - 1 >= 0))
						{
//							m_pcmKnightCapPreMoves[p][nFoeBitBoardCheckSum][nKnightLeg].pMoveList[nCapCount].nFrom = p;
//							m_pcmKnightCapPreMoves[p][nFoeBitBoardCheckSum][nKnightLeg].pMoveList[nCapCount].nTo = p - 21;
//			
//							m_pcmKnightCapPreMoves[p][nFoeBitBoardCheckSum][nKnightLeg].pMoveList[nCapCount].nKind = EAT;
							
							FILLCHESSMOVE(m_pcmKnightCapPreMoves[p][nFoeBitBoardCheckSum][nKnightLeg].pMoveList[nCapCount], 
								p, p - 21, BV_KNIGHT_OPEN_GAME, NOCHESS, EAT);

							nCapCount++;
						}
						
						//上
						if ((nFoeBitBoardCheckSum & m_nMask[4]) && (j + 1 < 10))
						{
//							m_pcmKnightCapPreMoves[p][nFoeBitBoardCheckSum][nKnightLeg].pMoveList[nCapCount].nFrom = p;
//							m_pcmKnightCapPreMoves[p][nFoeBitBoardCheckSum][nKnightLeg].pMoveList[nCapCount].nTo = p - 19;
//
//							m_pcmKnightCapPreMoves[p][nFoeBitBoardCheckSum][nKnightLeg].pMoveList[nCapCount].nKind = EAT;
							
							FILLCHESSMOVE(m_pcmKnightCapPreMoves[p][nFoeBitBoardCheckSum][nKnightLeg].pMoveList[nCapCount], 
								p, p - 19, BV_KNIGHT_OPEN_GAME, NOCHESS, EAT);

							nCapCount++;
						}
					}
					
					//右
					if (((nKnightLeg & m_nMask[1]) == 0) && (i + 2 < 9))
					{
						//下
						if ((nFoeBitBoardCheckSum & m_nMask[3]) && (j - 1 >= 0))
						{
//							m_pcmKnightCapPreMoves[p][nFoeBitBoardCheckSum][nKnightLeg].pMoveList[nCapCount].nFrom = p;
//							m_pcmKnightCapPreMoves[p][nFoeBitBoardCheckSum][nKnightLeg].pMoveList[nCapCount].nTo = p + 19;
//
//							m_pcmKnightCapPreMoves[p][nFoeBitBoardCheckSum][nKnightLeg].pMoveList[nCapCount].nKind = EAT;							
							
							FILLCHESSMOVE(m_pcmKnightCapPreMoves[p][nFoeBitBoardCheckSum][nKnightLeg].pMoveList[nCapCount], 
								p, p + 19, BV_KNIGHT_OPEN_GAME, NOCHESS, EAT);

							nCapCount++;
						}
						
						//上
						if ((nFoeBitBoardCheckSum & m_nMask[5]) && (j + 1 < 10))
						{
//							m_pcmKnightCapPreMoves[p][nFoeBitBoardCheckSum][nKnightLeg].pMoveList[nCapCount].nFrom = p;
//							m_pcmKnightCapPreMoves[p][nFoeBitBoardCheckSum][nKnightLeg].pMoveList[nCapCount].nTo = p + 21;
//
//							m_pcmKnightCapPreMoves[p][nFoeBitBoardCheckSum][nKnightLeg].pMoveList[nCapCount].nKind = EAT;							
							
							FILLCHESSMOVE(m_pcmKnightCapPreMoves[p][nFoeBitBoardCheckSum][nKnightLeg].pMoveList[nCapCount], 
								p, p + 21, BV_KNIGHT_OPEN_GAME, NOCHESS, EAT);

							nCapCount++;
						}
					}
					
					//下
					if (((nKnightLeg & m_nMask[2]) == 0) && (j - 2 >= 0))
					{
						//左
						if ((nFoeBitBoardCheckSum & m_nMask[0]) && (i - 1 >= 0))
						{
//							m_pcmKnightCapPreMoves[p][nFoeBitBoardCheckSum][nKnightLeg].pMoveList[nCapCount].nFrom = p;
//							m_pcmKnightCapPreMoves[p][nFoeBitBoardCheckSum][nKnightLeg].pMoveList[nCapCount].nTo = p - 12;
//
//							m_pcmKnightCapPreMoves[p][nFoeBitBoardCheckSum][nKnightLeg].pMoveList[nCapCount].nKind = EAT;							
							
							FILLCHESSMOVE(m_pcmKnightCapPreMoves[p][nFoeBitBoardCheckSum][nKnightLeg].pMoveList[nCapCount], 
								p, p - 12, BV_KNIGHT_OPEN_GAME, NOCHESS, EAT);

							nCapCount++;
						}
						
						//右
						if ((nFoeBitBoardCheckSum & m_nMask[1]) && (i + 1 < 9))
						{
//							m_pcmKnightCapPreMoves[p][nFoeBitBoardCheckSum][nKnightLeg].pMoveList[nCapCount].nFrom = p;
//							m_pcmKnightCapPreMoves[p][nFoeBitBoardCheckSum][nKnightLeg].pMoveList[nCapCount].nTo = p + 8;
//
//							m_pcmKnightCapPreMoves[p][nFoeBitBoardCheckSum][nKnightLeg].pMoveList[nCapCount].nKind = EAT;							
							
							FILLCHESSMOVE(m_pcmKnightCapPreMoves[p][nFoeBitBoardCheckSum][nKnightLeg].pMoveList[nCapCount], 
								p, p + 8, BV_KNIGHT_OPEN_GAME, NOCHESS, EAT);

							nCapCount++;
						}
					}
					
					//上
					if (((nKnightLeg & m_nMask[3]) == 0) && j + 2 < 10)
					{
						//左
						if ((nFoeBitBoardCheckSum & m_nMask[6]) && (i - 1 >= 0))
						{
//							m_pcmKnightCapPreMoves[p][nFoeBitBoardCheckSum][nKnightLeg].pMoveList[nCapCount].nFrom = p;
//							m_pcmKnightCapPreMoves[p][nFoeBitBoardCheckSum][nKnightLeg].pMoveList[nCapCount].nTo = p - 8;
//
//							m_pcmKnightCapPreMoves[p][nFoeBitBoardCheckSum][nKnightLeg].pMoveList[nCapCount].nKind = EAT;							
							
							FILLCHESSMOVE(m_pcmKnightCapPreMoves[p][nFoeBitBoardCheckSum][nKnightLeg].pMoveList[nCapCount], 
								p, p - 8, BV_KNIGHT_OPEN_GAME, NOCHESS, EAT);

							nCapCount++;
						}
						
						//右
						if ((nFoeBitBoardCheckSum & m_nMask[7]) && (i + 1 < 9))
						{
//							m_pcmKnightCapPreMoves[p][nFoeBitBoardCheckSum][nKnightLeg].pMoveList[nCapCount].nFrom = p;
//							m_pcmKnightCapPreMoves[p][nFoeBitBoardCheckSum][nKnightLeg].pMoveList[nCapCount].nTo = p + 12;
//
//							m_pcmKnightCapPreMoves[p][nFoeBitBoardCheckSum][nKnightLeg].pMoveList[nCapCount].nKind = EAT;							
							
							FILLCHESSMOVE(m_pcmKnightCapPreMoves[p][nFoeBitBoardCheckSum][nKnightLeg].pMoveList[nCapCount], 
								p, p + 12, BV_KNIGHT_OPEN_GAME, NOCHESS, EAT);

							nCapCount++;
						}
					}
				}
			}
		}
	}

#ifdef MEMCALC

	cout << "马所有走法内存:" << nKnightMovesSize * 1.0 / (1 << 20) << "M" << endl;
	cout << "马所有吃子走法内存:" << nKnightCapMovesSize * 1.0 / (1 << 20) << "M" << endl;

#endif
}

/*******************************************************************************************************
函数名 : GenCannonPreMoves
描  述 : 炮走法预产生
参  数 : 无
返回值 : 无
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年8月7日 12:38:01
*******************************************************************************************************/
FSVOID CSearchEngine::GenCannonPreMoves()
{
    FSINT p;
	FSINT m, n;
	FSINT nAllCount;
	FSINT nNoCapCount;
	FSINT nCapCount;
	FSINT p1, p2;
	FSINT i, j, k;
	FSINT nFlag;
	FSINT nCount1, nCount2;
	FSINT nBitFile, nBitRank;//位行,位列
	FSINT nFoeBitFile, nFoeBitRank;//对方子位行,位列
	FSINT nFoeFileCheckNum, nFoeRankCheckNum;//对方子位行和位列校验序号

#ifdef MEMCALC

	FSINT nCannonMovesSize = 0;
	FSINT nCannonCapMovesSize = 0;

#endif

	for (p = 0; p < 90; p++)
	{
		i = m_CGN2Pos[p].x;
		j = m_CGN2Pos[p].y;
		
		//行走法
		for (k = 0; k < CANNONFILECHECKSUMMAXCNT; k++)
		{
			//必须!!!
			if ((nBitFile = m_nCannonFileRankCheckSum[i][k]) == INVALIDCHECKSUM)
			{
				break;
			}
			
			nCount1 = 0;
			nFlag = 0;
			p1 = -1;
			for (m = i - 1; m >= 0; m--)
			{
				if (m_nMask[m] & nBitFile)
				{
					//if语句体不能颠倒
					if (nFlag == 1)
					{
						p1 = m;
						break;
					}
					else
					{
						nFlag = TRUE;
					}
				}
				
				//必须!!!
				if (nFlag == 0)
				{
					nCount1++;
				}
			}
			
			nCount2 = 0;
			nFlag = 0;
			p2 = -1;
			for (n = i + 1; n < 9; n++)
			{
				if (m_nMask[n] & nBitFile)
				{
					if (nFlag == 1)
					{
						p2 = n;
						break;
					}
					else
					{
						nFlag = TRUE;
					}
				}
				
				//必须!!!
				if (nFlag == 0)
				{
					nCount2++;
				}
			}
			
			nNoCapCount = nCount1 + nCount2;

			// 行不吃子走法[2005/10/23 15:34:40]
			m_pcmCannonFileNoCapPreMoves[p][k].nCount = nNoCapCount;
			if (nNoCapCount > 0)
			{
				DEBUGASSERT(nNoCapCount <= MAXCANNONFILENOCAPMOVECNT);
				TRYNEW(m_pcmCannonFileNoCapPreMoves[p][k].pMoveList = 
					new CHESSMOVE[nNoCapCount];);
				
				//行不吃子走法
				for (m = i - 1; m > i - 1 - nCount1; m--)
				{
					DEBUGASSERT(i - 1 - m >= 0  && i - 1 - m < nNoCapCount);
					
					FILLCHESSMOVE(m_pcmCannonFileNoCapPreMoves[p][k].pMoveList[i - 1 - m], 
						p, m_nPos2CGN[m][j], BV_CANNON_OPEN_GAME, NOCHESS, NORMAL);
				}
				for (n = i + 1; n < i + 1 + nCount2; n++)
				{
					DEBUGASSERT(n - i - 1 + nCount1 >= 0 && n - i - 1 + nCount1 < nNoCapCount);
					
					FILLCHESSMOVE(m_pcmCannonFileNoCapPreMoves[p][k].pMoveList[n - i - 1 + nCount1], 
						p, m_nPos2CGN[n][j], BV_CANNON_OPEN_GAME, NOCHESS, NORMAL);
				}
			}

#ifdef MEMCALC

			nCannonCapMovesSize += m_nBytes[nNoCapCount];

#endif

			for (nFoeFileCheckNum = 0; nFoeFileCheckNum < 4; nFoeFileCheckNum++)
			{
				//必须!!!
				if ((nFoeBitFile = m_nCannonFoeFileRankCheckSum[i][k][nFoeFileCheckNum]) == INVALIDCHECKSUM)
				{
					break;
				}

				nAllCount = nNoCapCount;

				if (p1 != -1 && (m_nMask[p1] & nFoeBitFile))//说明p1位对方有子
				{
					DEBUGASSERT(nCount1 < i);
					nAllCount++;
				}
				if (p2!=-1 && (m_nMask[p2] & nFoeBitFile))//说明p2位对方有子
				{
					nAllCount++;
				}
				DEBUGASSERT(nAllCount < 9);
				
				nCapCount = nAllCount - nNoCapCount;
				m_pcmCannonFilePreMoves[p][k][nFoeFileCheckNum].nCount = nAllCount;	
				m_pcmCannonFileCapPreMoves[p][k][nFoeFileCheckNum].nCount = nCapCount;
				if (nAllCount)
				{
					TRYNEW(m_pcmCannonFilePreMoves[p][k][nFoeFileCheckNum].pMoveList = new CHESSMOVE[nAllCount];);

					// 行不吃子走法[2005/10/23 15:34:40]
					if (nNoCapCount > 0)
					{
						//行不吃子走法
						for (m = i - 1; m > i - 1 - nCount1; m--)
						{
							DEBUGASSERT(i - 1 - m >= 0  && i - 1 - m < nAllCount);
							
							FILLCHESSMOVE(m_pcmCannonFilePreMoves[p][k][nFoeFileCheckNum].pMoveList[i - 1 - m], 
								p, m_nPos2CGN[m][j], BV_CANNON_OPEN_GAME, NOCHESS, NORMAL);

							FILLCHESSMOVE(m_pcmCannonFileNoCapPreMoves[p][k].pMoveList[i - 1 - m], 
								p, m_nPos2CGN[m][j], BV_CANNON_OPEN_GAME, NOCHESS, NORMAL);
						}
						for (n = i + 1; n < i + 1 + nCount2; n++)
						{
							DEBUGASSERT(n - i - 1 + nCount1 >= 0 && n - i - 1 + nCount1 < nAllCount);
							
							FILLCHESSMOVE(m_pcmCannonFilePreMoves[p][k][nFoeFileCheckNum].pMoveList[n - i - 1 + nCount1], 
								p, m_nPos2CGN[n][j], BV_CANNON_OPEN_GAME, NOCHESS, NORMAL);

							FILLCHESSMOVE(m_pcmCannonFileNoCapPreMoves[p][k].pMoveList[n - i - 1 + nCount1], 
								p, m_nPos2CGN[n][j], BV_CANNON_OPEN_GAME, NOCHESS, NORMAL);
						}
					}

					// 行吃子走法[2005/10/23 15:33:06]
					if (nCapCount > 0)
					{
						DEBUGASSERT(nCapCount <= MAXCANNONFILECAPMOVECNT);
						TRYNEW(m_pcmCannonFileCapPreMoves[p][k][nFoeFileCheckNum].pMoveList = 
							new CHESSMOVE[nCapCount];);

						if (p1 != -1 && (m_nMask[p1] & nFoeBitFile))//说明p1为对方有子
						{
							FILLCHESSMOVE(m_pcmCannonFilePreMoves[p][k][nFoeFileCheckNum].pMoveList[nCount1 + nCount2], 
								p, m_nPos2CGN[p1][j], BV_CANNON_OPEN_GAME, NOCHESS, EAT);
							
							FILLCHESSMOVE(m_pcmCannonFileCapPreMoves[p][k][nFoeFileCheckNum].pMoveList[0], 
								p, m_nPos2CGN[p1][j], BV_CANNON_OPEN_GAME, NOCHESS, EAT);
						}
						if (p2!=-1 && (m_nMask[p2] & nFoeBitFile))//说明p2位对方有子
						{
							FILLCHESSMOVE(m_pcmCannonFilePreMoves[p][k][nFoeFileCheckNum].pMoveList[nAllCount - 1], 
								p, m_nPos2CGN[p2][j], BV_CANNON_OPEN_GAME, NOCHESS, EAT);
							
							FILLCHESSMOVE(m_pcmCannonFileCapPreMoves[p][k][nFoeFileCheckNum].pMoveList[nCapCount - 1], 
								p, m_nPos2CGN[p2][j], BV_CANNON_OPEN_GAME, NOCHESS, EAT);
						}
					}
#ifdef MEMCALC
					
					nCannonMovesSize += m_nBytes[nAllCount];
					
#endif
				}
			}
		}

		//列走法
		for (k = 0; k < CANNONRANKCHECKSUMMAXCNT; k ++)
		{
			//必须!!!
			if ((nBitRank = m_nCannonFileRankCheckSum[j][k]) == INVALIDCHECKSUM)
			{
				break;
			}
			
			nCount1 = 0;
			nFlag = 0;
			p1=-1;
			for (m = j - 1; m >= 0; m--)
			{
				if (m_nMask[m] & nBitRank)
				{
					if (nFlag == 1)
					{
						p1 = m;
						break;
					}
					else
					{
						nFlag = TRUE;
					}
				}
				
				//必须!!!
				if (nFlag == 0)
				{
					nCount1++;
				}
			}
			
			nCount2 = 0;
			nFlag = 0;
			p2 = -1;
			for (n = j + 1; n < 10; n++)
			{
				if (m_nMask[n] & nBitRank)
				{
					if (nFlag == 1)
					{
						p2 = n;
						break;
					}
					else
					{
						nFlag = TRUE;
					}
				}
				
				//必须!!!
				if (nFlag == 0)
				{
					nCount2++;
				}
			}
			
			nNoCapCount = nCount1 + nCount2;

			// 列不吃子走法[2005/10/23 15:39:36]
			m_pcmCannonRankNoCapPreMoves[p][k].nCount = nNoCapCount;
			if (nNoCapCount > 0)
			{
				DEBUGASSERT(nNoCapCount <= MAXCANNONRANKNOCAPMOVECNT);

				TRYNEW(m_pcmCannonRankNoCapPreMoves[p][k].pMoveList = 
					new CHESSMOVE[nNoCapCount];);
				
				for (m = j - 1; m > j - 1 - nCount1; m--)
				{
					DEBUGASSERT(j - 1 - m >= 0 && j - 1 - m < nNoCapCount);
								
					FILLCHESSMOVE(m_pcmCannonRankNoCapPreMoves[p][k].pMoveList[j - 1 - m], 
						p, m_nPos2CGN[i][m], BV_CANNON_OPEN_GAME, NOCHESS, NORMAL);
				}
				
				for (n = j + 1; n < j + 1 + nCount2; n++)
				{
					DEBUGASSERT(n - j - 1 + nCount1 >= 0 && n - j - 1 + nCount1 < nNoCapCount);
		
					FILLCHESSMOVE(m_pcmCannonRankNoCapPreMoves[p][k].pMoveList[n - j - 1 + nCount1], 
						p, m_nPos2CGN[i][n], BV_CANNON_OPEN_GAME, NOCHESS, NORMAL);
				}
			}

#ifdef MEMCALC

			nCannonCapMovesSize += m_nBytes[nCapCount];

#endif

			for (nFoeRankCheckNum = 0; nFoeRankCheckNum < 4; nFoeRankCheckNum++)
			{
				//必须!!!
				if ((nFoeBitRank = m_nCannonFoeFileRankCheckSum[j][k][nFoeRankCheckNum]) == INVALIDCHECKSUM)
				{
					break;
				}
				
				nAllCount = nNoCapCount;

				if (p1 != -1 && (m_nMask[p1] & nFoeBitRank))//说明p1为对方有子
				{
					nAllCount++;
				}
				if (p2 != -1 && (m_nMask[p2] & nFoeBitRank))//说明p2为对方有子
				{
					nAllCount++;
				}
				DEBUGASSERT(nAllCount < 10);
				
				nCapCount = nAllCount - nNoCapCount;
				m_pcmCannonRankPreMoves[p][k][nFoeRankCheckNum].nCount = nAllCount;
				m_pcmCannonRankCapPreMoves[p][k][nFoeRankCheckNum].nCount = nCapCount;
				if (nAllCount > 0)
				{
					TRYNEW(m_pcmCannonRankPreMoves[p][k][nFoeRankCheckNum].pMoveList = 
						new CHESSMOVE[nAllCount];);

					// 列不吃子走法[2005/10/23 15:39:36]
					if (nNoCapCount > 0)
					{
						for (m = j - 1; m > j - 1 - nCount1; m--)
						{
							DEBUGASSERT(j - 1 - m >= 0 && j - 1 - m < nAllCount);
							
							FILLCHESSMOVE(m_pcmCannonRankPreMoves[p][k][nFoeRankCheckNum].pMoveList[j - 1 - m], 
								p, m_nPos2CGN[i][m], BV_CANNON_OPEN_GAME, NOCHESS, NORMAL);

							FILLCHESSMOVE(m_pcmCannonRankNoCapPreMoves[p][k].pMoveList[j - 1 - m], 
								p, m_nPos2CGN[i][m], BV_CANNON_OPEN_GAME, NOCHESS, NORMAL);
						}
						
						for (n = j + 1; n < j + 1 + nCount2; n++)
						{
							DEBUGASSERT(n - j - 1 + nCount1 >= 0 && n - j - 1 + nCount1 < nAllCount);
							
							FILLCHESSMOVE(m_pcmCannonRankPreMoves[p][k][nFoeRankCheckNum].pMoveList[n - j - 1 + nCount1], 
								p, m_nPos2CGN[i][n], BV_CANNON_OPEN_GAME, NOCHESS, NORMAL);

							FILLCHESSMOVE(m_pcmCannonRankNoCapPreMoves[p][k].pMoveList[n - j - 1 + nCount1], 
								p, m_nPos2CGN[i][n], BV_CANNON_OPEN_GAME, NOCHESS, NORMAL);
						}
					}

					// 列吃子走法[2005/10/23 15:38:19]
					if (nCapCount > 0)
					{
						DEBUGASSERT(nCapCount <= MAXCANNONRANKCAPMOVECNT);
						TRYNEW(m_pcmCannonRankCapPreMoves[p][k][nFoeRankCheckNum].pMoveList = 
							new CHESSMOVE[nCapCount];);

						if (p1 != -1 && (m_nMask[p1] & nFoeBitRank))//说明p1位对方有子
						{
							FILLCHESSMOVE(m_pcmCannonRankPreMoves[p][k][nFoeRankCheckNum].pMoveList[nCount1 + nCount2], 
								p, m_nPos2CGN[i][p1], BV_CANNON_OPEN_GAME, NOCHESS, EAT);
							
							FILLCHESSMOVE(m_pcmCannonRankCapPreMoves[p][k][nFoeRankCheckNum].pMoveList[0], 
								p, m_nPos2CGN[i][p1], BV_CANNON_OPEN_GAME, NOCHESS, EAT);
						}
						if (p2 != -1 && (m_nMask[p2] & nFoeBitRank))//说明p2位对方有子
						{
							FILLCHESSMOVE(m_pcmCannonRankPreMoves[p][k][nFoeRankCheckNum].pMoveList[nAllCount - 1], 
								p, m_nPos2CGN[i][p2], BV_CANNON_OPEN_GAME, NOCHESS, EAT);
							
							FILLCHESSMOVE(m_pcmCannonRankCapPreMoves[p][k][nFoeRankCheckNum].pMoveList[nCapCount - 1], 
								p, m_nPos2CGN[i][p2], BV_CANNON_OPEN_GAME, NOCHESS, EAT);
						}
					}
#ifdef MEMCALC

					nCannonMovesSize += m_nBytes[nAllCount];
					
#endif
				}
			}
		}
	}

#ifdef MEMCALC

	cout << "炮所有走法内存:" << nCannonMovesSize * 1.0 / (1 << 20) << "M" << endl;
	cout << "炮所有吃子走法内存:" << (nCannonMovesSize - nCannonCapMovesSize) * 1.0 / (1 << 20) << "M" << endl;

#endif
}

/*******************************************************************************************************
函数名 : GenKingPreMoves
描  述 : 将走法预产生
参  数 : 无
返回值 : 无
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年8月7日 12:38:17
*******************************************************************************************************/
FSVOID CSearchEngine::GenKingPreMoves()
{
    FSINT i, j;
    FSINT nCount;
	int nNoCapCount;
	FSINT nCapCount;
    FSINT nAroundCheckSum;
	FSINT nPosCheckNum;
	FSINT nPosCheckSum;
	FSINT nFoeAroundCheckSum;

#ifdef MEMCALC

	FSINT nKingMovesSize = 0;
	FSINT nKingCapMovesSize = 0;

#endif

	//将位置校验序号
	for (nPosCheckNum = 0; nPosCheckNum < 18; nPosCheckNum++)
	{
		nPosCheckSum = m_nKingPosCheckSum[nPosCheckNum];
		i = m_CGN2Pos[nPosCheckSum].x;
		j = m_CGN2Pos[nPosCheckSum].y;

		//将周围情况校验和
		for (nAroundCheckSum = 0;nAroundCheckSum<16;nAroundCheckSum++)
		{
			//将周围对方情况校验和
			for (nFoeAroundCheckSum = 0; nFoeAroundCheckSum < 16; nFoeAroundCheckSum++)
			{
                nCount = 0;

                //左
                if (i>=4)
                {
                    if (nAroundCheckSum & m_nMask[0])//左边有子
                    {
                        if (nFoeAroundCheckSum & m_nMask[0])//左边为对方子
						{
                            nCount++;
						}
                    }
                    else
					{
                        nCount++;
					}
                }
                //右
                if (i <= 4)
                {
                    if (nAroundCheckSum & m_nMask[1])//右边有子
                    {
                        if (nFoeAroundCheckSum & m_nMask[1])//右边为对方子
						{
                            nCount++;
						}
                    }
                    else
					{
                        nCount++;
					}
                }
				//下
                if ((j>=1 && j<=2) || (j>=8 && j<=9))
                {
                    if (nAroundCheckSum & m_nMask[2])//下边有子
                    {
                        if (nFoeAroundCheckSum & m_nMask[2])//下边为对方子
						{
                            nCount++;
						}
                    }
                    else
					{
                        nCount++;
					}
                }
                //上
                if ((j>= 0 && j<=1) || (j>=7 && j<=8))
                {
                    if (nAroundCheckSum & m_nMask[3])//上边有子
                    {
                        if (nFoeAroundCheckSum & m_nMask[3])//上边为对方子
						{
                            nCount++;
						}
                    }
                    else
					{
                        nCount++;
					}
                }
				DEBUGASSERT(nCount >= 0 && nCount <= 4);
                
                m_pcmKingPreMoves[nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].nCount = nCount;
                if (nCount)
                {
					TRYNEW(m_pcmKingPreMoves[nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList = new CHESSMOVE[nCount];);

#ifdef MEMCALC

					nKingMovesSize += m_nBytes[nCount];

#endif

                    nCount = 0;

                    //左
                    if (i>=4)
                    {
                        if (nAroundCheckSum & m_nMask[0])//左边有子
                        {
                            if (nFoeAroundCheckSum & m_nMask[0])//左边为对方子
                            {
								FILLCHESSMOVE(m_pcmKingPreMoves[nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
									nPosCheckSum, nPosCheckSum - 10, BV_KING, NOCHESS, EAT);

                                nCount++;
                            }
                        }
                        else
                        {
							FILLCHESSMOVE(m_pcmKingPreMoves[nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
								nPosCheckSum, nPosCheckSum - 10, BV_KING, NOCHESS, NORMAL);

                            nCount++;
                        }
                    }
                    //右
                    if (i <= 4)
                    {
                        if (nAroundCheckSum & m_nMask[1])//右边有子
                        {
                            if (nFoeAroundCheckSum & m_nMask[1])//右边为对方子
                            {
								FILLCHESSMOVE(m_pcmKingPreMoves[nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
									nPosCheckSum, nPosCheckSum + 10, BV_KING, NOCHESS, EAT);

                                nCount++;
                            }
                        }
                        else
                        {
							FILLCHESSMOVE(m_pcmKingPreMoves[nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
								nPosCheckSum, nPosCheckSum + 10, BV_KING, NOCHESS, NORMAL);

                            nCount++;
                        }
                    }
					//下
                    if ((j>=1 && j<=2) || (j>=8 && j<=9))
                    {
                        if (nAroundCheckSum & m_nMask[2])//下边有子
                        {
                            if (nFoeAroundCheckSum & m_nMask[2])//下边为对方子
                            {
								FILLCHESSMOVE(m_pcmKingPreMoves[nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
									nPosCheckSum, nPosCheckSum - 1, BV_KING, NOCHESS, EAT);

                                nCount++;
                            }
                        }
                        else
                        {
							FILLCHESSMOVE(m_pcmKingPreMoves[nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
									nPosCheckSum, nPosCheckSum - 1, BV_KING, NOCHESS, NORMAL);

                            nCount++;
                        }
                    }
                    //上
                    if ((j>= 0 && j<=1) || (j>=7 && j<=8))
                    {
                        if (nAroundCheckSum & m_nMask[3])//上边有子
                        {
                            if (nFoeAroundCheckSum & m_nMask[3])//上边为对方子
                            {
								FILLCHESSMOVE(m_pcmKingPreMoves[nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
									nPosCheckSum, nPosCheckSum + 1, BV_KING, NOCHESS, EAT);

                                nCount++;
                            }
                        }
                        else
                        {
							FILLCHESSMOVE(m_pcmKingPreMoves[nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
									nPosCheckSum, nPosCheckSum + 1, BV_KING, NOCHESS, NORMAL);

                            nCount++;
                        }
                    }
                }
			}
		}
	}

	//将位置校验序号
	for (nPosCheckNum = 0; nPosCheckNum < 18; nPosCheckNum++)
	{
		nPosCheckSum = m_nKingPosCheckSum[nPosCheckNum];
		i = m_CGN2Pos[nPosCheckSum].x;
		j = m_CGN2Pos[nPosCheckSum].y;
		
		//不吃子走法
		//将周围情况校验和
		for (nAroundCheckSum = 0; nAroundCheckSum < 16; nAroundCheckSum++)
		{
			nNoCapCount = 0;
			
			//左,左边无子
			if (i >= 4 && (nAroundCheckSum & m_nMask[0]) == 0)
			{
				nNoCapCount++;
			}
			//右,右边无子
			if (i <= 4 && (nAroundCheckSum & m_nMask[1]) == 0)
			{
				nNoCapCount++;
			}
			//下,下边无子
			if (((j>=1 && j<=2) || (j>=8 && j<=9)) && (nAroundCheckSum & m_nMask[2]) == 0)
			{
				nNoCapCount++;
			}
			//上,上边无子
			if (((j>= 0 && j<=1) || (j>=7 && j<=8)) && (nAroundCheckSum & m_nMask[3]) == 0)
			{
				nNoCapCount++;
			}
			DEBUGASSERT(nNoCapCount <= MAXKINGNOCAPMOVECNT);
			
			m_pcmKingNoCapPreMoves[nPosCheckNum][nAroundCheckSum].nCount = nNoCapCount;
			if (nNoCapCount)
			{
				TRYNEW(m_pcmKingNoCapPreMoves[nPosCheckNum][nAroundCheckSum].pMoveList = 
					new CHESSMOVE[nNoCapCount];);

#ifdef MEMCALC
				
				nKingCapMovesSize += m_nBytes[nNoCapCount];
				
#endif
				
				nNoCapCount = 0;
				
				//左,左边无子
				if (i >= 4 && (nAroundCheckSum & m_nMask[0]) == 0)
				{
					FILLCHESSMOVE(m_pcmKingNoCapPreMoves[nPosCheckNum][nAroundCheckSum].pMoveList[nNoCapCount], 
						nPosCheckSum, nPosCheckSum - 10, BV_KING, NOCHESS, EAT);

					nNoCapCount++;
				}
				//右,右边无子
				if (i <= 4 && (nAroundCheckSum & m_nMask[1]) == 0)
				{
					FILLCHESSMOVE(m_pcmKingNoCapPreMoves[nPosCheckNum][nAroundCheckSum].pMoveList[nNoCapCount], 
						nPosCheckSum, nPosCheckSum + 10, BV_KING, NOCHESS, EAT);

					nNoCapCount++;
				}
				//下,下边无子
				if (((j>=1 && j<=2) || (j>=8 && j<=9)) && (nAroundCheckSum & m_nMask[2]) == 0)
				{
					FILLCHESSMOVE(m_pcmKingNoCapPreMoves[nPosCheckNum][nAroundCheckSum].pMoveList[nNoCapCount], 
						nPosCheckSum, nPosCheckSum - 1, BV_KING, NOCHESS, EAT);

					nNoCapCount++;
				}
				//上,上边无子
				if (((j>= 0 && j<=1) || (j>=7 && j<=8)) && (nAroundCheckSum & m_nMask[3]) == 0)
				{
					FILLCHESSMOVE(m_pcmKingNoCapPreMoves[nPosCheckNum][nAroundCheckSum].pMoveList[nNoCapCount], 
						nPosCheckSum, nPosCheckSum + 1, BV_KING, NOCHESS, EAT);

					nNoCapCount++;
				}
			}
		}

		//吃子走法
		//将周围对方情况校验和
		for (nFoeAroundCheckSum = 0; nFoeAroundCheckSum < 16; nFoeAroundCheckSum++)
		{
			nCapCount = 0;
			
			//左,左边为对方子
			if (i >= 4 && (nFoeAroundCheckSum & m_nMask[0]))
			{
				nCapCount++;
			}
			//右,右边为对方子
			if (i <= 4 && (nFoeAroundCheckSum & m_nMask[1]))
			{
				nCapCount++;
			}
			//下,下边为对方子
			if (((j>=1 && j<=2) || (j>=8 && j<=9)) && (nFoeAroundCheckSum & m_nMask[2]))
			{
				nCapCount++;
			}
			//上,上边为对方子
			if (((j>= 0 && j<=1) || (j>=7 && j<=8)) && (nFoeAroundCheckSum & m_nMask[3]))
			{
				nCapCount++;
			}
			DEBUGASSERT(nCapCount <= 4);
			
			m_pcmKingCapPreMoves[nPosCheckNum][nFoeAroundCheckSum].nCount = nCapCount;
			if (nCapCount)
			{
				TRYNEW(m_pcmKingCapPreMoves[nPosCheckNum][nFoeAroundCheckSum].pMoveList = new CHESSMOVE[nCapCount];);

#ifdef MEMCALC
				
				nKingCapMovesSize += m_nBytes[nCapCount];
				
#endif
				
				nCapCount = 0;
				
				//左,左边为对方子
				if (i >= 4 && (nFoeAroundCheckSum & m_nMask[0]))
				{
					FILLCHESSMOVE(m_pcmKingCapPreMoves[nPosCheckNum][nFoeAroundCheckSum].pMoveList[nCapCount], 
						nPosCheckSum, nPosCheckSum - 10, BV_KING, NOCHESS, EAT);

					nCapCount++;
				}
				//右,右边为对方子
				if (i <= 4 && (nFoeAroundCheckSum & m_nMask[1]))
				{
					FILLCHESSMOVE(m_pcmKingCapPreMoves[nPosCheckNum][nFoeAroundCheckSum].pMoveList[nCapCount], 
						nPosCheckSum, nPosCheckSum + 10, BV_KING, NOCHESS, EAT);

					nCapCount++;
				}
				//下,下边为对方子
				if (((j>=1 && j<=2) || (j>=8 && j<=9)) && (nFoeAroundCheckSum & m_nMask[2]))
				{
					FILLCHESSMOVE(m_pcmKingCapPreMoves[nPosCheckNum][nFoeAroundCheckSum].pMoveList[nCapCount], 
						nPosCheckSum, nPosCheckSum - 1, BV_KING, NOCHESS, EAT);

					nCapCount++;
				}
				//上,上边为对方子
				if (((j>= 0 && j<=1) || (j>=7 && j<=8)) && (nFoeAroundCheckSum & m_nMask[3]))
				{
					FILLCHESSMOVE(m_pcmKingCapPreMoves[nPosCheckNum][nFoeAroundCheckSum].pMoveList[nCapCount], 
						nPosCheckSum, nPosCheckSum + 1, BV_KING, NOCHESS, EAT);

					nCapCount++;
				}
			}
		}
	}

#ifdef MEMCALC

	cout << "将所有走法内存:" << nKingMovesSize * 1.0 / (1 << 20) << "M" << endl;
	cout << "将所有吃子走法内存:" << nKingCapMovesSize * 1.0 / (1 << 20) << "M" << endl;

#endif
}

/*******************************************************************************************************
函数名 : GenAdvisorPreMoves
描  述 : 士的走法预产生
参  数 : 无
返回值 : 无
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年8月7日 12:38:36
*******************************************************************************************************/
FSVOID CSearchEngine::GenAdvisorPreMoves()
{
    FSINT i, j;
    FSINT nCount;
	FSINT nNoCapCount;
	FSINT nCapCount;
    FSINT nAroundCheckSum;
    FSINT nPosCheckNum;
    FSINT nPosCheckSum;
    FSINT nFoeAroundCheckSum;

#ifdef MEMCALC

	FSINT nAdvisorMovesSize = 0;
	FSINT nAdvisorCapMovesSize = 0;

#endif

	//士位置校验序号
    for (nPosCheckNum = 0; nPosCheckNum < 10; nPosCheckNum++)
    {
		nPosCheckSum = m_nAdvisorPosCheckSum[nPosCheckNum];
		i = m_CGN2Pos[nPosCheckSum].x;
		j = m_CGN2Pos[nPosCheckSum].y;

		//士周围对方子情况校验和
		for (nFoeAroundCheckSum = 0; nFoeAroundCheckSum < 16; nFoeAroundCheckSum++)
		{
			//不吃子走法
			//士周围情况校验和
			for (nAroundCheckSum = 0;nAroundCheckSum<16;nAroundCheckSum++)
			{
                nCount = 0;
                
                //左上角
                if (nPosCheckSum==50 || nPosCheckSum==41 || nPosCheckSum==57 || nPosCheckSum==48)
                {
                    if (nAroundCheckSum & m_nMask[2])//左上角有子
                    {
                        if (nFoeAroundCheckSum & m_nMask[2])//左上角为对方子
						{
                            nCount++;
						}
                    }
                    else
					{
                        nCount++;
					}
                }
                //右上角
                if (nPosCheckSum == 30 || nPosCheckSum == 41 || 
					nPosCheckSum == 37 || nPosCheckSum == 48)
                {
                    if (nAroundCheckSum & m_nMask[3])//右上角有子
                    {
                        if (nFoeAroundCheckSum & m_nMask[3])//右上角为对方子
						{
                            nCount++;
						}
                    }
                    else
					{
                        nCount++;
					}
                }
                //左下角
                if (nPosCheckSum == 52 || nPosCheckSum == 41 || 
					nPosCheckSum == 59 || nPosCheckSum == 48)
                {
                    if (nAroundCheckSum & m_nMask[0])//左下角有子
                    {
                        if (nFoeAroundCheckSum & m_nMask[0])//左下角为对方子
						{
                            nCount++;
						}
                    }
                    else
					{
                        nCount++;
					}
                }
                //右下角
                if (nPosCheckSum==32 || nPosCheckSum==41 || nPosCheckSum==39 || nPosCheckSum==48)
                {
                    if (nAroundCheckSum & m_nMask[1])//右下角有子
                    {
                        if (nFoeAroundCheckSum & m_nMask[1])//右下角为对方子
						{
                            nCount++;
						}
                    }
                    else
					{
                        nCount++;
					}
                }
				DEBUGASSERT(nCount >= 0 && nCount <= 4);

                m_pcmAdvisorPreMoves[nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].nCount = nCount;
                if (nCount)
                {
					TRYNEW(m_pcmAdvisorPreMoves[nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList = new CHESSMOVE[nCount];);

#ifdef MEMCALC

					nAdvisorMovesSize += m_nBytes[nCount];

#endif

                    nCount = 0;

                    //左上角
                    if (nPosCheckSum==50 || nPosCheckSum==41 || nPosCheckSum==57 || nPosCheckSum==48)
                    {
                        if (nAroundCheckSum & m_nMask[2])//左上角有子
                        {
                            if (nFoeAroundCheckSum & m_nMask[2])//左上角为对方子
                            {
								FILLCHESSMOVE(m_pcmAdvisorPreMoves[nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
									nPosCheckSum, nPosCheckSum - 9, BV_ADVISOR, NOCHESS, EAT);

                                nCount++;
                            }
                        }
                        else
                        {
							FILLCHESSMOVE(m_pcmAdvisorPreMoves[nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
								nPosCheckSum, nPosCheckSum - 9, BV_ADVISOR, NOCHESS, NORMAL);

                            nCount++;
                        }
                    }
                    //右上角
                    if (nPosCheckSum==30 || nPosCheckSum==41 || nPosCheckSum==37 || nPosCheckSum==48)
                    {
                        if (nAroundCheckSum & m_nMask[3])//右上角有子
                        {
                            if (nFoeAroundCheckSum & m_nMask[3])//右上角为对方子
                            {
								FILLCHESSMOVE(m_pcmAdvisorPreMoves[nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
									nPosCheckSum, nPosCheckSum + 11, BV_ADVISOR, NOCHESS, EAT);

                                nCount++;
                            }
                        }
                        else
                        {
							FILLCHESSMOVE(m_pcmAdvisorPreMoves[nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
									nPosCheckSum, nPosCheckSum + 11, BV_ADVISOR, NOCHESS, NORMAL);

                            nCount++;
                        }
                    }
                    //左下角
                    if (nPosCheckSum==52 || nPosCheckSum==41 || nPosCheckSum==59 || nPosCheckSum==48)
                    {
                        if (nAroundCheckSum & m_nMask[0])//左下角有子
                        {
                            if (nFoeAroundCheckSum & m_nMask[0])//左下角为对方子
                            {
								FILLCHESSMOVE(m_pcmAdvisorPreMoves[nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
									nPosCheckSum, nPosCheckSum - 11, BV_ADVISOR, NOCHESS, EAT);

                                nCount++;
                            }
                        }
                        else
                        {
							FILLCHESSMOVE(m_pcmAdvisorPreMoves[nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
									nPosCheckSum, nPosCheckSum - 11, BV_ADVISOR, NOCHESS, NORMAL);

                            nCount++;
                        }
                    }
                    //右下角
                    if (nPosCheckSum==32 || nPosCheckSum==41 || nPosCheckSum==39 || nPosCheckSum==48)
                    {
                        if (nAroundCheckSum & m_nMask[1])//右下角有子
                        {
                            if (nFoeAroundCheckSum & m_nMask[1])//右下角为对方子
                            {
								FILLCHESSMOVE(m_pcmAdvisorPreMoves[nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
									nPosCheckSum, nPosCheckSum + 9, BV_ADVISOR, NOCHESS, EAT);

                                nCount++;
                            }
                        }
                        else
                        {
							FILLCHESSMOVE(m_pcmAdvisorPreMoves[nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
									nPosCheckSum, nPosCheckSum + 9, BV_ADVISOR, NOCHESS, NORMAL);
                          
							nCount++;
                        }
                    }
                }
            }
		}
	}

	//士位置校验序号
    for (nPosCheckNum = 0; nPosCheckNum < 10; nPosCheckNum++)
    {
		nPosCheckSum = m_nAdvisorPosCheckSum[nPosCheckNum];
		i = m_CGN2Pos[nPosCheckSum].x;
		j = m_CGN2Pos[nPosCheckSum].y;

		//不吃子走法
		//士周围对方子情况校验和
		for (nAroundCheckSum = 0; nAroundCheckSum < 16; nAroundCheckSum++)
		{
			nNoCapCount = 0;
			
			//左上角, 左上角为对方子
			if ((nPosCheckSum == 50 || nPosCheckSum == 41 || 
				nPosCheckSum == 57 || nPosCheckSum == 48) && 
				(nAroundCheckSum & m_nMask[2]) == 0)
			{
				nNoCapCount++;
			}
			//右上角, 右上角为对方子
			if ((nPosCheckSum == 30 || nPosCheckSum == 41 || 
				nPosCheckSum == 37 || nPosCheckSum == 48) && 
				(nAroundCheckSum & m_nMask[3]) == 0)
			{
				nNoCapCount++;
			}
			//左下角, 左下角为对方子
			if ((nPosCheckSum == 52 || nPosCheckSum == 41 || 
				nPosCheckSum == 59 || nPosCheckSum == 48) && 
				(nAroundCheckSum & m_nMask[0]) == 0)
			{
				nNoCapCount++;
			}
			//右下角, 右下角为对方子
			if ((nPosCheckSum == 32 || nPosCheckSum == 41 || 
				nPosCheckSum == 39 || nPosCheckSum == 48) && 
				(nAroundCheckSum & m_nMask[1]) == 0)
			{
				nNoCapCount++;
			}
			DEBUGASSERT(nNoCapCount <= 4);
			
			m_pcmAdvisorNoCapPreMoves[nPosCheckNum][nAroundCheckSum].nCount = nNoCapCount;
			if (nNoCapCount)
			{
				TRYNEW(m_pcmAdvisorNoCapPreMoves[nPosCheckNum][nAroundCheckSum].pMoveList = 
					new CHESSMOVE[nNoCapCount];);
				
#ifdef MEMCALC
				
				nAdvisorCapMovesSize += m_nBytes[nNoCapCount];
				
#endif
				
				nNoCapCount = 0;
				
				//左上角, 左上角为对方子
				if ((nPosCheckSum == 50 || nPosCheckSum == 41 || 
					nPosCheckSum == 57 || nPosCheckSum == 48) && 
					(nAroundCheckSum & m_nMask[2]) == 0)
				{
					FILLCHESSMOVE(m_pcmAdvisorNoCapPreMoves[nPosCheckNum][nAroundCheckSum].pMoveList[nNoCapCount], 
						nPosCheckSum, nPosCheckSum - 9, BV_ADVISOR, NOCHESS, EAT);

					nNoCapCount++;
				}
				//右上角, 右上角为对方子
				if ((nPosCheckSum == 30 || nPosCheckSum == 41 || 
					nPosCheckSum == 37 || nPosCheckSum == 48) && 
					(nAroundCheckSum & m_nMask[3]) == 0)
				{
					FILLCHESSMOVE(m_pcmAdvisorNoCapPreMoves[nPosCheckNum][nAroundCheckSum].pMoveList[nNoCapCount], 
						nPosCheckSum, nPosCheckSum + 11, BV_ADVISOR, NOCHESS, EAT);

					nNoCapCount++;
				}
				//左下角, 左下角为对方子
				if ((nPosCheckSum == 52 || nPosCheckSum == 41 || 
					nPosCheckSum == 59 || nPosCheckSum == 48) && 
					(nAroundCheckSum & m_nMask[0]) == 0)
				{
					FILLCHESSMOVE(m_pcmAdvisorNoCapPreMoves[nPosCheckNum][nAroundCheckSum].pMoveList[nNoCapCount], 
						nPosCheckSum, nPosCheckSum - 11, BV_ADVISOR, NOCHESS, EAT);

					nNoCapCount++;
				}
				//右下角, 右下角为对方子
				if ((nPosCheckSum == 32 || nPosCheckSum == 41 || 
					nPosCheckSum == 39 || nPosCheckSum == 48) && 
					(nAroundCheckSum & m_nMask[1]) == 0)
				{
					FILLCHESSMOVE(m_pcmAdvisorNoCapPreMoves[nPosCheckNum][nAroundCheckSum].pMoveList[nNoCapCount], 
						nPosCheckSum, nPosCheckSum + 9, BV_ADVISOR, NOCHESS, EAT);

					nNoCapCount++;
				}
			}
        }

		// 吃子走法[2005/10/23 16:18:49]
		//士周围对方子情况校验和
		for (nFoeAroundCheckSum = 0; nFoeAroundCheckSum < 16; nFoeAroundCheckSum++)
		{
			nCapCount = 0;
			
			//左上角, 左上角为对方子
			if ((nPosCheckSum == 50 || nPosCheckSum == 41 || 
				nPosCheckSum == 57 || nPosCheckSum == 48) && 
				(nFoeAroundCheckSum & m_nMask[2]))
			{
				nCapCount++;
			}
			//右上角, 右上角为对方子
			if ((nPosCheckSum == 30 || nPosCheckSum == 41 || 
				nPosCheckSum == 37 || nPosCheckSum == 48) && 
				(nFoeAroundCheckSum & m_nMask[3]))
			{
				nCapCount++;
			}
			//左下角, 左下角为对方子
			if ((nPosCheckSum == 52 || nPosCheckSum == 41 || 
				nPosCheckSum == 59 || nPosCheckSum == 48) && 
				(nFoeAroundCheckSum & m_nMask[0]))
			{
				nCapCount++;
			}
			//右下角, 右下角为对方子
			if ((nPosCheckSum == 32 || nPosCheckSum == 41 || 
				nPosCheckSum == 39 || nPosCheckSum == 48) && 
				(nFoeAroundCheckSum & m_nMask[1]))
			{
				nCapCount++;
			}
			DEBUGASSERT(nCapCount <= 4);
			
			m_pcmAdvisorCapPreMoves[nPosCheckNum][nFoeAroundCheckSum].nCount = nCapCount;
			if (nCapCount)
			{
				TRYNEW(m_pcmAdvisorCapPreMoves[nPosCheckNum][nFoeAroundCheckSum].pMoveList = new CHESSMOVE[nCapCount];);
				
#ifdef MEMCALC
				
				nAdvisorCapMovesSize += m_nBytes[nCapCount];
				
#endif
				
				nCapCount = 0;
				
				//左上角, 左上角为对方子
				if ((nPosCheckSum == 50 || nPosCheckSum == 41 || 
					nPosCheckSum == 57 || nPosCheckSum == 48) && 
					(nFoeAroundCheckSum & m_nMask[2]))
				{
					FILLCHESSMOVE(m_pcmAdvisorCapPreMoves[nPosCheckNum][nFoeAroundCheckSum].pMoveList[nCapCount], 
						nPosCheckSum, nPosCheckSum - 9, BV_ADVISOR, NOCHESS, EAT);

					nCapCount++;
				}
				//右上角, 右上角为对方子
				if ((nPosCheckSum == 30 || nPosCheckSum == 41 || 
					nPosCheckSum == 37 || nPosCheckSum == 48) && 
					(nFoeAroundCheckSum & m_nMask[3]))
				{
					FILLCHESSMOVE(m_pcmAdvisorCapPreMoves[nPosCheckNum][nFoeAroundCheckSum].pMoveList[nCapCount], 
						nPosCheckSum, nPosCheckSum + 11, BV_ADVISOR, NOCHESS, EAT);

					nCapCount++;
				}
				//左下角, 左下角为对方子
				if ((nPosCheckSum == 52 || nPosCheckSum == 41 || 
					nPosCheckSum == 59 || nPosCheckSum == 48) && 
					(nFoeAroundCheckSum & m_nMask[0]))
				{
					FILLCHESSMOVE(m_pcmAdvisorCapPreMoves[nPosCheckNum][nFoeAroundCheckSum].pMoveList[nCapCount], 
						nPosCheckSum, nPosCheckSum - 11, BV_ADVISOR, NOCHESS, EAT);

					nCapCount++;
				}
				//右下角, 右下角为对方子
				if ((nPosCheckSum == 32 || nPosCheckSum == 41 || 
					nPosCheckSum == 39 || nPosCheckSum == 48) && 
					(nFoeAroundCheckSum & m_nMask[1]))
				{
					FILLCHESSMOVE(m_pcmAdvisorCapPreMoves[nPosCheckNum][nFoeAroundCheckSum].pMoveList[nCapCount], 
						nPosCheckSum, nPosCheckSum + 9, BV_ADVISOR, NOCHESS, EAT);

					nCapCount++;
				}
			}
        }
    }

#ifdef MEMCALC

	cout << "士所有走法内存:" << nAdvisorMovesSize * 1.0 / (1 << 20) << "M" << endl;
	cout << "士所有吃子走法内存:" << nAdvisorCapMovesSize * 1.0 / (1 << 20) << "M" << endl;

#endif
}

/*******************************************************************************************************
函数名 : GenBishopPreMoves
描  述 : 象的走法预产生
参  数 : 无
返回值 : 无
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年8月7日 12:38:52
*******************************************************************************************************/
FSVOID CSearchEngine::GenBishopPreMoves()
{
    FSINT i, j;
    FSINT nCount;
	FSINT nNoCapCount;
	FSINT nCapCount;
    FSINT nAroundCheckSum;
    FSINT nPosCheckNum;
    FSINT nPosCheckSum;
    FSINT nFoeAroundCheckSum;
    FSINT nBishopEyeCheckSum;
    
#ifdef MEMCALC

	FSINT nBishopMovesSize = 0;
	FSINT nBishopCapMovesSize = 0;

#endif

	//象位置校验序号
    for (nPosCheckNum = 0; nPosCheckNum < 14; nPosCheckNum++)
    {
		nPosCheckSum = m_nBishopPosCheckSum[nPosCheckNum];
		i = m_CGN2Pos[nPosCheckSum].x;
		j = m_CGN2Pos[nPosCheckSum].y;
		
		//象眼校验和
		for (nBishopEyeCheckSum = 0; nBishopEyeCheckSum < 16; nBishopEyeCheckSum++)
        {
			//象周围对方子力校验和
            for (nFoeAroundCheckSum = 0; nFoeAroundCheckSum < 16; nFoeAroundCheckSum++)
            {
				//象周围子力校验和
				for (nAroundCheckSum = 0; nAroundCheckSum < 16; nAroundCheckSum++)
                {
                    nCount = 0;
                    
                    //左上角
					//首先判断左上角有子的条件是否成立
                    if ((IsInRBoard(nPosCheckSum) && IsInRBoard(nPosCheckSum - 18)) ||
					   (IsInBBoard(nPosCheckSum) && IsInBBoard(nPosCheckSum - 18)))
                    {
						if ((nBishopEyeCheckSum & m_nMask[2]) == 0)//无象眼
						{
							if (nAroundCheckSum & m_nMask[2])//左上角有子
							{
								if (nFoeAroundCheckSum & m_nMask[2])//左上角为对方子
								{
									nCount++;
								}
							}
							else//左上角无子
							{
								nCount++;
							}
						}
                    }
                    //右上角
					//首先判断右上角有子的条件是否成立
                    if ((IsInRBoard(nPosCheckSum) && IsInRBoard(nPosCheckSum + 22)) ||
					   (IsInBBoard(nPosCheckSum) && IsInBBoard(nPosCheckSum + 22)))
                    {
						if (!(nBishopEyeCheckSum & m_nMask[3]))//无象眼
						{
							if (nAroundCheckSum & m_nMask[3])//右上角有子
							{
								if (nFoeAroundCheckSum & m_nMask[3])//右上角为对方子
								{
									nCount++;
								}
							}
							else
							{
								nCount++;
							}
						}
                    }
                    //左下角
					//首先判断左下角有子的条件是否成立
                    if ((IsInRBoard(nPosCheckSum) && IsInRBoard(nPosCheckSum - 22)) ||
					   (IsInBBoard(nPosCheckSum) && IsInBBoard(nPosCheckSum - 22)))
                    {
						if (!(nBishopEyeCheckSum & m_nMask[0]))//无象眼
						{
							if (nAroundCheckSum & m_nMask[0])//左下角有子
							{
								if (nFoeAroundCheckSum & m_nMask[0])//左下角为对方子
								{
									nCount++;
								}
							}
							else
							{
								nCount++;
							}
						}
                    }
                    //右下角
					//首先判断右下角有子的条件是否成立
                    if ((IsInRBoard(nPosCheckSum) && IsInRBoard(nPosCheckSum + 18)) || 
					   (IsInBBoard(nPosCheckSum) && IsInBBoard(nPosCheckSum + 18)))
                    {
						if (!(nBishopEyeCheckSum & m_nMask[1]))//无象眼
						{
							if (nAroundCheckSum & m_nMask[1])//右下角有子
							{
								if (nFoeAroundCheckSum & m_nMask[1])//右下角为对方子
								{
									nCount++;
								}
							}
							else
							{
								nCount++;
							}
						}
                    }
					DEBUGASSERT(nCount >= 0 && nCount <= 4);

					m_pcmBishopPreMoves[nPosCheckNum][nBishopEyeCheckSum][nAroundCheckSum][nFoeAroundCheckSum].nCount = nCount;
					if (nCount)
					{
						TRYNEW(m_pcmBishopPreMoves[nPosCheckNum][nBishopEyeCheckSum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList = new CHESSMOVE[nCount];);

#ifdef MEMCALC

						nBishopMovesSize += m_nBytes[nCount];

#endif

						nCount = 0;
						
						//左上角
						if ((IsInRBoard(nPosCheckSum) && IsInRBoard(nPosCheckSum - 18)) || 
						   (IsInBBoard(nPosCheckSum) && IsInBBoard(nPosCheckSum - 18)))
						{
							if (!(nBishopEyeCheckSum & m_nMask[2]))//无象眼
							{
								if (nAroundCheckSum & m_nMask[2])//左上角有子
								{
									if (nFoeAroundCheckSum & m_nMask[2])//左上角为对方子
									{
										FILLCHESSMOVE(m_pcmBishopPreMoves[nPosCheckNum][nBishopEyeCheckSum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
											nPosCheckSum, nPosCheckSum - 18, BV_BISHOP, NOCHESS, EAT);

										nCount++;
									}
								}
								else
								{
									FILLCHESSMOVE(m_pcmBishopPreMoves[nPosCheckNum][nBishopEyeCheckSum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
											nPosCheckSum, nPosCheckSum - 18, BV_BISHOP, NOCHESS, NORMAL);

									nCount++;
								}
							}
						}

						//右上角
						if ((IsInRBoard(nPosCheckSum) && IsInRBoard(nPosCheckSum + 22)) ||
						  (IsInBBoard(nPosCheckSum) && IsInBBoard(nPosCheckSum + 22)))
						{
							if (!(nBishopEyeCheckSum & m_nMask[3]))//无象眼
							{
								if (nAroundCheckSum & m_nMask[3])//右上角有子
								{
									if (nFoeAroundCheckSum & m_nMask[3])//右上角为对方子
									{
										FILLCHESSMOVE(m_pcmBishopPreMoves[nPosCheckNum][nBishopEyeCheckSum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
											nPosCheckSum, nPosCheckSum + 22, BV_BISHOP, NOCHESS, EAT);

										nCount++;
									}
								}
								else
								{
									FILLCHESSMOVE(m_pcmBishopPreMoves[nPosCheckNum][nBishopEyeCheckSum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
											nPosCheckSum, nPosCheckSum + 22, BV_BISHOP, NOCHESS, NORMAL);

									nCount++;
								}
							}
						}

						//左下角
						if ((IsInRBoard(nPosCheckSum) && IsInRBoard(nPosCheckSum - 22)) ||
						   (IsInBBoard(nPosCheckSum) && IsInBBoard(nPosCheckSum - 22)))
						{
							if (!(nBishopEyeCheckSum & m_nMask[0]))//无象眼
							{
								if (nAroundCheckSum & m_nMask[0])//左下角有子
								{
									if (nFoeAroundCheckSum & m_nMask[0])//左下角为对方子
									{
										FILLCHESSMOVE(m_pcmBishopPreMoves[nPosCheckNum][nBishopEyeCheckSum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
											nPosCheckSum, nPosCheckSum - 22, BV_BISHOP, NOCHESS, EAT);

										nCount++;
									}
								}
								else
								{
									FILLCHESSMOVE(m_pcmBishopPreMoves[nPosCheckNum][nBishopEyeCheckSum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
											nPosCheckSum, nPosCheckSum - 22, BV_BISHOP, NOCHESS, NORMAL);

									nCount++;
								}
							}
						}
						
						//右下角
				        if ((IsInRBoard(nPosCheckSum) && IsInRBoard(nPosCheckSum + 18)) ||
						   (IsInBBoard(nPosCheckSum) && IsInBBoard(nPosCheckSum + 18)))
						{
							if (!(nBishopEyeCheckSum & m_nMask[1]))//无象眼
							{
								if (nAroundCheckSum & m_nMask[1])//右下角有子
								{
									if (nFoeAroundCheckSum & m_nMask[1])//右下角为对方子
									{
										FILLCHESSMOVE(m_pcmBishopPreMoves[nPosCheckNum][nBishopEyeCheckSum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
											nPosCheckSum, nPosCheckSum + 18, BV_BISHOP, NOCHESS, EAT);

										nCount++;
									}
								}
								else
								{
									FILLCHESSMOVE(m_pcmBishopPreMoves[nPosCheckNum][nBishopEyeCheckSum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
											nPosCheckSum, nPosCheckSum + 18, BV_BISHOP, NOCHESS, NORMAL);

									nCount++;
								}
							}
						}
					}
                }
			}

			//不吃子走法
			//象周围对方子力校验和
            for (nAroundCheckSum = 0; nAroundCheckSum < 16; nAroundCheckSum++)
            {
				nNoCapCount = 0;
				
				//左上角
				//首先判断左上角有子的条件是否成立, 无象眼, 左上角为对方子
				if (((IsInRBoard(nPosCheckSum) && IsInRBoard(nPosCheckSum - 18)) ||
					(IsInBBoard(nPosCheckSum) && IsInBBoard(nPosCheckSum - 18))) && 
					((nBishopEyeCheckSum & m_nMask[2]) == 0) && 
					(nAroundCheckSum & m_nMask[2]) == 0)
				{
					nNoCapCount++;
				}
				//右上角
				//首先判断右上角有子的条件是否成立, 无象眼, 右上角为对方子
				if (((IsInRBoard(nPosCheckSum) && IsInRBoard(nPosCheckSum + 22)) ||
					(IsInBBoard(nPosCheckSum) && IsInBBoard(nPosCheckSum + 22))) && 
					(!(nBishopEyeCheckSum & m_nMask[3])) && 
					(nAroundCheckSum & m_nMask[3]) == 0)
				{
					nNoCapCount++;
				}
				//左下角
				//首先判断左下角有子的条件是否成立, 无象眼, 左下角为对方子
				if (((IsInRBoard(nPosCheckSum) && IsInRBoard(nPosCheckSum - 22)) ||
					(IsInBBoard(nPosCheckSum) && IsInBBoard(nPosCheckSum - 22))) && 
					(!(nBishopEyeCheckSum & m_nMask[0])) && 
					(nAroundCheckSum & m_nMask[0]) == 0)
				{
					nNoCapCount++;
				}
				//右下角
				//首先判断右下角有子的条件是否成立, 无象眼, 右下角为对方子
				if (((IsInRBoard(nPosCheckSum) && IsInRBoard(nPosCheckSum + 18)) || 
					(IsInBBoard(nPosCheckSum) && IsInBBoard(nPosCheckSum + 18))) && 
					(!(nBishopEyeCheckSum & m_nMask[1])) && 
					(nAroundCheckSum & m_nMask[1]) == 0)
				{
					nNoCapCount++;
				}
				DEBUGASSERT(nNoCapCount <= 4);
				
				m_pcmBishopNoCapPreMoves[nPosCheckNum][nBishopEyeCheckSum][nAroundCheckSum].nCount = 
					nNoCapCount;
				if (nNoCapCount)
				{
					TRYNEW(m_pcmBishopNoCapPreMoves[nPosCheckNum][nBishopEyeCheckSum][nAroundCheckSum].pMoveList = 
						new CHESSMOVE[nNoCapCount];);

#ifdef MEMCALC
					
					nBishopCapMovesSize += m_nBytes[nNoCapCount];
					
#endif
					
					nNoCapCount = 0;
					
					//左上角
					//首先判断左上角有子的条件是否成立, 无象眼, 左上角为对方子
					if (((IsInRBoard(nPosCheckSum) && IsInRBoard(nPosCheckSum - 18)) ||
						(IsInBBoard(nPosCheckSum) && IsInBBoard(nPosCheckSum - 18))) && 
						((nBishopEyeCheckSum & m_nMask[2]) == 0) && 
						(nAroundCheckSum & m_nMask[2]) == 0)
					{
						FILLCHESSMOVE(m_pcmBishopNoCapPreMoves[nPosCheckNum][nBishopEyeCheckSum][nAroundCheckSum].pMoveList[nNoCapCount], 
							nPosCheckSum, nPosCheckSum - 18, BV_BISHOP, NOCHESS, EAT);

						nNoCapCount++;
					}
					//右上角
					//首先判断右上角有子的条件是否成立, 无象眼, 右上角为对方子
					if (((IsInRBoard(nPosCheckSum) && IsInRBoard(nPosCheckSum + 22)) ||
						(IsInBBoard(nPosCheckSum) && IsInBBoard(nPosCheckSum + 22))) && 
						(!(nBishopEyeCheckSum & m_nMask[3])) && 
						(nAroundCheckSum & m_nMask[3]) == 0)
					{
						FILLCHESSMOVE(m_pcmBishopNoCapPreMoves[nPosCheckNum][nBishopEyeCheckSum][nAroundCheckSum].pMoveList[nNoCapCount], 
							nPosCheckSum, nPosCheckSum + 22, BV_BISHOP, NOCHESS, EAT);

						nNoCapCount++;
					}
					//左下角
					//首先判断左下角有子的条件是否成立, 无象眼, 左下角为对方子
					if (((IsInRBoard(nPosCheckSum) && IsInRBoard(nPosCheckSum - 22)) ||
						(IsInBBoard(nPosCheckSum) && IsInBBoard(nPosCheckSum - 22))) && 
						(!(nBishopEyeCheckSum & m_nMask[0])) && 
						(nAroundCheckSum & m_nMask[0]) == 0)
					{
						FILLCHESSMOVE(m_pcmBishopNoCapPreMoves[nPosCheckNum][nBishopEyeCheckSum][nAroundCheckSum].pMoveList[nNoCapCount], 
							nPosCheckSum, nPosCheckSum - 22, BV_BISHOP, NOCHESS, EAT);

						nNoCapCount++;
					}
					//右下角
					//首先判断右下角有子的条件是否成立, 无象眼, 右下角为对方子
					if (((IsInRBoard(nPosCheckSum) && IsInRBoard(nPosCheckSum + 18)) || 
						(IsInBBoard(nPosCheckSum) && IsInBBoard(nPosCheckSum + 18))) && 
						(!(nBishopEyeCheckSum & m_nMask[1])) && 
						(nAroundCheckSum & m_nMask[1]) == 0)
					{
						FILLCHESSMOVE(m_pcmBishopNoCapPreMoves[nPosCheckNum][nBishopEyeCheckSum][nAroundCheckSum].pMoveList[nNoCapCount], 
							nPosCheckSum, nPosCheckSum + 18, BV_BISHOP, NOCHESS, EAT);

						nNoCapCount++;
					}
                }
            }

			//吃子走法
			//象周围对方子力校验和
            for (nFoeAroundCheckSum = 0; nFoeAroundCheckSum < 16; nFoeAroundCheckSum++)
            {
				nCapCount = 0;
				
				//左上角
				//首先判断左上角有子的条件是否成立, 无象眼, 左上角为对方子
				if (((IsInRBoard(nPosCheckSum) && IsInRBoard(nPosCheckSum - 18)) ||
					(IsInBBoard(nPosCheckSum) && IsInBBoard(nPosCheckSum - 18))) && 
					((nBishopEyeCheckSum & m_nMask[2]) == 0) && 
					(nFoeAroundCheckSum & m_nMask[2]))
				{
					nCapCount++;
				}
				//右上角
				//首先判断右上角有子的条件是否成立, 无象眼, 右上角为对方子
				if (((IsInRBoard(nPosCheckSum) && IsInRBoard(nPosCheckSum + 22)) ||
					(IsInBBoard(nPosCheckSum) && IsInBBoard(nPosCheckSum + 22))) && 
					(!(nBishopEyeCheckSum & m_nMask[3])) && 
					(nFoeAroundCheckSum & m_nMask[3]))
				{
					nCapCount++;
				}
				//左下角
				//首先判断左下角有子的条件是否成立, 无象眼, 左下角为对方子
				if (((IsInRBoard(nPosCheckSum) && IsInRBoard(nPosCheckSum - 22)) ||
					(IsInBBoard(nPosCheckSum) && IsInBBoard(nPosCheckSum - 22))) && 
					(!(nBishopEyeCheckSum & m_nMask[0])) && 
					(nFoeAroundCheckSum & m_nMask[0]))
				{
					nCapCount++;
				}
				//右下角
				//首先判断右下角有子的条件是否成立, 无象眼, 右下角为对方子
				if (((IsInRBoard(nPosCheckSum) && IsInRBoard(nPosCheckSum + 18)) || 
					(IsInBBoard(nPosCheckSum) && IsInBBoard(nPosCheckSum + 18))) && 
					(!(nBishopEyeCheckSum & m_nMask[1])) && 
					(nFoeAroundCheckSum & m_nMask[1]))
				{
					nCapCount++;
				}
				DEBUGASSERT(nCapCount <= 4);
				
				m_pcmBishopCapPreMoves[nPosCheckNum][nBishopEyeCheckSum][nFoeAroundCheckSum].nCount = 
					nCapCount;
				if (nCapCount)
				{
					TRYNEW(m_pcmBishopCapPreMoves[nPosCheckNum][nBishopEyeCheckSum][nFoeAroundCheckSum].pMoveList = 
						new CHESSMOVE[nCapCount];);

#ifdef MEMCALC
					
					nBishopCapMovesSize += m_nBytes[nCapCount];
					
#endif
					
					nCapCount = 0;
					
					//左上角
					//首先判断左上角有子的条件是否成立, 无象眼, 左上角为对方子
					if (((IsInRBoard(nPosCheckSum) && IsInRBoard(nPosCheckSum - 18)) ||
						(IsInBBoard(nPosCheckSum) && IsInBBoard(nPosCheckSum - 18))) && 
						((nBishopEyeCheckSum & m_nMask[2]) == 0) && 
						(nFoeAroundCheckSum & m_nMask[2]))
					{
						FILLCHESSMOVE(m_pcmBishopCapPreMoves[nPosCheckNum][nBishopEyeCheckSum][nFoeAroundCheckSum].pMoveList[nCapCount], 
							nPosCheckSum, nPosCheckSum - 18, BV_BISHOP, NOCHESS, EAT);

						nCapCount++;
					}
					//右上角
					//首先判断右上角有子的条件是否成立, 无象眼, 右上角为对方子
					if (((IsInRBoard(nPosCheckSum) && IsInRBoard(nPosCheckSum + 22)) ||
						(IsInBBoard(nPosCheckSum) && IsInBBoard(nPosCheckSum + 22))) && 
						(!(nBishopEyeCheckSum & m_nMask[3])) && 
						(nFoeAroundCheckSum & m_nMask[3]))
					{
						FILLCHESSMOVE(m_pcmBishopCapPreMoves[nPosCheckNum][nBishopEyeCheckSum][nFoeAroundCheckSum].pMoveList[nCapCount], 
							nPosCheckSum, nPosCheckSum + 22, BV_BISHOP, NOCHESS, EAT);

						nCapCount++;
					}
					//左下角
					//首先判断左下角有子的条件是否成立, 无象眼, 左下角为对方子
					if (((IsInRBoard(nPosCheckSum) && IsInRBoard(nPosCheckSum - 22)) ||
						(IsInBBoard(nPosCheckSum) && IsInBBoard(nPosCheckSum - 22))) && 
						(!(nBishopEyeCheckSum & m_nMask[0])) && 
						(nFoeAroundCheckSum & m_nMask[0]))
					{
						FILLCHESSMOVE(m_pcmBishopCapPreMoves[nPosCheckNum][nBishopEyeCheckSum][nFoeAroundCheckSum].pMoveList[nCapCount], 
							nPosCheckSum, nPosCheckSum - 22, BV_BISHOP, NOCHESS, EAT);

						nCapCount++;
					}
					//右下角
					//首先判断右下角有子的条件是否成立, 无象眼, 右下角为对方子
					if (((IsInRBoard(nPosCheckSum) && IsInRBoard(nPosCheckSum + 18)) || 
						(IsInBBoard(nPosCheckSum) && IsInBBoard(nPosCheckSum + 18))) && 
						(!(nBishopEyeCheckSum & m_nMask[1])) && 
						(nFoeAroundCheckSum & m_nMask[1]))
					{
						FILLCHESSMOVE(m_pcmBishopCapPreMoves[nPosCheckNum][nBishopEyeCheckSum][nFoeAroundCheckSum].pMoveList[nCapCount], 
							nPosCheckSum, nPosCheckSum + 18, BV_BISHOP, NOCHESS, EAT);

						nCapCount++;
					}
                }
            }
        }
    }

#ifdef MEMCALC

	cout << "象所有走法内存:" << B2M(nBishopMovesSize) << "M" << endl;
	cout << "象所有吃子走法内存:" << B2M(nBishopCapMovesSize) << "M" << endl;

#endif
}

/*******************************************************************************************************
函数名 : GenPawnPreMoves
描  述 : 兵的走法预产生
参  数 : 无
返回值 : 无
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年8月7日 12:39:22
*******************************************************************************************************/
FSVOID CSearchEngine::GenPawnPreMoves()
{
	FSINT i, j;
    FSINT nCount;
	FSINT nNoCapCount;
	FSINT nCapCount;
    FSINT nAroundCheckSum;
	FSINT nPosCheckNum;			//兵位置校验序号
	FSINT nPosCheckSum;			//兵位置校验和
	FSINT nFoeAroundCheckSum;	//兵周围情况校验和

#ifdef MEMCALC

	FSINT nPawnMovesSize = 0;
	FSINT nPawnCapMovesSize = 0;

#endif

	//兵位置校验序号
	for (nPosCheckNum = 0; nPosCheckNum < 55; nPosCheckNum++)
	{
		//兵周围对方子力校验和
		for (nFoeAroundCheckSum = 0; nFoeAroundCheckSum < 8; nFoeAroundCheckSum++)
		{
			//兵周围子力校验和
			for (nAroundCheckSum = 0; nAroundCheckSum < 8; nAroundCheckSum++)
			{
				//黑卒
				nPosCheckSum = m_nPawnPosCheckSum[BLACK][nPosCheckNum];
				
				i = m_CGN2Pos[nPosCheckSum].x;
				j = m_CGN2Pos[nPosCheckSum].y;
				
				nCount = 0;
				
				//左
				if (j <= 4 && i >= 1)
				{
					if (nAroundCheckSum & m_nMask[0])//左边有子
					{
						if (nFoeAroundCheckSum & m_nMask[0])//左边为对方子
						{
							nCount++;
						}
					}
					else
					{
						nCount++;
					}
				}
				
				//右
				if (j <= 4 && i <= 7)
				{
					if (nAroundCheckSum & m_nMask[1])//右边有子
					{
						if (nFoeAroundCheckSum & m_nMask[1])//右边为对方子
						{
							nCount++;
						}
					}
					else
					{
						nCount++;
					}
				}
				
				//下
				if (j>=1)
				{
					if (nAroundCheckSum & m_nMask[2])//下边有子
					{
						if (nFoeAroundCheckSum & m_nMask[2])//下边为对方子
						{
							nCount++;
						}
					}
					else
					{
						nCount++;
					}
				}
				DEBUGASSERT(nCount >= 0 && nCount <= 3);
				
				m_PawnPreMoves[BLACK][nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].nCount = nCount;
				if (nCount)
				{
					TRYNEW(m_PawnPreMoves[BLACK][nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList = new CHESSMOVE[nCount];);

#ifdef MEMCALC
					
					nPawnMovesSize += m_nBytes[nCount];
					
#endif
					nCount = 0;
					
					//左
					if (j <= 4 && i >= 1)
					{
						if (nAroundCheckSum & m_nMask[0])//左边有子
						{
							if (nFoeAroundCheckSum & m_nMask[0])//左边为对方子
							{
								FILLCHESSMOVE(m_PawnPreMoves[BLACK][nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
									nPosCheckSum, nPosCheckSum - 10, BV_PAWN_OPEN_GAME, NOCHESS, EAT);

								nCount++;
							}
						}
						else
						{
							FILLCHESSMOVE(m_PawnPreMoves[BLACK][nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
								nPosCheckSum, nPosCheckSum - 10, BV_PAWN_OPEN_GAME, NOCHESS, NORMAL);

							nCount++;
						}
					}
					
					//右
					if (j <= 4 && i <= 7)
					{
						if (nAroundCheckSum & m_nMask[1])//右边有子
						{
							if (nFoeAroundCheckSum & m_nMask[1])//右边为对方子
							{
								FILLCHESSMOVE(m_PawnPreMoves[BLACK][nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
									nPosCheckSum, nPosCheckSum + 10, BV_PAWN_OPEN_GAME, NOCHESS, EAT);

								nCount++;
							}
						}
						else
						{
							FILLCHESSMOVE(m_PawnPreMoves[BLACK][nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
								nPosCheckSum, nPosCheckSum + 10, BV_PAWN_OPEN_GAME, NOCHESS, NORMAL);

							nCount++;
						}
					}
					
					//下
					if (j>=1)
					{
						if (nAroundCheckSum & m_nMask[2])//下边有子
						{
							if (nFoeAroundCheckSum & m_nMask[2])//下边为对方子
							{
								FILLCHESSMOVE(m_PawnPreMoves[BLACK][nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
									nPosCheckSum, nPosCheckSum - 1, BV_PAWN_OPEN_GAME, NOCHESS, EAT);

								nCount++;
							}
						}
						else
						{
							FILLCHESSMOVE(m_PawnPreMoves[BLACK][nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
								nPosCheckSum, nPosCheckSum - 1, BV_PAWN_OPEN_GAME, NOCHESS, NORMAL);

							nCount++;
						}
					}
				}
				
				//红兵
				nPosCheckSum = m_nPawnPosCheckSum[RED][nPosCheckNum];
				
				i = m_CGN2Pos[nPosCheckSum].x;
				j = m_CGN2Pos[nPosCheckSum].y;
				
				nCount = 0;
				
				//左
				if (j>=5 && i>=1)
				{
					if (nAroundCheckSum & m_nMask[0])//左边有子
					{
						if (nFoeAroundCheckSum & m_nMask[0])//左边为对方子
						{
							nCount++;
						}
					}
					else
					{
						nCount++;
					}
				}
				
				//右
				if (j>=5 && i<=7)
				{
					if (nAroundCheckSum & m_nMask[1])//右边有子
					{
						if (nFoeAroundCheckSum & m_nMask[1])//右边为对方子
						{
							nCount++;
						}
					}
					else
					{
						nCount++;
					}
				}
				
				//上
				if (j <= 8)
				{
					if (nAroundCheckSum & m_nMask[2])//下边有子
					{
						if (nFoeAroundCheckSum & m_nMask[2])//下边为对方子
						{
							nCount++;
						}
					}
					else
					{
						nCount++;
					}
				}
				DEBUGASSERT(nCount >= 0 && nCount <= 3);
				
				m_PawnPreMoves[RED][nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].nCount = nCount;
				if (nCount)
				{
					TRYNEW(m_PawnPreMoves[RED][nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList = new CHESSMOVE[nCount];);
					
#ifdef MEMCALC
					
					nPawnMovesSize += m_nBytes[nCount];
					
#endif
					
					nCount = 0;
					
					//左
					if (j>=5 && i>=1)
					{
						if (nAroundCheckSum & m_nMask[0])//左边有子
						{
							if (nFoeAroundCheckSum & m_nMask[0])//左边为对方子
							{
								FILLCHESSMOVE(m_PawnPreMoves[RED][nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
									nPosCheckSum, nPosCheckSum - 10, BV_PAWN_OPEN_GAME, NOCHESS, EAT);

								nCount++;
							}
						}
						else
						{
							FILLCHESSMOVE(m_PawnPreMoves[RED][nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
								nPosCheckSum, nPosCheckSum - 10, BV_PAWN_OPEN_GAME, NOCHESS, NORMAL);

							nCount++;
						}
					}
					
					//右
					if (j>=5 && i<=7)
					{
						if (nAroundCheckSum & m_nMask[1])//右边有子
						{
							if (nFoeAroundCheckSum & m_nMask[1])//右边为对方子
							{
								FILLCHESSMOVE(m_PawnPreMoves[RED][nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
									nPosCheckSum, nPosCheckSum + 10, BV_PAWN_OPEN_GAME, NOCHESS, EAT);

								nCount++;
							}
						}
						else
						{
							FILLCHESSMOVE(m_PawnPreMoves[RED][nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
								nPosCheckSum, nPosCheckSum + 10, BV_PAWN_OPEN_GAME, NOCHESS, NORMAL);

							nCount++;
						}
					}
					
					//上
					if (j <= 8)
					{
						if (nAroundCheckSum & m_nMask[2])//上边有子
						{
							if (nFoeAroundCheckSum & m_nMask[2])//上边为对方子
							{
								FILLCHESSMOVE(m_PawnPreMoves[RED][nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
									nPosCheckSum, nPosCheckSum + 1, BV_PAWN_OPEN_GAME, NOCHESS, EAT);

								nCount++;
							}
						}
						else
						{
							FILLCHESSMOVE(m_PawnPreMoves[RED][nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].pMoveList[nCount], 
									nPosCheckSum, nPosCheckSum + 1, BV_PAWN_OPEN_GAME, NOCHESS, NORMAL);

							nCount++;
						}
					}
				}
			}
		}

		//不吃子走法
		//兵周围对方子力校验和
		for (nAroundCheckSum = 0; nAroundCheckSum < 8; nAroundCheckSum++)
		{
			//黑卒
			nPosCheckSum = m_nPawnPosCheckSum[BLACK][nPosCheckNum];
			
			i = m_CGN2Pos[nPosCheckSum].x;
			j = m_CGN2Pos[nPosCheckSum].y;
			
			nNoCapCount = 0;
			
			//左, 左边为对方子
			if ((j <= 4 && i >= 1) && (nAroundCheckSum & m_nMask[0]) == 0)
			{
				nNoCapCount++;
			}
			
			//右, 右边为对方子
			if ((j <= 4 && i <= 7) && (nAroundCheckSum & m_nMask[1]) == 0)
			{
				nNoCapCount++;
			}
			
			//下, 下边为对方子
			if ((j>=1) && (nAroundCheckSum & m_nMask[2]) == 0)
			{
				nNoCapCount++;
			}
			DEBUGASSERT(nNoCapCount <= 3);
			
			m_PawnNoCapPreMoves[BLACK][nPosCheckNum][nAroundCheckSum].nCount = nNoCapCount;
			if (nNoCapCount)
			{
				TRYNEW(m_PawnNoCapPreMoves[BLACK][nPosCheckNum][nAroundCheckSum].pMoveList = new CHESSMOVE[nNoCapCount];);
				
#ifdef MEMCALC
				
				nPawnCapMovesSize += m_nBytes[nNoCapCount];
				
#endif
				nNoCapCount = 0;
				
				//左, 左边为对方子
				if ((j <= 4 && i >= 1) && (nAroundCheckSum & m_nMask[0]) == 0)
				{
					FILLCHESSMOVE(m_PawnNoCapPreMoves[BLACK][nPosCheckNum][nAroundCheckSum].pMoveList[nNoCapCount], 
						nPosCheckSum, nPosCheckSum - 10, BV_PAWN_OPEN_GAME, NOCHESS, EAT);

					nNoCapCount++;
				}
				
				//右, 右边为对方子
				if ((j <= 4 && i <= 7) && (nAroundCheckSum & m_nMask[1]) == 0)
				{
					FILLCHESSMOVE(m_PawnNoCapPreMoves[BLACK][nPosCheckNum][nAroundCheckSum].pMoveList[nNoCapCount], 
						nPosCheckSum, nPosCheckSum + 10, BV_PAWN_OPEN_GAME, NOCHESS, EAT);

					nNoCapCount++;
				}
				
				//下, 下边为对方子
				if ((j>=1) && (nAroundCheckSum & m_nMask[2]) == 0)
				{
					FILLCHESSMOVE(m_PawnNoCapPreMoves[BLACK][nPosCheckNum][nAroundCheckSum].pMoveList[nNoCapCount], 
						nPosCheckSum, nPosCheckSum - 1, BV_PAWN_OPEN_GAME, NOCHESS, EAT);

					nNoCapCount++;
				}
			}
			
			//红兵
			nPosCheckSum = m_nPawnPosCheckSum[RED][nPosCheckNum];
			
			i = m_CGN2Pos[nPosCheckSum].x;
			j = m_CGN2Pos[nPosCheckSum].y;
			
			nNoCapCount = 0;
			
			//左, 左边为对方子
			if ((j>=5 && i>=1) && (nAroundCheckSum & m_nMask[0]) == 0)
			{
				nNoCapCount++;
			}
			
			//右, 右边为对方子
			if ((j>=5 && i<=7) && (nAroundCheckSum & m_nMask[1]) == 0)
			{
				nNoCapCount++;
			}
			
			//上, 下边为对方子
			if ((j <= 8) && (nAroundCheckSum & m_nMask[2]) == 0)
			{
				nNoCapCount++;
			}
			DEBUGASSERT(nNoCapCount <= 3);
			
			m_PawnNoCapPreMoves[RED][nPosCheckNum][nAroundCheckSum].nCount = nNoCapCount;
			if (nNoCapCount)
			{
				TRYNEW(m_PawnNoCapPreMoves[RED][nPosCheckNum][nAroundCheckSum].pMoveList = new CHESSMOVE[nNoCapCount];);
				
#ifdef MEMCALC
				
				nPawnMovesSize += m_nBytes[nNoCapCount];
				
#endif
				
				nNoCapCount = 0;
				
				//左, 左边为对方子
				if ((j>=5 && i>=1) && (nAroundCheckSum & m_nMask[0]) == 0)
				{
					FILLCHESSMOVE(m_PawnNoCapPreMoves[RED][nPosCheckNum][nAroundCheckSum].pMoveList[nNoCapCount], 
						nPosCheckSum, nPosCheckSum - 10, BV_PAWN_OPEN_GAME, NOCHESS, EAT);

					nNoCapCount++;
				}
				
				//右, 右边为对方子
				if ((j>=5 && i<=7) && (nAroundCheckSum & m_nMask[1]) == 0)
				{
					FILLCHESSMOVE(m_PawnNoCapPreMoves[RED][nPosCheckNum][nAroundCheckSum].pMoveList[nNoCapCount], 
						nPosCheckSum, nPosCheckSum + 10, BV_PAWN_OPEN_GAME, NOCHESS, EAT);

					nNoCapCount++;
				}
				
				//上, 下边为对方子
				if ((j <= 8) && (nAroundCheckSum & m_nMask[2]) == 0)
				{
					FILLCHESSMOVE(m_PawnNoCapPreMoves[RED][nPosCheckNum][nAroundCheckSum].pMoveList[nNoCapCount], 
						nPosCheckSum, nPosCheckSum + 1, BV_PAWN_OPEN_GAME, NOCHESS, EAT);

					nNoCapCount++;
				}
			}
		}

		//吃子走法
		//兵周围对方子力校验和
		for (nFoeAroundCheckSum = 0; nFoeAroundCheckSum < 8; nFoeAroundCheckSum++)
		{
			//黑卒
			nPosCheckSum = m_nPawnPosCheckSum[BLACK][nPosCheckNum];
			
			i = m_CGN2Pos[nPosCheckSum].x;
			j = m_CGN2Pos[nPosCheckSum].y;
			
			nCapCount = 0;
			
			//左, 左边为对方子
			if ((j <= 4 && i >= 1) && (nFoeAroundCheckSum & m_nMask[0]))
			{
				nCapCount++;
			}
			
			//右, 右边为对方子
			if ((j <= 4 && i <= 7) && (nFoeAroundCheckSum & m_nMask[1]))
			{
				nCapCount++;
			}
			
			//下, 下边为对方子
			if ((j>=1) && (nFoeAroundCheckSum & m_nMask[2]))
			{
				nCapCount++;
			}
			DEBUGASSERT(nCapCount <= 3);
			
			m_PawnCapPreMoves[BLACK][nPosCheckNum][nFoeAroundCheckSum].nCount = nCapCount;
			if (nCapCount)
			{
				TRYNEW(m_PawnCapPreMoves[BLACK][nPosCheckNum][nFoeAroundCheckSum].pMoveList = new CHESSMOVE[nCapCount];);
				
#ifdef MEMCALC
				
				nPawnCapMovesSize += m_nBytes[nCapCount];
				
#endif
				nCapCount = 0;
				
				//左, 左边为对方子
				if ((j <= 4 && i >= 1) && (nFoeAroundCheckSum & m_nMask[0]))
				{
					FILLCHESSMOVE(m_PawnCapPreMoves[BLACK][nPosCheckNum][nFoeAroundCheckSum].pMoveList[nCapCount], 
						nPosCheckSum, nPosCheckSum - 10, BV_PAWN_OPEN_GAME, NOCHESS, EAT);

					nCapCount++;
				}
				
				//右, 右边为对方子
				if ((j <= 4 && i <= 7) && (nFoeAroundCheckSum & m_nMask[1]))
				{
					FILLCHESSMOVE(m_PawnCapPreMoves[BLACK][nPosCheckNum][nFoeAroundCheckSum].pMoveList[nCapCount], 
						nPosCheckSum, nPosCheckSum + 10, BV_PAWN_OPEN_GAME, NOCHESS, EAT);

					nCapCount++;
				}
				
				//下, 下边为对方子
				if ((j>=1) && (nFoeAroundCheckSum & m_nMask[2]))
				{
					FILLCHESSMOVE(m_PawnCapPreMoves[BLACK][nPosCheckNum][nFoeAroundCheckSum].pMoveList[nCapCount], 
						nPosCheckSum, nPosCheckSum - 1, BV_PAWN_OPEN_GAME, NOCHESS, EAT);

					nCapCount++;
				}
			}
			
			//红兵
			nPosCheckSum = m_nPawnPosCheckSum[RED][nPosCheckNum];
			
			i = m_CGN2Pos[nPosCheckSum].x;
			j = m_CGN2Pos[nPosCheckSum].y;
			
			nCapCount = 0;
			
			//左, 左边为对方子
			if ((j>=5 && i>=1) && (nFoeAroundCheckSum & m_nMask[0]))
			{
				nCapCount++;
			}
			
			//右, 右边为对方子
			if ((j>=5 && i<=7) && (nFoeAroundCheckSum & m_nMask[1]))
			{
				nCapCount++;
			}
			
			//上, 下边为对方子
			if ((j <= 8) && (nFoeAroundCheckSum & m_nMask[2]))
			{
				nCapCount++;
			}
			DEBUGASSERT(nCapCount <= 3);
			
			m_PawnCapPreMoves[RED][nPosCheckNum][nFoeAroundCheckSum].nCount = nCapCount;
			if (nCapCount)
			{
				TRYNEW(m_PawnCapPreMoves[RED][nPosCheckNum][nFoeAroundCheckSum].pMoveList = new CHESSMOVE[nCapCount];);
				
#ifdef MEMCALC
				
				nPawnMovesSize += m_nBytes[nCapCount];
				
#endif
				
				nCapCount = 0;
				
				//左, 左边为对方子
				if ((j>=5 && i>=1) && (nFoeAroundCheckSum & m_nMask[0]))
				{
					FILLCHESSMOVE(m_PawnCapPreMoves[RED][nPosCheckNum][nFoeAroundCheckSum].pMoveList[nCapCount], 
						nPosCheckSum, nPosCheckSum - 10, BV_PAWN_OPEN_GAME, NOCHESS, EAT);

					nCapCount++;
				}
				
				//右, 右边为对方子
				if ((j>=5 && i<=7) && (nFoeAroundCheckSum & m_nMask[1]))
				{
					FILLCHESSMOVE(m_PawnCapPreMoves[RED][nPosCheckNum][nFoeAroundCheckSum].pMoveList[nCapCount], 
						nPosCheckSum, nPosCheckSum + 10, BV_PAWN_OPEN_GAME, NOCHESS, EAT);

					nCapCount++;
				}
				
				//上, 下边为对方子
				if ((j <= 8) && (nFoeAroundCheckSum & m_nMask[2]))
				{
					FILLCHESSMOVE(m_PawnCapPreMoves[RED][nPosCheckNum][nFoeAroundCheckSum].pMoveList[nCapCount], 
						nPosCheckSum, nPosCheckSum + 1, BV_PAWN_OPEN_GAME, NOCHESS, EAT);

					nCapCount++;
				}
			}
		}
	}

#ifdef MEMCALC

	cout << "兵所有走法内存:" << nPawnMovesSize * 1.0 / (1 << 20) << "M" << endl;
	cout << "兵所有吃子走法内存:" << nPawnCapMovesSize * 1.0 / (1 << 20) << "M" << endl;

#endif
}

/*******************************************************************************************************
函数名 : LoadKnightPreMove
描  述 : 
参  数 : 无
返回值 : 类型为 FSBOOL
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年10月21日 22:28:37
*******************************************************************************************************/
/*FSBOOL CSearchEngine::LoadKnightPreMove()
{
#ifdef CFILE
	FILE *fp = fopen(KNIGHTPREMOVESFILE, "rb");

	if (!fp)
	{
		return FALSE;
	}
#else
	// 文件存在
	ifstream ifs(KNIGHTPREMOVESFILE);

	if (!ifs)
	{
		return FALSE;
	}
#endif

	FSINT p, i, j;
	FSINT nCount;
	FSINT nSizeOfFRINT = sizeof(FSINT);

	// 加载不吃子走法[2005/10/21 21:53:40]
	for (p = 0; p < 90; p++)
	{
		for (i = 0; i < 256; i++)
		{
			if (m_bIsKnightInvalidArroundCheckSum[p][i] == FALSE)
			{
				continue;
			}

			for (j = 0; j < 16; j++)
			{
				if (m_bIsKnightInvalidLegCheckSum[p][i][j] == FALSE)
				{
					continue;
				}

			#ifdef CFILE
				fread(&nCount, nSizeOfFRINT, 1, fp);
			#else
				ifs.read((char*)&nCount, nSizeOfFRINT);
			#endif

				if (nCount < 0)
				{
					return FALSE;
				}

				m_pcmKnightNoCapPreMoves[p][i][j].nCount = nCount;
				if (nCount == 0)
				{
					continue;
				}

				TRYNEW(m_pcmKnightNoCapPreMoves[p][i][j].pMoveList = new CHESSMOVE[nCount];);

			#ifdef CFILE
				fread(m_pcmKnightNoCapPreMoves[p][i][j].pMoveList, sizeof(FSCHAR), m_nBytes[nCount], fp);
			#else
				ifs.read((FSCHAR*)m_pcmKnightNoCapPreMoves[p][i][j].pMoveList, m_nBytes[nCount]);
			#endif
			}
		}
	}

	// 加载吃子走法[2005/10/21 21:53:40]
	for (p = 0; p < 90; p++)
	{
		for (i = 0; i < 256; i++)
		{
			if (m_bIsKnightInvalidArroundCheckSum[p][i] == FALSE)
			{
				continue;
			}

			for (j = 0; j < 16; j++)
			{
				if (m_bIsKnightInvalidLegCheckSum[p][i][j] == FALSE)
				{
					continue;
				}

			#ifdef CFILE
				fread(&nCount, nSizeOfFRINT, 1, fp);
			#else
				ifs.read((char*)&nCount, nSizeOfFRINT);
			#endif

				if (nCount < 0)
				{
					return FALSE;
				}

				m_pcmKnightCapPreMoves[p][i][j].nCount = nCount;
				if (nCount == 0)
				{
					continue;
				}

				TRYNEW(m_pcmKnightCapPreMoves[p][i][j].pMoveList = new CHESSMOVE[nCount];);

			#ifdef CFILE
				fread(m_pcmKnightCapPreMoves[p][i][j].pMoveList, sizeof(FSCHAR), m_nBytes[nCount], fp);
			#else
				ifs.read((FSCHAR*)m_pcmKnightCapPreMoves[p][i][j].pMoveList, m_nBytes[nCount]);
			#endif
			}
		}
	}

	return TRUE;
}*/

/*******************************************************************************************************
函数名 : SaveKnightPreMoves
描  述 : 
参  数 : 无
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年10月21日 22:33:06
*******************************************************************************************************/
/*FSVOID CSearchEngine::SaveKnightPreMoves()
{
	//文件存在 [2005/10/21 21:53:35]
	if (GetFileAttributes(KNIGHTPREMOVESFILE) != -1)
	{
		return;
	}

#ifdef CFILE
	FILE *fp = fopen(KNIGHTPREMOVESFILE, "wb");

	if (!fp)
	{
		return;
	}
#else
	ofstream ofs(KNIGHTPREMOVESFILE);

	if (!ofs)
	{
		return;
	}
#endif
	
	FSINT p, i, j;
	FSINT nCount;
	FSINT nSizeOfFRINT = sizeof(FSINT);

	// 保存不吃子走法[2005/10/21 21:53:40]
	for (p = 0; p < 90; p++)
	{
		for (i = 0; i < 256; i++)
		{
			if (m_bIsKnightInvalidArroundCheckSum[p][i] == FALSE)
			{
				continue;
			}
			
			for (j = 0; j < 16; j++)
			{
				if (m_bIsKnightInvalidLegCheckSum[p][i][j] == FALSE)
				{
					continue;
				}
				
				nCount = m_pcmKnightNoCapPreMoves[p][i][j].nCount;
				DEBUGASSERT(nCount >= 0 && nCount <= MAXKNIGHTNOCAPMOVECNT);

			#ifdef CFILE
				fwrite(&nCount, nSizeOfFRINT, 1, fp);
			#else
				ofs.write((char*)&nCount, nSizeOfFRINT);
			#endif

				if (nCount == 0)
				{
					continue;
				}

			#ifdef CFILE
				fwrite(m_pcmKnightNoCapPreMoves[p][i][j].pMoveList, sizeof(FSCHAR), m_nBytes[nCount], fp);
			#else
				ofs.write((FSCHAR*)m_pcmKnightNoCapPreMoves[p][i][j].pMoveList, m_nBytes[nCount]);
			#endif
			}
		}
	}

	// 保存吃子走法[2005/10/21 21:53:40]
	for (p = 0; p < 90; p++)
	{
		for (i = 0; i < 256; i++)
		{
			if (m_bIsKnightInvalidArroundCheckSum[p][i] == FALSE)
			{
				continue;
			}

			for (j = 0; j < 16; j++)
			{
				if (m_bIsKnightInvalidLegCheckSum[p][i][j] == FALSE)
				{
					continue;
				}
				
				nCount = m_pcmKnightNoCapPreMoves[p][i][j].nCount;
				DEBUGASSERT(nCount >= 0 && nCount <= MAXKNIGHTCAPMOVECNT);
				
			#ifdef CFILE
				fwrite(&nCount, nSizeOfFRINT, 1, fp);
			#else
				ofs.write((FSCHAR*)&nCount, nSizeOfFRINT);
			#endif

				if (nCount == 0)
				{
					continue;
				}

			#ifdef CFILE
				fwrite(m_pcmKnightCapPreMoves[p][i][j].pMoveList, sizeof(FSCHAR), m_nBytes[nCount], fp);
			#else
				ofs.write((FSCHAR*)m_pcmKnightCapPreMoves[p][i][j].pMoveList, m_nBytes[nCount]);
			#endif
			}
		}
	}
}*/

/*******************************************************************************************************
函数名 : GenPreCheck
描  述 : 产生将军预判断数组
参  数 : 无
返回值 : 无
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:45:43
*******************************************************************************************************/
FSVOID CSearchEngine::GenPreCheck()
{
	GenRookPreCheck  ();	//车
	GenKnightPreCheck();	//马
	GenCannonPreCheck();	//炮
	GenPawnPreCheck  ();	//兵
}

/*******************************************************************************************************
函数名 : GenRookPreCheck
描  述 : 产生车将军预判断数组
参  数 : 无
返回值 : 无
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:46:09
*******************************************************************************************************/
FSVOID CSearchEngine::GenRookPreCheck()
{
	FSINT p;
	FSINT i,j,k;
	FSINT nWho;					//何方
	FSINT nFoeKingPosCheckNum;	//对方将校验和
	FSINT nFoeKingPosCheckSum;	//对方将位置
	FSINT nBitFile;
	FSINT nBitRank;
	FSINT nFoeKingI;
	FSINT nFoeKingJ;
	FSBOOL bFlag;
	
	for (nWho = 0; nWho < 2; nWho++)
	{
		for (p = 0; p < 90; p++)
		{
			i = m_CGN2Pos[p].x;
			j = m_CGN2Pos[p].y;
			
			for (nFoeKingPosCheckNum = 0; nFoeKingPosCheckNum < 9; nFoeKingPosCheckNum++)
			{
				nFoeKingPosCheckSum=m_nBRKingPosCheckSum[1-nWho][nFoeKingPosCheckNum];
				
				nFoeKingI = m_CGN2Pos[nFoeKingPosCheckSum].x;
				nFoeKingJ = m_CGN2Pos[nFoeKingPosCheckSum].y;

				//m_bIsRookFilePreCheck
				for (nBitFile = 0; nBitFile < 512; nBitFile++)
				{
					m_bIsRookFilePreCheck[nWho][p][nFoeKingPosCheckNum][nBitFile] = FALSE;
					
					if (j != m_CGN2Pos[nFoeKingPosCheckSum].y)
					{
						continue;
					}
					
					bFlag = FALSE;
					if (i < nFoeKingI)
					{
						for (k = i + 1; k < nFoeKingI; k++)
						{
							if (nBitFile & m_nMask[k])
							{
								bFlag = TRUE;
								break;
							}
						}
						
						if (bFlag == FALSE)
						{
							m_bIsRookFilePreCheck[nWho][p][nFoeKingPosCheckNum][nBitFile] = TRUE;
						}
					}
					else
					{
						for (k = i - 1; k > nFoeKingI; k--)
						{
							if (nBitFile & m_nMask[k])
							{
								bFlag = TRUE;
								break;
							}
						}
						
						if (bFlag == FALSE)
						{
							m_bIsRookFilePreCheck[nWho][p][nFoeKingPosCheckNum][nBitFile] = TRUE;
						}
					}
				}
				
				//m_bIsRookRankPreCheck
				for (nBitRank = 0; nBitRank < 1024; nBitRank++)
				{
					m_bIsRookRankPreCheck[nWho][p][nFoeKingPosCheckNum][nBitRank] = FALSE;
					
					if (i != m_CGN2Pos[nFoeKingPosCheckSum].x)
					{
						continue;
					}
					
					bFlag = FALSE;
					if (j < nFoeKingJ)
					{
						for (k = j + 1; k < nFoeKingJ; k++)
						{
							if (nBitRank & m_nMask[k])
							{
								bFlag = TRUE;
								break;
							}
						}
						
						if (bFlag == FALSE)
						{
							m_bIsRookRankPreCheck[nWho][p][nFoeKingPosCheckNum][nBitRank] = TRUE;
						}
					}
					else
					{
						for (k=j-1;k>nFoeKingJ;k--)
						{
							if (nBitRank & m_nMask[k])
							{
								bFlag = TRUE;
								break;
							}
						}
						
						if (bFlag == FALSE)
						{
							m_bIsRookRankPreCheck[nWho][p][nFoeKingPosCheckNum][nBitRank] = TRUE;
						}
					}
				}
			}
		}

		//p=90
		p = 90;
		for (nFoeKingPosCheckNum = 0; nFoeKingPosCheckNum < 9; nFoeKingPosCheckNum++)
		{
			//行
			for (nBitFile = 0; nBitFile < 512; nBitFile++)
			{
				m_bIsRookFilePreCheck[nWho][p][nFoeKingPosCheckNum][nBitFile] = FALSE;
			}
			//列
			for (nBitRank = 0; nBitRank < 1024; nBitRank++)
			{
				m_bIsRookRankPreCheck[nWho][p][nFoeKingPosCheckNum][nBitRank] = FALSE;
			}
		}
	}
}

/*******************************************************************************************************
函数名 : GenKnightPreCheck
描  述 : 马将军判断
参  数 : 无
返回值 : 无
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:46:45
*******************************************************************************************************/
FSVOID CSearchEngine::GenKnightPreCheck()
{
	FSINT p;
	FSINT i, j;
	FSINT nWho;//何方
	FSINT nFoeKingPosCheckNum;//对方将校验和
	FSINT nFoeKingPosCheckSum;//对方将位置
	FSINT nKnightLeg;//马腿
	FSINT nFoeKingI;
	FSINT nFoeKingJ;

	for (nWho = 0; nWho < 2; nWho++)
	{
		for (p = 0; p < 90; p++)
		{
			i = m_CGN2Pos[p].x;
			j = m_CGN2Pos[p].y;
			
			for (nFoeKingPosCheckNum = 0; nFoeKingPosCheckNum < 9; nFoeKingPosCheckNum++)
			{
				nFoeKingPosCheckSum=m_nBRKingPosCheckSum[1-nWho][nFoeKingPosCheckNum];
				
				nFoeKingI = m_CGN2Pos[nFoeKingPosCheckSum].x;
				nFoeKingJ = m_CGN2Pos[nFoeKingPosCheckSum].y;

				for (nKnightLeg = 0; nKnightLeg < 16; nKnightLeg++)
				{
					m_bIsKnightPreCheck[nWho][p][nFoeKingPosCheckNum][nKnightLeg] = 
						(!(nKnightLeg & m_nMask[0]) && 
						(i == nFoeKingI + 2 && (j == nFoeKingJ + 1 || j == nFoeKingJ - 1))) ||//将在马左
					    (!(nKnightLeg & m_nMask[1]) && 
					    (i == nFoeKingI - 2 && (j == nFoeKingJ + 1 || j == nFoeKingJ - 1))) ||//将在马右
					    (!(nKnightLeg & m_nMask[2]) && 
					    (j == nFoeKingJ + 2 && (i == nFoeKingI + 1 || i == nFoeKingI - 1))) ||//将在马下
					    (!(nKnightLeg & m_nMask[3]) && 
					    (j == nFoeKingJ - 2 && (i == nFoeKingI + 1 || i == nFoeKingI - 1)));  //将在马上
				}
			}
		}

		//p = 90
		p = 90;
		for (nFoeKingPosCheckNum = 0; nFoeKingPosCheckNum < 9; nFoeKingPosCheckNum++)
		{
			for (nKnightLeg = 0; nKnightLeg < 16; nKnightLeg++)
			{
				m_bIsKnightPreCheck[nWho][p][nFoeKingPosCheckNum][nKnightLeg] = FALSE;
			}
		}
	}
}

/*******************************************************************************************************
函数名 : GenCannonPreCheck
描  述 : 炮将军判断
参  数 : 无
返回值 : 无
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:46:59
*******************************************************************************************************/
FSVOID CSearchEngine::GenCannonPreCheck()
{
	FSINT p;
	FSINT i,j,k;
	FSINT nWho;//何方
	FSINT nFoeKingPosCheckNum;//对方将校验和
	FSINT nFoeKingPosCheckSum;//对方将位置
	FSINT nBitFile;
	FSINT nBitRank;
	FSINT nFoeKingI;
	FSINT nFoeKingJ;
	FSINT nCnt;

	for (nWho = 0; nWho < 2; nWho++)
	{
		for (p = 0; p < 90; p++)
		{
			i = m_CGN2Pos[p].x;
			j = m_CGN2Pos[p].y;

			for (nFoeKingPosCheckNum = 0; nFoeKingPosCheckNum < 9; nFoeKingPosCheckNum++)
			{
				nFoeKingPosCheckSum = m_nBRKingPosCheckSum[1-nWho][nFoeKingPosCheckNum];
				
				nFoeKingI = m_CGN2Pos[nFoeKingPosCheckSum].x;
				nFoeKingJ = m_CGN2Pos[nFoeKingPosCheckSum].y;

				//m_bIsCannonFilePreCheck
				for (nBitFile = 0; nBitFile < 512; nBitFile++)
				{
					m_bIsCannonFilePreCheck[nWho][p][nFoeKingPosCheckNum][nBitFile] = FALSE;

					if (j != m_CGN2Pos[nFoeKingPosCheckSum].y)
					{
						continue;
					}

					nCnt = 0;
					if (i < nFoeKingI)
					{
						for (k = i + 1; k < nFoeKingI; k++)
						{
							if (nBitFile & m_nMask[k])
							{
								nCnt++;
							}
						}

						if (nCnt == 1)
						{
							m_bIsCannonFilePreCheck[nWho][p][nFoeKingPosCheckNum][nBitFile] = TRUE;
						}
					}
					else
					{
						for (k=i-1;k>nFoeKingI;k--)
						{
							if (nBitFile & m_nMask[k])
							{
								nCnt++;
							}
						}

						if (nCnt == 1)
						{
							m_bIsCannonFilePreCheck[nWho][p][nFoeKingPosCheckNum][nBitFile] = TRUE;
						}
					}
				}

				//m_bIsCannonRankPreCheck
				for (nBitRank = 0; nBitRank < 1024; nBitRank++)
				{
					m_bIsCannonRankPreCheck[nWho][p][nFoeKingPosCheckNum][nBitRank] = FALSE;
					
					if (i != m_CGN2Pos[nFoeKingPosCheckSum].x)
					{
						continue;
					}
					
					nCnt = 0;
					if (j<nFoeKingJ)
					{
						for (k=j+1;k<nFoeKingJ;k++)
						{
							if (nBitRank & m_nMask[k])
							{
								nCnt++;
							}
						}
						
						if (nCnt == 1)
						{
							m_bIsCannonRankPreCheck[nWho][p][nFoeKingPosCheckNum][nBitRank] = TRUE;
						}
					}
					else
					{
						for (k=j-1;k>nFoeKingJ;k--)
						{
							if (nBitRank & m_nMask[k])
							{
								nCnt++;
							}
						}
						
						if (nCnt == 1)
						{
							m_bIsCannonRankPreCheck[nWho][p][nFoeKingPosCheckNum][nBitRank] = TRUE;
						}
					}
				}
			}
		}

		//p=90
		p = 90;
		for (nFoeKingPosCheckNum = 0; nFoeKingPosCheckNum < 9; nFoeKingPosCheckNum++)
		{
			//行
			for (nBitFile = 0; nBitFile < 512; nBitFile++)
			{
				m_bIsCannonFilePreCheck[nWho][p][nFoeKingPosCheckNum][nBitFile] = FALSE;
			}
			//列
			for (nBitRank = 0; nBitRank < 1024; nBitRank++)
			{
				m_bIsCannonRankPreCheck[nWho][p][nFoeKingPosCheckNum][nBitRank] = FALSE;
			}
		}
	}
}

/*******************************************************************************************************
函数名 : GenPawnPreCheck
描  述 : 兵将军判断
参  数 : 无
返回值 : 无
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:47:13
*******************************************************************************************************/
FSVOID CSearchEngine::GenPawnPreCheck()
{
	FSINT i, j;
	FSINT nWho;	//何方

	FSINT nPosCheckNum;	//兵位置校验序号
	FSINT nPosCheckSum;	//兵位置校验和

	FSINT nFoeKingPosCheckNum;	//对方将校验和
	FSINT nFoeKingPosCheckSum;	//对方将位置

	for (nWho = 0; nWho < 2; nWho++)
	{
		//兵位置校验序号
		for (nPosCheckNum = 0;nPosCheckNum<55;nPosCheckNum++)
		{
			nPosCheckSum=m_nPawnPosCheckSum[nWho][nPosCheckNum];

			i=m_CGN2Pos[nPosCheckSum].x;
			j=m_CGN2Pos[nPosCheckSum].y;

			for (nFoeKingPosCheckNum = 0; nFoeKingPosCheckNum < 9; nFoeKingPosCheckNum++)
			{
				nFoeKingPosCheckSum=m_nBRKingPosCheckSum[1 - nWho][nFoeKingPosCheckNum];
				
				m_bIsPawnPreCheck[nWho][nPosCheckNum][nFoeKingPosCheckNum] = FALSE;
				
				//红
				if (nWho)
				{
					if (nFoeKingPosCheckSum == nPosCheckSum - 10 ||
					   nFoeKingPosCheckSum == nPosCheckSum + 10 ||
					   nFoeKingPosCheckSum == nPosCheckSum + 1)
					{
						m_bIsPawnPreCheck[nWho][nPosCheckNum][nFoeKingPosCheckNum] = TRUE;
					}
				}
				else
				{
					if (nFoeKingPosCheckSum == nPosCheckSum - 10 ||
					   nFoeKingPosCheckSum == nPosCheckSum + 10 ||
				       nFoeKingPosCheckSum == nPosCheckSum - 1)
					{
						m_bIsPawnPreCheck[nWho][nPosCheckNum][nFoeKingPosCheckNum] = TRUE;
					}
				}
			}
		}

		//nPosCheckNum = 55
		nPosCheckNum = 55;
		for (nFoeKingPosCheckNum = 0; nFoeKingPosCheckNum < 9; nFoeKingPosCheckNum++)
		{
			m_bIsPawnPreCheck[nWho][nPosCheckNum][nFoeKingPosCheckNum] = FALSE;
		}
	}
}

/*******************************************************************************************************
函数名 : GenPreValidMove
描  述 : 产生走法有效性预判断数组
参  数 : 无
返回值 : 类型为 FSVOID
版  本 : 0.1
作  者 : ahei
时  间 : 2005年10月2日 20:44:58
*******************************************************************************************************/
FSVOID CSearchEngine::GenPreValidMove()
{
	GenRookPreValidMove();		//车
//	GenKnightPreValidMove();	//马
	GenCannonPreValidMove();	//炮
//	GenKingPreValidMove();		//将
//	GenAdvisorPreValidMove();	//士
//	GenBishopPreValidMove();	//象
//	GenPawnPreValidMove();		//兵
}

/*******************************************************************************************************
函数名 : GenRookPreValidMove
描  述 : 产生车走法有效性预判断数组
参  数 : 无
返回值 : 类型为 FSVOID
版  本 : 0.1
作  者 : ahei
时  间 : 2005年10月2日 20:44:53
*******************************************************************************************************/
FSVOID CSearchEngine::GenRookPreValidMove()
{
//	FSINT i, j;
//	FSINT nMinI, nMaxI;
//	FSINT nMinJ, nMaxJ;
//	FSINT nSrc, nDst;
//	FSINT nSrcI, nSrcJ;
//	FSINT nDstI, nDstJ;
//	FSINT nBitFile;
//	FSINT nBitRank;
//	FSBOOL bFlag;
//	FSINT nFEATBOOLSize = sizeof(FEATBOOL);

#ifdef TIMECALC

	FSINT nTimeStart = clock();

#endif

//	for (nSrc = 0; nSrc < 90; nSrc++)
//	{
//		nSrcI = m_CGN2Pos[nSrc].x;
//		nSrcJ = m_CGN2Pos[nSrc].y;
//
//		for (nDst = 0; nDst < 90; nDst++)
//		{
//			nDstI = m_CGN2Pos[nDst].x;
//			nDstJ = m_CGN2Pos[nDst].y;
//
//			//行
//			if (nSrcJ == nDstJ)
//			{
//				for (nBitFile = 0; nBitFile < 512; nBitFile++)
//				{
//					nMinI = min(nSrcI, nDstI);
//					nMaxI = max(nSrcI, nDstI);
//					
//					bFlag = TRUE;
//					for (i = nMinI + 1; i < nMaxI; i++)
//					{
//						if (m_nMask[i] & nBitFile)
//						{
//							bFlag = FALSE;
//							break;
//						}
//					}
//					m_bIsRookFilePreValidMove[nSrc][nDst][nBitFile] = bFlag;
//				}
//			}
//			else
//			{
//				memset(m_bIsRookFilePreValidMove[nSrc][nDst], FALSE, nFEATBOOLSize << 9);
//			}
//
//			//列
//			if (nSrcI != nDstI)
//			{
//				memset(m_bIsRookRankPreValidMove[nSrc][nDst], FALSE, nFEATBOOLSize << 10);
//				continue;
//			}
//
//			//nSrcJ == nDstJ
//			for (nBitRank = 0; nBitRank < 1024; nBitRank++)
//			{
//				nMinJ = min(nSrcJ, nDstJ);
//				nMaxJ = max(nSrcJ, nDstJ);
//
//				bFlag = TRUE;
//				for (j = nMinJ + 1; j < nMaxJ; j++)
//				{
//					if (m_nMask[j] & nBitRank)
//					{
//						bFlag = FALSE;
//						break;
//					}
//				}
//				m_bIsRookRankPreValidMove[nSrc][nDst][nBitRank] = bFlag;
//			}
//		}
//	}

	FSINT i;
	FSINT nMin, nMax;
	FSINT nSrc, nDst;
	FSINT nBit;
	FSBOOL bFlag;
	
	for (nSrc = 0; nSrc < 10; nSrc++)
	{
		for (nDst = 0; nDst < 10; nDst++)
		{
			for (nBit = 0; nBit < 1024; nBit++)
			{
				nMin = min(nSrc, nDst);
				nMax = max(nSrc, nDst);
				
				bFlag = TRUE;
				for (i = nMin + 1; i < nMax; i++)
				{
					if (m_nMask[i] & nBit)
					{
						bFlag = FALSE;
						break;
					}
				}
				m_bIsRookPreValidMove[nSrc][nDst][nBit] = bFlag;
			}
		}
	}

#ifdef TIMECALC

	cout << "产生车走法有效性预判断的时间:" << clock() - nTimeStart << endl;

#endif
}

/*******************************************************************************************************
函数名 : GenKnightPreValidMove
描  述 : 产生马走法有效性预判断数组
参  数 : 无
返回值 : 类型为 FSVOID
版  本 : 0.1
作  者 : ahei
时  间 : 2005年10月2日 20:44:46
*******************************************************************************************************/
//FSVOID CSearchEngine::GenKnightPreValidMove()
//{
//	FSINT nSrc, nDst;
//	FSINT nSrcI, nSrcJ;
//	FSINT nDstI, nDstJ;
//	FSINT nKnightLeg;//马腿
//
//	for (nSrc = 0; nSrc < 90; nSrc++)
//	{
//		nSrcI = m_CGN2Pos[nSrc].x;
//		nSrcJ = m_CGN2Pos[nSrc].y;
//		
//		for (nDst = 0; nDst < 90; nDst++)
//		{
//			nDstI = m_CGN2Pos[nDst].x;
//			nDstJ = m_CGN2Pos[nDst].y;
//			
//			for (nKnightLeg = 0; nKnightLeg < 16; nKnightLeg++)
//			{
//				m_bIsKnightPreValidMove[nSrc][nDst][nKnightLeg] = 
//					(!(nKnightLeg & m_nMask[0]) && 
//					(nSrcI == nDstI + 2 && (nSrcJ == nDstJ + 1 || nSrcJ == nDstJ - 1))) ||//将在马左
//					(!(nKnightLeg & m_nMask[1]) && 
//					(nSrcI == nDstI - 2 && (nSrcJ == nDstJ + 1 || nSrcJ == nDstJ - 1))) ||//将在马右
//					(!(nKnightLeg & m_nMask[2]) && 
//					(nSrcJ == nDstJ + 2 && (nSrcI == nDstI + 1 || nSrcI == nDstI - 1))) ||//将在马下
//					(!(nKnightLeg & m_nMask[3]) && 
//					(nSrcJ == nDstJ - 2 && (nSrcI == nDstI + 1 || nSrcI == nDstI - 1)));  //将在马上
//			}
//		}
//	}
//}

/*******************************************************************************************************
函数名 : GenCannonPreValidMove
描  述 : 产生炮走法有效性预判断数组
参  数 : 无
返回值 : 类型为 FSVOID
版  本 : 0.1
作  者 : ahei
时  间 : 2005年10月2日 20:44:40
*******************************************************************************************************/
FSVOID CSearchEngine::GenCannonPreValidMove()
{
//	FSINT i, j;
//	FSINT nMinI, nMaxI;
//	FSINT nMinJ, nMaxJ;
//	FSINT nSrc, nDst;
//	FSINT nSrcI, nSrcJ;
//	FSINT nDstI, nDstJ;
//	FSINT nBitFile;
//	FSINT nBitRank;
//	FSINT nCnt;
//	FSINT nFEATBOOLSize = sizeof(FEATBOOL);
//
//	//行
//	for (nSrc = 0; nSrc < 90; nSrc++)
//	{
//		nSrcI = m_CGN2Pos[nSrc].x;
//		nSrcJ = m_CGN2Pos[nSrc].y;
//
//		for (nDst = 0; nDst < 90; nDst++)
//		{
//			nDstI = m_CGN2Pos[nDst].x;
//			nDstJ = m_CGN2Pos[nDst].y;
//			
//			//行
//			if (nSrcJ == nDstJ)
//			{
//				nMinI = min(nSrcI, nDstI);
//				nMaxI = max(nSrcI, nDstI);
//				
//				for (nBitFile = 0; nBitFile < 512; nBitFile++)
//				{
//					nCnt = 0;
//					for (i = nMinI + 1; i < nMaxI; i++)
//					{
//						if ((nBitFile & m_nMask[i]) != 0)
//						{
//							nCnt++;
//						}
//					}
//					if (nCnt == 0)
//					{
//						m_bIsCannonFilePreValidMove[nSrc][nDst][nBitFile][FALSE] = TRUE;
//						m_bIsCannonFilePreValidMove[nSrc][nDst][nBitFile][TRUE] = FALSE;
//					}
//					else if (nCnt == 1)
//					{
//						m_bIsCannonFilePreValidMove[nSrc][nDst][nBitFile][FALSE] = FALSE;
//						m_bIsCannonFilePreValidMove[nSrc][nDst][nBitFile][TRUE] = TRUE;
//					}
//					else
//					{
//						m_bIsCannonFilePreValidMove[nSrc][nDst][nBitFile][FALSE] = FALSE;
//						m_bIsCannonFilePreValidMove[nSrc][nDst][nBitFile][TRUE] = FALSE;
//					}
//				}
//			}
//			else
//			{
//				for (nBitFile = 0; nBitFile < 512; nBitFile++)
//				{
//					m_bIsCannonFilePreValidMove[nSrc][nDst][nBitFile][FALSE] = FALSE;
//					m_bIsCannonFilePreValidMove[nSrc][nDst][nBitFile][TRUE] = FALSE;
//				}
//			}
//
//			//列
//			if (nSrcI != nDstI)
//			{
//				for (nBitRank = 0; nBitRank < 1024; nBitRank++)
//				{
//					m_bIsCannonRankPreValidMove[nSrc][nDst][nBitRank][FALSE] = FALSE;
//					m_bIsCannonRankPreValidMove[nSrc][nDst][nBitRank][TRUE] = FALSE;
//				}
//
//				continue;
//			}
//
//			//nSrcI == nDstI
//			nMinJ = min(nSrcJ, nDstJ);
//			nMaxJ = max(nSrcJ, nDstJ);
//			
//			for (nBitRank = 0; nBitRank < 1024; nBitRank++)
//			{
//				nCnt = 0;
//				for (j = nMinJ + 1; j < nMaxJ; j++)
//				{
//					if ((nBitRank & m_nMask[j]) != 0)
//					{
//						nCnt++;
//					}
//				}
//				if (nCnt == 0)
//				{
//					m_bIsCannonRankPreValidMove[nSrc][nDst][nBitRank][FALSE] = TRUE;
//					m_bIsCannonRankPreValidMove[nSrc][nDst][nBitRank][TRUE] = FALSE;
//				}
//				else if (nCnt == 1)
//				{
//					m_bIsCannonRankPreValidMove[nSrc][nDst][nBitRank][FALSE] = FALSE;
//					m_bIsCannonRankPreValidMove[nSrc][nDst][nBitRank][TRUE] = TRUE;
//				}
//				else
//				{
//					m_bIsCannonRankPreValidMove[nSrc][nDst][nBitRank][FALSE] = FALSE;
//					m_bIsCannonRankPreValidMove[nSrc][nDst][nBitRank][TRUE] = FALSE;
//				}
//			}
//		}
//	}

	FSINT i;
	FSINT nMin, nMax;
	FSINT nSrc, nDst;
	FSINT nBit;
	FSINT nCnt;

	for (nSrc = 0; nSrc < 10; nSrc++)
	{
		for (nDst = 0; nDst < 10; nDst++)
		{
			nMin = min(nSrc, nDst);
			nMax = max(nSrc, nDst);
			
			for (nBit = 0; nBit < 1024; nBit++)
			{
				nCnt = 0;
				for (i = nMin + 1; i < nMax; i++)
				{
					if ((nBit & m_nMask[i]) != 0)
					{
						nCnt++;
					}
				}
				if (nCnt == 0)
				{
					m_bIsCannonPreValidMove[nSrc][nDst][nBit][FALSE] = TRUE;
					m_bIsCannonPreValidMove[nSrc][nDst][nBit][TRUE] = FALSE;			
				}
				else if (nCnt == 1)
				{
					m_bIsCannonPreValidMove[nSrc][nDst][nBit][FALSE] = FALSE;
					m_bIsCannonPreValidMove[nSrc][nDst][nBit][TRUE] = TRUE;
				}
				else
				{
					m_bIsCannonPreValidMove[nSrc][nDst][nBit][FALSE] = FALSE;
					m_bIsCannonPreValidMove[nSrc][nDst][nBit][TRUE] = FALSE;
				}
			}
		}
	}
}

/*******************************************************************************************************
函数名 : GenKingPreValidMove
描  述 : 产生将走法有效性预判断数组
参  数 : 无
返回值 : 类型为 FSVOID
版  本 : 0.1
作  者 : ahei
时  间 : 2005年10月2日 20:44:18
*******************************************************************************************************/
//FSVOID CSearchEngine::GenKingPreValidMove()
//{
//	FSINT i;
//	FSINT nSrc, nDst;
//
//	for (nSrc = 0; nSrc < 90; nSrc++)
//	{
//		for (nDst = 0; nDst < 90; nDst++)
//		{
//			for (i = 0; i < MAXKINGMOVECNT; i++)
//			{
//				if (nDst - nSrc == g_nKingMoveDelta[i])
//				{
//					m_bIsKingPreValidMove[nSrc][nDst] = TRUE;
//					continue;
//				}
//				m_bIsKingPreValidMove[nSrc][nDst] = FALSE;
//			}
//		}
//	}
//}

/*******************************************************************************************************
函数名 : GenAdvisorPreValidMove
描  述 : 产生士走法有效性预判断数组
参  数 : 无
返回值 : 类型为 FSVOID
版  本 : 0.1
作  者 : ahei
时  间 : 2005年10月2日 20:44:14
*******************************************************************************************************/
//FSVOID CSearchEngine::GenAdvisorPreValidMove()
//{
//
//}

/*******************************************************************************************************
函数名 : GenBishopPreValidMove
描  述 : 产生象走法有效性预判断数组
参  数 : 无
返回值 : 类型为 FSVOID
版  本 : 0.1
作  者 : ahei
时  间 : 2005年10月2日 20:44:06
*******************************************************************************************************/
//FSVOID CSearchEngine::GenBishopPreValidMove()
//{
//
//}

/*******************************************************************************************************
函数名 : GenPawnPreValidMove
描  述 : 产生兵走法有效性预判断数组
参  数 : 无
返回值 : 类型为 FSVOID 
版  本 : 0.1
作  者 : ahei
时  间 : 2005年10月2日 20:43:59
*******************************************************************************************************/
//FSVOID CSearchEngine::GenPawnPreValidMove()
//{
//
//}

/*******************************************************************************************************
函数名 : GenMoves
描  述 : 产生的走法保存在m_pMoveList中
参  数 : 
        FSINT nDepth : 此时状态在博弈树中的层数
返回值 : 无
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:31:00
*******************************************************************************************************/
inline FSVOID CSearchEngine::GenMoves(FSINT nDepth)
{
	DEBUGASSERT(nDepth >= 0 && nDepth < MAXSEARCHDEPTH);

	FSINT nPiecePos;

	FSINT nRook;
	FSINT nKnight;
	FSINT nCannon;
	FSINT nAdvisor;
	FSINT nBishop;
	FSINT nPawn;

	nRook = m_nRook[m_bIsRed];
	nKnight = m_nKnight[m_bIsRed];
	nCannon = m_nCannon[m_bIsRed];
	nAdvisor = m_nAdvisor[m_bIsRed];
	nBishop = m_nBishop[m_bIsRed];
	nPawn = m_nPawn[m_bIsRed];

	m_nMovesCnt = 0;//走法数清零

	//车
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nRook][0]]) != INVALIDPIECEPOS)
	{
		GenRookMoves(nPiecePos, nDepth);
	}
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nRook][1]]) != INVALIDPIECEPOS)
	{
		GenRookMoves(nPiecePos, nDepth);
	}
	
	//马
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nKnight][0]]) != INVALIDPIECEPOS)
	{
		GenKnightMoves(nPiecePos, nDepth);
	}
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nKnight][1]]) != INVALIDPIECEPOS)
	{
		GenKnightMoves(nPiecePos, nDepth);
	}
	
	//炮
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nCannon][0]]) != INVALIDPIECEPOS)
	{
		GenCannonMoves(nPiecePos, nDepth);
	}
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nCannon][1]]) != INVALIDPIECEPOS)
	{
		GenCannonMoves(nPiecePos, nDepth);
	}
	
	//将
	GenKingMoves(m_nPiecePos[m_nKingID2PieceID[m_bIsRed]], nDepth);
	
	//士
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nAdvisor][0]]) != INVALIDPIECEPOS)
	{
		GenAdvisorMoves(nPiecePos, nDepth);
	}
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nAdvisor][1]]) != INVALIDPIECEPOS)
	{
		GenAdvisorMoves(nPiecePos, nDepth);
	}
	
	//象
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nBishop][0]]) != INVALIDPIECEPOS)
	{
		GenBishopMoves(nPiecePos, nDepth);
	}
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nBishop][1]]) != INVALIDPIECEPOS)
	{
		GenBishopMoves(nPiecePos, nDepth);
	}
	
	//兵
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nPawn][0]]) != INVALIDPIECEPOS)
	{
		GenPawnMoves(nPiecePos, nDepth);
	}
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nPawn][1]]) != INVALIDPIECEPOS)
	{
		GenPawnMoves(nPiecePos, nDepth);
	}
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nPawn][2]]) != INVALIDPIECEPOS)
	{
		GenPawnMoves(nPiecePos, nDepth);
	}
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nPawn][3]]) != INVALIDPIECEPOS)
	{
		GenPawnMoves(nPiecePos, nDepth);
	}
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nPawn][4]]) != INVALIDPIECEPOS)
	{
		GenPawnMoves(nPiecePos, nDepth);
	}
}

/*******************************************************************************************************
函数名 : GenRookMoves
描  述 : 车的走法产生
参  数 : 
        FSINT p         : 
		FSINT nDepth    : 
返回值 : FSVOID
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:32:47
*******************************************************************************************************/
inline FSVOID CSearchEngine::GenRookMoves(FSINT p, FSINT nDepth)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);
	DEBUGASSERT(nDepth >= 0 && nDepth < MAXSEARCHDEPTH);

	FSINT i, j;
	FSINT nCount;
	FSINT nRookCheckNum;//车校验序号
	FSINT nRookFoeCheckNum;
	PRECHESSMOVE pcmFile;
	PRECHESSMOVE pcmRank;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;
	
	//行
	nRookCheckNum = m_nRookFileRankCheckNum[i][m_nBitFile[j]];
	nRookFoeCheckNum = m_nRookFoeFileRankCheckNum[i][nRookCheckNum][m_nBRBitFile[1 - m_bIsRed][j]];
	pcmFile = m_pcmRookFilePreMoves[p][nRookCheckNum][nRookFoeCheckNum];
	nCount = pcmFile.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= 8);
	memcpy(m_MoveList[nDepth] + m_nMovesCnt, pcmFile.pMoveList, m_nBytes[nCount]);
	m_nMovesCnt += nCount;

	//列
	nRookCheckNum = m_nRookFileRankCheckNum[j][m_nBitRank[i]];
	nRookFoeCheckNum = m_nRookFoeFileRankCheckNum[j][nRookCheckNum][m_nBRBitRank[1 - m_bIsRed][i]];
	pcmRank = m_pcmRookRankPreMoves[p][nRookCheckNum][nRookFoeCheckNum];
	nCount = pcmRank.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= 9);
	memcpy(m_MoveList[nDepth] + m_nMovesCnt, pcmRank.pMoveList, m_nBytes[nCount]);
	m_nMovesCnt += nCount;
}

/*******************************************************************************************************
函数名 : GenKnightMoves
描  述 : 马的走法产生
参  数 : 
        FSINT p			  : 
		FSINT nDepth      : 
		FSBOOL m_bIsRed   : 
返回值 : FSVOID
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:33:33
*******************************************************************************************************/
inline FSVOID CSearchEngine::GenKnightMoves(FSINT p, FSINT nDepth)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);
	DEBUGASSERT(nDepth >= 0 && nDepth < MAXSEARCHDEPTH);

	FSINT i, j;
	FSINT nCount;
	FSINT nKnightLegCheckSum;
	FSINT nKnightFile0AroundCheckSum;
	FSINT nKnightFile1AroundCheckSum;
	FSINT nKnightFile2AroundCheckSum;
	FSINT nKnightFile3AroundCheckSum;
	FSINT nKnightFileAroundCheckSum;

	FSINT nKnightFoeFile0AroundCheckSum;
	FSINT nKnightFoeFile1AroundCheckSum;
	FSINT nKnightFoeFile2AroundCheckSum;
	FSINT nKnightFoeFile3AroundCheckSum;
	FSINT nKnightFoeFileAroundCheckSum;

	PRECHESSMOVE pcm;
	
	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;
	
	nKnightFile0AroundCheckSum = m_nFileRankAroundCheckSum[i][m_nBitFile[m_nAbs[j][2]]];
	nKnightFile1AroundCheckSum = m_nFileRankAroundCheckSum2[i][m_nBitFile[m_nAbs[j][1]]];
	nKnightFile2AroundCheckSum = m_nFileRankAroundCheckSum2[i][m_nBitFile[j + 1]];
	nKnightFile3AroundCheckSum = m_nFileRankAroundCheckSum[i][m_nBitFile[j + 2]];

	nKnightFoeFile0AroundCheckSum = m_nFileRankAroundCheckSum[i][m_nBRBitFile[1 - m_bIsRed][m_nAbs[j][2]]];
	nKnightFoeFile1AroundCheckSum = m_nFileRankAroundCheckSum2[i][m_nBRBitFile[1 - m_bIsRed][m_nAbs[j][1]]];
	nKnightFoeFile2AroundCheckSum = m_nFileRankAroundCheckSum2[i][m_nBRBitFile[1 - m_bIsRed][j + 1]];
	nKnightFoeFile3AroundCheckSum = m_nFileRankAroundCheckSum[i][m_nBRBitFile[1 - m_bIsRed][j + 2]];

	nKnightFileAroundCheckSum = 
		m_nKnightAroundCheckSum[m_nAroundCheckSum[nKnightFile0AroundCheckSum][nKnightFile1AroundCheckSum]][m_nAroundCheckSum[nKnightFile2AroundCheckSum][nKnightFile3AroundCheckSum]];
	nKnightFoeFileAroundCheckSum = 
		m_nKnightAroundCheckSum[m_nAroundCheckSum[nKnightFoeFile0AroundCheckSum][nKnightFoeFile1AroundCheckSum]][m_nAroundCheckSum[nKnightFoeFile2AroundCheckSum][nKnightFoeFile3AroundCheckSum]];
	nKnightLegCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBitFile[j]]][m_nFileRankAroundCheckSum[j][m_nBitRank[i]]];
	
	//不吃子走法
	pcm = m_pcmKnightNoCapPreMoves[p][nKnightFileAroundCheckSum][nKnightLegCheckSum];
	nCount = pcm.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= 8);
	memcpy(m_MoveList[nDepth] + m_nMovesCnt, pcm.pMoveList, m_nBytes[nCount]);
	m_nMovesCnt += nCount;

	//吃子走法
	pcm = m_pcmKnightCapPreMoves[p][nKnightFoeFileAroundCheckSum][nKnightLegCheckSum];
	nCount = pcm.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= 8);
	memcpy(m_MoveList[nDepth] + m_nMovesCnt, pcm.pMoveList, m_nBytes[nCount]);
	m_nMovesCnt += nCount;
}

/*******************************************************************************************************
函数名 : GenCannonMoves
描  述 : 炮的走法产生
参  数 : 
        FSINT p			: 
		FSINT nDepth    : 
返回值 : inline
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:34:39
*******************************************************************************************************/
inline FSVOID CSearchEngine::GenCannonMoves(FSINT p, FSINT nDepth)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);
	DEBUGASSERT(nDepth >= 0 && nDepth < MAXSEARCHDEPTH);	

	FSINT i, j;
	FSINT nCount;
	FSINT nCannonCheckNum;
	FSINT nCannonFoeCheckNum;
	PRECHESSMOVE pcmFile;
	PRECHESSMOVE pcmRank;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	//行
	nCannonCheckNum = m_nCannonFileRankCheckNum[i][m_nBitFile[j]];
	nCannonFoeCheckNum =
		m_nCannonFoeFileRankCheckNum[i][nCannonCheckNum][m_nBRBitFile[1 - m_bIsRed][j]];
	pcmFile = m_pcmCannonFilePreMoves[p][nCannonCheckNum][nCannonFoeCheckNum];
	nCount = pcmFile.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= 8);
	memcpy(m_MoveList[nDepth] + m_nMovesCnt, pcmFile.pMoveList, m_nBytes[nCount]);
	m_nMovesCnt += nCount;

	//列
	nCannonCheckNum = m_nCannonFileRankCheckNum[j][m_nBitRank[i]];
	nCannonFoeCheckNum = 
		m_nCannonFoeFileRankCheckNum[j][nCannonCheckNum][m_nBRBitRank[1 - m_bIsRed][i]];
	pcmRank = m_pcmCannonRankPreMoves[p][nCannonCheckNum][nCannonFoeCheckNum];
	nCount = pcmRank.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= 9);
	memcpy(m_MoveList[nDepth] + m_nMovesCnt, pcmRank.pMoveList, m_nBytes[nCount]);
	m_nMovesCnt += nCount;
}

/*******************************************************************************************************
函数名 : GenKingMoves
描  述 : 将的走法产生
参  数 : 
        FSINT p		: 
		FSINT nDepth  : 
返回值 : inline
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:35:11
*******************************************************************************************************/
inline FSVOID CSearchEngine::GenKingMoves(FSINT p,FSINT nDepth)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);
	DEBUGASSERT(nDepth >= 0 && nDepth < MAXSEARCHDEPTH);

	FSINT i, j;
	FSINT nCount;
	FSINT nPosCheckNum;
	FSINT nAroundCheckSum;
	FSINT nFoeAroundCheckSum;
	PRECHESSMOVE pcm;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	nPosCheckNum = m_nKingPosCheckNum[p];
	DEBUGASSERT(nPosCheckNum >= MINKINGPOSCHECKNUM && nPosCheckNum <= MAXKINGPOSCHECKNUM);
	nAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBitFile[j]]][m_nFileRankAroundCheckSum[j][m_nBitRank[i]]];
	nFoeAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBRBitFile[1 - m_bIsRed][j]]][m_nFileRankAroundCheckSum[j][m_nBRBitRank[1 - m_bIsRed][i]]];
	
	pcm = m_pcmKingPreMoves[nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum];
	nCount = pcm.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= 4);
	memcpy(m_MoveList[nDepth] + m_nMovesCnt, pcm.pMoveList, m_nBytes[nCount]);
	m_nMovesCnt += nCount;
}

/*******************************************************************************************************
函数名 : GenAdvisorMoves
描  述 : 士的走法产生
参  数 : 
        FSINT p		    : 
		FSINT nDepth    : 
		FSBOOL m_bIsRed : 
返回值 : inline
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:35:42
*******************************************************************************************************/
inline FSVOID CSearchEngine::GenAdvisorMoves(FSINT p,FSINT nDepth)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);
	DEBUGASSERT(nDepth >= 0 && nDepth < MAXSEARCHDEPTH);	

	FSINT i, j;
	FSINT nCount;
	FSINT nPosCheckNum;
	FSINT nAroundCheckSum;
	FSINT nFoeAroundCheckSum;
	PRECHESSMOVE pcm;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	nPosCheckNum = m_nAdvisorPosCheckNum[p];
	nAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBitFile[m_nAbs[j][1]]]][m_nFileRankAroundCheckSum[i][m_nBitFile[j + 1]]];
	nFoeAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBRBitFile[1 - m_bIsRed][m_nAbs[j][1]]]][m_nFileRankAroundCheckSum[i][m_nBRBitFile[1 - m_bIsRed][j + 1]]];
	
	pcm = m_pcmAdvisorPreMoves[nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum];
	nCount = pcm.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= 4);
	memcpy(m_MoveList[nDepth] + m_nMovesCnt, pcm.pMoveList, m_nBytes[nCount]);
	m_nMovesCnt += nCount;
}

/*******************************************************************************************************
函数名 : GenBishopMoves
描  述 : 象的走法产生
参  数 : 
        FSINT p       : 
		FSINT nDepth  : 
返回值 : inline
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:36:14
*******************************************************************************************************/
inline FSVOID CSearchEngine::GenBishopMoves(FSINT p,FSINT nDepth)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);
	DEBUGASSERT(nDepth >= 0 && nDepth < MAXSEARCHDEPTH);

	FSINT i, j;
	FSINT nCount;
	FSINT nPosCheckNum;
	FSINT nElephantEyeCheckSum;
	FSINT nAroundCheckSum;
	FSINT nFoeAroundCheckSum;
	PRECHESSMOVE pcm;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	nPosCheckNum = m_nBishopPosCheckNum[p];
	nAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum2[i][m_nBitFile[m_nAbs[j][2]]]][m_nFileRankAroundCheckSum2[i][m_nBitFile[j + 2]]];
	nFoeAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum2[i][m_nBRBitFile[1 - m_bIsRed][m_nAbs[j][2]]]][m_nFileRankAroundCheckSum2[i][m_nBRBitFile[1 - m_bIsRed][j + 2]]];
	nElephantEyeCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBitFile[m_nAbs[j][1]]]][m_nFileRankAroundCheckSum[i][m_nBitFile[j + 1]]];

	pcm = 
		m_pcmBishopPreMoves[nPosCheckNum][nElephantEyeCheckSum][nAroundCheckSum][nFoeAroundCheckSum];
	nCount = pcm.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= 4);
	memcpy(m_MoveList[nDepth] + m_nMovesCnt, pcm.pMoveList, m_nBytes[nCount]);
	m_nMovesCnt += nCount;
}

/*******************************************************************************************************
函数名 : GenPawnMoves
描  述 : 兵的走法产生
参  数 : 
        FSINT p         : 
		FSINT nDepth    : 
返回值 : 无
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:36:43
*******************************************************************************************************/
inline FSVOID CSearchEngine::GenPawnMoves(FSINT p, FSINT nDepth)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);
	DEBUGASSERT(nDepth >= 0 && nDepth < MAXSEARCHDEPTH);

	FSINT i, j;
	FSINT nCount;
	FSINT nPosCheckNum;
	FSINT nAroundCheckSum;
	FSINT nFoeAroundCheckSum;
	PRECHESSMOVE pcm;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	nPosCheckNum = m_nPawnPosCheckNum[m_bIsRed][p];
	nAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBitFile[j]]][m_nPawnRankAroundCheckSum[m_bIsRed][j][m_nBitRank[i]]];
	nFoeAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBRBitFile[1 - m_bIsRed][j]]][m_nPawnRankAroundCheckSum[m_bIsRed][j][m_nBRBitRank[1 - m_bIsRed][i]]];

	pcm = m_PawnPreMoves[m_bIsRed][nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum];
	nCount = pcm.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= MAXPAWNCAPMOVECNT);
	memcpy(m_MoveList[nDepth] + m_nMovesCnt, pcm.pMoveList, m_nBytes[nCount]);
	m_nMovesCnt += nCount;
}

/*******************************************************************************************************
函数名 : GenNoCapMoves
描  述 : 
参  数 : 
        FSINT nDepth : 
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年10月23日 17:15:46
*******************************************************************************************************/
FSVOID CSearchEngine::GenNoCapMoves(FSINT nDepth)
{
	DEBUGASSERT(nDepth >= 0 && nDepth < MAXSEARCHDEPTH);

	FSINT nPiecePos;

	FSINT nRook;
	FSINT nKnight;
	FSINT nCannon;
	FSINT nAdvisor;
	FSINT nBishop;
	FSINT nPawn;

	nRook = m_nRook[m_bIsRed];
	nKnight = m_nKnight[m_bIsRed];
	nCannon = m_nCannon[m_bIsRed];
	nAdvisor = m_nAdvisor[m_bIsRed];
	nBishop = m_nBishop[m_bIsRed];
	nPawn = m_nPawn[m_bIsRed];

	m_nMovesCnt = 0;	//走法数清零

	//车
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nRook][0]]) != INVALIDPIECEPOS)
	{
		GenRookNoCapMoves(nPiecePos, nDepth);
	}
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nRook][1]]) != INVALIDPIECEPOS)
	{
		GenRookNoCapMoves(nPiecePos, nDepth);
	}
	
	//马
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nKnight][0]]) != INVALIDPIECEPOS)
	{
		GenKnightNoCapMoves(nPiecePos, nDepth);
	}
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nKnight][1]]) != INVALIDPIECEPOS)
	{
		GenKnightNoCapMoves(nPiecePos, nDepth);
	}
	
	//炮
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nCannon][0]]) != INVALIDPIECEPOS)
	{
		GenCannonNoCapMoves(nPiecePos, nDepth);
	}
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nCannon][1]]) != INVALIDPIECEPOS)
	{
		GenCannonNoCapMoves(nPiecePos, nDepth);
	}
	
	//将
	GenKingNoCapMoves(m_nPiecePos[m_nKingID2PieceID[m_bIsRed]], nDepth);
	
	//士
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nAdvisor][0]]) != INVALIDPIECEPOS)
	{
		GenAdvisorNoCapMoves(nPiecePos, nDepth);
	}
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nAdvisor][1]]) != INVALIDPIECEPOS)
	{
		GenAdvisorNoCapMoves(nPiecePos, nDepth);
	}
	
	//象
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nBishop][0]]) != INVALIDPIECEPOS)
	{
		GenBishopNoCapMoves(nPiecePos, nDepth);
	}
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nBishop][1]]) != INVALIDPIECEPOS)
	{
		GenBishopNoCapMoves(nPiecePos, nDepth);
	}
	
	//兵
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nPawn][0]]) != INVALIDPIECEPOS)
	{
		GenPawnNoCapMoves(nPiecePos, nDepth);
	}
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nPawn][1]]) != INVALIDPIECEPOS)
	{
		GenPawnNoCapMoves(nPiecePos, nDepth);
	}
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nPawn][2]]) != INVALIDPIECEPOS)
	{
		GenPawnNoCapMoves(nPiecePos, nDepth);
	}
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nPawn][3]]) != INVALIDPIECEPOS)
	{
		GenPawnNoCapMoves(nPiecePos, nDepth);
	}
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nPawn][4]]) != INVALIDPIECEPOS)
	{
		GenPawnNoCapMoves(nPiecePos, nDepth);
	}
}

/*******************************************************************************************************
函数名 : GenRookNoCapMoves
描  述 : 
参  数 : 
        FSINT p      : 
		FSINT nDepth : 
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年10月23日 17:15:55
*******************************************************************************************************/
FSVOID CSearchEngine::GenRookNoCapMoves(FSINT p, FSINT nDepth)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);
	DEBUGASSERT(nDepth >= 0 && nDepth < MAXSEARCHDEPTH);

	FSINT i, j;
	FSINT nCount;
	FSINT nRookCheckNum;//车校验序号
	PRECHESSMOVE pcmFile;
	PRECHESSMOVE pcmRank;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;
	
	//行
	nRookCheckNum = m_nRookFileRankCheckNum[i][m_nBitFile[j]];
	pcmFile = m_pcmRookFileNoCapPreMoves[p][nRookCheckNum];
	nCount = pcmFile.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= MAXROOKFILENOCAPMOVECNT);
	memcpy(m_MoveList[nDepth] + m_nMovesCnt, pcmFile.pMoveList, m_nBytes[nCount]);
	m_nMovesCnt += nCount;

	//列
	nRookCheckNum = m_nRookFileRankCheckNum[j][m_nBitRank[i]];
	pcmRank = m_pcmRookRankNoCapPreMoves[p][nRookCheckNum];
	nCount = pcmRank.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= MAXROOKRANKNOCAPMOVECNT);
	memcpy(m_MoveList[nDepth] + m_nMovesCnt, pcmRank.pMoveList, m_nBytes[nCount]);
	m_nMovesCnt += nCount;
}

/*******************************************************************************************************
函数名 : GenKnightNoCapMoves
描  述 : 
参  数 : 
        FSINT p      : 
		FSINT nDepth : 
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年10月23日 17:16:03
*******************************************************************************************************/
FSVOID CSearchEngine::GenKnightNoCapMoves(FSINT p, FSINT nDepth)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);
	DEBUGASSERT(nDepth >= 0 && nDepth < MAXSEARCHDEPTH);

	FSINT i, j;
	FSINT nCount;
	FSINT nKnightLegCheckSum;
	FSINT nKnightFile0AroundCheckSum;
	FSINT nKnightFile1AroundCheckSum;
	FSINT nKnightFile2AroundCheckSum;
	FSINT nKnightFile3AroundCheckSum;
	FSINT nKnightFileAroundCheckSum;

	PRECHESSMOVE pcm;
	
	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;
	
	nKnightFile0AroundCheckSum = m_nFileRankAroundCheckSum[i][m_nBitFile[m_nAbs[j][2]]];
	nKnightFile1AroundCheckSum = m_nFileRankAroundCheckSum2[i][m_nBitFile[m_nAbs[j][1]]];
	nKnightFile2AroundCheckSum = m_nFileRankAroundCheckSum2[i][m_nBitFile[j + 1]];
	nKnightFile3AroundCheckSum = m_nFileRankAroundCheckSum[i][m_nBitFile[j + 2]];

	nKnightFileAroundCheckSum = 
		m_nKnightAroundCheckSum[m_nAroundCheckSum[nKnightFile0AroundCheckSum][nKnightFile1AroundCheckSum]][m_nAroundCheckSum[nKnightFile2AroundCheckSum][nKnightFile3AroundCheckSum]];
	nKnightLegCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBitFile[j]]][m_nFileRankAroundCheckSum[j][m_nBitRank[i]]];
	
	//不吃子走法
	pcm = m_pcmKnightNoCapPreMoves[p][nKnightFileAroundCheckSum][nKnightLegCheckSum];
	nCount = pcm.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= MAXKNIGHTNOCAPMOVECNT);
	memcpy(m_MoveList[nDepth] + m_nMovesCnt, pcm.pMoveList, m_nBytes[nCount]);
	m_nMovesCnt += nCount;
}

/*******************************************************************************************************
函数名 : GenCannonNoCapMoves
描  述 : 
参  数 : 
        FSINT p      : 
		FSINT nDepth : 
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年10月23日 17:16:09
*******************************************************************************************************/
FSVOID CSearchEngine::GenCannonNoCapMoves(FSINT p, FSINT nDepth)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);
	DEBUGASSERT(nDepth >= 0 && nDepth < MAXSEARCHDEPTH);	

	FSINT i, j;
	FSINT nCount;
	FSINT nCannonCheckNum;
	PRECHESSMOVE pcmFile;
	PRECHESSMOVE pcmRank;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	//行
	nCannonCheckNum = m_nCannonFileRankCheckNum[i][m_nBitFile[j]];
	pcmFile = m_pcmCannonFileNoCapPreMoves[p][nCannonCheckNum];
	nCount = pcmFile.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= MAXCANNONFILENOCAPMOVECNT);
	memcpy(m_MoveList[nDepth] + m_nMovesCnt, pcmFile.pMoveList, m_nBytes[nCount]);
	m_nMovesCnt += nCount;

	//列
	nCannonCheckNum = m_nCannonFileRankCheckNum[j][m_nBitRank[i]];
	pcmRank = m_pcmCannonRankNoCapPreMoves[p][nCannonCheckNum];
	nCount = pcmRank.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= MAXCANNONRANKNOCAPMOVECNT);
	memcpy(m_MoveList[nDepth] + m_nMovesCnt, pcmRank.pMoveList, m_nBytes[nCount]);
	m_nMovesCnt += nCount;
}

/*******************************************************************************************************
函数名 : GenKingNoCapMoves
描  述 : 
参  数 : 
        FSINT p      : 
		FSINT nDepth : 
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年10月23日 17:16:15
*******************************************************************************************************/
FSVOID CSearchEngine::GenKingNoCapMoves(FSINT p, FSINT nDepth)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);
	DEBUGASSERT(nDepth >= 0 && nDepth < MAXSEARCHDEPTH);

	FSINT i, j;
	FSINT nCount;
	FSINT nPosCheckNum;
	FSINT nAroundCheckSum;
	PRECHESSMOVE pcm;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	nPosCheckNum = m_nKingPosCheckNum[p];
	nAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBitFile[j]]][m_nFileRankAroundCheckSum[j][m_nBitRank[i]]];
	
	pcm = m_pcmKingNoCapPreMoves[nPosCheckNum][nAroundCheckSum];
	nCount = pcm.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= MAXKINGNOCAPMOVECNT);
	memcpy(m_MoveList[nDepth] + m_nMovesCnt, pcm.pMoveList, m_nBytes[nCount]);
	m_nMovesCnt += nCount;
}

/*******************************************************************************************************
函数名 : GenAdvisorNoCapMoves
描  述 : 
参  数 : 
        FSINT p      : 
		FSINT nDepth : 
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年10月23日 17:16:21
*******************************************************************************************************/
FSVOID CSearchEngine::GenAdvisorNoCapMoves(FSINT p, FSINT nDepth)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);
	DEBUGASSERT(nDepth >= 0 && nDepth < MAXSEARCHDEPTH);	

	FSINT i, j;
	FSINT nCount;
	FSINT nPosCheckNum;
	FSINT nAroundCheckSum;
	PRECHESSMOVE pcm;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	nPosCheckNum = m_nAdvisorPosCheckNum[p];
	nAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBitFile[m_nAbs[j][1]]]][m_nFileRankAroundCheckSum[i][m_nBitFile[j + 1]]];
	
	pcm = m_pcmAdvisorNoCapPreMoves[nPosCheckNum][nAroundCheckSum];
	nCount = pcm.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= 4);
	memcpy(m_MoveList[nDepth] + m_nMovesCnt, pcm.pMoveList, m_nBytes[nCount]);
	m_nMovesCnt += nCount;
}

/*******************************************************************************************************
函数名 : GenBishopNoCapMoves
描  述 : 
参  数 : 
        FSINT p      : 
		FSINT nDepth : 
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年10月23日 17:16:26
*******************************************************************************************************/
FSVOID CSearchEngine::GenBishopNoCapMoves(FSINT p, FSINT nDepth)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);
	DEBUGASSERT(nDepth >= 0 && nDepth < MAXSEARCHDEPTH);

	FSINT i, j;
	FSINT nCount;
	FSINT nPosCheckNum;
	FSINT nElephantEyeCheckSum;
	FSINT nAroundCheckSum;
	PRECHESSMOVE pcm;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	nPosCheckNum = m_nBishopPosCheckNum[p];
	nAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum2[i][m_nBitFile[m_nAbs[j][2]]]][m_nFileRankAroundCheckSum2[i][m_nBitFile[j + 2]]];
	nElephantEyeCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBitFile[m_nAbs[j][1]]]][m_nFileRankAroundCheckSum[i][m_nBitFile[j + 1]]];

	pcm = m_pcmBishopNoCapPreMoves[nPosCheckNum][nElephantEyeCheckSum][nAroundCheckSum];
	nCount = pcm.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= 4);
	memcpy(m_MoveList[nDepth] + m_nMovesCnt, pcm.pMoveList, m_nBytes[nCount]);
	m_nMovesCnt += nCount;
}

/*******************************************************************************************************
函数名 : GenPawnNoCapMoves
描  述 : 
参  数 : 
        FSINT p      : 
		FSINT nDepth : 
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年10月23日 17:16:31
*******************************************************************************************************/
FSVOID CSearchEngine::GenPawnNoCapMoves(FSINT p, FSINT nDepth)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);
	DEBUGASSERT(nDepth >= 0 && nDepth < MAXSEARCHDEPTH);

	FSINT i, j;
	FSINT nCount;
	FSINT nPosCheckNum;
	FSINT nAroundCheckSum;
	PRECHESSMOVE pcm;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	nPosCheckNum = m_nPawnPosCheckNum[m_bIsRed][p];
	nAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBitFile[j]]][m_nPawnRankAroundCheckSum[m_bIsRed][j][m_nBitRank[i]]];

	pcm = m_PawnNoCapPreMoves[m_bIsRed][nPosCheckNum][nAroundCheckSum];
	nCount = pcm.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= MAXPAWNNOCAPMOVECNT);
	memcpy(m_MoveList[nDepth] + m_nMovesCnt, pcm.pMoveList, m_nBytes[nCount]);
	m_nMovesCnt += nCount;
}

/*******************************************************************************************************
函数名 : GenCapMoves
描  述 : 
参  数 : 
        FSINT nDepth : 
返回值 : 类型为 FSVOID
版  本 : 0.1
作  者 : ahei
时  间 : 2005年10月4日 22:27:51
*******************************************************************************************************/
FSVOID CSearchEngine::GenCapMoves(FSINT nDepth)
{
	DEBUGASSERT(nDepth >= 0 && nDepth < MAXSEARCHDEPTH);

	FSINT nPiecePos;

	FSINT nRook;
	FSINT nKnight;
	FSINT nCannon;
	FSINT nAdvisor;
	FSINT nBishop;
	FSINT nPawn;

	nRook = m_nRook[m_bIsRed];
	nKnight = m_nKnight[m_bIsRed];
	nCannon = m_nCannon[m_bIsRed];
	nAdvisor = m_nAdvisor[m_bIsRed];
	nBishop = m_nBishop[m_bIsRed];
	nPawn = m_nPawn[m_bIsRed];

	m_nMovesCnt = 0;	//走法数清零

	//车
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nRook][0]]) != INVALIDPIECEPOS)
	{
		GenRookCapMoves(nPiecePos, nDepth);
	}
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nRook][1]]) != INVALIDPIECEPOS)
	{
		GenRookCapMoves(nPiecePos, nDepth);
	}
	
	//马
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nKnight][0]]) != INVALIDPIECEPOS)
	{
		GenKnightCapMoves(nPiecePos, nDepth);
	}
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nKnight][1]]) != INVALIDPIECEPOS)
	{
		GenKnightCapMoves(nPiecePos, nDepth);
	}
	
	//炮
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nCannon][0]]) != INVALIDPIECEPOS)
	{
		GenCannonCapMoves(nPiecePos, nDepth);
	}
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nCannon][1]]) != INVALIDPIECEPOS)
	{
		GenCannonCapMoves(nPiecePos, nDepth);
	}
	
	//将
	GenKingCapMoves(m_nPiecePos[m_nKingID2PieceID[m_bIsRed]], nDepth);
	
	//士
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nAdvisor][0]]) != INVALIDPIECEPOS)
	{
		GenAdvisorCapMoves(nPiecePos, nDepth);
	}
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nAdvisor][1]]) != INVALIDPIECEPOS)
	{
		GenAdvisorCapMoves(nPiecePos, nDepth);
	}
	
	//象
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nBishop][0]]) != INVALIDPIECEPOS)
	{
		GenBishopCapMoves(nPiecePos, nDepth);
	}
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nBishop][1]]) != INVALIDPIECEPOS)
	{
		GenBishopCapMoves(nPiecePos, nDepth);
	}
	
	//兵
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nPawn][0]]) != INVALIDPIECEPOS)
	{
		GenPawnCapMoves(nPiecePos, nDepth);
	}
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nPawn][1]]) != INVALIDPIECEPOS)
	{
		GenPawnCapMoves(nPiecePos, nDepth);
	}
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nPawn][2]]) != INVALIDPIECEPOS)
	{
		GenPawnCapMoves(nPiecePos, nDepth);
	}
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nPawn][3]]) != INVALIDPIECEPOS)
	{
		GenPawnCapMoves(nPiecePos, nDepth);
	}
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nPawn][4]]) != INVALIDPIECEPOS)
	{
		GenPawnCapMoves(nPiecePos, nDepth);
	}
}

/*******************************************************************************************************
函数名 : GenRookCapMoves
描  述 : 
参  数 : 
        FSINT p      : 
		FSINT nDepth : 
返回值 : 类型为 FSVOID
版  本 : 0.1
作  者 : ahei
时  间 : 2005年10月4日 22:27:55
*******************************************************************************************************/
inline FSVOID CSearchEngine::GenRookCapMoves(FSINT p, FSINT nDepth)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);
	DEBUGASSERT(nDepth >= 0 && nDepth < MAXSEARCHDEPTH);

	FSINT i, j;
	FSINT nCount;
	FSINT nRookCheckNum;//车校验序号
	FSINT nRookFoeCheckNum;
	PRECHESSMOVE pcmFile;
	PRECHESSMOVE pcmRank;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;
	
	//行
	nRookCheckNum = m_nRookFileRankCheckNum[i][m_nBitFile[j]];
	nRookFoeCheckNum = m_nRookFoeFileRankCheckNum[i][nRookCheckNum][m_nBRBitFile[1 - m_bIsRed][j]];
	pcmFile = m_pcmRookFileCapPreMoves[p][nRookCheckNum][nRookFoeCheckNum];
	nCount = pcmFile.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= MAXROOKFILECAPMOVECNT);
	memcpy(m_MoveList[nDepth] + m_nMovesCnt, pcmFile.pMoveList, m_nBytes[nCount]);
	m_nMovesCnt += nCount;

	//列
	nRookCheckNum = m_nRookFileRankCheckNum[j][m_nBitRank[i]];
	nRookFoeCheckNum = m_nRookFoeFileRankCheckNum[j][nRookCheckNum][m_nBRBitRank[1 - m_bIsRed][i]];
	pcmRank = m_pcmRookRankCapPreMoves[p][nRookCheckNum][nRookFoeCheckNum];
	nCount = pcmRank.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= MAXROOKRANKCAPMOVECNT);
	memcpy(m_MoveList[nDepth] + m_nMovesCnt, pcmRank.pMoveList, m_nBytes[nCount]);
	m_nMovesCnt += nCount;
}

/*******************************************************************************************************
函数名 : GenKnightCapMoves
描  述 : 
参  数 : 
        FSINT p      : 
		FSINT nDepth : 
返回值 : 类型为 FSVOID
版  本 : 0.1
作  者 : ahei
时  间 : 2005年10月4日 22:28:01
*******************************************************************************************************/
inline FSVOID CSearchEngine::GenKnightCapMoves(FSINT p, FSINT nDepth)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);
	DEBUGASSERT(nDepth >= 0 && nDepth < MAXSEARCHDEPTH);

	FSINT i, j;
	FSINT nCount;
	PRECHESSMOVE pcm;
	FSINT nKnightLegCheckSum;

	FSINT nKnightFoeFile0AroundCheckSum;
	FSINT nKnightFoeFile1AroundCheckSum;
	FSINT nKnightFoeFile2AroundCheckSum;
	FSINT nKnightFoeFile3AroundCheckSum;
	FSINT nKnightFoeFileAroundCheckSum;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	nKnightFoeFile0AroundCheckSum = m_nFileRankAroundCheckSum[i][m_nBRBitFile[1 - m_bIsRed][m_nAbs[j][2]]];
	nKnightFoeFile1AroundCheckSum = m_nFileRankAroundCheckSum2[i][m_nBRBitFile[1 - m_bIsRed][m_nAbs[j][1]]];
	nKnightFoeFile2AroundCheckSum = m_nFileRankAroundCheckSum2[i][m_nBRBitFile[1 - m_bIsRed][j + 1]];
	nKnightFoeFile3AroundCheckSum = m_nFileRankAroundCheckSum[i][m_nBRBitFile[1 - m_bIsRed][j + 2]];

	nKnightFoeFileAroundCheckSum = 
		m_nKnightAroundCheckSum[m_nAroundCheckSum[nKnightFoeFile0AroundCheckSum][nKnightFoeFile1AroundCheckSum]][m_nAroundCheckSum[nKnightFoeFile2AroundCheckSum][nKnightFoeFile3AroundCheckSum]];
	nKnightLegCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBitFile[j]]][m_nFileRankAroundCheckSum[j][m_nBitRank[i]]];

	//吃子走法
	pcm = m_pcmKnightCapPreMoves[p][nKnightFoeFileAroundCheckSum][nKnightLegCheckSum];
	nCount = pcm.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= MAXKNIGHTCAPMOVECNT);
	memcpy(m_MoveList[nDepth] + m_nMovesCnt, pcm.pMoveList, m_nBytes[nCount]);
	m_nMovesCnt += nCount;
}

/*******************************************************************************************************
函数名 : GenCannonCapMoves
描  述 : 
参  数 : 
        FSINT p      : 
		FSINT nDepth : 
返回值 : 类型为 FSVOID
版  本 : 0.1
作  者 : ahei
时  间 : 2005年10月4日 22:28:05
*******************************************************************************************************/
inline FSVOID CSearchEngine::GenCannonCapMoves(FSINT p, FSINT nDepth)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);
	DEBUGASSERT(nDepth >= 0 && nDepth < MAXSEARCHDEPTH);	

	FSINT i, j;
	FSINT nCount;
	FSINT nCannonCheckNum;
	FSINT nCannonFoeCheckNum;
	PRECHESSMOVE pcmFile;
	PRECHESSMOVE pcmRank;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	//行
	nCannonCheckNum = m_nCannonFileRankCheckNum[i][m_nBitFile[j]];
	nCannonFoeCheckNum =
		m_nCannonFoeFileRankCheckNum[i][nCannonCheckNum][m_nBRBitFile[1 - m_bIsRed][j]];
	pcmFile = m_pcmCannonFileCapPreMoves[p][nCannonCheckNum][nCannonFoeCheckNum];
	nCount = pcmFile.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= 2);
	memcpy(m_MoveList[nDepth] + m_nMovesCnt, pcmFile.pMoveList, m_nBytes[nCount]);
	m_nMovesCnt += nCount;

	//列
	nCannonCheckNum = m_nCannonFileRankCheckNum[j][m_nBitRank[i]];
	nCannonFoeCheckNum = 
		m_nCannonFoeFileRankCheckNum[j][nCannonCheckNum][m_nBRBitRank[1 - m_bIsRed][i]];
	pcmRank = m_pcmCannonRankCapPreMoves[p][nCannonCheckNum][nCannonFoeCheckNum];
	nCount = pcmRank.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= 2);
	memcpy(m_MoveList[nDepth] + m_nMovesCnt, pcmRank.pMoveList, m_nBytes[nCount]);
	m_nMovesCnt += nCount;
}

/*******************************************************************************************************
函数名 : GenKingCapMoves
描  述 : 
参  数 : 
        FSINT p      : 
		FSINT nDepth : 
返回值 : 类型为 FSVOID
版  本 : 0.1
作  者 : ahei
时  间 : 2005年10月4日 22:28:11
*******************************************************************************************************/
inline FSVOID CSearchEngine::GenKingCapMoves(FSINT p, FSINT nDepth)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);
	DEBUGASSERT(nDepth >= 0 && nDepth < MAXSEARCHDEPTH);

	FSINT i, j;
	FSINT nCount;
	FSINT nPosCheckNum;
	FSINT nFoeAroundCheckSum;
	PRECHESSMOVE pcm;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	nPosCheckNum = m_nKingPosCheckNum[p];
	nFoeAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBRBitFile[1 - m_bIsRed][j]]][m_nFileRankAroundCheckSum[j][m_nBRBitRank[1 - m_bIsRed][i]]];
	
	pcm = m_pcmKingCapPreMoves[nPosCheckNum][nFoeAroundCheckSum];
	nCount = pcm.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= MAXKINGCAPMOVECNT);
	memcpy(m_MoveList[nDepth] + m_nMovesCnt, pcm.pMoveList, m_nBytes[nCount]);
	m_nMovesCnt += nCount;
}

/*******************************************************************************************************
函数名 : GenAdvisorCapMoves
描  述 : 
参  数 : 
        FSINT p      : 
		FSINT nDepth : 
返回值 : 类型为 FSVOID
版  本 : 0.1
作  者 : ahei
时  间 : 2005年10月4日 22:28:17
*******************************************************************************************************/
inline FSVOID CSearchEngine::GenAdvisorCapMoves(FSINT p, FSINT nDepth)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);
	DEBUGASSERT(nDepth >= 0 && nDepth < MAXSEARCHDEPTH);	

	FSINT i, j;
	FSINT nCount;
	FSINT nPosCheckNum;
	FSINT nFoeAroundCheckSum;
	PRECHESSMOVE pcm;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	nPosCheckNum = m_nAdvisorPosCheckNum[p];
	DEBUGASSERT(nPosCheckNum >= 0);
	nFoeAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBRBitFile[1 - m_bIsRed][m_nAbs[j][1]]]][m_nFileRankAroundCheckSum[i][m_nBRBitFile[1 - m_bIsRed][j + 1]]];
	
	pcm = m_pcmAdvisorCapPreMoves[nPosCheckNum][nFoeAroundCheckSum];
	nCount = pcm.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= MAXADVISORCAPMOVECNT);
	memcpy(m_MoveList[nDepth] + m_nMovesCnt, pcm.pMoveList, m_nBytes[nCount]);
	m_nMovesCnt += nCount;
}

/*******************************************************************************************************
函数名 : GenBishopCapMoves
描  述 : 
参  数 : 
        FSINT p      : 
		FSINT nDepth : 
返回值 : 类型为 FSVOID
版  本 : 0.1
作  者 : ahei
时  间 : 2005年10月4日 22:28:22
*******************************************************************************************************/
inline FSVOID CSearchEngine::GenBishopCapMoves(FSINT p, FSINT nDepth)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);
	DEBUGASSERT(nDepth >= 0 && nDepth < MAXSEARCHDEPTH);

	FSINT i, j;
	FSINT nCount;
	FSINT nPosCheckNum;
	FSINT nElephantEyeCheckSum;
	FSINT nFoeAroundCheckSum;
	PRECHESSMOVE pcm;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	nPosCheckNum = m_nBishopPosCheckNum[p];
	nFoeAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum2[i][m_nBRBitFile[1 - m_bIsRed][m_nAbs[j][2]]]][m_nFileRankAroundCheckSum2[i][m_nBRBitFile[1 - m_bIsRed][j + 2]]];
	nElephantEyeCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBitFile[m_nAbs[j][1]]]][m_nFileRankAroundCheckSum[i][m_nBitFile[j + 1]]];

	pcm = 
		m_pcmBishopCapPreMoves[nPosCheckNum][nElephantEyeCheckSum][nFoeAroundCheckSum];
	nCount = pcm.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= MAXBISHOPCAPMOVECNT);
	memcpy(m_MoveList[nDepth] + m_nMovesCnt, pcm.pMoveList,
		   m_nBytes[nCount]);
	m_nMovesCnt += nCount;
}

/*******************************************************************************************************
函数名 : GenPawnCapMoves
描  述 : 
参  数 : 
        FSINT p      : 
		FSINT nDepth : 
返回值 : 类型为 FSVOID
版  本 : 0.1
作  者 : ahei
时  间 : 2005年10月4日 22:28:27
*******************************************************************************************************/
inline FSVOID CSearchEngine::GenPawnCapMoves(FSINT p, FSINT nDepth)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);
	DEBUGASSERT(nDepth >= 0 && nDepth < MAXSEARCHDEPTH);

	FSINT i, j;
	FSINT nCount;
	FSINT nPosCheckNum;
	FSINT nFoeAroundCheckSum;
	PRECHESSMOVE pcm;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	nPosCheckNum = m_nPawnPosCheckNum[m_bIsRed][p];
	nFoeAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBRBitFile[1 - m_bIsRed][j]]][m_nPawnRankAroundCheckSum[m_bIsRed][j][m_nBRBitRank[1 - m_bIsRed][i]]];

	pcm = m_PawnCapPreMoves[m_bIsRed][nPosCheckNum][nFoeAroundCheckSum];
	nCount = pcm.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= MAXPAWNCAPMOVECNT);
	memcpy(m_MoveList[nDepth] + m_nMovesCnt, pcm.pMoveList, m_nBytes[nCount]);
	m_nMovesCnt += nCount;
}

/*******************************************************************************************************
函数名 : IsInCity
描  述 : 
参  数 : 
        FSINT p : 
返回值 : FSBOOL
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:39:38
*******************************************************************************************************/
inline FSBOOL CSearchEngine::IsInCity(FSINT p)
{
	if (p < 0 || p > 89)
	{
		return FALSE;
	}

	FSINT i, j;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	return IsInCity(i,j);
}

/*******************************************************************************************************
函数名 : IsInCity
描  述 : (i, j)是否在九宫内
参  数 : 
        FSINT i : 
		FSINT j : 
返回值 : inline
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:39:48
*******************************************************************************************************/
inline FSBOOL CSearchEngine::IsInCity(FSINT i, FSINT j)
{
	return (i > 3) && (i <= 5) && ((j >= 0 && j <= 2) || (j >= 7 && j <= 9));
}

/*******************************************************************************************************
函数名 : IsInBoard
描  述 : 
参  数 : 
        FSINT p : 
返回值 : inline
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:39:57
*******************************************************************************************************/
inline FSBOOL CSearchEngine::IsInBoard(FSINT p)
{
	return (p >= 0) && (p < 90);
}

/*******************************************************************************************************
函数名 : IsInBoard
描  述 : 
参  数 : 
        FSINT i : 
		FSINT j : 
返回值 : inline
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:40:06
*******************************************************************************************************/
inline FSBOOL CSearchEngine::IsInBoard(FSINT i, FSINT j)
{
	return (i >= 0) && (i < 9) && (j >= 0) && (j < 10);
}

/*******************************************************************************************************
函数名 : IsInBBoard
描  述 : 
参  数 : 
        FSINT p : 
返回值 : inline
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:40:14
*******************************************************************************************************/
inline FSBOOL CSearchEngine::IsInBBoard(FSINT p)
{
	if (p < 0 || p > 89)
	{
		return FALSE;
	}

	FSINT i, j;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	return IsInBBoard(i,j);
}

/*******************************************************************************************************
函数名 : IsInRBoard
描  述 : 
参  数 : 
        FSINT p : 
返回值 : inline
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:40:23
*******************************************************************************************************/
inline FSBOOL CSearchEngine::IsInRBoard(FSINT p)
{
	if (p < 0 || p > 89)
	{
		return FALSE;
	}

	FSINT i, j;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	return IsInRBoard(i, j);
}

/*******************************************************************************************************
函数名 : IsInBBoard
描  述 : 
参  数 : 
        FSINT i : 
		FSINT j : 
返回值 : inline
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:40:31
*******************************************************************************************************/
inline FSBOOL CSearchEngine::IsInBBoard(FSINT i, FSINT j)
{
	return IsInBoard(i, j) && (j <= 4);
}

/*******************************************************************************************************
函数名 : IsInRBoard
描  述 : 
参  数 : 
        FSINT i : 
		FSINT j : 
返回值 : inline
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:40:40
*******************************************************************************************************/
inline FSBOOL CSearchEngine::IsInRBoard(FSINT i, FSINT j)
{
	return IsInBoard(i, j) && (j > 4);
}

/*******************************************************************************************************
函数名 : Evaluate
描  述 : 估值函数
参  数 : 无
返回值 : FSINT
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:40:57
*******************************************************************************************************/
inline FSINT CSearchEngine::Evaluate()
{
	FSINT nRetVal;
//	FSINT i, j, k;
//	
//	nRetVal = 0;
//
//	//红方
////	for (i = 0; i < 16; i++)
//	{
//		j = m_nPiecePos[i];
//		if (j != INVALIDPIECEPOS)
//		{
//			k = m_nPieceID2ChessID[i] - 7;
//			DEBUGASSERT(k >= 0 && k < 8);	// [2005/10/20 19:52:50]
//
//			nRetVal -= g_nBasicValues[k];
//			nRetVal -= g_nPosValues[k][j];
//		}
//	}
//	//黑方
//	for (i = 16; i < 32; i++)
//	{
//		j = m_nPiecePos[i];
//		if (j != INVALIDPIECEPOS)
//		{
//			k = m_nPieceID2ChessID[i];
//			DEBUGASSERT(k >= 0 && k < 8);	// [2005/10/20 19:52:45]
//
//			nRetVal += g_nBasicValues[k];
//			nRetVal += g_nPosValues[k][89 - j];
//		}
//	}
//	
//	nRetVal = m_nBoardValue + GetFlexValue(m_bIsRed) - GetFlexValue(1 - m_bIsRed); //灵活性估值
//	nRetVal = m_nBoardValue + m_nEmptyHeartCannonValue[IsEmptyHeartCannon(m_bIsRed)] - m_nEmptyHeartCannonValue[IsEmptyHeartCannon(1 - m_bIsRed)];
	nRetVal = m_nBoardValue;

	return m_bIsRed ? -nRetVal : nRetVal;
//	return nRetVal - (nRetVal << 1) * m_bIsRed;
//	return nRetVal - (nRetVal << 1) & m_nHave[m_bIsRed];
//	return nRetVal * m_nSign[m_bIsRed];
}

/*******************************************************************************************************
函数名 : MakeMove
描  述 : 
参  数 : 
        CHESSMOVE cm      : 走法
		FSINT& nToChessID : 目标位置的棋子类型编号
		FSINT& nToPieceID : 目标位置棋子编号
返回值 : 无
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:41:18
*******************************************************************************************************/
inline FSVOID CSearchEngine::MakeMove(CHESSMOVE& cm, FSINT& nToChessID, FSINT& nToPieceID)
{
#ifdef DEBUG
	m_nTestTimer++;
#endif

	FSINT nFrom = cm.nFrom;
	FSINT nTo = cm.nTo;

	FSINT nFromI = m_CGN2Pos[nFrom].x;
	FSINT nFromJ = m_CGN2Pos[nFrom].y;

	FSINT nToI = m_CGN2Pos[nTo].x;
	FSINT nToJ = m_CGN2Pos[nTo].y;

	FSINT nFromChessID;
	FSINT nFromPieceID;

	//nFromPieceID
	//nToPieceID
	nFromPieceID = m_nCBPieceID[nFrom];
	nToPieceID = m_nCBPieceID[nTo];

	//走法的起始位置和目标位置都在棋盘上
	DEBUGASSERT(ISINBOARD(nFrom));
	DEBUGASSERT(ISINBOARD(nTo));
	
	//棋子编号合法
	DEBUGASSERT(nFromPieceID >= MINVALIDPIECEID && nFromPieceID <= MAXVALIDPIECEID);
	DEBUGASSERT(nToPieceID >= MINVALIDPIECEID && nToPieceID <= MAXPIECEID);
	
	//源和目的棋子颜色不同,下面两个值相与的结果有0, 1, 10b, 11b,只有为0时两棋子颜色才相同
	DEBUGASSERT((m_bIsBlack_PieceID[nFromPieceID] ^ m_bIsBlack_PieceID[nToPieceID]) > 0);

	//nFromChessID
	//nToChessID
	nFromChessID = m_nCBChessID[nFrom];
	nToChessID = m_nCBChessID[nTo];

	m_nChessCnt[nToChessID]--;	// [2006/1/2 18:53:17]

	//m_nBoardValue
	if (m_bIsRed == RED)
	{
		DEBUGASSERT(nFromChessID >= RBEGIN && nFromChessID <= REND);
		DEBUGASSERT((nToChessID >= BBEGIN && nToChessID <= BEND) || nToChessID == NOCHESS);

		m_nBoardValue -= g_nBasicValues[nToChessID];	//减去目标位置黑方

		m_nBoardValue += g_nPosValues[nFromChessID - 7][nFrom];		//加上源位置红方
		m_nBoardValue -= g_nPosValues[nFromChessID - 7][nTo];		//减去目标位置红方
		m_nBoardValue -= g_nPosValues[nToChessID][89 - nTo];		//减去目标位置黑方
	}
	else
	{
		DEBUGASSERT((nFromChessID >= BBEGIN && nFromChessID <= BEND) || nFromChessID == NOCHESS);
		DEBUGASSERT((nToChessID >= RBEGIN && nToChessID <= REND) || nToChessID == NOCHESS);

		m_nBoardValue += g_nBasicValues[g_nChessID2Chess[nToChessID]];	//加上目标位置红方

		m_nBoardValue -= g_nPosValues[nFromChessID][89 - nFrom];			//减去源位置黑方
		m_nBoardValue += g_nPosValues[nFromChessID][89 - nTo];				//加上目标位置黑方
		m_nBoardValue += g_nPosValues[g_nChessID2Chess[nToChessID]][nTo];	//加上目标位置红方
	}

	ChangeSide();

	//m_nPiecePos
	m_nPiecePos[nFromPieceID] = nTo;
	m_nPiecePos[nToPieceID] = INVALIDPIECEPOS;

	//m_nCBPieceID
	m_nCBPieceID[nFrom] = INVALIDPIECEID;
	m_nCBPieceID[nTo] = nFromPieceID;

	//m_nBitFile
	m_nBitFile[nFromJ] &= m_nReverseMask[nFromI];
	m_nBitFile[nToJ] |= m_nMask[nToI];

	//m_nBitRank
	m_nBitRank[nFromI] &= m_nReverseMask[nFromJ];
	m_nBitRank[nToI] |= m_nMask[nToJ];

	//m_nCBChessID
	m_nCBChessID[nFrom] = NOCHESS;
	m_nCBChessID[nTo] = nFromChessID;
	
	//m_nBRBitFile:注意,bug易发区
	//去掉nFromJ位行上的nFrom位置上的棋子
	m_nBRBitFile[m_bIsRed_PieceID[nFromPieceID]][nFromJ] &= m_nReverseMask[nFromI];
	//添加nToJ位行上的nFrom位置上的棋子
	m_nBRBitFile[m_bIsRed_PieceID[nFromPieceID]][nToJ] |= m_nMask[nToI];
	//去掉nToJ位行上的nTo位置上的棋子,如果存在的话
	m_nBRBitFile[m_bIsRed_PieceID[nToPieceID]][nToJ] &= m_nReverseMask[nToI];

	//m_nBRBitRank:注意,bug易发区
	//去掉nFromI位列上的nFrom位置上的棋子
	m_nBRBitRank[m_bIsRed_PieceID[nFromPieceID]][nFromI] &= m_nReverseMask[nFromJ];
	//添加nToI位列上的nFrom位置上的棋子
	m_nBRBitRank[m_bIsRed_PieceID[nFromPieceID]][nToI] |= m_nMask[nToJ];
	//去掉nToI位列上的nTo位置上的棋子,如果存在的话
	m_nBRBitRank[m_bIsRed_PieceID[nToPieceID]][nToI] &= m_nReverseMask[nToJ];

	//m_nHashTabIndex
	m_nHashTabIndex ^= m_nHashKey32[nFromChessID][nFromI][nFromJ];	//去掉原来的nFrom
	m_nHashTabIndex ^= m_nHashKey32[NOCHESS][nFromI][nFromJ];		//加上新的nFrom
	m_nHashTabIndex ^= m_nHashKey32[nToChessID][nToI][nToJ];		//去掉原来的nTo
	m_nHashTabIndex ^= m_nHashKey32[nFromChessID][nToI][nToJ];		//加上新的nTo

	//m_nCBCheckSum
	m_nCBCheckSum ^= m_nHashKey64[nFromChessID][nFromI][nFromJ];	//去掉原来的nFrom
	m_nCBCheckSum ^= m_nHashKey64[NOCHESS][nFromI][nFromJ];			//加上新的nFrom
	m_nCBCheckSum ^= m_nHashKey64[nToChessID][nToI][nToJ];			//去掉原来的nTo
	m_nCBCheckSum ^= m_nHashKey64[nFromChessID][nToI][nToJ];		//加上新的nTo
	
	// 用于长将检测[2006/1/2 19:26:50]
	m_nCurDepth++;
//	if (IsChecked(1 - m_bIsRed) == TRUE)
//	{
//		cm.nKind = CHECK;
//	}
	cm.nKind = m_nCheck[IsChecked(1 - m_bIsRed)][cm.nKind];		// [2006/1/2 19:33:08]

	DEBUGASSERT(m_nCurDepth < MAXSEARCHDEPTH);
	m_cmFight[m_nCurDepth] = cm;
#ifdef GETMAXSCHDPT
	if (m_nCurDepth > m_nMaxSearchDepth)
	{
		m_nMaxSearchDepth = m_nCurDepth;
	}
#endif

	// 用于检测重复局面[2005/11/10 16:18:45]
	if (m_nRepHashTab[m_nHashTabIndex & m_nRepHashTabMask] == -1)
	{
		m_nRepHashTab[m_nHashTabIndex & m_nRepHashTabMask] = m_nCurDepth;
	}

	m_nHistoryCBCheckSum[m_nCurDepth] = m_nCBCheckSum;

	if (nToPieceID == INVALIDPIECEID)
	{
		m_nNoEatMoveCnt_IS++;
	}
	else
	{
		m_nNoEatMoveCnt_IS = 0;
		m_nNoEatMoveCnt_IS_Backup = m_nNoEatMoveCnt_IS;
	}
}

/*******************************************************************************************************
函数名 : UnMakeMove
描  述 : 
参  数 : 
        CHESSMOVE cm	 : 走法
		FSINT nToChessID : 目标位置的棋子类型编号(这里的目标指的是产生走法时的目标,cm就是产生走法时的走法)
		FSINT nToPieceID : 目标位置棋子编号(这里的目标指的是产生走法时的目标,cm就是产生走法时的走法)
返回值 : 无
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:41:28
*******************************************************************************************************/
inline FSVOID CSearchEngine::UnMakeMove(const CHESSMOVE& cm, FSINT nToChessID, FSINT nToPieceID)
{
	DEBUGASSERT(cm.nFrom >= 0 && cm.nFrom < MAXCHESSGRIDNUM);
	DEBUGASSERT(cm.nTo >= 0 && cm.nTo < MAXCHESSGRIDNUM);

	DEBUGASSERT(nToChessID >= MINVALIDCHESSID && nToChessID <= MAXVALIDCHESSID);
	DEBUGASSERT(nToPieceID >= MINVALIDPIECEID && nToPieceID <= MAXPIECEID);

#ifdef DEBUG
//	m_nTestTimer++;
#endif

	FSINT nFrom = cm.nFrom;
	FSINT nTo = cm.nTo;

	FSINT nFromI = m_CGN2Pos[nFrom].x;
	FSINT nFromJ = m_CGN2Pos[nFrom].y;

	FSINT nToI = m_CGN2Pos[nTo].x;
	FSINT nToJ = m_CGN2Pos[nTo].y;

	ChangeSide();

	//m_nCBChessID
	m_nCBChessID[nFrom] = m_nCBChessID[nTo];
	m_nCBChessID[nTo] = nToChessID;

	//注意:m_nPiecePos和m_nCBPieceID的更新不能颠倒顺序,如若颠倒顺序,其相应的更新代码也要改变
	//准确的讲:m_nPiecePos的更新依赖于m_nCBPieceID,而m_nCBPieceID的更新不依赖于m_nPiecePos
	//m_nPiecePos
	m_nPiecePos[m_nCBPieceID[nTo]] = nFrom;
	m_nPiecePos[nToPieceID] = nTo;

	//m_nCBPieceID
	m_nCBPieceID[nFrom] = m_nCBPieceID[nTo];
	m_nCBPieceID[nTo] = nToPieceID;

	//m_nBitFile
	//注意:bug易发区
	m_nBitFile[nToJ] = m_nBitFile[nToJ] & 
		m_nReverseMask[nToI] | (m_nPieceID2Bit[nToPieceID] << nToI);
	m_nBitFile[nFromJ] |= m_nMask[nFromI];

	//m_nBitRank
	//注意:bug易发区
	m_nBitRank[nToI] = m_nBitRank[nToI] & 
		m_nReverseMask[nToJ] | (m_nPieceID2Bit[nToPieceID] << nToJ);
	m_nBitRank[nFromI] |= m_nMask[nFromJ];

	m_nChessCnt[nToChessID]++;	// [2006/1/2 18:53:17]

	FSINT nFromChessID = m_nCBChessID[nFrom];
	FSINT nFromPieceID = m_nCBPieceID[nFrom];

	//m_nBoardValue
	if (m_bIsRed == RED)
	{
		DEBUGASSERT(nFromChessID >= RBEGIN && nFromChessID <= REND);
		DEBUGASSERT((nToChessID >= BBEGIN && nToChessID <= BEND) || nToChessID == NOCHESS);

		m_nBoardValue += g_nBasicValues[nToChessID];	//加上目标位置黑方

		m_nBoardValue -= g_nPosValues[nFromChessID - 7][nFrom];		//减去源位置红方
		m_nBoardValue += g_nPosValues[nFromChessID - 7][nTo];		//加上目标位置红方
		m_nBoardValue += g_nPosValues[nToChessID][89 - nTo];		//加上目标位置黑方		
	}
	else
	{
		DEBUGASSERT((nFromChessID >= BBEGIN && nFromChessID <= BEND) || nFromChessID == NOCHESS);
		DEBUGASSERT((nToChessID >= RBEGIN && nToChessID <= REND) || nToChessID == NOCHESS);

		m_nBoardValue -= g_nBasicValues[g_nChessID2Chess[nToChessID]];	//减去目标位置红方

		m_nBoardValue += g_nPosValues[nFromChessID][89 - nFrom];			//加上源位置黑方
		m_nBoardValue -= g_nPosValues[nFromChessID][89 - nTo];				//减去目标位置黑方
		m_nBoardValue -= g_nPosValues[g_nChessID2Chess[nToChessID]][nTo];	//减去目标位置红方		
	}

	//m_nBRBitFile
	//注意:bug易发区,和m_nBitFile的处理方法不同,切记!!!
	m_nBRBitFile[m_bIsRed_PieceID[nFromPieceID]][nToJ] = 
		m_nBRBitFile[m_bIsRed_PieceID[nFromPieceID]][nToJ] & m_nReverseMask[nToI];
	m_nBRBitFile[m_bIsRed_PieceID[nFromPieceID]][nFromJ] |= m_nMask[nFromI];
	m_nBRBitFile[m_bIsRed_PieceID[nToPieceID]][nToJ] |= m_nMask[nToI];

	//m_nBRBitRank
	//注意:bug易发区,和m_nBitRank的处理方法不同,切记!!!
	m_nBRBitRank[m_bIsRed_PieceID[nFromPieceID]][nToI] = 
		m_nBRBitRank[m_bIsRed_PieceID[nFromPieceID]][nToI] & m_nReverseMask[nToJ];
	m_nBRBitRank[m_bIsRed_PieceID[nFromPieceID]][nFromI] |= m_nMask[nFromJ];
	m_nBRBitRank[m_bIsRed_PieceID[nToPieceID]][nToI] |= m_nMask[nToJ];

	// 重复局面检测中用到[2005/11/10 16:33:14]
	if (m_nRepHashTab[m_nHashTabIndex & m_nRepHashTabMask] == m_nCurDepth)
	{
		m_nRepHashTab[m_nHashTabIndex & m_nRepHashTabMask] = -1;
	}

	//m_nHashTabIndex
	m_nHashTabIndex ^= m_nHashKey32[NOCHESS][nFromI][nFromJ];		//去掉原来的nFrom
	m_nHashTabIndex ^= m_nHashKey32[nFromChessID][nFromI][nFromJ];	//加上新的nFrom
	m_nHashTabIndex ^= m_nHashKey32[nFromChessID][nToI][nToJ];		//去掉原来的nTo
	m_nHashTabIndex ^= m_nHashKey32[nToChessID][nToI][nToJ];		//加上新的nTo

	//m_nCBCheckSum
	m_nCBCheckSum ^= m_nHashKey64[NOCHESS][nFromI][nFromJ];			//去掉原来的nFrom
	m_nCBCheckSum ^= m_nHashKey64[nFromChessID][nFromI][nFromJ];	//加上新的nFrom
	m_nCBCheckSum ^= m_nHashKey64[nFromChessID][nToI][nToJ];		//去掉原来的nTo
	m_nCBCheckSum ^= m_nHashKey64[nToChessID][nToI][nToJ];			//加上新的nTo

	m_nCurDepth--;

	if (nToPieceID == INVALIDPIECEID)
	{
		m_nNoEatMoveCnt_IS--;
	}
	else
	{
		m_nNoEatMoveCnt_IS = m_nNoEatMoveCnt_IS_Backup;
	}
}

/*******************************************************************************************************
函数名 : SearchInit
描  述 : 
参  数 : 无
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年10月21日 21:48:24
*******************************************************************************************************/
FSVOID CSearchEngine::SearchInit()
{
	m_esUCCIEgnStatus = EgnSBusy;	//置引擎忙标志

	//走法的历史得分表
	memset(m_nHistoryScore[BLACK], 0, (MAXCHESSGRIDNUM + 1) * (MAXCHESSGRIDNUM + 1) * sizeof(SCORETYPE));
	memset(m_nHistoryScore[RED], 0, (MAXCHESSGRIDNUM + 1) * (MAXCHESSGRIDNUM + 1) * sizeof(SCORETYPE));

//	HASHITEM *pHashItem = &m_pHashTab[m_bIsRed][m_nHashTabIndex & m_nHashTabMask];
//	if (pHashItem->nCBCheckSum == m_nCBCheckSum)
//	{
//		pHashItem->nDepth = 1;
//	}

	HASHITEM hashItem = m_pHashTab[m_bIsRed][m_nHashTabIndex & m_nHashTabMask];
	if (hashItem.nCBCheckSum == m_nCBCheckSum)
	{
		hashItem.nDepth = 1;
	}

	// 清空hash表
	ClearHash();

	if (hashItem.nCBCheckSum == m_nCBCheckSum)
	{
		m_pHashTab[m_bIsRed][m_nHashTabIndex & m_nHashTabMask] = hashItem;
	}

	memset(m_nMoveSearchedCnt, 0, MAXCHESSGRIDNUM * MAXCHESSGRIDNUM * sizeof(FSUINT));
	memset(m_nMoveCutoffCnt, 0, MAXCHESSGRIDNUM * MAXCHESSGRIDNUM * sizeof(FSUINT));

	//m_nKillerMoveCnt
	memset(m_nKillerMoveCnt, 0, MAXSEARCHDEPTH * sizeof(FSINT));

	m_bStopThink = FALSE;

//	if (m_bIsHaveCheckMateMove == TRUE)
	if (m_bIsHaveCheckMateMove == TRUE || m_bIsHaveGoodPosition == TRUE)
	{
		m_nCheckExtensionPlies = 2;
	}
	else
	{
		m_nCheckExtensionPlies = 1;
	}

	// [2006/1/10 17:18:56]
	m_bIsHaveCheckMateMove = FALSE;
	m_bIsHaveGoodPosition = FALSE;

	// [2006/1/4 16:53:18]
	if (m_nCurDepth > 0)
	{
		m_cmFight[0] = m_cmFight[m_nCurDepth];
		m_nCurDepth = 0;
	}
	else
	{
		m_cmFight[0] = m_cmNullMove;
	}

	m_nSearchCnt++;

	STATEMENT_STAT
	(
	m_nProbeHashCnt = 0;
	m_nRecordHashCnt = 0;
	m_nHitHashCnt = 0;

	m_nProbeQSHashCnt = 0;
	m_nRecordQSHashCnt = 0;
	m_nHitQSHashCnt = 0;
	);

	m_nTimeStartThink = clock();
}

/*******************************************************************************************************
函数名 : SearchMain
描  述 : 
参  数 : 无
返回值 : DWORD
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:41:40
*******************************************************************************************************/
DWORD WINAPI CSearchEngine::SearchMain()
{
	// 待搜索的局面非法
	if (IsValidPosition() == FALSE)
	{
		return 0;
	}

	// 超过自然限着判和
	if (m_nCurDepth > m_nDrawMoves)
	{
		DISPMSG1_ENDL(cout,	"nobestmove");
		return 0;
	}

	FSINT	i;
	FSINT	nEval;
	FSBOOL	bPrintEndL = FALSE;
	FSINT	nLegalNum;
	FSINT	nLegalCnt;

	//搜索之前初始化
	SearchInit();

	//无合法走法
	if ((nLegalCnt = LegalMoves(nLegalNum)) == 0)
	{
		DISPMSG1_ENDL(cout, "nobestmove");
		return 0;
	}
	//仅有一种合法走法或直接吃对方将
	else if ((nLegalCnt == 1 && m_bOnlyMoveOn == TRUE) || nLegalCnt == -1)
	{
		m_PVLine.nCount = 1;
		m_PVLine.pMoveList[0] = m_MoveList[0][nLegalNum];

		SearchStop();

		return 0;
	}

	//先搜索开局库 [2005/12/7 21:29:34]
	DISPMSG1_ENDL(cout, "info Searching OpenBook...");
//	if (m_bOpenBookOn == TRUE && m_nChessAge == OPENGAME)
//	if (m_bOpenBookOn == TRUE && (m_nGetOpenBookMoveErrCnt <= 1) || m_nBigChessCnt == 12)
	if (m_bOpenBookOn == TRUE && m_nGetOpenBookMoveErrCnt <= 1 && m_nChessAge != END_GAME)
	{
		if (m_nGetOpenBookMoveErrCnt > 1)
		{
			m_nGetOpenBookMoveErrCnt = 0;
		}

		CHESSMOVE cmTemp;
		
		if ((cmTemp = GetMoveFromBook()) != m_cmNullMove)
		{
			m_PVLine.nCount = 1;
			m_PVLine.pMoveList[0] = cmTemp;
			
			SearchStop();
			
			SWITCHEGN(
			DISPMSG1_ENDL(cout,	"info move from book"););
			
			return 0;
		}
	}

	//深度为0返回局面估值
	if (m_nSearchDepth == 0)
	{
		DISPMSG2_ENDL(cout,	"the position score is ", (nEval = Evaluate()));
		return 0;
	}

	m_nTimeStartThink = clock();

	// 按思考方式进行思考 [2005/10/22 20:39:53]
	if (m_ThinkType == TT_DEPTH)
	{
		//迭代加深
		for (i = 1; i <= m_nSearchDepth; i++)
		{
			DEBUGASSERT(m_nCurDepth == 0);

			if (bPrintEndL == FALSE)
			{
				bPrintEndL = TRUE;
				switch(m_EgnProtocol)
				{
				case EgnPUCCI:
				case EgnPAhei:
					cout << endl;
					break;
				
				case EgnPQianHong:
					STATEMENT_TESTQHP(cout << endl;);
					break;

				default:
					break;
				}
			}

			IDSearchInit();
			m_nCurSearchDepth_ID = i;

			switch(m_EgnProtocol)
			{
			case EgnPUCCI:
			case EgnPAhei:
				cout << endl;
				cout << "info depth " << i << endl;
				break;
				
			case EgnPQianHong:
				STATEMENT_TESTQHP
				(
				cout << endl;
				cout << "info depth " << i << endl;
				);
				break;
				
			default:
				break;
			}

			// [2006/1/8 10:44:50]
//			if (m_nCheckExtensionPlies == 1 && i >= 4 && (m_PVLine.pMoveList[0].nKind & EAT) == 0 && IsCheckMove(m_PVLine.pMoveList[0]) == TRUE)
//			if (m_nCheckExtensionPlies == 1 && i >= 4 && ((m_PVLine.pMoveList[0].nKind & EAT) == 0 && IsCheckMove(m_PVLine.pMoveList[0]) == TRUE) || m_bIsHaveGoodPosition == TRUE)
			if (m_nCheckExtensionPlies == 1 && i >= 4 && 
				(g_nChessID2Chess[m_nCBChessID[m_PVLine.pMoveList[0].nFrom]] == ROOK && 
				(m_PVLine.pMoveList[0].nKind & EAT) == 0 && IsCheckMove(m_PVLine.pMoveList[0]) == TRUE) || m_bIsHaveGoodPosition == TRUE)
			{
				m_nCheckExtensionPlies = 2;
			}

			nEval = Search(0, i, g_nPerCheckValue, -g_nPerCheckValue, m_KillerMoveTab);
//			nEval = Search(0, g_nPerCheckValue, -g_nPerCheckValue, m_KillerMoveTab);
//			nEval = SearchTest(0, i, g_nPerCheckValue, -g_nPerCheckValue, m_KillerMoveTab);
			if (abs(nEval) != g_nTimeOutScore)
			{
				DEBUGASSERT(i >= 0);
				DEBUGASSERT(m_nBytes[i] >= 0);

				SWITCHEGN(
				DISPMSG2_ENDL(cout,	"info time ", MS2S(clock() - m_nTimeStartThink));
				);

				if (abs(nEval) + MAXSEARCHDEPTH >= g_nMateValue)
				{
					m_bIsHaveCheckMateMove = TRUE;
					break;
				}

				// [2006/2/24 9:23:53]
				m_bIsHaveGoodPosition = (abs(nEval) >= g_nGoodPositionValue);
			}
			else
			{
				break;
			}
		}
//		m_nCurSearchDepth_ID = m_nSearchDepth;
//		SearchTest(0, g_nPerCheckValue, -g_nPerCheckValue, m_KillerMoveTab);
//		Search(0, g_nPerCheckValue, -g_nPerCheckValue, m_KillerMoveTab);
	}
	else if (m_ThinkType == TT_TIME)
	{
		FSBOOL bStop = FALSE;

		//迭代加深
		for (i = 1; i <= MAXSEARCHDEPTH; i++)
		{
			DEBUGASSERT(m_nCurDepth == 0);

			if (bPrintEndL == FALSE)
			{
				bPrintEndL = TRUE;
				switch(m_EgnProtocol)
				{
				case EgnPUCCI:
				case EgnPAhei:
					cout << endl;
					break;
				
				case EgnPQianHong:
					STATEMENT_TESTQHP(cout << endl;);
					break;

				default:
					break;
				}
			}

			IDSearchInit();
			m_nCurSearchDepth_ID = i;

			switch(m_EgnProtocol)
			{
			case EgnPUCCI:
			case EgnPAhei:
				cout << endl;
				cout << "info depth " << i << endl;
				break;
				
			case EgnPQianHong:
				STATEMENT_TESTQHP
				(
				cout << endl;
				cout << "info depth " << i << endl;
				);
				break;
				
			default:
				break;
			}

			// [2006/1/8 10:44:50]
			//if (m_nCheckExtensionPlies == 1 && i >= 4 && (m_PVLine.pMoveList[0].nKind & EAT) == 0 && IsCheckMove(m_PVLine.pMoveList[0]) == TRUE)
//			if (m_nCheckExtensionPlies == 1 && i >= 4 && ((m_PVLine.pMoveList[0].nKind & EAT) == 0 && IsCheckMove(m_PVLine.pMoveList[0]) == TRUE) || m_bIsHaveGoodPosition == TRUE)
			if (m_nCheckExtensionPlies == 1 && i >= 4 && 
				(g_nChessID2Chess[m_nCBChessID[m_PVLine.pMoveList[0].nFrom]] == ROOK && 
				(m_PVLine.pMoveList[0].nKind & EAT) == 0 && IsCheckMove(m_PVLine.pMoveList[0]) == TRUE) || m_bIsHaveGoodPosition == TRUE)
			{
				m_nCheckExtensionPlies = 2;
			}

			nEval = Search(0, i, g_nPerCheckValue, -g_nPerCheckValue, m_KillerMoveTab);
//			nEval = Search(0, g_nPerCheckValue, -g_nPerCheckValue, m_KillerMoveTab);
			if (abs(nEval) != g_nTimeOutScore)
			{
				DEBUGASSERT(i >= 0);
				DEBUGASSERT(m_nBytes[i] >= 0);

				SWITCHEGN(
				DISPMSG2_ENDL(cout,	"info time ", MS2S(clock() - m_nTimeStartThink));
				);

				if (abs(nEval) + MAXSEARCHDEPTH >= g_nMateValue)
				{
					m_bIsHaveCheckMateMove = TRUE;
					break;
				}

				// [2006/2/24 9:23:47]
				m_bIsHaveGoodPosition = (abs(nEval) >= g_nGoodPositionValue);
			}
			else
			{
				break;
			}
		}

		if (bStop == TRUE)
		{
			m_nSearchDepth = i - 1;
		}
	}

	SearchStop();

	return 0;
}

/*******************************************************************************************************
函数名 : Search
描  述 : 
参  数 : 
        FSINT nDepth		: 当前深度
		FSINT nSearchDepth	: 搜索深度
		FSINT alpha			: 
		FSINT beta			: 
返回值 : FSINT
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:41:48
*******************************************************************************************************/
FSINT CSearchEngine::Search(FSINT nDepth, FSINT nSearchDepth, FSINT alpha, FSINT beta, KILLERMOVETAB& killerMoveTab)
{
	DEBUGASSERT(nDepth >= 0 && nDepth <= MAXSEARCHDEPTH);
//	DEBUGASSERT(nDepth >= 0);
	DEBUGASSERT(beta <= -g_nPerCheckValue);
	DEBUGASSERT(alpha >= g_nPerCheckValue && alpha <= beta);

	//双将俱在
	DEBUGASSERT(m_nPiecePos[m_nKingID2PieceID[m_bIsRed]] != INVALIDPIECEPOS);
	DEBUGASSERT(m_nPiecePos[m_nKingID2PieceID[1 - m_bIsRed]] != INVALIDPIECEPOS);

	FSINT	i;
	FSINT	nBestEval = m_nCurDepth - g_nMateValue;
	FSINT	nNewDepth;
	FSINT	nCount;							//走法数
	FSINT	nEval;							//估值
	FSINT	nToChessID;						//被吃的棋子类型编号
	FSINT	nToPieceID;						//被吃的棋子编号
	FSINT	nHashFlag = HASHALPHA;			//hash节点标志
	FSBOOL	bCutoff = FALSE;				//是否被截断
	FSBOOL	bFindPV = FALSE;				//是否找到PV走法
	FSINT	nBestMoveNum = 0;				//好的走法号,用于记录历史得分
	FSINT	bChecked;						//当前局面是否被将军,用于历史裁剪和将军延伸
	FSBOOL	bFirstMove = TRUE;				//正搜索的走法是否是第一个走法,用于历史裁剪中
	FSBOOL	bMateThreat = FALSE;			//杀棋威胁,用于杀棋威胁延伸
	FSBOOL	bSingleResponse = FALSE;		//单一应着,用于单一应着延伸
//	FSBOOL	bDangerPosition = FALSE;		//危险局面,用于威胁局面延伸中[2006/1/1 11:24:24]
	FSINT	nCanMoveCnt = 0;				//可以应着的走发数,用于单一应着中

	CHESSMOVE		cm = m_cmNullMove;
	CHESSMOVE		cmBestMove = m_cmNullMove;
	KILLERMOVETAB	subKillerMoveTab; //杀手走法表

	//按时间思考时,超时返回
	if (m_ThinkType == TT_TIME && (clock() - m_nTimeStartThink) > m_nTimeThinkLimit && 
		(m_bPonder == FALSE || m_bPonderHit == TRUE))
	{
		SearchStop();
#ifdef _WIN32
		ExitThread(0);
#else
#endif
	}

	if (nDepth > 0)
	{
		//杀棋,直接返回
		if (nBestEval >= beta)
		{
			return nBestEval;
		}
		
		//重复检测
		//	if ((nEval = IsRep(1)) != FALSE)
		//	if ((nEval = IsRep(3)) != FALSE)
		//	if ((nEval = IsRep(nDepth <= 1 ? 3 : 1)) != FALSE)
		if ((nEval = IsRep(nDepth == 1 ? 3 : 1)) != FALSE)
		{
			return RepValue(nEval);
		}
		
		//和棋裁剪
		if (IsDraw() == TRUE)
		{
			return DrawValue();
		}
		
		// 防止延伸过深[2005/11/2 13:14:45]
		if (nDepth >= MAXSEARCHDEPTH)
		{
			return Evaluate();
		}
		
		//根节点处调用静态搜索
		if (nDepth >= nSearchDepth)
		{
//			bDangerPosition = IsEmptyHeartCannon(1 - m_bIsRed);
//			if (bDangerPosition == TRUE)
//			{
//				return Search(nDepth + 1, nDepth + 2, alpha, beta, subKillerMoveTab);
//			}
			return QuiesSearch(nDepth + 1, alpha, beta);
			//		return Evaluate();
		}

//		if (nDepth == 1)
//		{
//			if (m_cmFight[1] == m_PVLine.pMoveList[0])
//			{
////				nSearchDepth += 2;
//				nSearchDepth += 1;
//			}
//		}
	}

#ifdef STAT
	DEBUGASSERT(m_nVistNodeCnt >= 0 && m_nVistNodeCnt + 1 <= UINT_MAX);

	// 访问的节点数加1
	m_nVistNodeCnt++;
#endif

	// 探测hash表
	if ((nEval = ProbeHash(nDepth, nSearchDepth, alpha, beta, cm)) != g_nInvalidValue)
	{
		// 根节点检查重复[2005/11/14 16:55:27]
		if (nDepth > 0)
		{
			return nEval;
		}

		MakeMove(cm, nToChessID, nToPieceID);
		if (IsRep(3) == FALSE)
		{
			UnMakeMove(cm, nToChessID, nToPieceID);

			m_PVLine.nCount = 0;
			GetPVLineFromHash(m_nCurSearchDepth_ID);
			PrintPVLine();
			
			return nEval;
		}
		// 说明已经发生循环,下面的置换表启发就不能用这个走法[2005/12/21 22:02:08]
		cm = m_cmNullMove;
		UnMakeMove(cm, nToChessID, nToPieceID);
	}

/*
 ************************************************************
 *                                                          *
 *   try to null move					                    *
 *                                                          *
 ************************************************************
 */
	if (m_bNullMoveOn == TRUE)
	{
		//Null Move
		MakeNullMove();
		
		if (IsChecked(m_bIsRed) == FALSE)
		{
			UnMakeNullMove();
			
			//双空着检验
			if (nDepth + 2 + m_nDRF_NM < nSearchDepth)
			{
				MakeNullMove();
				MakeNullMove();
				
				nEval = Search(nDepth + 2 + m_nDRF_NM, nSearchDepth, beta - 1, beta, subKillerMoveTab);
				
				UnMakeNullMove();
				UnMakeNullMove();
			}
			else
			{
				nEval = beta;
			}
			
			if (nEval >= beta)
			{
				MakeNullMove();
				
				nEval = -Search(nDepth + 1 + m_nDRF_NM, nSearchDepth, -beta, 1 - beta, subKillerMoveTab);
				
				UnMakeNullMove();
				
				if (nEval >= beta)
				{
					#ifdef STAT
					m_nNullNodeCnt++;
					#endif
					
					return beta;
				}

				if (nEval == m_nCurDepth + 2 - m_nDRF_NM)
				{
					bMateThreat = TRUE;
				}

				//ProbeHash(nDepth, nSearchDepth, g_nPerCheckValue, -g_nPerCheckValue, cm);
			}
			
			//		nEval = -Search(nDepth + 1 + m_nDRF_NM, -beta, 1 - beta, subKillerMoveTab);
			//		
			//		UnMakeNullMove();
			//
			//		if (nEval >= beta)
			//		{
			//#			ifdef STAT
			//			m_nNullNodeCnt++;
			//#			endif
			
			//			return beta;
			//		}
		}
		else
		{
			UnMakeNullMove();
		}
	}

	bChecked = IsChecked(1 - m_bIsRed);
	
	//单一应着
//	GenMoves(nDepth);
//	nCount = m_nMovesCnt;
//	for (i = 0; i < nCount; i++)
//	{
//		cm = m_MoveList[nDepth][i];
//
//		MakeMove(cm, nToChessID, nToPieceID);
//
//		//判断该走法是否合法
//		if (IsLegalMove(cm) == FALSE)
//		{
//			UnMakeMove(cm, nToChessID, nToPieceID);
//			continue;
//		}
//
//		nCanMoveCnt++;
//	}
//	if (nCanMoveCnt == 1)
//	{
//		bSingleResponse = TRUE;
//	}

	// [2006/1/6 16:27:50]
	FSBOOL bExtension = FALSE;

	if (bChecked == TRUE)
	{
//		nNewDepth = nSearchDepth + 2;
//		nNewDepth = nSearchDepth + 1;
		nNewDepth = nSearchDepth + m_nCheckExtensionPlies;
	}
	else if (bMateThreat == TRUE || bSingleResponse == TRUE)
	{
		nNewDepth = nSearchDepth + 1;
	}
	else
	{
		nNewDepth = nSearchDepth;

		bExtension = TRUE;
	}
//	nNewDepth = nSearchDepth;

	// 节点排序1:Hash Move [2005/10/25 13:05:50]
	// 内部迭代加深[2005/10/25 19:19:24]
	if (cm == m_cmNullMove && nDepth + 2 < m_nCurSearchDepth_ID)
	{
		Search(nDepth + 2, nSearchDepth, alpha, beta, killerMoveTab);
		ProbeHash(nDepth, nSearchDepth, alpha, beta, cm);
//		ProbeHash(nDepth, nSearchDepth, g_nPerCheckValue, -g_nPerCheckValue, cm);
	}
	if (cm != m_cmNullMove)
	{
		MakeMove(cm, nToChessID, nToPieceID);

		DEBUGASSERT(IsLegalMove(cm) == TRUE);

		m_nMoveSearchedCnt[cm.nFrom][cm.nTo]++;
		
		// 将军延伸,杀棋威胁延伸,单一应着延伸,兑子延伸[2005/11/1 18:49:14]
		DEBUGASSERT(m_nCurDepth >= 1);
		if (bExtension == TRUE)
		{
			nNewDepth = (m_cmFight[m_nCurDepth - 1].nKind == EAT && cm.nKind == EAT && m_cmFight[m_nCurDepth - 1].nTo == cm.nTo) ? (nSearchDepth + 1) : nSearchDepth;
//			nNewDepth = (m_cmFight[m_nCurDepth - 1].nKind == EAT && cm.nKind == EAT) ? (nSearchDepth + 1) : nSearchDepth;
		}
//		if (bExtension == TRUE)
//		{
//			if (m_cmFight[m_nCurDepth - 1].nKind == EAT && cm.nKind == EAT && m_cmFight[m_nCurDepth - 1].nTo == cm.nTo)
//			{
//				nNewDepth = nSearchDepth + 1;
//			}
//			else
//			{
		//挺兵延伸
//				FSINT nFromJ;
//
//				if (m_bIsRed == RED && m_nCBChessID[cm.nTo] == RPAWN)
//				{
//					if (cm.nFrom == cm.nTo + 1)
//					{
//						nFromJ = m_CGN2Pos[cm.nFrom].y;
//						if (nFromJ == 5 || nFromJ == 6)
//						{
//							nNewDepth = nSearchDepth + 1;
//						}
//					}
//				}
//				else if (m_nCBChessID[cm.nTo] == RPAWN)
//				{
//					if (cm.nFrom == cm.nTo - 1)
//					{
//						nFromJ = m_CGN2Pos[cm.nFrom].y;
//						if (nFromJ == 3 || nFromJ == 4)
//						{
//							nNewDepth = nSearchDepth + 1;
//						}
//					}
//				}
//			}
//		}

		nEval = -Search(nDepth + 1, nNewDepth, -beta, -alpha, subKillerMoveTab);
		
		UnMakeMove(cm, nToChessID, nToPieceID);

		bFirstMove = FALSE;

		if (nEval > nBestEval)
		{
			nBestEval = nEval;
			cmBestMove = cm;
			cmBestMove.nScore = nEval;

			if (nEval > alpha)
			{
				alpha = nEval;
				
				if (nBestEval >= beta)
				{
					nHashFlag = HASHBETA;

					killerMoveTab.cmKiller[killerMoveTab.nPointer] = cm;
					killerMoveTab.nPointer = (killerMoveTab.nPointer + 1) & (MAXKILLERMOVECNT - 1);
					if (killerMoveTab.nKillerCount <= MAXKILLERMOVECNT - 1)
					{
						killerMoveTab.nKillerCount++;
					}
					DEBUGASSERT(killerMoveTab.nPointer >= 0 && killerMoveTab.nPointer <= MAXKILLERMOVECNT - 1);
					DEBUGASSERT(killerMoveTab.nKillerCount >= 1 && killerMoveTab.nKillerCount <= MAXKILLERMOVECNT);

					RecordHash(nDepth, nSearchDepth, alpha, nHashFlag, cmBestMove);

					m_nMoveCutoffCnt[cm.nFrom][cm.nTo]++;

					STATEMENT_STAT(m_nBetaNodeCnt++;);
					STATEMENT_STAT(m_nHashBetaNodeCnt++;);

					return alpha;
				}
				
				bFindPV = TRUE;
				nHashFlag = HASHEXACT;

				if (nDepth == 0)
				{
					PrintBetterPV(nDepth, nSearchDepth, nEval, nHashFlag, cmBestMove);
					if (nEval + MAXSEARCHDEPTH >= g_nMateValue)
					{
						return nEval;
					}
				}
			}
		}
	}

	// 节点排序2:Capture Move[2005/10/23 18:44:05]
	// 先搜索吃子走法[2005/10/22 23:02:02]
	//产生所有吃子走法
	GenCapMoves(nDepth);
	nCount = m_nMovesCnt;
	DEBUGASSERT(nCount >= 0 && nCount <= MAXALLMOVESCNT);

	//取历史得分
	for (i = 0; i < nCount; i++)
	{
		cm = m_MoveList[nDepth][i];
		m_MoveList[nDepth][i].nScore = m_nHistoryScore[m_bIsRed][cm.nFrom][cm.nTo];
	}

	//对走法排序
	sort(m_MoveList[nDepth], m_MoveList[nDepth] + nCount, ::UDgreater);

#ifdef TESTHH
	if (nDepth == 0)
	{
		cout << "The best move history score is " << m_MoveList[nDepth][0].nScore << endl;
	}
#endif

//	bFirstMove = TRUE;

	//扫描所有走法
	for (i = 0; i < nCount; i++)
	{
		cm = m_MoveList[nDepth][i];

		if (IsGoodCapMove(cm) == FALSE)
		{
			continue;
		}

		MakeMove(cm, nToChessID, nToPieceID);

		//判断该走法是否合法
		if (IsLegalMove(cm) == FALSE)
		{
			UnMakeMove(cm, nToChessID, nToPieceID);
			continue;
		}

		m_nMoveSearchedCnt[cm.nFrom][cm.nTo]++;
		
		// 将军延伸,杀棋威胁延伸,单一应着延伸,兑子延伸[2005/11/1 18:49:14]
		DEBUGASSERT(m_nCurDepth >= 1);
		if (bExtension == TRUE)
		{
			nNewDepth = (m_cmFight[m_nCurDepth - 1].nKind == EAT && cm.nKind == EAT && m_cmFight[m_nCurDepth - 1].nTo == cm.nTo) ? (nSearchDepth + 1) : nSearchDepth;
//			nNewDepth = (m_cmFight[m_nCurDepth - 1].nKind == EAT && cm.nKind == EAT) ? (nSearchDepth + 1) : nSearchDepth;
		}
		
		if (bFindPV == TRUE)
		{
			nEval = -Search(nDepth + 1, nNewDepth, -alpha - 1, -alpha, subKillerMoveTab);
			if (nEval > alpha && nEval < beta)
			{
				nEval = -Search(nDepth + 1, nNewDepth, -beta, -alpha, subKillerMoveTab);
			}
		}
		else
		{
			nEval = -Search(nDepth + 1, nNewDepth, -beta, -alpha, subKillerMoveTab);
		}
		
		UnMakeMove(cm, nToChessID, nToPieceID);

		bFirstMove = FALSE;

		if (nEval > nBestEval)
		{
			nBestEval = nEval;
			nBestMoveNum = i;
			cmBestMove = cm;
			cmBestMove.nScore = nEval;

			if (nEval > alpha)
			{
				alpha = nEval;
				
				if (nBestEval >= beta)
				{
					nHashFlag = HASHBETA;
					
					killerMoveTab.cmKiller[killerMoveTab.nPointer] = cm;
					killerMoveTab.nPointer = (killerMoveTab.nPointer + 1) & (MAXKILLERMOVECNT - 1);
					if (killerMoveTab.nKillerCount <= MAXKILLERMOVECNT - 1)
					{
						killerMoveTab.nKillerCount++;
					}
					DEBUGASSERT(killerMoveTab.nPointer >= 0 && killerMoveTab.nPointer <= MAXKILLERMOVECNT - 1);
					DEBUGASSERT(killerMoveTab.nKillerCount >= 1 && killerMoveTab.nKillerCount <= MAXKILLERMOVECNT);

					RecordHash(nDepth, nSearchDepth, alpha, nHashFlag, cmBestMove);

					m_nMoveCutoffCnt[cm.nFrom][cm.nTo]++;
					
					STATEMENT_STAT(m_nBetaNodeCnt++;);
					STATEMENT_STAT(m_nCapMoveBetaNodeCnt++;);

					return alpha;
				}
				
				bFindPV = TRUE;
				nHashFlag = HASHEXACT;

				if (nDepth == 0)
				{
					PrintBetterPV(nDepth, nSearchDepth, nEval, nHashFlag, cmBestMove);
					if (nEval + MAXSEARCHDEPTH >= g_nMateValue)
					{
						return nEval;
					}
				}
			}
		}
	}

	// 节点排序3:Killer Move[2005/10/23 18:44:42]
//	bFirstMove = TRUE;

	//扫描杀手走法
	nCount = killerMoveTab.nKillerCount;
	for (i = 0; i < nCount; i++)
	{
		cm = killerMoveTab.cmKiller[i];

		if (IsValidMove(cm) == FALSE)
		{
			continue;
		}

		MakeMove(cm, nToChessID, nToPieceID);

		//判断该走法是否合法
		if (IsLegalMove(cm) == FALSE)
		{
			UnMakeMove(cm, nToChessID, nToPieceID);
			continue;
		}

		m_nMoveSearchedCnt[cm.nFrom][cm.nTo]++;

		// 将军延伸,杀棋威胁延伸,单一应着延伸,兑子延伸[2005/11/1 18:49:14]
		DEBUGASSERT(m_nCurDepth >= 1);
		if (bExtension == TRUE)
		{
			nNewDepth = (m_cmFight[m_nCurDepth - 1].nKind == EAT && cm.nKind == EAT && m_cmFight[m_nCurDepth - 1].nTo == cm.nTo) ? (nSearchDepth + 1) : nSearchDepth;
//			nNewDepth = (m_cmFight[m_nCurDepth - 1].nKind == EAT && cm.nKind == EAT) ? (nSearchDepth + 1) : nSearchDepth;
		}

		if (bFindPV == TRUE)
		{
			nEval = -Search(nDepth + 1, nNewDepth, -alpha - 1, -alpha, subKillerMoveTab);
			if (nEval > alpha && nEval < beta)
			{
				nEval = -Search(nDepth + 1, nNewDepth, -beta, -alpha, subKillerMoveTab);
			}
		}
		else
		{
			nEval = -Search(nDepth + 1, nNewDepth, -beta, -alpha, subKillerMoveTab);
		}
		
		UnMakeMove(cm, nToChessID, nToPieceID);

		bFirstMove = FALSE;

		m_MoveList[nDepth][i].nScore = nEval;

		if (nEval > nBestEval)
		{
			nBestEval = nEval;
			nBestMoveNum = i;
			cmBestMove = cm;
			cmBestMove.nScore = nEval;

			if (nEval > alpha)
			{
				alpha = nEval;
				
				if (nBestEval >= beta)
				{
					nHashFlag = HASHBETA;
					RecordHash(nDepth, nSearchDepth, alpha, nHashFlag, cmBestMove);

					DEBUGASSERT(m_nHistoryScore[m_bIsRed][cmBestMove.nFrom][cmBestMove.nTo] >= 0 && 
						m_nHistoryScore[m_bIsRed][cmBestMove.nFrom][cmBestMove.nTo] + 
						(1 << (nSearchDepth - nDepth)) <= INT_MAX);
					m_nHistoryScore[m_bIsRed][cmBestMove.nFrom][cmBestMove.nTo] += 
						(1 << (nSearchDepth - nDepth));
					
					m_nMoveCutoffCnt[cm.nFrom][cm.nTo]++;

					STATEMENT_STAT(m_nBetaNodeCnt++;);
					STATEMENT_STAT(m_nKillerMoveBetaNodeCnt++;);

					return alpha;
				}
				
				bFindPV = TRUE;
				nHashFlag = HASHEXACT;

				if (nDepth == 0)
				{
					PrintBetterPV(nDepth, nSearchDepth, nEval, nHashFlag, cmBestMove);
					if (nEval + MAXSEARCHDEPTH >= g_nMateValue)
					{
						return nEval;
					}
				}
			}
		}
	}

	// 节点排序4:History Move[2005/10/23 18:45:05]
	//产生所有不吃子走法
//	GenNoCapMoves(nDepth);
	GenMoves(nDepth);
	nCount = m_nMovesCnt;
	DEBUGASSERT(nCount >= 0 && nCount <= MAXALLMOVESCNT);

	//取历史得分
	for (i = 0; i < nCount; i++)
	{
		cm = m_MoveList[nDepth][i];
		m_MoveList[nDepth][i].nScore = m_nHistoryScore[m_bIsRed][cm.nFrom][cm.nTo];
	}

	//对走法排序
	sort(m_MoveList[nDepth], m_MoveList[nDepth] + nCount, ::UDgreater);

	bFirstMove = TRUE;

	//扫描所有走法
	for (i = 0; i < nCount; i++)
	{
		cm = m_MoveList[nDepth][i];

		// [2006/1/5 21:01:57]
//		if ((cm.nKind & EAT) != 0 && IsGoodCapMove(cm) == TRUE)
//		{
//			continue;
//		}

		MakeMove(cm, nToChessID, nToPieceID);

		//判断该走法是否合法
		if (IsLegalMove(cm) == FALSE)
		{
			UnMakeMove(cm, nToChessID, nToPieceID);
			continue;
		}

		m_nMoveSearchedCnt[cm.nFrom][cm.nTo]++;
		
		// 将军延伸,杀棋威胁延伸,单一应着延伸,兑子延伸[2005/11/1 18:49:14]
		DEBUGASSERT(m_nCurDepth >= 1);
		if (bExtension == TRUE)
		{
			nNewDepth = (m_cmFight[m_nCurDepth - 1].nKind == EAT && cm.nKind == EAT && m_cmFight[m_nCurDepth - 1].nTo == cm.nTo) ? (nSearchDepth + 1) : nSearchDepth;
//			nNewDepth = (m_cmFight[m_nCurDepth - 1].nKind == EAT && cm.nKind == EAT) ? (nSearchDepth + 1) : nSearchDepth;
		}

		DEBUGASSERT(m_nMoveCutoffCnt[cm.nFrom][cm.nTo] <= m_nMoveSearchedCnt[cm.nFrom][cm.nTo]);
		if (m_nMoveCutoffCnt[cm.nFrom][cm.nTo] < (m_nMoveSearchedCnt[cm.nFrom][cm.nTo] >> m_nHistoryShift) && 
			bChecked == FALSE && m_cmFight[m_nCurDepth - 1].nKind != CHECK && cm.nKind != EAT && 
			alpha + 1 == beta && bFirstMove == FALSE && nNewDepth == nSearchDepth)
		{
			nEval = -Search(nDepth + 2, nNewDepth, -beta, -alpha, subKillerMoveTab);
			//if (nEval >= beta)
			if (nEval > alpha)
			{
				nEval = -Search(nDepth + 1, nNewDepth, -beta, -alpha, subKillerMoveTab);
			}
			else
			{
				//m_nMoveCutoffCnt[cm.nFrom][cm.nTo]++;
			}
		}
		else
		{
			if (bFindPV == TRUE)
			{
				nEval = -Search(nDepth + 1, nNewDepth, -alpha - 1, -alpha, subKillerMoveTab);
				if (nEval > alpha && nEval < beta)
				{
					nEval = -Search(nDepth + 1, nNewDepth, -beta, -alpha, subKillerMoveTab);
				}
			}
			else
			{
				nEval = -Search(nDepth + 1, nNewDepth, -beta, -alpha, subKillerMoveTab);
			}
		}
		
		UnMakeMove(cm, nToChessID, nToPieceID);

		bFirstMove = FALSE;

		m_MoveList[nDepth][i].nScore = nEval;

		if (nEval > nBestEval)
		{
			nBestEval = nEval;
			nBestMoveNum = i;
			cmBestMove = cm;
			cmBestMove.nScore = nEval;

			if (nEval > alpha)
			{
				alpha = nEval;
				
				if (nBestEval >= beta)
				{
					nHashFlag = HASHBETA;
					
					killerMoveTab.cmKiller[killerMoveTab.nPointer] = cm;
					killerMoveTab.nPointer = (killerMoveTab.nPointer + 1) & (MAXKILLERMOVECNT - 1);
					if (killerMoveTab.nKillerCount <= MAXKILLERMOVECNT - 1)
					{
						killerMoveTab.nKillerCount++;
					}
					DEBUGASSERT(killerMoveTab.nPointer >= 0 && killerMoveTab.nPointer <= MAXKILLERMOVECNT - 1);
					DEBUGASSERT(killerMoveTab.nKillerCount >= 1 && killerMoveTab.nKillerCount <= MAXKILLERMOVECNT);

					m_nMoveCutoffCnt[cm.nFrom][cm.nTo]++;

					STATEMENT_STAT(m_nBetaNodeCnt++;);
					STATEMENT_STAT(m_nNoCapMoveBetaNodeCnt++;);

					break;
				}
				
				bFindPV = TRUE;
				nHashFlag = HASHEXACT;

				if (nDepth == 0)
				{
					PrintBetterPV(nDepth, nSearchDepth, nEval, nHashFlag, cmBestMove);
					if (nEval + MAXSEARCHDEPTH >= g_nMateValue)
					{
						return nEval;
					}
				}
			}
		}
	}

	if (nHashFlag != HASHALPHA)
	{
		DEBUGASSERT(m_nHistoryScore[m_bIsRed][cmBestMove.nFrom][cmBestMove.nTo] >= 0 && 
			m_nHistoryScore[m_bIsRed][cmBestMove.nFrom][cmBestMove.nTo] + 
			(1 << (nSearchDepth - nDepth)) <= INT_MAX);
		m_nHistoryScore[m_bIsRed][cmBestMove.nFrom][cmBestMove.nTo] += 
			(1 << (nSearchDepth - nDepth));

		STATEMENT_STAT(m_nAlphaNodeCnt++;);

		//更新PVLine
//		if (nHashFlag == HASHEXACT)
//		{
//			m_PVLineThink.pMoveList[m_nCurDepth] = cmBestMove;
//			
//			STATEMENT_STAT(m_nPVNodeCnt++;);
//		}
	}

	//非叶子节点的值保存到hash表
	// TT存储的Fail Low修正算法(ZMBL)[2005/12/22 19:48:02]
//	if (nHashFlag == HASHALPHA)
//	{
//		HASHITEM *pHashItem = &m_pHashTab[m_bIsRed][m_nHashTabIndex & m_nHashTabMask];
//
//		if (pHashItem->nCBCheckSum == m_nCBCheckSum && pHashItem->cmBestMove != m_cmNullMove)
//		{
//			cmBestMove = pHashItem->cmBestMove;
//		}
//	}
	RecordHash(nDepth, nSearchDepth, alpha, nHashFlag, cmBestMove);

	return nBestEval;
}

/*******************************************************************************************************
函数名 : QuiesSearch
描  述 : 静态搜索
参  数 : 
        FSINT nDepth : 
		FSINT alpha  : 
		FSINT beta   : 
返回值 : 类型为 FSINT
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年10月5日 19:24:28
*******************************************************************************************************/
FSINT CSearchEngine::QuiesSearch(FSINT nDepth, FSINT alpha, FSINT beta)
{
	DEBUGASSERT(nDepth >= 0 && nDepth < MAXSEARCHDEPTH);
	DEBUGASSERT(beta <= -g_nPerCheckValue);
	DEBUGASSERT(alpha >= g_nPerCheckValue && alpha <= beta);

	//双将俱在
	DEBUGASSERT(m_nPiecePos[m_nKingID2PieceID[m_bIsRed]] != INVALIDPIECEPOS);
	DEBUGASSERT(m_nPiecePos[m_nKingID2PieceID[1 - m_bIsRed]] != INVALIDPIECEPOS);

	//按时间思考时,超时返回
	if (m_ThinkType == TT_TIME && (clock() - m_nTimeStartThink) > m_nTimeThinkLimit && 
		(m_bPonder == FALSE || m_bPonderHit == TRUE))
	{
		SearchStop();
#ifdef _WIN32
		ExitThread(0);
#else
#endif
	}

#ifdef STAT

	DEBUGASSERT(m_nVistNodeCnt >= 0 && m_nVistNodeCnt + 1 <= UINT_MAX);

	//访问的节点数加1
	m_nVistNodeCnt++;

	m_nQuiesNodeCnt++;

#endif

	FSINT	i;
	FSINT	nEval;
	FSINT	nCount;
	FSINT	nToChessID;
	FSINT	nToPieceID;
	FSINT	nHashFlag = HASHALPHA;	//hash节点标志
	FSINT	nBestMoveNum = 0;
	FSINT	nBestEval = nDepth - g_nMateValue;

	CHESSMOVE cm;
	CHESSMOVE cmBestMove;

	//杀棋,直接返回
	if (nBestEval >= beta)
	{
		return nBestEval;
	}

	//重复检测
	if ((nEval = IsRep(1)) != FALSE)
//	if ((nEval = IsRep(3)) != FALSE)
	{
		return RepValue(nEval);
	}

	//和棋裁剪
	if (IsDraw() == TRUE)
	{
		return DrawValue();
	}

	//探测hash表
	if ((nEval = ProbeQSHash(alpha, beta)) != g_nInvalidValue)
	{
		return nEval;
	}

	// 防止延伸过深[2005/11/2 13:14:45]
	if (nDepth >= MAXSEARCHDEPTH)
	{
		return Evaluate();
	}

	//如果被将军,则需要产生所有走法解将
	if (IsChecked(1 - m_bIsRed) == TRUE)
	{
		//产生所有吃子走法
		GenCapMoves(nDepth);
		nCount = m_nMovesCnt;
		DEBUGASSERT(nCount >= 0 && nCount <= MAXCAPMOVECNT);
		
		//取走法得分 [2005/10/20 23:14:14]
		for (i = 0; i < nCount; i++)
		{
			cm = m_MoveList[nDepth][i];
			
			//被吃子的价值 - 吃子的价值,即MVV - LVA
			//		m_MoveList[nDepth][i].nScore = 
			//			g_nBasicValues[g_nChessID2Chess[m_nCBChessID[cm.nTo]]] - cm.nScore;
			
			//被吃子的价值,吃子的价值,即先比较被吃子的价值,再比较吃子的价值
			m_MoveList[nDepth][i].nScore = 
				(g_nBasicValues[g_nChessID2Chess[m_nCBChessID[cm.nTo]]] << 16) | cm.nScore;
		}
		
		//对走法排序
		sort(m_MoveList[nDepth], m_MoveList[nDepth] + nCount, ::UDgreater);
		
		//扫描所有不吃子走法
		for (i = 0; i < nCount; i++)
		{
			cm = m_MoveList[nDepth][i];
			
			if (IsGoodCapMove(cm) == FALSE)
			{
				continue;
			}
			
			MakeMove(cm, nToChessID, nToPieceID);
			
			//判断该走法是否合法
			if (IsLegalMove(cm) == FALSE)
			{
				UnMakeMove(cm, nToChessID, nToPieceID);
				continue;
			}
			
			nEval = -QuiesSearch(nDepth + 1, -beta, -alpha);
			
			UnMakeMove(cm, nToChessID, nToPieceID);
			
			//超时
			if (nEval == -g_nTimeOutScore)
			{
				return g_nTimeOutScore;
			}
			
			if (nEval > nBestEval)
			{
				nBestEval = nEval;
				if (nEval > alpha)
				{
					alpha = nEval;
					
					nBestMoveNum = i;
					cmBestMove = cm;
					
					if (alpha >= beta)
					{
						nHashFlag = HASHBETA;
						RecordQSHash(alpha, nHashFlag, cmBestMove);

						INCREMENT_STAT(m_nQuiesBetaNodeCnt);
						return alpha;
					}
					
					nHashFlag = HASHEXACT;
				}
			}
		}
		
		//产生所有不吃子走法
//		GenNoCapMoves(nDepth);
		GenMoves(nDepth);
		nCount = m_nMovesCnt;
		DEBUGASSERT(nCount >= 0 && nCount <= MAXCAPMOVECNT);
		
		//取历史得分
		for (i = 0; i < nCount; i++)
		{
			cm = m_MoveList[nDepth][i];
			m_MoveList[nDepth][i].nScore = m_nHistoryScore[m_bIsRed][cm.nFrom][cm.nTo];
		}

		//取走法得分 [2005/10/20 23:14:14]
//		for (i = 0; i < nCount; i++)
//		{
//			cm = m_MoveList[nDepth][i];
//			
//			//被吃子的价值 - 吃子的价值,即MVV - LVA
//			//		m_MoveList[nDepth][i].nScore = 
//			//			g_nBasicValues[g_nChessID2Chess[m_nCBChessID[cm.nTo]]] - cm.nScore;
//			
//			//被吃子的价值,吃子的价值,即先比较被吃子的价值,再比较吃子的价值
//			m_MoveList[nDepth][i].nScore = 
//				(g_nBasicValues[g_nChessID2Chess[m_nCBChessID[cm.nTo]]] << 16) | cm.nScore;
//		}
		
		//对走法排序
		sort(m_MoveList[nDepth], m_MoveList[nDepth] + nCount, ::UDgreater);
		
		//扫描所有不吃子走法
		for (i = 0; i < nCount; i++)
		{
			cm = m_MoveList[nDepth][i];
			
			MakeMove(cm, nToChessID, nToPieceID);
			
			//判断该走法是否合法
			if (IsLegalMove(cm) == FALSE)
			{
				UnMakeMove(cm, nToChessID, nToPieceID);
				continue;
			}
			
			nEval = -QuiesSearch(nDepth + 1, -beta, -alpha);
			
			UnMakeMove(cm, nToChessID, nToPieceID);
			
			if (nEval > nBestEval)
			{
				nBestEval = nEval;
				if (nEval > alpha)
				{
					alpha = nEval;
					
					nBestMoveNum = i;
					cmBestMove = cm;
					
					if (alpha >= beta)
					{
						nHashFlag = HASHBETA;
						INCREMENT_STAT(m_nQuiesBetaNodeCnt);
						break;
					}
					
					nHashFlag = HASHEXACT;
				}
			}
		}

		INCREMENT_STAT(nHashFlag == HASHALPHA ? m_nQuiesAlphaNodeCnt : m_nQuiesPVNodeCnt);

		RecordQSHash(alpha, nHashFlag, cmBestMove);

		return nBestEval;
	}

	nEval = Evaluate();
	if (nEval > nBestEval)
	{
		nBestEval = nEval;
		if (nEval > alpha)
		{
			alpha = nEval;
			
			if (alpha >= beta)
			{
				//RecordQSHash(beta, HASHBETA, m_cmNullMove);
				INCREMENT_STAT(m_nQuiesNullNodeCnt);

				return beta;
			}
			
			nHashFlag = HASHEXACT;
		}
	}

	//产生所有吃子走法
	GenCapMoves(nDepth);
	nCount = m_nMovesCnt;
	DEBUGASSERT(nCount >= 0 && nCount <= MAXCAPMOVECNT);

	//取走法得分 [2005/10/20 23:14:14]
	for (i = 0; i < nCount; i++)
	{
		cm = m_MoveList[nDepth][i];

		//被吃子的价值 - 吃子的价值,即MVV - LVA
//		m_MoveList[nDepth][i].nScore = 
//			g_nBasicValues[g_nChessID2Chess[m_nCBChessID[cm.nTo]]] - cm.nScore;

		//被吃子的价值,吃子的价值,即先比较被吃子的价值,再比较吃子的价值
		m_MoveList[nDepth][i].nScore = 
			(g_nBasicValues[g_nChessID2Chess[m_nCBChessID[cm.nTo]]] << 16) | cm.nScore;
	}

	//对走法排序
	sort(m_MoveList[nDepth], m_MoveList[nDepth] + nCount, ::UDgreater);

	//扫描所有吃子走法
	for (i = 0; i < nCount; i++)
	{
		cm = m_MoveList[nDepth][i];

		//不是好的吃子走法
//		if (IsGoodCapMove(cm) == FALSE)
//		{
//			continue;
//		}

		MakeMove(cm, nToChessID, nToPieceID);

		//判断该走法是否合法
		if (IsLegalMove(cm) == FALSE)
		{
			UnMakeMove(cm, nToChessID, nToPieceID);
			continue;
		}

		nEval = -QuiesSearch(nDepth + 1, -beta, -alpha);

		UnMakeMove(cm, nToChessID, nToPieceID);

		if (nEval > nBestEval)
		{
			nBestEval = nEval;
			if (nEval > alpha)
			{
				alpha = nEval;
				nBestMoveNum = i;
				
				if (alpha >= beta)
				{
					nHashFlag = HASHBETA;
					INCREMENT_STAT(m_nQuiesBetaNodeCnt);
					break;
				}
				
				nHashFlag = HASHEXACT;
			}
		}
	}
	
	cm = m_MoveList[nDepth][nBestMoveNum];
	RecordQSHash(alpha, nHashFlag, cm);
	INCREMENT_STAT(nHashFlag == HASHALPHA ? m_nQuiesAlphaNodeCnt : m_nQuiesPVNodeCnt);

	return nBestEval;
}

/*******************************************************************************************************
函数名 : IsLegalMove
描  述 : 注意:这个函数在MakeMove函数执行后才调用!
		 走法cm是否合法,即走了走法cm后是否产生长将,长捉,己方被将军
参  数 : 
        CHESSMOVE cm : 走法
返回值 : FSINT
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:42:05
*******************************************************************************************************/
inline FSBOOL CSearchEngine::IsLegalMove(CHESSMOVE cm)
{
	DEBUGASSERT(cm.nFrom >= 0 && cm.nFrom < MAXCHESSGRIDNUM);
	DEBUGASSERT(cm.nTo >= 0 && cm.nTo < MAXCHESSGRIDNUM);
	
	FSINT nBKingPos;
	FSINT nRKingPos;

	nBKingPos = m_nPiecePos[m_nChessID2PieceID[BKING][0]];
	nRKingPos = m_nPiecePos[m_nChessID2PieceID[RKING][0]];

	DEBUGASSERT(nBKingPos != INVALIDPIECEPOS);
	DEBUGASSERT(nRKingPos != INVALIDPIECEPOS);

	//双将是否照面,1 - m_bIsRed方是否被将军,是否产生长将,长捉
	return 1 - (m_bIsTwoKingMeet[m_nBRKingPosCheckNum[BLACK][nBKingPos]][m_nBRKingPosCheckNum[RED][nRKingPos]][m_nBitRank[m_CGN2Pos[nBKingPos].x]] | 
		IsChecked(m_bIsRed));
}

/*******************************************************************************************************
函数名 : IsValidMove
描  述 : 判断走法cm是否是有效的走法,仅用在杀手着法启发中,在函数MakeMove之前运行
参  数 : 
        CHESSMOVE cm : 杀手走法
返回值 : 类型为 FSBOOL
版  本 : 0.1
作  者 : ahei
时  间 : 2005年10月1日 20:05:51
*******************************************************************************************************/
inline FSBOOL CSearchEngine::IsValidMove(CHESSMOVE cm)
{
	FSINT nFrom;
	FSINT nTo;

	nFrom = cm.nFrom;
	nTo = cm.nTo;

	//走法的起始位置和目标位置都在棋盘上
	DEBUGASSERT(ISINBOARD(nFrom));
	DEBUGASSERT(ISINBOARD(nTo));

	FSINT nFromPieceID;

	//源无本方棋子,这里要求m_bIsRed_PieceID[INVALIDPIECEID] != 0,1
	if (m_bIsRed_PieceID[nFromPieceID = m_nCBPieceID[nFrom]] != m_bIsRed)
	{
		return FALSE;
	}

	FSINT nToPieceID = m_nCBPieceID[nTo];
	
	//源和目的棋子颜色相同
	if ((m_bIsBlack_PieceID[nFromPieceID] ^ m_bIsBlack_PieceID[nToPieceID]) == 0)
	{
		return FALSE;
	}

//	FSINT nFromI, nFromJ;
//
//	switch(g_nChessID2Chess[m_nCBChessID[nFrom]])
//	{
//	case ROOK:
//		nFromI = m_CGN2Pos[nFrom].x;
//		nFromJ = m_CGN2Pos[nFrom].y;
//		DEBUGASSERT(m_bIsRookFilePreValidMove[nFrom][nTo][m_nBitFile[nFromJ]] == FALSE || 
//					m_bIsRookFilePreValidMove[nFrom][nTo][m_nBitFile[nFromJ]] == TRUE);
//		DEBUGASSERT(m_bIsRookRankPreValidMove[nFrom][nTo][m_nBitRank[nFromI]] == FALSE || 
//					m_bIsRookRankPreValidMove[nFrom][nTo][m_nBitRank[nFromI]] == TRUE);
//		return m_bIsRookFilePreValidMove[nFrom][nTo][m_nBitFile[nFromJ]] || 
//			   m_bIsRookRankPreValidMove[nFrom][nTo][m_nBitRank[nFromI]];
//
//	case KNIGHT:
//		DEBUGASSERT(nTo - nFrom + 21 >= 0 && nTo - nFrom + 21 < 43);
//		return m_nCBPieceID[nFrom + g_nKnightLegPos[nTo - nFrom + 21]] == INVALIDPIECEID;
//
//	case CANNON:
//		nFromI = m_CGN2Pos[nFrom].x;
//		nFromJ = m_CGN2Pos[nFrom].y;
//		DEBUGASSERT(m_bIsCannonFilePreValidMove[nFrom][nTo][m_nBitFile[nFromJ]][m_nPieceID2Bit[nToPieceID]] == FALSE || 
//					m_bIsCannonFilePreValidMove[nFrom][nTo][m_nBitFile[nFromJ]][m_nPieceID2Bit[nToPieceID]] == TRUE);
//		DEBUGASSERT(m_bIsCannonRankPreValidMove[nFrom][nTo][m_nBitRank[nFromI]][m_nPieceID2Bit[nToPieceID]] == FALSE || 
//					m_bIsCannonRankPreValidMove[nFrom][nTo][m_nBitRank[nFromI]][m_nPieceID2Bit[nToPieceID]] == TRUE);
//		return m_bIsCannonFilePreValidMove[nFrom][nTo][m_nBitFile[nFromJ]][m_nPieceID2Bit[nToPieceID]] || 
//			   m_bIsCannonRankPreValidMove[nFrom][nTo][m_nBitRank[nFromI]][m_nPieceID2Bit[nToPieceID]];
//
//	case KING:
//	case ADVISOR:
//	case PAWN:
//		return TRUE;
//
//	case BBISHOP:
//		return m_nCBPieceID[(nFrom + nTo) >> 1] == INVALIDPIECEID;
//	
//	default:
//		DEBUGASSERT(0 > 1);
//		return FALSE;
//	}

	FSINT nFromI, nFromJ;
	FSINT nToI, nToJ;

	switch(g_nChessID2Chess[m_nCBChessID[nFrom]])
	{
	case ROOK:
		nFromI = m_CGN2Pos[nFrom].x;
		nFromJ = m_CGN2Pos[nFrom].y;
		nToI = m_CGN2Pos[nTo].x;
		nToJ = m_CGN2Pos[nTo].y;
		DEBUGASSERT(m_bIsRookPreValidMove[nFromI][nToI][m_nBitFile[nFromJ]] == FALSE || 
					m_bIsRookPreValidMove[nFromI][nToI][m_nBitFile[nFromJ]] == TRUE);
		DEBUGASSERT(m_bIsRookPreValidMove[nFromJ][nToJ][m_nBitRank[nFromI]] == FALSE || 
					m_bIsRookPreValidMove[nFromJ][nToJ][m_nBitRank[nFromI]] == TRUE);
		return (nFromJ == nToJ && m_bIsRookPreValidMove[nFromI][nToI][m_nBitFile[nFromJ]]) || 
			   (nFromI == nToI && m_bIsRookPreValidMove[nFromJ][nToJ][m_nBitRank[nFromI]]);

	case KNIGHT:
		DEBUGASSERT(nTo - nFrom + 21 >= 0 && nTo - nFrom + 21 < 43);
		return m_nCBPieceID[nFrom + g_nKnightLegPos[nTo - nFrom + 21]] == INVALIDPIECEID;

	case CANNON:
		nFromI = m_CGN2Pos[nFrom].x;
		nFromJ = m_CGN2Pos[nFrom].y;
		nToI = m_CGN2Pos[nTo].x;
		nToJ = m_CGN2Pos[nTo].y;
		DEBUGASSERT(m_bIsCannonPreValidMove[nFromI][nToI][m_nBitFile[nFromJ]][m_nPieceID2Bit[nToPieceID]] == FALSE || 
					m_bIsCannonPreValidMove[nFromI][nToI][m_nBitFile[nFromJ]][m_nPieceID2Bit[nToPieceID]] == TRUE);
		DEBUGASSERT(m_bIsCannonPreValidMove[nFromJ][nToJ][m_nBitRank[nFromI]][m_nPieceID2Bit[nToPieceID]] == FALSE || 
					m_bIsCannonPreValidMove[nFromJ][nToJ][m_nBitRank[nFromI]][m_nPieceID2Bit[nToPieceID]] == TRUE);
		return (nFromJ == nToJ && m_bIsCannonPreValidMove[nFromI][nToI][m_nBitFile[nFromJ]][m_nPieceID2Bit[nToPieceID]]) || 
			   (nFromI == nToI && m_bIsCannonPreValidMove[nFromJ][nToJ][m_nBitRank[nFromI]][m_nPieceID2Bit[nToPieceID]]);

	case KING:
	case ADVISOR:
	case PAWN:
		return TRUE;

	case BBISHOP:
		return m_nCBPieceID[(nFrom + nTo) >> 1] == INVALIDPIECEID;
	
	default:
		DEBUGASSERT(0 > 1);
		return FALSE;
	}
}

/*******************************************************************************************************
函数名 : IsGoodCapMove
描  述 : 此函数要在MakeMove函数运行之前运行,这是与IsLegalMove函数的不同之处
参  数 : 
        CHESSMOVE cm : 
返回值 : 类型为 FSBOOL
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年10月24日 22:20:10
*******************************************************************************************************/
FSBOOL CSearchEngine::IsGoodCapMove(const CHESSMOVE& cm)
{
	FSINT nFrom = cm.nFrom;
	FSINT nTo = cm.nTo;
	FSINT nToChess;

	nToChess = g_nChessID2Chess[m_nCBChessID[nTo]];

	//走法的起始位置和目标位置都在棋盘上
	DEBUGASSERT(ISINBOARD(nFrom));
	DEBUGASSERT(ISINBOARD(nTo));

	//不能吃将
	DEBUGASSERT(nToChess != KING);

	//士相
	if (nToChess == ADVISOR || nToChess == BISHOP)
	{
		return FALSE;
	}

	//车马炮
	if (nToChess == ROOK || nToChess == KNIGHT || nToChess == CANNON)
	{
		return TRUE;
	}

	// 兵[2005/10/25 10:39:59]
	return m_bIsRed ? (m_CGN2Pos[nTo].y <= 4) : (m_CGN2Pos[nTo].y >= 5);
}

/*******************************************************************************************************
函数名 : ProbeHash
描  述 : 探测hash表
参  数 : 
        FSINT nDepth : 
		FSINT alpha  : 
		FSINT beta   : 
返回值 : 类型为 inline
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年10月18日 16:42:10
*******************************************************************************************************/
inline FSINT CSearchEngine::ProbeHash(FSINT nDepth, FSINT nSearchDepth, FSINT alpha, FSINT beta, CHESSMOVE& cmBestMove)
{
	HASHITEM hashItem;

	DEBUGASSERT((m_nHashTabIndex & m_nHashTabMask) >= 0 && (m_nHashTabIndex & m_nHashTabMask) <= m_nHashTabMask);
	
	STATEMENT_STAT(m_nProbeHashCnt++;);

	hashItem = m_pHashTab[m_bIsRed][m_nHashTabIndex & m_nHashTabMask];
	if (hashItem.nCBCheckSum == m_nCBCheckSum)
	{
		FSINT nHashVal = hashItem.nEval;

		// 说明此hash项是长将时的局面,不能要[2005/12/22 18:23:51]
		if (nHashVal > g_nMateValue)
		{
			return g_nInvalidValue;
		}

		//返回最佳走法
		cmBestMove = hashItem.cmBestMove;
//		if (hashItem.nDepth >= 5)
//		{
//			cmBestMove = hashItem.cmBestMove;
//		}

		if (hashItem.nDepth < nSearchDepth - nDepth)
		{
			return g_nInvalidValue;
		}

		if (hashItem.nHashFlag == HASHEXACT)
		{
			m_PVLineThink.pMoveList[nDepth] = hashItem.cmBestMove;
			STATEMENT_STAT(m_nHitHashCnt++;);
			return nHashVal;
		}

		//hashItem.nHashFlag != HASHEXACT
		if (hashItem.nHashFlag == HASHALPHA && nHashVal <= alpha)
		{
			STATEMENT_STAT(m_nHitHashCnt++;);
			return alpha;
		}

		if (hashItem.nHashFlag == HASHBETA && nHashVal >= beta)
		{
			STATEMENT_STAT(m_nHitHashCnt++;);
			return beta;
		}

		return g_nInvalidValue;
	}

	return g_nInvalidValue;

//	FSINT nRet = GetHash(m_pHashTabByDepth, nDepth, nSearchDepth, alpha, beta, cmBestMove);
//
//	if (nRet != g_nInvalidValue)
//	//if (nRet != g_nInvalidValue || cmBestMove == m_cmNullMove)
//	{
//		return nRet;
//	}
//
//	return GetHash(m_pHashTab, nDepth, nSearchDepth, alpha, beta, cmBestMove);
}

/*******************************************************************************************************
函数名 : RecordHash
描  述 : 录入hash表
参  数 : 
        FSINT nDepth    : 
		FSINT nVal      : 
		FSINT nHashFlag : 
返回值 : 类型为 inline
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年10月18日 16:42:01
*******************************************************************************************************/
inline FSVOID CSearchEngine::RecordHash(FSINT nDepth, FSINT nSearchDepth, FSINT nEval, FSINT nHashFlag, CHESSMOVE cmBestMove)
{
	DEBUGASSERT(cmBestMove == m_cmNullMove || 
		(ISINBOARD(cmBestMove.nFrom) && ISINBOARD(cmBestMove.nTo)));

	STATEMENT_STAT(m_nRecordHashCnt++;);

	// 深度替换策略[2005/10/31 18:48:11]
//	HASHITEM *pHashItem = &m_pHashTabByDepth[m_bIsRed][m_nHashTabIndex & m_nHashTabMask];
//	
//	if (pHashItem->nDepth <= m_nCurSearchDepth_ID - nDepth)
//	{
//		pHashItem->nDepth = m_nCurSearchDepth_ID - nDepth;
//		pHashItem->nEval = nEval;
//		pHashItem->nHashFlag = nHashFlag;
//		pHashItem->cmBestMove = cmBestMove;
//		pHashItem->nCBCheckSum = m_nCBCheckSum;
//
//		return;
//	}

	// 改进的深度替换策略[2005/10/31 18:48:23]
//	HASHITEM *pHashItem = &m_pHashTab[m_bIsRed][m_nHashTabIndex & m_nHashTabMask];
//
//	if (pHashItem->nCBCheckSum == m_nCBCheckSum && 
//		pHashItem->nDepth >= m_nCurSearchDepth_ID - nDepth)
//	{
//		return;
//	}
//
//	pHashItem->nDepth = m_nCurSearchDepth_ID - nDepth;
//	pHashItem->nEval = nEval;
//	pHashItem->nHashFlag = nHashFlag;
//	pHashItem->cmBestMove = cmBestMove;
//	pHashItem->nCBCheckSum = m_nCBCheckSum;

	HASHITEM hashItem;

	hashItem.nDepth = nSearchDepth - nDepth;
	hashItem.nEval = nEval;
	hashItem.nHashFlag = nHashFlag;
	hashItem.cmBestMove = cmBestMove;
	hashItem.nCBCheckSum = m_nCBCheckSum;
	
	m_pHashTab[m_bIsRed][m_nHashTabIndex & m_nHashTabMask] = hashItem;
}

/*******************************************************************************************************
函数名 : ProbeQSHash
描  述 : 
参  数 : 
        FSINT alpha : 
		FSINT beta  : 
返回值 : 类型为 FSINT
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年10月24日 16:04:11
*******************************************************************************************************/
inline FSINT CSearchEngine::ProbeQSHash(FSINT alpha, FSINT beta)
{
	QSHASHITEM hashItem;

	DEBUGASSERT((m_nHashTabIndex & m_nHashTabMask) >= 0 && (m_nHashTabIndex & m_nHashTabMask) <= m_nHashTabMask);
	
	STATEMENT_STAT(m_nProbeQSHashCnt++;);

	hashItem = m_pQSHashTab[m_bIsRed][m_nHashTabIndex & m_nHashTabMask];
	if (hashItem.nCBCheckSum == m_nCBCheckSum)
	{
		FSINT nHashVal = hashItem.nEval;

		// 说明此hash项是长将时的局面,不能要[2005/12/22 18:23:51]
		if (nHashVal > g_nMateValue)
		{
			return g_nInvalidValue;
		}

		if (hashItem.nHashFlag == HASHEXACT)
		{
			STATEMENT_STAT(m_nHitQSHashCnt++;);
			return nHashVal;
		}

		//hashItem.nHashFlag != HASHEXACT
		if (hashItem.nHashFlag == HASHALPHA && nHashVal <= alpha)
		{
			STATEMENT_STAT(m_nHitQSHashCnt++;);
			return alpha;
		}

		if (hashItem.nHashFlag == HASHBETA && nHashVal >= beta)
		{
			STATEMENT_STAT(m_nHitQSHashCnt++;);
			return beta;
		}
	}

	return g_nInvalidValue;
}

/*******************************************************************************************************
函数名 : RecordQSHash
描  述 : 
参  数 : 
        FSINT nVal           : 
		FSINT nHashFlag      : 
		CHESSMOVE cmBestMove : 
返回值 : 类型为 inline
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年10月24日 16:04:03
*******************************************************************************************************/
inline FSVOID CSearchEngine::RecordQSHash(FSINT nVal, FSINT nHashFlag, CHESSMOVE cmBestMove)
{
	STATEMENT_STAT(m_nRecordQSHashCnt++;);

	QSHASHITEM hashItem;

	hashItem.nEval = nVal;
	hashItem.nHashFlag = nHashFlag;
	hashItem.cmBestMove = cmBestMove;
	hashItem.nCBCheckSum = m_nCBCheckSum;
	
	m_pQSHashTab[m_bIsRed][m_nHashTabIndex & m_nHashTabMask] = hashItem;
}

/*******************************************************************************************************
函数名 : UCCIProc
描  述 : 
参  数 : 无
返回值 : FSINT
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:42:44
*******************************************************************************************************/
FSVOID CSearchEngine::UCCIProc()
{
	UCCICmd	ucciCmd;
	FSCHAR	*szUCCICmd = new FSCHAR[MAXBUFFERLEN];
	FSINT	nHashItemSize = sizeof(HASHITEM);
	FSCHAR	*szCmdCpy = szUCCICmd;	//szUCCICmd的拷贝,用于释放szUCCICmd的内存
	FSINT	nLimitMovesCnt;			//限着数
	FSINT	nAddTime;				//加时
	FSINT	nTemp;
#ifdef _WIN32
	FSDWORD	(WINAPI CSearchEngine::*p1)() = SearchMain;
	FSDWORD	(WINAPI *p2)(LPVOID);
	FSVOID (CALLBACK CSearchEngine::*pInternalCallBack)
		(FSUINT uID, FSUINT uMsg, FSDWORD dwUser, FSDWORD dw1, FSDWORD dw2) = ThinkTimeOut;
//	LPTIMECALLBACK pTimeOutCallBack;

	memcpy(&p2, &p1, sizeof(p1));
//	memcpy(&pTimeOutCallBack, &pInternalCallBack, sizeof(pInternalCallBack));

#else
#endif
	
	//显示引擎的版本号,版权,作者和授权用户
	DISPMSG0_ENDL(cout);
	DISPMSG1_ENDL(cout, "id name "ENGINENAME" "ENGINEVERSION);
	DISPMSG1_ENDL(cout, "id copyright 2004 - 2006");
	DISPMSG1_ENDL(cout, "id author "ENGINEAUTHOR);
	DISPMSG1_ENDL(cout, "id user Unknown User");

	//显示引擎所支持的选项
	DISPMSG7_ENDL(cout, "option hashsize type spin", 
		" min ", 2 * MINHASHTABSIZE * nHashItemSize / (1 << 20), 
		" max ", 2 * MAXHASHTABSIZE * nHashItemSize / (1 << 20), 
		" default ", 2 * (m_nHashTabMask + 1) * nHashItemSize / (1 << 20));
	DISPMSG3_ENDL(cout, "option bookfiles type string default ", m_szDir, BOOKFILE);

	//显示版权检查信息
	cout << "copyprotection OK" << endl;
	cout << "ucciok" << endl;

	if (m_bAllocateMem == FALSE)
	{
		Initial();
	}

	LoadFEN((FSCHAR*)g_szMidGamePos);

GetUCCICmd:
	szUCCICmd = szCmdCpy;
	*szUCCICmd = '\0';
	gets(szUCCICmd);
	if (strncmp(szUCCICmd, "", 1))
	{
		STATEMENT_TEST
		(
		CString str;

		str.Format("UCCI协议输入命令:%s", szUCCICmd);
		MessageBox(NULL, str, "命令提示", MB_OK);
		);
	}

	szUCCICmd = JumpNoMatterChar(szUCCICmd);
	ucciCmd = GetUCCICmd(szUCCICmd);
	szUCCICmd = JumpNoMatterChar(szUCCICmd);

	switch(ucciCmd)
	{
	case UCCICIsReady:
		if (m_esUCCIEgnStatus == EgnSBusy || m_esUCCIEgnStatus == EgnSSuspend)
		{
			UCCIBusy();
			break;
		}

		cout << "readyok" << endl;
		break;

	case UCCICPosition:
		if (m_esUCCIEgnStatus == EgnSBusy || m_esUCCIEgnStatus == EgnSSuspend)
		{
			UCCIBusy();
			break;
		}

		//特殊局面,特殊处理
		if (IsSpecialPos(szUCCICmd) == TRUE)
		{
			break;
		}

		if (LoadFEN(szUCCICmd) == FALSE)
		{
			cout <<	"FEN串"	<< szUCCICmd << "语法错误!" << endl;
			
			LoadFEN((FSCHAR*)g_szOpenGamePos);
		}
		break;

	case UCCICGo:
		if (m_esUCCIEgnStatus == EgnSBusy || m_esUCCIEgnStatus == EgnSSuspend)
		{
			UCCIBusy();
			break;
		}

		//后台思考
		if (!strncmp(szUCCICmd, "ponder", 6))
		{
			m_bPonder = TRUE;
			szUCCICmd += 6;
			szUCCICmd = JumpNoMatterChar(szUCCICmd);
		}
		else
		{
			m_bPonder = FALSE;
		}

		if (!strncmp(szUCCICmd, "depth", 5))
		{
			m_ThinkType = TT_DEPTH;		//按深度思考

			szUCCICmd += 5;		//跳过"depth"
			szUCCICmd = JumpNoMatterChar(szUCCICmd);
			m_nSearchDepth = atoi(szUCCICmd);	//读入搜索层数
			if (m_nSearchDepth > MAXSEARCHDEPTH)
			{
				cout <<	"搜索深度不能超过" << MAXSEARCHDEPTH << endl;
				break;
			}
			else if (m_nSearchDepth < 0)
			{
				cout <<	"搜索深度不能小于0" << endl;
				break;
			}
		}
		else if (!strncmp(szUCCICmd, "time", 4))
		{
			m_ThinkType = TT_TIME;		//按时间思考

			szUCCICmd += 4;		//跳过"time"
			szUCCICmd = JumpNoMatterChar(szUCCICmd);
			m_nTimeThinkLimit = atoi(szUCCICmd);	//读入思考时间
			m_nTimeThinkLimit *= 1000;
			szUCCICmd = JumpDigit(szUCCICmd);
			szUCCICmd = JumpNoMatterChar(szUCCICmd);
			if (szUCCICmd[0] == '\0')
			{
				nLimitMovesCnt = 1;
				m_nTimeThinkLimit /= nLimitMovesCnt;
			}
			else if(strncmp(szUCCICmd, "movestogo", 9) == 0)
			{
				szUCCICmd += 9;	//跳过"movestogo"
				szUCCICmd = JumpNoMatterChar(szUCCICmd);
				nLimitMovesCnt = atoi(szUCCICmd);
				if (nLimitMovesCnt <= 0)
				{
					nLimitMovesCnt = 1;

					cout << "限着数不能小于0,本引擎将按限着数为1处理" << endl;
				}
				m_nTimeThinkLimit /= nLimitMovesCnt;
			}
			else if (strncmp(szUCCICmd, "increment", 9) == 0)
			{
				szUCCICmd += 9;	//跳过"increment"
				szUCCICmd = JumpNoMatterChar(szUCCICmd);
				nAddTime = atoi(szUCCICmd);
				m_nTimeThinkLimit = (m_nTimeThinkLimit + nAddTime * 1000) / 20;
			}

//			#ifdef _WIN32
//			if (timeSetEvent(m_nTimeThinkLimit, 1, pTimeOutCallBack, (FSDWORD)this, 
//				TIME_PERIODIC | TIME_CALLBACK_FUNCTION) == NULL)
//			if (timeSetEvent(m_nTimeThinkLimit, 1, pTimeOutCallBack, 0, 
//				TIME_PERIODIC | TIME_CALLBACK_FUNCTION) == NULL)
//			{
//				DISPMSG1_ENDL(cout,	"创建定时器失败!");
//				break;
//			}
//			#else
//			#endif
		}
		else if (!strncmp(szUCCICmd, "", 1) || !strncmp(szUCCICmd, "infinite", 8))
		{
			m_ThinkType = TT_DEPTH;		//按深度思考
			m_nSearchDepth = MAXSEARCHDEPTH;
		}
		else
		{
			cout << "不可识别的UCCI协议指令:" << szCmdCpy << endl;
			break;
		}

		SearchGo();
		break;

	case UCCICSetOption:
		if (m_esUCCIEgnStatus == EgnSBusy || m_esUCCIEgnStatus == EgnSSuspend)
		{
			UCCIBusy();
			break;
		}

		if (strncmp(szUCCICmd, "bookfiles", 9) == 0)
		{
			strcpy(m_szBookFile, JumpNoMatterChar(szUCCICmd + 9));
			break;
		}

		if (strncmp(szUCCICmd, "loadbook", 8) == 0)
		{
			m_nGetOpenBookMoveErrCnt = 0;
			m_bIsHaveCheckMateMove = FALSE;
			break;
		}

		if (strncmp(szUCCICmd, "style", 5) == 0)
		{
			SetChessStyle(szUCCICmd + 5);
			break;
		}

		if (strncmp(szUCCICmd, "drawmoves", 9) == 0)
		{
			nTemp = m_nDrawMoves;

			m_nDrawMoves = atoi(JumpNoMatterChar(szUCCICmd + 9));
			if (m_nDrawMoves <= 0)
			{
				m_nDrawMoves = nTemp;

				DISPMSG1_ENDL(cout,	"自然限着数必须大于0!");
			}
			break;
		}
		
		DISPMSG1_ENDL(cout,	"setoption命令语法错误!");
		break;

	case UCCICPonderHit:
		m_bPonderHit = TRUE;
		break;

	case UCCICDebug:
		if (m_esUCCIEgnStatus == EgnSBusy || m_esUCCIEgnStatus == EgnSSuspend)
		{
			UCCIBusy();
			break;
		}
		break;

	case UCCICAddPosition:
		if (m_esUCCIEgnStatus == EgnSBusy || m_esUCCIEgnStatus == EgnSSuspend)
		{
			UCCIBusy();
			break;
		}
		break;
	
	case UCCICStop://停止思考
		UCCIStopThink();
		break;

	case UCCIPause://暂停思考
		#ifdef _WIN32
		if (SuspendThread(m_hThinkThread) == 0xFFFFFFFF)
		{
			cout << "暂停思考失败,请稍候再试!" << endl;
		}
		#else
		#endif
		break;

	case UCCIResume://继续思考
		STRIDEPLATFORM
		(
		if (ResumeThread(m_hThinkThread) == 0xFFFFFFFF)
		{
			cout << "继续思考失败,请稍候再试!" << endl;
		}, 
		;
		);
		break;

	case UCCICQuitUCCI://退出UCCI协议
		//结束搜索线程
		KillThread(m_hThinkThread);

		TRYDELETE(SAFEDELETE(szCmdCpy));
		return;

	case UCCICQuitEgn://退出引擎
		//结束搜索线程
		KillThread(m_hThinkThread);

		cout << "bye" << endl;

		TRYDELETE(SAFEDELETE(szCmdCpy));

		ExitEgn();	//退出引擎
		exit(EXIT_SUCCESS);
		break;

	case UCCIStatus:
		PrintEgnStatus();
		break;

	case UCCINullMove:
		if (szUCCICmd[0] == '\0')
		{
			if (m_bNullMoveOn == TRUE)
			{
				DISPMSG1_ENDL(cout,	"Null Move is on");
			}
			else
			{
				DISPMSG1_ENDL(cout,	"Null Move is off");
			}

			break;
		}

		if (!strncmp(szUCCICmd, "on", 3))
		{
			m_bNullMoveOn = TRUE;

			DISPMSG1_ENDL(cout,	"OK - Null Move On Now");
		}
		else if (!strncmp(szUCCICmd, "off", 4))
		{
			m_bNullMoveOn = FALSE;

			DISPMSG1_ENDL(cout,	"OK - Null Move Off Now");
		}
		else
		{
			DISPMSG2_ENDL(cout,	"错误的Null Move指令", szCmdCpy);
		}
		break;

	case UCCIOpenBook:
		if (szUCCICmd[0] == '\0')
		{
			if (m_bOpenBookOn == TRUE)
			{
				DISPMSG1_ENDL(cout,	"OpenBook is on");
			}
			else
			{
				DISPMSG1_ENDL(cout,	"OpenBook is off");
			}

			break;
		}

		if (!strncmp(szUCCICmd, "on", 3))
		{
			m_bOpenBookOn = TRUE;

			DISPMSG1_ENDL(cout,	"OK - OpenBook On Now");
		}
		else if (!strncmp(szUCCICmd, "off", 4))
		{
			m_bOpenBookOn = FALSE;

			DISPMSG1_ENDL(cout,	"OK - OpenBook Off Now");
		}
		else
		{
			DISPMSG2_ENDL(cout,	"错误的OpenBook指令", szCmdCpy);
		}
		break;

	case UCCILoadBook:
		m_nGetOpenBookMoveErrCnt = 0;
		break;

	case UCCIClearHash:
		ClearHash();
		
		DISPMSG1_ENDL(cout,	"OK - Clear Hash Success!");
		break;

	case UCCIHistroyShit:
		if (szUCCICmd[0] == '\0')
		{
			DISPMSG2_ENDL(cout,	"HistoryShift is ", m_nHistoryShift);

			break;
		}

		m_nHistoryShift = atoi(JumpNoMatterChar(szUCCICmd));

		DISPMSG1_ENDL(cout,	"OK");
		break;

	case UCCIDrawValue:
		if (szUCCICmd[0] == '\0')
		{
			DISPMSG2_ENDL(cout,	"DrawValue is ", m_nContemptFactor);

			break;
		}

		m_nContemptFactor = atoi(JumpNoMatterChar(szUCCICmd));

		DISPMSG1_ENDL(cout,	"OK");
		break;

	case UCCIDrawMoves:
		if (szUCCICmd[0] == '\0')
		{
			DISPMSG2_ENDL(cout,	"DrawMoves is ", m_nDrawMoves);

			break;
		}

		nTemp = m_nDrawMoves;

		m_nDrawMoves = atoi(JumpNoMatterChar(szUCCICmd));
		if (m_nDrawMoves <= 0)
		{
			m_nDrawMoves = nTemp;
			
			DISPMSG1_ENDL(cout,	"自然限着数必须大于0!");
			break;
		}

		DISPMSG1_ENDL(cout,	"OK");
		break;

	case UCCICEPlies:
//		if (szUCCICmd[0] == '\0')
		{
			DISPMSG2_ENDL(cout,	"CE Plies is ", m_nCheckExtensionPlies);

			break;
		}

//		nTemp = m_nCheckExtensionPlies;
//
//		m_nCheckExtensionPlies = atoi(JumpNoMatterChar(szUCCICmd));
//		if (m_nCheckExtensionPlies <= 0)
//		{
//			m_nCheckExtensionPlies = nTemp;
//			
//			DISPMSG1_ENDL(cout,	"将军延伸层数必须大于0!");
//			break;
//		}
//
//		DISPMSG1_ENDL(cout,	"OK");
		break;

	case UCCIOnlyMove:
		if (szUCCICmd[0] == '\0')
		{
			if (m_bOnlyMoveOn == TRUE)
			{
				DISPMSG1_ENDL(cout,	"OnlyMove is on");
			}
			else
			{
				DISPMSG1_ENDL(cout,	"OnlyMove is off");
			}

			break;
		}

		if (!strncmp(szUCCICmd, "on", 3))
		{
			m_bOnlyMoveOn = TRUE;

			DISPMSG1_ENDL(cout,	"OK - OnlyMove On Now");
		}
		else if (!strncmp(szUCCICmd, "off", 4))
		{
			m_bOnlyMoveOn = FALSE;

			DISPMSG1_ENDL(cout,	"OK - OnlyMove Off Now");
		}
		else
		{
			DISPMSG2_ENDL(cout,	"错误的OnlyMove指令", szCmdCpy);
		}
		break;

	case UCCICInvalid:
		cout << "不可识别的UCCI协议指令:" << szCmdCpy << endl;
		break;

	case UCCITime:
		DISPMSG3_ENDL(cout,	"Time Searched is ", MS2S(clock() - m_nTimeStartThink), "s");
		break;

	case UCCIGetSpecialFEN:
		DISPMSG1_ENDL(cout,	GetSpecialFEN(szUCCICmd));
		break;

	default:
		break;
	}

	Idle();

	goto GetUCCICmd;
}

/*******************************************************************************************************
函数名 : GetUCCICmd
描  述 : 
参  数 : 
        const FSCHAR *szUCCICmd : 
返回值 : UCCICmd
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:42:58
*******************************************************************************************************/
UCCICmd CSearchEngine::GetUCCICmd(FSCHAR * &szUCCICmd)
{
	if (strlen(szUCCICmd) >= 8)
	{
		FSCHAR	szTemp[MAXBUFFERLEN];
		
		szUCCICmd = JumpNoMatterChar(szUCCICmd);
		
		memcpy(szTemp, szUCCICmd, 8);
		szTemp[8] = '\0';
		strlwr(szTemp);
		
		if (!strncmp(szTemp,"position", 8))
		{
			szUCCICmd += 8;
			return UCCICPosition;
		}
	}

	strlwr(szUCCICmd);

	if (!strncmp(szUCCICmd,"ponderhit", 9))
	{
		szUCCICmd += 9;
		return UCCICPonderHit;
	}

	if (!strncmp(szUCCICmd,"stop", 4))
	{
		szUCCICmd += 4;
		return UCCICStop;
	}

	if (!strncmp(szUCCICmd,"pause", 5))
	{
		szUCCICmd += 5;
		return UCCIPause;
	}

	if (!strncmp(szUCCICmd,"resume", 6))
	{
		szUCCICmd += 6;
		return UCCIResume;
	}

	if (!strncmp(szUCCICmd,"isready", 7))
	{
		szUCCICmd += 7;
		return UCCICIsReady;
	}

	if (!strncmp(szUCCICmd,"setoption", 9))
	{
		szUCCICmd += 9;
		return UCCICSetOption;
	}

	if (!strncmp(szUCCICmd,"go", 2))
	{
		szUCCICmd += 2;
		return UCCICGo;
	}

	//此命令要放在quit命令前面处理
	if (!strncmp(szUCCICmd,"quitucci", 8))
	{
		szUCCICmd += 8;
		return UCCICQuitUCCI;
	}

	if (!strncmp(szUCCICmd,"quit", 4))
	{
		szUCCICmd += 4;
		return UCCICQuitEgn;
	}

	if (!strncmp(szUCCICmd,"debug",5))
	{
		szUCCICmd += 5;
		return UCCICDebug;
	}

	if (!strncmp(szUCCICmd,"addposition", 11))
	{
		szUCCICmd += 11;
		return UCCICAddPosition;
	}

	if (!strncmp(szUCCICmd, "status", 7))
	{
		return UCCIStatus;
	}

	if (!strncmp(szUCCICmd, "nullmove", 8))
	{
		szUCCICmd += 8;
		return UCCINullMove;
	}

	if (!strncmp(szUCCICmd, "openbook", 8))
	{
		szUCCICmd += 8;
		return UCCIOpenBook;
	}

	if (!strncmp(szUCCICmd, "endbook", 7))
	{
		szUCCICmd += 7;
		return UCCIEndBook;
	}

	if (!strncmp(szUCCICmd, "loadbook", 9))
	{
		szUCCICmd += 8;
		return UCCILoadBook;
	}

	if (!strncmp(szUCCICmd, "clearhash", 10))
	{
		szUCCICmd += 9;
		return UCCIClearHash;
	}

	if (!strncmp(szUCCICmd, "historyshift", 12))
	{
		szUCCICmd += 12;
		return UCCIHistroyShit;
	}
	
	if (!strncmp(szUCCICmd, "onlymove", 8))
	{
		szUCCICmd += 8;
		return UCCIOnlyMove;
	}
	
	if (!strncmp(szUCCICmd, "drawvalue", 9))
	{
		szUCCICmd += 9;
		return UCCIDrawValue;
	}

	if (strncmp(szUCCICmd, "time", 4) == 0)
	{
		szUCCICmd += 4;
		return UCCITime;
	}

	if (strncmp(szUCCICmd, "drawmoves", 9) == 0)
	{
		szUCCICmd += 9;
		return UCCIDrawMoves;
	}
	
	if (strncmp(szUCCICmd, "ceplies", 7) == 0)
	{
		szUCCICmd += 7;
		return UCCICEPlies;
	}

	if (strncmp(szUCCICmd, "getspecialfen", 13) == 0)
	{
		szUCCICmd += 13;
		return UCCIGetSpecialFEN;
	}

	if (strncmp(szUCCICmd, "", 1))
	{
		return UCCICInvalid;
	}

	return UCCICNone;
}

/*******************************************************************************************************
函数名 : LoadFEN
描  述 : 
参  数 : 
        FSCHAR *szStr : 
返回值 : 无
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:43:11
*******************************************************************************************************/
FSBOOL CSearchEngine::LoadFEN(FSCHAR *szFEN)
{
	if (szFEN == 0)
	{
		DISPMSG1_ENDL(cout,	"FEN串不能为空");

		return FALSE;
	}

	FSINT p;
	FSINT i, j;
	FSINT nCuri = 0;
	FSINT nCurj = 9;
	FSINT nPos;

	//清0原来棋盘特征
	// m_nBitFile
	// m_nBitRank
	memset(m_nBitFile, 0, MAXYSCALENUM * sizeof(FSINT));
	memset(m_nBitRank, 0, MAXXSCALENUM * sizeof(FSINT));

	// m_nBRBitFile
	memset(m_nBRBitFile[BLACK], 0, MAXYSCALENUM * sizeof(FSINT));
	memset(m_nBRBitFile[RED], 0, MAXYSCALENUM * sizeof(FSINT));

	// m_nBRBitRank
	memset(m_nBRBitRank[BLACK], 0, MAXXSCALENUM * sizeof(FSINT));
	memset(m_nBRBitRank[RED], 0, MAXXSCALENUM * sizeof(FSINT));

	// m_nCBChessID
	// m_nCBPieceID
	memset(m_nCBChessID, NOCHESS, MAXCHESSGRIDNUM * sizeof(FSINT));
	for (i = 0; i < MAXCHESSGRIDNUM; i++)
	{
		m_nCBPieceID[i] = INVALIDPIECEID;
	}

	// m_nPiecePos
	for (i = 0; i < MAXPIECEID; i++)
	{
		m_nPiecePos[i] = INVALIDPIECEPOS;
	}

	for (i = 0; i < m_nRepHashTabMask + 1; i++)
	{
		m_nRepHashTab[i] = -1;
	}

	m_nCurDepth = 0;

	m_nAllChessCnt = 0;
	m_nBigChessCnt = 0;

	//m_nChessCnt
	memset(m_nChessCnt, 0, 15 * sizeof(FSINT));

	//根据FEN字符串更新m_nCBChessID
	//跳过前面的空格,Tab键
	szFEN = JumpNoMatterChar(szFEN);

	FSCHAR szTemp[4];

	memcpy(szTemp, szFEN, 3);
	strlwr(szTemp);
	if (strncmp(szTemp, "fen", 3) != 0)
	{
		DISPMSG1_ENDL(cout,	"FEN串语法错误,缺少关键字fen");

		return FALSE;
	}

	szFEN = JumpNoMatterChar(szFEN + 3);
	nPos = 0;
	while(szFEN[nPos] != ' ')
	{
		if (isdigit(szFEN[nPos]))
		{
			nCuri += szFEN[nPos] - '0';
		}
		else if (szFEN[nPos] == '/')
		{
			nCuri = 0;
			nCurj--;
		}
		else if (szFEN[nPos] >= 'a' && szFEN[nPos] <= 'z')//为小写字母
		{
			m_nCBChessID[10 * nCuri + nCurj] = g_nAlpha2Chess[szFEN[nPos] -'a'];
			nCuri++;
		}
		else if (szFEN[nPos] >= 'A' && szFEN[nPos] <= 'Z')//为大写字母
		{
			m_nCBChessID[10 * nCuri + nCurj] = g_nAlpha2Chess[szFEN[nPos] -'A'] + 7;
			nCuri++;
		}
		else//错误处理
		{
			return FALSE;
		}

		nPos++;
	}

	//更新棋盘特征
	FSINT nCnt[15] = {0};
	FSINT nChessID;
	FSINT nChess;
	FSINT nPieceID;

	m_nHashTabIndex = 0;
	m_nCBCheckSum = 0;
	for (p = 0; p < 90; p++)
	{
		i = m_CGN2Pos[p].x;
		j = m_CGN2Pos[p].y;

		nChessID = m_nCBChessID[p];

		m_nHashTabIndex ^= m_nHashKey32[nChessID][i][j];
		m_nCBCheckSum ^= m_nHashKey64[nChessID][i][j];

		m_nChessCnt[nChessID]++;	// [2006/1/2 18:52:11]

		if (nChessID == NOCHESS)
		{
			m_nCBPieceID[p] = INVALIDPIECEID;
			continue;
		}

		//所有子位行位列
		m_nBitFile[j] |= m_nMask[i];
		m_nBitRank[i] |= m_nMask[j];

		//单方子位行位列
		m_nBRBitFile[m_bIsRed_ChessID[nChessID]][j] |= m_nMask[i];
		m_nBRBitRank[m_bIsRed_ChessID[nChessID]][i] |= m_nMask[j];

		nPieceID = m_nChessID2PieceID[nChessID][nCnt[nChessID]];

		m_nPiecePos[nPieceID] = p;
		m_nCBPieceID[p] = nPieceID;

		nCnt[nChessID]++;

		m_nAllChessCnt++;

		if ((nChess = g_nChessID2Chess[nChessID]) == ROOK || nChess == KNIGHT || nChess == CANNON)
		{
			m_nBigChessCnt++;
		}
	}

	if (m_nBigChessCnt >= 11)
	{
		m_nChessAge = OPEN_GAME;
	}
	else if (m_nBigChessCnt <= 4)
	{
		m_nChessAge = END_GAME;
	}
	else
	{
		m_nChessAge = MID_GAME;
	}

//	g_nBasicValues[KNIGHT] = m_nKnightBasicValue[m_nChessAge];
//	g_nBasicValues[CANNON] = m_nCannonBasicValue[m_nChessAge];
//	g_nBasicValues[PAWN] = m_nPawnBasicValue[m_nChessAge];

	if (m_nBigChessCnt <= 1)
	{
		m_bNullMoveOn = FALSE;
	}
	else
	{
		m_bNullMoveOn = TRUE;
	}
	
	for (i = 1; i < 15; i++)
	{
		for (j = nCnt[i]; j < m_nInitChessCnt[i]; j++)
		{
			m_nPiecePos[m_nChessID2PieceID[i][j]] = INVALIDPIECEPOS;
		}
	}

	FSCHAR		szMove[5];	//走法
	CHESSMOVE	cm;
	UNDOMOVE	undoMove;
	
	szFEN = JumpNoMatterChar(szFEN + nPos);
	if (*szFEN == '\0')
	{
		return FALSE;
	}
	//读入该走方
	if (*szFEN  == 'b')
	{
		m_bIsRed = BLACK;
	}
	else
	{
		m_bIsRed = RED;

		// [2005/11/10 11:21:57]
		m_nHashTabIndex ^= m_nHashKeyPlayer32;
		m_nCBCheckSum ^= m_nHashKeyPlayer64;
	}

	szFEN = JumpNoMatterChar(szFEN + 1);
//	if (*szFEN == '-')
//	{
//		szFEN = JumpNoMatterChar(szFEN + 1);
//		szFEN = JumpNoMatterChar(szFEN + 1);
//		
//		m_nBouts = atoi(szFEN);	//读入双方没有吃子的走棋步数(半回合数)
//		szFEN = JumpDigit(szFEN);
//		szFEN = JumpNoMatterChar(szFEN);
//		
//		m_nCurBout = atoi(szFEN);//读入当前回合
//		szFEN = JumpDigit(szFEN);
//		szFEN = JumpNoMatterChar(szFEN);
//	}

	m_nNoEatMoveCnt_BS = 0;
	if ((szFEN = strstr(szFEN, "moves")) != NULL)
	{
		FSINT nToChessID;
		FSINT nToPieceID;
		
		szFEN += 5;
		while (*szFEN != '\0')
		{
			szFEN = JumpNoMatterChar(szFEN);
			if (*szFEN == '\0')
			{
				break;
			}
			
			strncpy(szMove, szFEN , 4);
			if (Str2ChessMove(szMove, cm) == FALSE)
			{
				return FALSE;
			}
			m_nHistoryCBCheckSum_NoEat[m_nNoEatMoveCnt_BS] = m_nCBCheckSum;
			MakeMove(m_Moves_NoEat[m_nNoEatMoveCnt_BS] = cm, nToChessID, nToPieceID);
			m_Moves_NoEat[m_nNoEatMoveCnt_BS].nKind = m_cmFight[m_nCurDepth - 1].nKind;
			m_nNoEatMoveCnt_BS++;
			
			FILLUNDOMOVE(undoMove, cm, nToChessID, nToPieceID);
			m_UndoMoveStack.push(undoMove);
			
			szFEN += 4;
		}
	}
	m_nNoEatMoveCnt_IS = m_nNoEatMoveCnt_BS;

	FSINT k;

	m_nBoardValue = 0;
	//红方
	for (i = 0; i < 16; i++)
	{
		j = m_nPiecePos[i];
		if (j != INVALIDPIECEPOS)
		{
			k = m_nPieceID2ChessID[i] - 7;
			DEBUGASSERT(k >= 0 && k < 8);

			m_nBoardValue -= g_nBasicValues[k];
			m_nBoardValue -= g_nPosValues[k][j];
		}
	}
	//黑方
	for (i = 16; i < 32; i++)
	{
		j = m_nPiecePos[i];
		if (j != INVALIDPIECEPOS)
		{
			k = m_nPieceID2ChessID[i];
			DEBUGASSERT(k >= 0 && k < 8);

			m_nBoardValue += g_nBasicValues[k];
			m_nBoardValue += g_nPosValues[k][89 - j];
		}
	}

	return TRUE;
}

/*******************************************************************************************************
函数名 : ChessMove2Str
描  述 : 
参  数 : 
        CHESSMOVE cm   : 
		FSCHAR *szMove : 
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年11月6日 15:49:39
*******************************************************************************************************/
FSVOID CSearchEngine::ChessMove2Str(const CHESSMOVE& cm, FSCHAR *szMove)
{
	switch(m_EgnProtocol)
	{
	case EgnPUCCI:
		ChessMove2UCCIStr(cm, szMove);
		break;
	
	case EgnPQianHong:
		ChessMove2QHStr(cm, szMove);
		break;

	default:
		break;
	}
}

/*******************************************************************************************************
函数名 : Str2ChessMove
描  述 : 
参  数 : 
        FSCHAR *szMoveStr : 
		CHESSMOVE& cm     : 
返回值 : 类型为 FSBOOL
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年11月6日 15:49:33
*******************************************************************************************************/
FSBOOL CSearchEngine::Str2ChessMove(FSCHAR *szMoveStr, CHESSMOVE& cm)
{
	switch(m_EgnProtocol)
	{
	case EgnPUCCI:
		return UCCIStr2ChessMove(szMoveStr, cm);
	
	case EgnPQianHong:
		return QHStr2ChessMove(szMoveStr, cm);

	default:
		return TRUE;
	}
}

/*******************************************************************************************************
函数名 : OutChessMove
描  述 : 
参  数 : 
        CHESSMOVE cm : 
返回值 : 无
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:43:20
*******************************************************************************************************/
FSVOID CSearchEngine::ChessMove2UCCIStr(const CHESSMOVE& cm, FSCHAR *szMove)
{
	_ASSERT(cm.nFrom >= 0 && cm.nFrom <= MAXCHESSGRIDNUM);
	_ASSERT(cm.nTo >= 0 && cm.nTo <= MAXCHESSGRIDNUM);
	_ASSERT(szMove != 0);

	szMove[0] = m_CGN2Pos[cm.nFrom].x + 'a';
	szMove[1] = m_CGN2Pos[cm.nFrom].y + '0';
	
	szMove[2] = m_CGN2Pos[cm.nTo].x + 'a';
	szMove[3] = m_CGN2Pos[cm.nTo].y + '0';

	szMove[4] = '\0';
}

/*******************************************************************************************************
函数名 : UCCIStr2ChessMove
描  述 : 
参  数 : 
        const FSCHAR *szMove : 
		CHESSMOVE& cm        : 
返回值 : 类型为 FSBOOL
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年11月6日 16:00:28
*******************************************************************************************************/
FSBOOL CSearchEngine::UCCIStr2ChessMove(FSCHAR *szMove, CHESSMOVE& cm)
{
	_ASSERT(szMove != 0);
	
	strlwr(szMove);

	if (szMove[0] < 'a' || szMove[0] > 'i' || szMove[1] < '0' || szMove[1] > '9' || 
		szMove[2] < 'a' || szMove[2] > 'i' || szMove[3] < '0' || szMove[3] > '9')
	{
		return FALSE;
	}

	cm.nFrom = m_nPos2CGN[szMove[0] - 'a'][szMove[1] - '0'];
	cm.nTo = m_nPos2CGN[szMove[2] - 'a'][szMove[3] - '0'];

	return TRUE;
}

/*******************************************************************************************************
函数名 : ChessMove2QHStr
描  述 : 
参  数 : 
        CHESSMOVE cm   : 
		FSCHAR *szICCS : 
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年11月6日 15:49:47
*******************************************************************************************************/
FSVOID CSearchEngine::ChessMove2QHStr(const CHESSMOVE& cm, FSCHAR *szICCS)
{
	_ASSERT(cm.nFrom >= 0 && cm.nFrom <= MAXCHESSGRIDNUM);
	_ASSERT(cm.nTo >= 0 && cm.nTo <= MAXCHESSGRIDNUM);
	_ASSERT(szICCS != 0);

	szICCS[0] = m_CGN2Pos[cm.nFrom].x + 'A';
	szICCS[1] = m_CGN2Pos[cm.nFrom].y + '0';
	
	szICCS[2] = '-';

	szICCS[3] = m_CGN2Pos[cm.nTo].x + 'A';
	szICCS[4] = m_CGN2Pos[cm.nTo].y + '0';

	szICCS[5] = '\0';
}

/*******************************************************************************************************
函数名 : QHStr2ChessMove
描  述 : 
参  数 : 
        FSCHAR *szICCS : 
		CHESSMOVE& cm  : 
返回值 : 类型为 FSBOOL
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年11月6日 15:49:52
*******************************************************************************************************/
FSBOOL CSearchEngine::QHStr2ChessMove(FSCHAR *szICCS, CHESSMOVE& cm)
{
	_ASSERT(szICCS != 0);
	
	if (szICCS[0] < 'a' || szICCS[0] > 'i' || szICCS[1] < '0' || szICCS[1] > '9' || 
		szICCS[3] < 'a' || szICCS[3] > 'i' || szICCS[4] < '0' || szICCS[4] > '9')
	{
		return FALSE;
	}

	cm.nFrom = m_nPos2CGN[szICCS[0] - 'a'][szICCS[1] - '0'];
	cm.nTo = m_nPos2CGN[szICCS[3] - 'a'][szICCS[4] - '0'];

	return TRUE;
}

/*******************************************************************************************************
函数名 : AheiPProc
描  述 : AHEI协议处理
参  数 : 无
返回值 : 无
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:43:27
*******************************************************************************************************/
FSVOID CSearchEngine::AheiPProc()
{

}

/*******************************************************************************************************
函数名 : QianHongPProc
描  述 : 浅红协议处理
参  数 : 无
返回值 : 无
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:43:40
*******************************************************************************************************/
FSVOID CSearchEngine::QianHongPProc(FSCHAR *szParam)
{
	STATEMENT_TEST
	(
	if (strncmp(szParam, "", 1))
	{
		CString str;
		
		str.Format("浅红协议输入命令:%s", szParam);
		MessageBox(NULL, str, "命令", MB_OK);
	}
	);

	if (!strncmp(szParam, "-info", 6))
	{
		PrintQHInfo();
		ExitEgn();	//退出引擎
		exit(EXIT_SUCCESS);
	}
	
	FSCHAR	*szQianHongCmd = new FSCHAR[MAXBUFFERLEN];
	FSCHAR	*szCmdCpy = szQianHongCmd; //szQianHongCmd的拷贝,用于释放szUCCICmd的内存
	FSINT	nQHLevel;
	FSINT	nToChessID;
	FSINT	nToPieceID;

	QianHongCmd	qianHongCmd;
	UNDOMOVE	undoMove;
	CHESSMOVE	cm;

#ifdef _WIN32
	FSVOID (CALLBACK CSearchEngine::*pInternalCallBack)
		(FSUINT uID, FSUINT uMsg, FSDWORD dwUser, FSDWORD dw1, FSDWORD dw2) = ThinkTimeOut;
//	LPTIMECALLBACK pTimeOutCallBack;

//	memcpy(&pTimeOutCallBack, &pInternalCallBack, sizeof(pInternalCallBack));
#else
#endif

	if (m_bAllocateMem == FALSE)
	{
		Initial();
	}

	LoadFEN((FSCHAR*)g_szOpenGamePos);

	m_nDrawMoves = 320;

GetQianHongCmd:
	szQianHongCmd = szCmdCpy;
	strncpy(szQianHongCmd, "", 1);
	gets(szQianHongCmd);

	if (strncmp(szQianHongCmd, "", 1))
	{
		STATEMENT_TEST
		(
		CString str;
		
		str.Format("浅红协议输入命令:%s", szQianHongCmd);
		MessageBox(NULL, str, "命令", MB_OK);
		);
	}

	qianHongCmd = GetQianHongCmd(szQianHongCmd);
	szQianHongCmd = JumpNoMatterChar(szQianHongCmd);

	switch(qianHongCmd)
	{
	case QHCNone:
		break;

	case QHCInvalid:
		DISPMSG2_ENDL(cout,	"不可识别的浅红协议指令:", szCmdCpy);
		break;

	case QHCLevel:
		if (szQianHongCmd[0] == '\0')
		{
			DISPMSG1_ENDL(cout,	m_nQHLevel);
		}
		else
		{
			nQHLevel = atoi(szQianHongCmd);
			if (nQHLevel < MINQHLEVEL || nQHLevel > MAXQHLEVEL)
			{
				DISPMSG6_ENDL(cout,	"ERROR - Bad level ", nQHLevel, 
					" use ", MINQHLEVEL, " to ", MAXQHLEVEL);

				STATEMENT_TEST
				(
				CString str;
				
				str.Format("ERROR - Bad level %d use %d to %d", nQHLevel, MINQHLEVEL, MAXQHLEVEL);
				MessageBox(NULL, str, "命令", MB_OK);
				);
			}
			else
			{
				m_nQHLevel = nQHLevel;
				m_nSearchDepth = g_nQHSchDptByLvl[m_nQHLevel - 1];
				DISPMSG2_ENDL(cout,	"OK - Set AI level to ", nQHLevel);

				STATEMENT_TEST
				(
				CString str;
				
				str.Format("OK - Set AI level to %d", nQHLevel);
				MessageBox(NULL, str, "命令", MB_OK);
				);
			}
		}
		break;

	case QHCFEN:
		szQianHongCmd = JumpNoMatterChar(szQianHongCmd);
		if (strlen(szQianHongCmd) < 3)
		{
			DISPMSG3_ENDL(cout,	"ERROR - The FEN ", szQianHongCmd, "is syntax error");

			break;
		}

		//特殊局面,特殊处理
		if (IsSpecialPos(szQianHongCmd) == TRUE)
		{
			break;
		}

		if (LoadFEN(szQianHongCmd) == TRUE)
		{
			DISPMSG1_ENDL(cout,	"OK");
		}
		else
		{
			DISPMSG3_ENDL(cout,	"ERROR - The FEN ", szQianHongCmd, "is syntax error");
		}
		break;

	case QHCPlay:
		if (QHStr2ChessMove(szQianHongCmd, cm) == FALSE)
		{
			DISPMSG2_ENDL(cout,	"ERROR - Bad move:", szQianHongCmd);
			break;
		}
		// 由于要判断循环,所以需要把非吃子走法保存起来[2005/12/22 14:44:28]
		if (m_nCBPieceID[cm.nTo] == INVALIDPIECEID)
		{
			m_nHistoryCBCheckSum_NoEat[m_nNoEatMoveCnt_BS] = m_nCBCheckSum;
			MakeMove(m_Moves_NoEat[m_nNoEatMoveCnt_BS] = cm, nToChessID, nToPieceID);
			m_Moves_NoEat[m_nNoEatMoveCnt_BS].nKind = m_cmFight[m_nCurDepth - 1].nKind;
			m_nNoEatMoveCnt_BS++;
		}
		else
		{
			m_nNoEatMoveCnt_BS = 0;
			MakeMove(cm, nToChessID, nToPieceID);
		}

		if (m_RedoMoveStack.size() > 0)
		{
			if (m_RedoMoveStack.top().cm == cm)
			{
				m_RedoMoveStack.pop();
			}
			else
			{
				ClearStack(m_RedoMoveStack);
			}
		}

		FILLUNDOMOVE(undoMove, cm, nToChessID, nToPieceID);
		m_UndoMoveStack.push(undoMove);

		DISPMSG1_ENDL(cout,	"OK");
		break;

	case QHCLoad:
		break;

	case QHCAI:
		m_ThinkType = TT_DEPTH;

		STATEMENT_DEBUG(memcpy(m_nPiecePosBeforeAI, m_nPiecePos, 32 * sizeof(FSINT)););

		//保存当前局面FEN串
		CBChessID2FEN(m_szFEN);

		SearchGo();
		break;

	case QHCAbort:
		QHStopThink();
		break;

	case QHCQuit:
		//结束搜索线程
		KillThread(m_hThinkThread);

		cout << "bye" << endl;

		TRYDELETE(SAFEDELETE(szCmdCpy));

		ExitEgn();	//退出引擎
		exit(EXIT_SUCCESS);
		break;

	case QHCQuitQH:
		return;

	case QHCUndo:
		#ifdef _WIN32
		if (m_UndoMoveStack.size() == 0)
		#else
		#endif
		{
			DISPMSG1_ENDL(cout,	"ERROR");
		}
		else
		{
			undoMove = m_UndoMoveStack.top();
			UnMakeMove(undoMove.cm, undoMove.nToChessID, undoMove.nToPieceID);
			m_UndoMoveStack.pop();
			m_RedoMoveStack.push(undoMove);

			DISPMSG1_ENDL(cout,	"OK");
		}
		break;

	case QHCHints:
		break;

	case QHCBan:
		break;

	case QHCBGThink:
		break;

	case QHCTimeOut:
		QHStopThink();
		break;
	}

	Sleep(1);

	goto GetQianHongCmd;
}

/*******************************************************************************************************
函数名 : 
描  述 : 运行引擎,szParam用于浅红协议
参  数 : 无
返回值 : 
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:44:17
*******************************************************************************************************/
FSVOID CSearchEngine::RunEngine(FSCHAR *szParam)
{
	FSCHAR szProtocol[MAXBUFFERLEN];

	// 浅红协议[2005/11/3 13:38:27]
	if (!strncmp(szParam, "-plugin", 7) || !strncmp(szParam, "-info", 5))
	{
		m_EgnProtocol = EgnPQianHong;
		QianHongPProc(szParam);
	}
	
	// 输出引擎信息
	DISPMSG1_ENDL(cout, "引擎信息");
	DISPMSG1_ENDL(cout, "引擎名字 : "ENGINENAME);
	DISPMSG1_ENDL(cout, "引擎版本 : "ENGINEVERSION);
	DISPMSG1_ENDL(cout, "出生时间 : "__DATE__" "__TIME__);
	DISPMSG0_ENDL(cout);

	// 输出所支持的界面引擎协议
	DISPMSG1_ENDL(cout, "protocol UCCI");
	DISPMSG1_ENDL(cout, "protocol QianHong");
	DISPMSG1_ENDL(cout, "protocol ACCI");

GetEgnP:
	*szProtocol = '\0';
	gets(szProtocol);

	strlwr(szProtocol);
	
	switch(GetEgnP(szProtocol))
	{
	case EgnPUCCI:	//ucci协议
		m_EgnProtocol = EgnPUCCI;
		UCCIProc();
		break;

	case EgnPQianHong:	//浅红协议
		m_EgnProtocol = EgnPQianHong;
		QianHongPProc("");
		break;

	case EgnPAhei:	//ahei协议
		m_EgnProtocol = EgnPAhei;
		AheiPProc();
		break;

	case EgnPNone:
		if (!strncmp(szProtocol, "quit", 5))
		{
			cout <<	"bye" << endl;

			exit(EXIT_SUCCESS);
		}

		if (strncmp(szProtocol, "", 1))
		{
			DISPMSG2_ENDL(cout,		"不可识别的协议名:", szProtocol);
		}

		break;

	default:
		break;
	}

	Idle();

	goto GetEgnP;
}

/*******************************************************************************************************
函数名 : GetEgnP
描  述 : 得到引擎协议,输入字符串全为小写
参  数 : 
        const FSCHAR *szStr : 
返回值 : EgnProtocol
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:44:48
*******************************************************************************************************/
EgnProtocol CSearchEngine::GetEgnP(FSCHAR *szStr)
{
	FSINT nFSCHARSize = sizeof(FSCHAR);

	szStr = JumpNoMatterChar(szStr);

	//UCCI协议
	if (!strncmp(szStr, "ucci", 5))
	{
		return EgnPUCCI;
	}

	//ahei协议
	if (!strncmp(szStr, "ahei", 5))
	{
		return EgnPAhei;
	}
	
	//浅红协议
	if (!strncmp(szStr, "qianhong", 9))
	{
		return EgnPQianHong;
	}

	return EgnPNone;
}

/*******************************************************************************************************
函数名 : IsChecked
描  述 : 1 - bIsRed方是否被bIsRed方将军.
参  数 : 
        BOOL bIsRed : 
返回值 : FSBOOL
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:47:30
*******************************************************************************************************/
inline FSBOOL CSearchEngine::IsChecked(BOOL bIsRed)
{
	FSINT nFoeKingP;
	FSINT nFoeKingI;
	FSINT nFoeKingJ;
	FSINT nFoeKingPosCheckNum;

	FSINT nRookPos1;
	FSINT nRookPos2;

	FSINT nKnightI1;
	FSINT nKnightJ1;
	FSINT nKnightI2;
	FSINT nKnightJ2;
	FSINT nKnightPos1;
	FSINT nKnightPos2;
	FSINT nKnightLegCheckSum1;
	FSINT nKnightLegCheckSum2;
	
	FSINT nCannonPos1;
	FSINT nCannonPos2;

	FSINT nPawnPosCheckNum1;
	FSINT nPawnPosCheckNum2;
	FSINT nPawnPosCheckNum3;
	FSINT nPawnPosCheckNum4;
	FSINT nPawnPosCheckNum5;

	FSINT nFoeKingBitFile;
	FSINT nFoeKingBitRank;

	FSINT nRook;
	FSINT nKnight;
	FSINT nCannon;
	FSINT nPawn;

	nRook = m_nRook[bIsRed];
	nKnight = m_nKnight[bIsRed];
	nCannon = m_nCannon[bIsRed];
	nPawn = m_nPawn[bIsRed];

	//将
	nFoeKingP = m_nPiecePos[m_nKingID2PieceID[1 - bIsRed]];

	nFoeKingPosCheckNum = m_nBRKingPosCheckNum[1 - bIsRed][nFoeKingP];
	
	nFoeKingI = m_CGN2Pos[nFoeKingP].x;
	nFoeKingJ = m_CGN2Pos[nFoeKingP].y;

	//车
	nRookPos1 = m_nPiecePos[m_nChessID2PieceID[nRook][0]];
	nRookPos2 = m_nPiecePos[m_nChessID2PieceID[nRook][1]];

	//马
	nKnightPos1 = m_nPiecePos[m_nChessID2PieceID[nKnight][0]];
	nKnightPos2 = m_nPiecePos[m_nChessID2PieceID[nKnight][1]];

	nKnightI1 = m_CGN2Pos[nKnightPos1].x;
	nKnightJ1 = m_CGN2Pos[nKnightPos1].y;

	nKnightI2 = m_CGN2Pos[nKnightPos2].x;
	nKnightJ2 = m_CGN2Pos[nKnightPos2].y;

	nKnightLegCheckSum1 = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[nKnightI1][m_nBitFile[nKnightJ1]]][m_nFileRankAroundCheckSum[nKnightJ1][m_nBitRank[nKnightI1]]];
	nKnightLegCheckSum2 = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[nKnightI2][m_nBitFile[nKnightJ2]]][m_nFileRankAroundCheckSum[nKnightJ2][m_nBitRank[nKnightI2]]];

	//炮
	nCannonPos1 = m_nPiecePos[m_nChessID2PieceID[nCannon][0]];
	nCannonPos2 = m_nPiecePos[m_nChessID2PieceID[nCannon][1]];

	//兵
	nPawnPosCheckNum1 = 
		m_nPawnPosCheckNum[bIsRed][m_nPiecePos[m_nChessID2PieceID[nPawn][0]]];
	nPawnPosCheckNum2 = 
		m_nPawnPosCheckNum[bIsRed][m_nPiecePos[m_nChessID2PieceID[nPawn][1]]];
	nPawnPosCheckNum3 = 
		m_nPawnPosCheckNum[bIsRed][m_nPiecePos[m_nChessID2PieceID[nPawn][2]]];
	nPawnPosCheckNum4 = 
		m_nPawnPosCheckNum[bIsRed][m_nPiecePos[m_nChessID2PieceID[nPawn][3]]];
	nPawnPosCheckNum5 = 
		m_nPawnPosCheckNum[bIsRed][m_nPiecePos[m_nChessID2PieceID[nPawn][4]]];
	DEBUGASSERT(nPawnPosCheckNum1 >= MINPAWNPOSCHECKNUM && nPawnPosCheckNum1 <= MAXPAWNPOSCHECKNUM);
	DEBUGASSERT(nPawnPosCheckNum2 >= MINPAWNPOSCHECKNUM && nPawnPosCheckNum2 <= MAXPAWNPOSCHECKNUM);
	DEBUGASSERT(nPawnPosCheckNum3 >= MINPAWNPOSCHECKNUM && nPawnPosCheckNum3 <= MAXPAWNPOSCHECKNUM);
	DEBUGASSERT(nPawnPosCheckNum4 >= MINPAWNPOSCHECKNUM && nPawnPosCheckNum4 <= MAXPAWNPOSCHECKNUM);
	DEBUGASSERT(nPawnPosCheckNum5 >= MINPAWNPOSCHECKNUM && nPawnPosCheckNum5 <= MAXPAWNPOSCHECKNUM);

	nFoeKingBitFile = m_nBitFile[nFoeKingJ];
	nFoeKingBitRank = m_nBitRank[nFoeKingI];

	return m_bIsRookFilePreCheck[bIsRed][nRookPos1][nFoeKingPosCheckNum][nFoeKingBitFile] ||	//车行1
		   m_bIsRookFilePreCheck[bIsRed][nRookPos2][nFoeKingPosCheckNum][nFoeKingBitFile] ||	//车行2
		   m_bIsRookRankPreCheck[bIsRed][nRookPos1][nFoeKingPosCheckNum][nFoeKingBitRank] ||	//车列1
		   m_bIsRookRankPreCheck[bIsRed][nRookPos2][nFoeKingPosCheckNum][nFoeKingBitRank] ||	//车列2
		   
		   m_bIsKnightPreCheck[bIsRed][nKnightPos1][nFoeKingPosCheckNum][nKnightLegCheckSum1] ||	//马1
		   m_bIsKnightPreCheck[bIsRed][nKnightPos2][nFoeKingPosCheckNum][nKnightLegCheckSum2] ||	//马2

		   m_bIsCannonFilePreCheck[bIsRed][nCannonPos1][nFoeKingPosCheckNum][nFoeKingBitFile] ||	//炮行1
		   m_bIsCannonFilePreCheck[bIsRed][nCannonPos2][nFoeKingPosCheckNum][nFoeKingBitFile] ||	//炮行2
		   m_bIsCannonRankPreCheck[bIsRed][nCannonPos1][nFoeKingPosCheckNum][nFoeKingBitRank] ||	//炮列1
		   m_bIsCannonRankPreCheck[bIsRed][nCannonPos2][nFoeKingPosCheckNum][nFoeKingBitRank] ||	//炮列2

		   m_bIsPawnPreCheck[bIsRed][nPawnPosCheckNum1][nFoeKingPosCheckNum] ||	//兵1
		   m_bIsPawnPreCheck[bIsRed][nPawnPosCheckNum2][nFoeKingPosCheckNum] ||	//兵2
		   m_bIsPawnPreCheck[bIsRed][nPawnPosCheckNum3][nFoeKingPosCheckNum] ||	//兵3
		   m_bIsPawnPreCheck[bIsRed][nPawnPosCheckNum4][nFoeKingPosCheckNum] ||	//兵4
		   m_bIsPawnPreCheck[bIsRed][nPawnPosCheckNum5][nFoeKingPosCheckNum];	//兵5
}

/*******************************************************************************************************
函数名 : IsRep
描  述 : 该函数在MakeMove函数后调用.
参  数 : 无
返回值 : FSBOOL 返回1表示重复, 2表示己方长将, 4表示对方长将
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:47:56
*******************************************************************************************************/
inline FSBOOL CSearchEngine::IsRep(FSINT nCanRep)
{
	if (m_nNoEatMoveCnt_BS + m_nCurDepth <= 4)
	{
		return FALSE;
	}

	//hash表中未记录此局面,肯定不会重复
	if (m_nRepHashTab[m_nHashTabIndex & m_nRepHashTabMask] == -1)
	{
		return FALSE;
	}

	FSINT	nIndex = m_nCurDepth - 2;
	FSBOOL	bOwnPerpCheck = TRUE;	//己方长将
	FSBOOL	bOppPerpCheck = TRUE;	//对方长将
	FSBOOL	bBreak = FALSE;
	FSINT	nRepCnt = 0;

	CHESSMOVE cmThis;

	DEBUGASSERT(m_nCurDepth == 0 || m_cmFight[m_nCurDepth] == m_cmNullMove || 
		m_nHistoryCBCheckSum[m_nCurDepth] == m_nCBCheckSum);
	
	// m_cmFight是从index为1开始的,故此处判断条件为>=1[2005/11/11 22:58:08]
	while (nIndex >= 1)
	{
		cmThis = m_cmFight[nIndex];

		//空着或吃子走法,肯定不会重复
		if (cmThis == m_cmNullMove || cmThis.nKind == EAT)
		{
			bBreak = TRUE;
			break;
		}

		if (cmThis.nKind != CHECK)
		{
			bOppPerpCheck = FALSE;
		}
		if (m_cmFight[nIndex + 1].nKind != CHECK)
		{
			bOwnPerpCheck = FALSE;
		}

		if (m_nHistoryCBCheckSum[nIndex] == m_nCBCheckSum)
		{
			nRepCnt++;
			
			if (nRepCnt >= nCanRep)
			{
				if (bOppPerpCheck == TRUE)
				{
					return 4;
				}
				
				if (bOwnPerpCheck == TRUE)
				{
					return 2;
				}
				
				return 1;
			}
		}
		
		nIndex -= 2;
	}

	if (bBreak == TRUE)
	{
		return FALSE;
	}

	//走法路线中含有偶数个走法,此时走法路线中的走法都已检查过
	if ((m_nCurDepth & 1) == 0)
	{
		nIndex = m_nNoEatMoveCnt_BS - 1;
	}
	else
	{
		m_Moves_NoEat[m_nNoEatMoveCnt_BS] = m_cmFight[1];
		nIndex = m_nNoEatMoveCnt_BS;
	}

	nIndex--;
	while (nIndex >= 0)
	{
		cmThis = m_Moves_NoEat[nIndex];

		if (cmThis.nKind != CHECK)
		{
			bOppPerpCheck = FALSE;
		}
		if (m_Moves_NoEat[nIndex + 1].nKind != CHECK)
		{
			bOwnPerpCheck = FALSE;
		}

		if (m_nHistoryCBCheckSum_NoEat[nIndex] == m_nCBCheckSum)
		{
			nRepCnt++;

			if (nRepCnt >= nCanRep)
			{
				if (bOppPerpCheck == TRUE)
				{
					return 4;
				}
				
				if (bOwnPerpCheck == TRUE)
				{
					return 2;
				}
				
				return 1;
			}
		}

		nIndex -= 2;
	}

	return FALSE;
}

/*******************************************************************************************************
函数名 : GetVersion
描  述 : 得到本引擎版本号
参  数 : 
        FSCHAR *szVersion : 
返回值 : 无
版  本 : 0.1
作  者 : ahei
时  间 : 2005年8月7日 12:48:02
*******************************************************************************************************/
FSVOID CSearchEngine::GetVersion(FSCHAR *szVersion) const
{
	_ASSERT(szVersion != 0);

	szVersion = ENGINEVERSION;
}

/*******************************************************************************************************
函数名 : JumpNoMatterChar
描  述 : 跳过szStr前面的空格,Tab
参  数 : 
        FSCHAR *szStr :  
返回值 : 类型为 FSCHAR *
版  本 : 0.1
作  者 : ahei
时  间 : 2005年9月28日 22:23:23
*******************************************************************************************************/
FSCHAR * CSearchEngine::JumpNoMatterChar(FSCHAR *szStr)
{
	_ASSERT(szStr != 0);

	FSINT nPos = 0;

	//跳过前面的空格,Tab键
	while (szStr[nPos] == ' ' || szStr[nPos] == '\t')
	{
		nPos++;
	}

	return szStr + nPos;
}

/*******************************************************************************************************
函数名 : JumpDigit
描  述 : 跳过szStr前面的数字
参  数 : 
        FSCHAR *szStr :  
返回值 : 类型为 FSCHAR *
版  本 : 0.1
作  者 : ahei
时  间 : 2005年9月28日 22:23:51
*******************************************************************************************************/
FSCHAR * CSearchEngine::JumpDigit(FSCHAR *szStr)
{
	_ASSERT(szStr != 0);

	FSINT nPos = 0;

	//跳过前面的数字
	while (isdigit(szStr[nPos]))
	{
		nPos++;
	}

	return szStr + nPos;
}

/*******************************************************************************************************
函数名 : Rand32
描  述 : 产生32位的随机数
参  数 : 无
返回值 : 类型为 FSULONG
版  本 : 0.1
作  者 : ahei
时  间 : 2005年9月28日 22:24:02
*******************************************************************************************************/
inline FSULONG CSearchEngine::Rand32()
{
	return rand() ^ ((FSULONG)rand() << 15) ^ ((FSULONG)rand() << 30);
}

/*******************************************************************************************************
函数名 : Rand64
描  述 : 产生32位的随机数
参  数 : 无
返回值 : 类型为 FSUINT64
版  本 : 0.1
作  者 : ahei
时  间 : 2005年9月28日 22:24:19
*******************************************************************************************************/
inline FSUINT64 CSearchEngine::Rand64()
{
	return rand() ^ ((FSUINT64)rand() << 15) ^ ((FSUINT64)rand() << 30) ^
		((FSUINT64)rand() << 45) ^ ((FSUINT64)rand() << 60);
}

/*******************************************************************************************************
函数名 : UDgreater
描  述 : 根据走法的得分比较两个走法cm1, cm2
参  数 : 
        CHESSMOVE cm1 :  
		CHESSMOVE cm2 :  
返回值 : 类型为 FSbool
版  本 : 0.1
作  者 : ahei
时  间 : 2005年9月28日 22:24:31
*******************************************************************************************************/
inline FSbool UDgreater(CHESSMOVE cm1, CHESSMOVE cm2)
{
	return cm1.nScore > cm2.nScore;
}

/*******************************************************************************************************
函数名 : ExitEgn
描  述 : 
参  数 : 无
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年10月21日 22:40:19
*******************************************************************************************************/
FSVOID CSearchEngine::ExitEgn()
{
//	SaveKnightPreMoves();
	if (m_bAllocateMem == TRUE)
	{
		FreeMem();
	}
}

/*******************************************************************************************************
函数名 : IsValidPosition
描  述 : 
参  数 : 无
返回值 : 类型为 FSBOOL
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年10月24日 16:04:17
*******************************************************************************************************/
FSBOOL CSearchEngine::IsValidPosition()
{
	//两方有任何一方老将已不存在
//	if (m_nPiecePos[m_nChessID2PieceID[m_nKing[m_bIsRed]][0]] == INVALIDPIECEPOS || 
//		m_nPiecePos[m_nChessID2PieceID[m_nKing[1 - m_bIsRed]][0]] == INVALIDPIECEPOS)
//	{
//		return FALSE;
//	}

	return TRUE;
}

/*******************************************************************************************************
函数名 : PrintPVLine
描  述 : 输出PV走法
参  数 : 无
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年10月24日 22:20:03
*******************************************************************************************************/
inline FSVOID CSearchEngine::PrintPVLine()
{
	if (m_PVLine.nCount <= 0)
	{
		return;
	}
	
	FSINT	i;
	FSINT	t = clock() - m_nTimeStartThink;
	FSCHAR	szMove[MAXMOVESTRLEN];

	SWITCHEGN
	(
	DISPMSG5(cout, "info depth ", m_nCurSearchDepth_ID, " score ", m_PVLine.pMoveList[0].nScore, " pv");
	for (i = 0; i < m_PVLine.nCount; i++)
	{
		ChessMove2Str(m_PVLine.pMoveList[i], szMove);
		DISPMSG2(cout, " ", szMove);
	}
	DISPMSG0_ENDL(cout);
	DISPMSG2_ENDL(cout, "info time ", MS2S(t));
	STATEMENT_STAT
	(
		if (t > 0)
		{
			DISPMSG2_ENDL(cout, "info nps ", m_nVistNodeCnt * 1000 / t);
		}
		m_nNonQuiesNodeCnt = m_nVistNodeCnt - m_nQuiesNodeCnt;
		
		DISPMSG2_ENDL(cout, "info nodes ", m_nVistNodeCnt);
		
		DISPMSG7_ENDL(cout, "info nonquies nodes ", m_nNonQuiesNodeCnt, " ", 
			setprecision(3), setw(3), RATE(m_nNonQuiesNodeCnt, m_nVistNodeCnt), "%");
		DISPMSG11_ENDL(cout, "info alpha nodes ", m_nAlphaNodeCnt, " ", 
			setprecision(3), setw(3), RATE(m_nAlphaNodeCnt, m_nVistNodeCnt), "% in all and ", 
			setprecision(3), setw(3), RATE(m_nAlphaNodeCnt, m_nNonQuiesNodeCnt), "% in nonquies nodes");
		DISPMSG11_ENDL(cout, "info pv nodes ", m_nAlphaNodeCnt, " ", 
			setprecision(3), setw(3), RATE(m_nPVNodeCnt, m_nVistNodeCnt), "% in all and ", 
			setprecision(3), setw(3), RATE(m_nPVNodeCnt, m_nNonQuiesNodeCnt), "% in nonquies nodes");
		DISPMSG11_ENDL(cout, "info beta nodes ", m_nAlphaNodeCnt, " ", 
			setprecision(3), setw(3), RATE(m_nBetaNodeCnt, m_nVistNodeCnt), "% in all and ", 
			setprecision(3), setw(3), RATE(m_nBetaNodeCnt, m_nNonQuiesNodeCnt), "% in nonquies nodes");
		DISPMSG11_ENDL(cout, "info null nodes ", m_nAlphaNodeCnt, " ", 
			setprecision(3), setw(3), RATE(m_nNullNodeCnt, m_nVistNodeCnt), "% in all and ", 
			setprecision(3), setw(3), RATE(m_nNullNodeCnt, m_nNonQuiesNodeCnt), "% in nonquies nodes");
		
		DISPMSG7_ENDL(cout, "info quies nodes ", m_nQuiesNodeCnt, " ", 
			setprecision(3), setw(3), RATE(m_nQuiesNodeCnt, m_nVistNodeCnt), "%");
		DISPMSG11_ENDL(cout, "info quies alpha nodes ", m_nQuiesAlphaNodeCnt, " ", 
			setprecision(3), setw(3), RATE(m_nQuiesAlphaNodeCnt, m_nVistNodeCnt), "% in all and ", 
			setprecision(3), setw(3), RATE(m_nQuiesAlphaNodeCnt, m_nQuiesNodeCnt), "% in quies nodes");
		DISPMSG11_ENDL(cout, "info quies pv nodes ", m_nQuiesAlphaNodeCnt, " ", 
			setprecision(3), setw(3), RATE(m_nQuiesPVNodeCnt, m_nVistNodeCnt), "% in all and ", 
			setprecision(3), setw(3), RATE(m_nQuiesPVNodeCnt, m_nQuiesNodeCnt), "% in quies nodes");
		DISPMSG11_ENDL(cout, "info quies beta nodes ", m_nQuiesAlphaNodeCnt, " ", 
			setprecision(3), setw(3), RATE(m_nQuiesBetaNodeCnt, m_nVistNodeCnt), "% in all and ", 
			setprecision(3), setw(3), RATE(m_nQuiesBetaNodeCnt, m_nQuiesNodeCnt), "% in quies nodes");
		DISPMSG11_ENDL(cout, "info quies null nodes ", m_nQuiesAlphaNodeCnt, " ", 
			setprecision(3), setw(3), RATE(m_nQuiesNullNodeCnt, m_nVistNodeCnt), "% in all and ", 
			setprecision(3), setw(3), RATE(m_nQuiesNullNodeCnt, m_nQuiesNodeCnt), "% in quies nodes");

		DISPMSG4_ENDL(cout, "Hash hit rate is ", setprecision(3), 
			RATE(m_nHitHashCnt, m_nRecordHashCnt), "%");
		DISPMSG4_ENDL(cout, "Hash probe hit rate is ", setprecision(3), 
			RATE(m_nHitHashCnt, m_nProbeHashCnt), "%");
		DISPMSG4_ENDL(cout, "QSHash hit rate is ", setprecision(3), 
			RATE(m_nHitQSHashCnt, m_nRecordQSHashCnt), "%");
		DISPMSG4_ENDL(cout, "QSHash probe hit rate is ", setprecision(3), 
			RATE(m_nHitQSHashCnt, m_nProbeQSHashCnt), "%");

		DISPMSG4_ENDL(cout, "Hash move cut rate is ", setprecision(3), 
			RATE(m_nHashBetaNodeCnt, m_nBetaNodeCnt), "%");
		DISPMSG4_ENDL(cout, "Eat move cut rate is ", setprecision(3), 
			RATE(m_nCapMoveBetaNodeCnt, m_nBetaNodeCnt), "%");
		DISPMSG4_ENDL(cout, "Killer move cut rate is ", setprecision(3), 
			RATE(m_nKillerMoveBetaNodeCnt, m_nBetaNodeCnt), "%");
		DISPMSG4_ENDL(cout, "NoCap move cut rate is ", setprecision(3), 
			RATE(m_nNoCapMoveBetaNodeCnt, m_nBetaNodeCnt), "%");
	);
	STATEMENT_GETMAXSCHDPT(DEBUGASSERT(m_nMaxSearchDepth == 0 || 
		m_nMaxSearchDepth >= m_nCurSearchDepth_ID););
	STATEMENT_GETMAXSCHDPT(DISPMSG2_ENDL(cout, "info maxdepth ", m_nMaxSearchDepth););
	STATEMENT_GETMAXSELSCHDPT(DISPMSG2_ENDL(cout, "info seldepth ", m_nMaxSelSchDpt););
	);
}

/*******************************************************************************************************
函数名 : SetBestMove
描  述 : 
参  数 : 无
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年10月26日 15:59:26
*******************************************************************************************************/
//FSVOID CSearchEngine::SetBestMove()
//{
//
//}

/*******************************************************************************************************
函数名 : SearchTest
描  述 : 
参  数 : 
        FSINT nDepth                 : 
		FSINT alpha                  : 
		FSINT beta                   : 
		KILLERMOVETAB &killerMoveTab : 
返回值 : 类型为 FSINT
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年10月26日 15:59:21
*******************************************************************************************************/
/*FSINT CSearchEngine::SearchTest(FSINT nDepth, FSINT nSearchDepth, FSINT alpha, FSINT beta, KILLERMOVETAB& killerMoveTab)
{
	if (nDepth >= nSearchDepth)
	{
	//	return QuiesSearch(nDepth + 1, alpha, beta);
		return Evaluate();
	}
	
	DEBUGASSERT(nDepth >= 0 && nDepth <= MAXSEARCHDEPTH);
//	DEBUGASSERT(nDepth >= 0);
	DEBUGASSERT(beta <= -g_nPerCheckValue);
	DEBUGASSERT(alpha >= g_nPerCheckValue && alpha <= beta);

	//双将俱在
	DEBUGASSERT(m_nPiecePos[m_nKingID2PieceID[m_bIsRed]] != INVALIDPIECEPOS);
	DEBUGASSERT(m_nPiecePos[m_nKingID2PieceID[1 - m_bIsRed]] != INVALIDPIECEPOS);

	FSINT	i;
	FSINT	nBestEval = m_nCurDepth - g_nMateValue;
	FSINT	nNewDepth;
	FSINT	nCount;							//走法数
	FSINT	nEval;							//估值
	FSINT	nToChessID;						//被吃的棋子类型编号
	FSINT	nToPieceID;						//被吃的棋子编号
	FSINT	nHashFlag = HASHALPHA;			//hash节点标志
	FSBOOL	bCutoff = FALSE;				//是否被截断
	FSBOOL	bFindPV = FALSE;				//是否找到PV走法
	FSINT	nBestMoveNum = 0;				//好的走法号,用于记录历史得分
	FSBOOL	bFirstMove = TRUE;				//正搜索的走法是否是第一个走法,用于历史裁剪中
	FSBOOL	bMateThreat = FALSE;			//杀棋威胁,用于杀棋威胁延伸
	FSBOOL	bSingleResponse = FALSE;		//单一应着,用于单一应着延伸
//	FSBOOL	bDangerPosition = FALSE;		//危险局面,用于威胁局面延伸中[2006/1/1 11:24:24]
	FSINT	nCanMoveCnt = 0;				//可以应着的走发数,用于单一应着中

	CHESSMOVE		cm = m_cmNullMove;
	CHESSMOVE		cmBestMove = m_cmNullMove;
	KILLERMOVETAB	subKillerMoveTab; //杀手走法表

	//按时间思考时,超时返回
	if (m_ThinkType == TT_TIME && (clock() - m_nTimeStartThink) > m_nTimeThinkLimit && 
		(m_bPonder == FALSE || m_bPonderHit == TRUE))
	{
		SearchStop();
#ifdef _WIN32
		ExitThread(0);
#else
#endif
	}

	//探测hash表
	if ((nEval = ProbeHash(nDepth, nSearchDepth, alpha, beta, cm)) != g_nInvalidValue)
	{
		// 根节点检查重复[2005/11/14 16:55:27]
		if (nDepth > 0)
		{
			return nEval;
		}

		MakeMove(cm, nToChessID, nToPieceID);
		if (IsRep(3) == FALSE)
		{
			UnMakeMove(cm, nToChessID, nToPieceID);

			m_PVLine.nCount = 0;
			GetPVLineFromHash(m_nCurSearchDepth_ID);
			PrintPVLine();
			
			return nEval;
		}
		// 说明已经发生循环,下面的置换表启发就不能用这个走法[2005/12/21 22:02:08]
		cm = m_cmNullMove;
		UnMakeMove(cm, nToChessID, nToPieceID);
	}

	// 节点排序4:History Move[2005/10/23 18:45:05]
	//产生所有不吃子走法
	CHESSMOVE cmPrev = m_cmFight[m_nCurDepth];

	GenMoves(nDepth);
	nCount = m_nMovesCnt;
	
	for (i = 0; i < nCount; i++)
	{
		cm = m_MoveList[nDepth][i];

//		if ((cm.nKind & CAP) == 0 || (cm.nKind & ESCAPE) == 0)
		if ((cm.nKind & CAP) == 0)
		{
			continue;
		}

		MakeMove(cm, nToChessID, nToPieceID);

		//判断该走法是否合法
		if (IsLegalMove(cm) == FALSE)
		{
			UnMakeMove(cm, nToChessID, nToPieceID);
			continue;
		}
		
		// 将军延伸,杀棋威胁延伸,单一应着延伸,兑子延伸[2005/11/1 18:49:14]
		DEBUGASSERT(m_nCurDepth >= 1);
		nNewDepth = nSearchDepth;

		nEval = -SearchTest(nDepth + 1, nNewDepth, -beta, -alpha, subKillerMoveTab);
		
		UnMakeMove(cm, nToChessID, nToPieceID);

		m_MoveList[nDepth][i].nScore = nEval;

		if (nEval > nBestEval)
		{
			nBestEval = nEval;
			nBestMoveNum = i;
			cmBestMove = cm;
			cmBestMove.nScore = nEval;

			if (nEval > alpha)
			{
				alpha = nEval;
				
				if (nBestEval >= beta)
				{
					nHashFlag = HASHBETA;
					
					m_nMoveCutoffCnt[cm.nFrom][cm.nTo]++;

					STATEMENT_STAT(m_nBetaNodeCnt++;);
					STATEMENT_STAT(m_nNoCapMoveBetaNodeCnt++;);

					RecordHash(nDepth, nSearchDepth, alpha, nHashFlag, cmBestMove);

					return alpha;
				}
				
				nHashFlag = HASHEXACT;

				if (nDepth == 0)
				{
					PrintBetterPV(nDepth, nSearchDepth, nEval, nHashFlag, cmBestMove);
					if (nEval + MAXSEARCHDEPTH >= g_nMateValue)
					{
						return nEval;
					}
				}
			}
		}
	}

//	for (i = 0; i < nCount; i++)
//	{
//		cm = m_MoveList[nDepth][i];
//
//		if ((cm.nKind & ESCAPE) == 0)
//		{
//			continue;
//		}
//
//		MakeMove(cm, nToChessID, nToPieceID);
//
//		//判断该走法是否合法
//		if (IsLegalMove(cm) == FALSE)
//		{
//			UnMakeMove(cm, nToChessID, nToPieceID);
//			continue;
//		}
//		
//		// 将军延伸,杀棋威胁延伸,单一应着延伸,兑子延伸[2005/11/1 18:49:14]
//		DEBUGASSERT(m_nCurDepth >= 1);
//		nNewDepth = nSearchDepth;
//
//		nEval = -SearchTest(nDepth + 1, nNewDepth, -beta, -alpha, subKillerMoveTab);
//		
//		UnMakeMove(cm, nToChessID, nToPieceID);
//
//		m_MoveList[nDepth][i].nScore = nEval;
//
//		if (nEval > nBestEval)
//		{
//			nBestEval = nEval;
//			nBestMoveNum = i;
//			cmBestMove = cm;
//			cmBestMove.nScore = nEval;
//
//			if (nEval > alpha)
//			{
//				alpha = nEval;
//				
//				if (nBestEval >= beta)
//				{
//					nHashFlag = HASHBETA;
//					
//					m_nMoveCutoffCnt[cm.nFrom][cm.nTo]++;
//
//					STATEMENT_STAT(m_nBetaNodeCnt++;);
//					STATEMENT_STAT(m_nNoCapMoveBetaNodeCnt++;);
//
//					RecordHash(nDepth, nSearchDepth, alpha, nHashFlag, cmBestMove);
//
//					return alpha;
//				}
//				
//				nHashFlag = HASHEXACT;
//
//				if (nDepth == 0)
//				{
//					PrintBetterPV(nDepth, nSearchDepth, nEval, nHashFlag, cmBestMove);
//					if (nEval + MAXSEARCHDEPTH >= g_nMateValue)
//					{
//						return nEval;
//					}
//				}
//			}
//		}
//	}

//	if (cmPrev != m_cmNullMove)
//	{
////		CHESSMOVE pMoveList[MAXALLMOVESCNT];
//		CHESSMOVE pMoveList[MAXMOVECNT];
//		FSINT nTempCnt;
//		FSINT nChessID;
//		
//		m_bIsRed = 1 - m_bIsRed;
//		nTempCnt = (this->*m_GenMovesFun2[g_nChessID2Chess[m_nCBChessID[cmPrev.nTo]]])(cmPrev.nTo, pMoveList);
//		m_bIsRed = 1 - m_bIsRed;
//		m_nMovesCnt = 0;
//		for (i = 0; i < nTempCnt; i++)
//		{
//			cm = pMoveList[i];
//			
//			if ((nChessID = m_nCBChessID[cm.nTo]) != NOCHESS && m_bIsBlack_ChessID[nChessID] != m_bIsBlack_ChessID[m_nCBChessID[cmPrev.nTo]])
//			{
//				DEBUGASSERT((m_bIsBlack_ChessID[nChessID] ^ m_bIsBlack_ChessID[m_nCBChessID[cmPrev.nTo]]) > 0);
//				(this->*m_GenMovesFun[g_nChessID2Chess[nChessID]])(cm.nTo, nDepth);
//			}
//		}
//	}
//	else
//	{
//		GenMoves(nDepth);
//	}
//
//	nCount = m_nMovesCnt;
//	DEBUGASSERT(nCount >= 0 && nCount <= MAXALLMOVESCNT);
//	//扫描所有走法
//	for (i = 0; i < nCount; i++)
//	{
//		cm = m_MoveList[nDepth][i];
//
//		MakeMove(cm, nToChessID, nToPieceID);
//
//		//判断该走法是否合法
//		if (IsLegalMove(cm) == FALSE)
//		{
//			UnMakeMove(cm, nToChessID, nToPieceID);
//			continue;
//		}
//		
//		// 将军延伸,杀棋威胁延伸,单一应着延伸,兑子延伸[2005/11/1 18:49:14]
//		DEBUGASSERT(m_nCurDepth >= 1);
//		nNewDepth = nSearchDepth;
//
//		nEval = -SearchTest(nDepth + 1, nNewDepth, -beta, -alpha, subKillerMoveTab);
//		
//		UnMakeMove(cm, nToChessID, nToPieceID);
//
//		m_MoveList[nDepth][i].nScore = nEval;
//
//		if (nEval > nBestEval)
//		{
//			nBestEval = nEval;
//			nBestMoveNum = i;
//			cmBestMove = cm;
//			cmBestMove.nScore = nEval;
//
//			if (nEval > alpha)
//			{
//				alpha = nEval;
//				
//				if (nBestEval >= beta)
//				{
//					nHashFlag = HASHBETA;
//					
//					m_nMoveCutoffCnt[cm.nFrom][cm.nTo]++;
//
//					STATEMENT_STAT(m_nBetaNodeCnt++;);
//					STATEMENT_STAT(m_nNoCapMoveBetaNodeCnt++;);
//
//					RecordHash(nDepth, nSearchDepth, alpha, nHashFlag, cmBestMove);
//
//					return alpha;
//				//	break;
//				}
//				
//				nHashFlag = HASHEXACT;
//
//				if (nDepth == 0)
//				{
//					PrintBetterPV(nDepth, nSearchDepth, nEval, nHashFlag, cmBestMove);
//					if (nEval + MAXSEARCHDEPTH >= g_nMateValue)
//					{
//						return nEval;
//					}
//				}
//			}
//		}
//	}
//
//	GenMoves(nDepth);
//	nCount = m_nMovesCnt;
//	DEBUGASSERT(nCount >= 0 && nCount <= MAXALLMOVESCNT);
	//扫描所有走法
	for (i = 0; i < nCount; i++)
	{
		cm = m_MoveList[nDepth][i];

		MakeMove(cm, nToChessID, nToPieceID);

		//判断该走法是否合法
		if (IsLegalMove(cm) == FALSE)
		{
			UnMakeMove(cm, nToChessID, nToPieceID);
			continue;
		}
		
		// 将军延伸,杀棋威胁延伸,单一应着延伸,兑子延伸[2005/11/1 18:49:14]
		DEBUGASSERT(m_nCurDepth >= 1);
		nNewDepth = nSearchDepth;

		nEval = -SearchTest(nDepth + 1, nNewDepth, -beta, -alpha, subKillerMoveTab);
		
		UnMakeMove(cm, nToChessID, nToPieceID);

		m_MoveList[nDepth][i].nScore = nEval;

		if (nEval > nBestEval)
		{
			nBestEval = nEval;
			nBestMoveNum = i;
			cmBestMove = cm;
			cmBestMove.nScore = nEval;

			if (nEval > alpha)
			{
				alpha = nEval;
				
				if (nBestEval >= beta)
				{
					nHashFlag = HASHBETA;
					
					m_nMoveCutoffCnt[cm.nFrom][cm.nTo]++;

					STATEMENT_STAT(m_nBetaNodeCnt++;);
					STATEMENT_STAT(m_nNoCapMoveBetaNodeCnt++;);

					RecordHash(nDepth, nSearchDepth, alpha, nHashFlag, cmBestMove);

					break;
				}
				
				nHashFlag = HASHEXACT;

				if (nDepth == 0)
				{
					PrintBetterPV(nDepth, nSearchDepth, nEval, nHashFlag, cmBestMove);
					if (nEval + MAXSEARCHDEPTH >= g_nMateValue)
					{
						return nEval;
					}
				}
			}
		}
	}

	RecordHash(nDepth, nSearchDepth, alpha, nHashFlag, cmBestMove);

	return nBestEval;
}*/

/*******************************************************************************************************
函数名 : SearchStop
描  述 : 
参  数 : 无
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年10月26日 15:59:10
*******************************************************************************************************/
FSVOID CSearchEngine::SearchStop()
{
	FSINT	nTimeThinked;
	FSCHAR	szMove[MAXBUFFERLEN];
	FSINT	nToChessID;
	FSINT	nToPieceID;

	CHESSMOVE cm;

	STATEMENT_TEST(CString str;);

	UNDOMOVE undoMove;
	
	nTimeThinked = clock() - m_nTimeStartThink;
	m_nTotalSearchTime += nTimeThinked;
	if (nTimeThinked > m_nMaxSearchTime)
	{
		m_nMaxSearchTime = nTimeThinked;
	}
	
	if (m_bStopThink == TRUE)
	{
		m_nCurSearchDepth_ID--;
	}

	switch(m_EgnProtocol)
	{
	case EgnPUCCI:
	case EgnPAhei:
		break;
	
	case EgnPQianHong:
		cm = m_PVLine.pMoveList[0];
		if (m_nCBPieceID[cm.nTo] == INVALIDPIECEID)
		{
			m_nHistoryCBCheckSum_NoEat[m_nNoEatMoveCnt_BS] = m_nCBCheckSum;
			MakeMove(m_Moves_NoEat[m_nNoEatMoveCnt_BS] = cm, nToChessID, nToPieceID);
			m_Moves_NoEat[m_nNoEatMoveCnt_BS].nKind = m_cmFight[m_nCurDepth - 1].nKind;
			m_nNoEatMoveCnt_BS++;
		}
		else
		{
			m_nNoEatMoveCnt_BS = 0;
			MakeMove(cm, nToChessID, nToPieceID);
		}

		FILLUNDOMOVE(undoMove, m_PVLine.pMoveList[0], nToChessID, nToPieceID);
		m_UndoMoveStack.push(undoMove);
		break;

	default:
		break;
	}

	//显示引擎思考结果
	ChessMove2Str(m_PVLine.pMoveList[0], szMove);
	switch(m_EgnProtocol)
	{
	case EgnPUCCI:
	case EgnPAhei:
		cout << "bestmove " << szMove;
		if (m_PVLine.nCount > 1)
		{
			ChessMove2Str(m_PVLine.pMoveList[1], szMove);
			cout << " ponder " << szMove;
		}
		cout << endl;
		DISPMSG2_ENDL(cout, "info time ", MS2S(nTimeThinked));
		cout << "info longest time " << setprecision(3) << MS2S(m_nMaxSearchTime) << endl;
		cout << "info average time " << setprecision(3) 
			 << m_nTotalSearchTime * 1.0 / (1000 * m_nSearchCnt)  << endl;
		STATEMENT_STAT(
		DISPMSG4_ENDL(cout, "Hash hit rate is ", setprecision(3), 
			RATE(m_nHitHashCnt, m_nRecordHashCnt), "%");
		DISPMSG4_ENDL(cout, "Hash probe hit rate is ", setprecision(3), 
			RATE(m_nHitHashCnt, m_nProbeHashCnt), "%");
		DISPMSG4_ENDL(cout, "QSHash hit rate is ", setprecision(3), 
			RATE(m_nHitQSHashCnt, m_nRecordQSHashCnt), "%");
		DISPMSG4_ENDL(cout, "QSHash probe hit rate is ", setprecision(3), 
			RATE(m_nHitQSHashCnt, m_nProbeQSHashCnt), "%");

		DISPMSG4_ENDL(cout, "Hash move cut rate is ", setprecision(3), 
			RATE(m_nHashBetaNodeCnt, m_nBetaNodeCnt), "%");
		DISPMSG4_ENDL(cout, "Eat move cut rate is ", setprecision(3), 
			RATE(m_nCapMoveBetaNodeCnt, m_nBetaNodeCnt), "%");
		DISPMSG4_ENDL(cout, "Killer move cut rate is ", setprecision(3), 
			RATE(m_nKillerMoveBetaNodeCnt, m_nBetaNodeCnt), "%");
		DISPMSG4_ENDL(cout, "NoCap move cut rate is ", setprecision(3), 
			RATE(m_nNoCapMoveBetaNodeCnt, m_nBetaNodeCnt), "%");
		);
		break;
		
	case EgnPQianHong:
		cout << szMove << endl;
		STATEMENT_TEST
		(
		str.Format("%s", szMove);
		MessageBox(NULL, str, "走子", MB_OK);
		)
		STATEMENT_TESTQHP
		(
		cout << "bestmove " << szMove;
		if (m_PVLine.nCount > 1)
		{
			ChessMove2Str(m_PVLine.pMoveList[1], szMove);
			cout << " ponder " << szMove;
		}
		cout << endl;
		DISPMSG2_ENDL(cout, "info time ", MS2S(t));
		cout << "info longest time " << setprecision(3) << MS2S(m_nMaxSearchTime) << endl;
		cout << "info average time " << setprecision(3) 
			 << m_nTotalSearchTime * 1.0 / (1000 * m_nSearchCnt)  << endl;
		);
		break;

	default:
		break;
	}
	
	ChessMove2Str(m_PVLine.pMoveList[0], szMove);
	if (m_nCurSearchDepth_ID > 1)
	{
		ChessMove2Str(m_PVLine.pMoveList[1], szMove);
	}

	m_bPonder = FALSE;
	m_bPonderHit = FALSE;

	m_hThinkThread = NULL;
	m_esUCCIEgnStatus = EgnSIdle;	//置引擎空闲标志
}

/*******************************************************************************************************
函数名 : UCCIBusy
描  述 : 
参  数 : 无
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年10月26日 15:59:04
*******************************************************************************************************/
inline FSVOID CSearchEngine::UCCIBusy()
{
	cout << "busy" << endl;
}

/*******************************************************************************************************
函数名 : ThinkTimeOut
描  述 : 
参  数 : 
        FSUINT uID     : 
		FSUINT uMsg    : 
		FSDWORD dwUser : 
		FSDWORD dw1    : 
		FSDWORD dw2    : 
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年10月26日 15:58:56
*******************************************************************************************************/
#ifdef _WIN32
FSVOID CALLBACK CSearchEngine::ThinkTimeOut(FSUINT uTimerID, FSUINT uMsg, FSDWORD dwUser, FSDWORD dw1, FSDWORD dw2)
{
//	MMRESULT mmResult = timeKillEvent(uTimerID);

//	if (mmResult == MMSYSERR_INVALPARAM)
	{
		DISPMSG3_ENDL(cout,		"定时器", uTimerID, "不存在!");
	}
//	UCCIStopThink();
	((CSearchEngine*)dwUser)->UCCIStopThink();
}
#else
#endif

/*******************************************************************************************************
函数名 : UCCIStopThink
描  述 : 
参  数 : 无
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年10月26日 15:58:51
*******************************************************************************************************/
#ifdef _WIN32
FSVOID CSearchEngine::UCCIStopThink()
{
	if (m_hThinkThread == NULL)
	{
		return;
	}
	
	m_bStopThink = TRUE;
	m_esUCCIEgnStatus = EgnSIdle;

	//结束搜索线程
	KillThread(m_hThinkThread);

	SearchStop();
}
#else
#endif

/*******************************************************************************************************
函数名 : IDSearchInit
描  述 : 
参  数 : 无
返回值 : 类型为 inline
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年11月6日 15:52:08
*******************************************************************************************************/
inline FSVOID CSearchEngine::IDSearchInit()
{
	STATEMENT_STAT
	(
	m_nVistNodeCnt = 0;
	m_nAlphaNodeCnt = 0;
	m_nPVNodeCnt = 0;
	m_nBetaNodeCnt = 0;
	m_nNullNodeCnt = 0;

	m_nHashBetaNodeCnt = 0;
	m_nCapMoveBetaNodeCnt = 0;
	m_nKillerMoveBetaNodeCnt = 0;
	m_nNoCapMoveBetaNodeCnt = 0;

	m_nQuiesNodeCnt = 0;
	m_nQuiesAlphaNodeCnt = 0;	
	m_nQuiesPVNodeCnt = 0;
	m_nQuiesBetaNodeCnt = 0;
	m_nQuiesNullNodeCnt = 0;
	);

	STATEMENT_GETMAXSCHDPT(m_nMaxSearchDepth = 0;);
	STATEMENT_GETMAXSELSCHDPT(m_nMaxSelSchDpt = 0;);
}

/*******************************************************************************************************
函数名 : MakeNullMove
描  述 : 
参  数 : 无
返回值 : 类型为 inline
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年11月6日 15:52:02
*******************************************************************************************************/
inline FSVOID CSearchEngine::MakeNullMove()
{
	ChangeSide();
	m_nCurDepth++;
	m_cmFight[m_nCurDepth] = m_cmNullMove;
	DEBUGASSERT(m_nCurDepth >= 1);
#	ifdef GETMAXSCHDPT
	if (m_nCurDepth > m_nMaxSearchDepth)
	{
		m_nMaxSearchDepth = m_nCurDepth;
	}
#	endif	
}

/*******************************************************************************************************
函数名 : UnMakeNullMove
描  述 : 
参  数 : 无
返回值 : 类型为 inline
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年11月6日 15:51:52
*******************************************************************************************************/
inline FSVOID CSearchEngine::UnMakeNullMove()
{
	ChangeSide();
	m_nCurDepth--;
	DEBUGASSERT(m_nCurDepth >= 0);
}

/*******************************************************************************************************
函数名 : PrintQHInfo
描  述 : 
参  数 : 无
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年11月6日 15:51:47
*******************************************************************************************************/
FSVOID CSearchEngine::PrintQHInfo()
{
	DISPMSG1_ENDL(cout, QIANHONGPVER);
	DISPMSG1_ENDL(cout, ENGINENAME" "ENGINEVERSION);
	
	DISPMSG2_ENDL(cout, "LEVELS ", QIANHONGLEVELCNT);
	DISPMSG1_ENDL(cout, "1 - 6 plies");
	DISPMSG1_ENDL(cout, "2 - 7 plies");
	DISPMSG1_ENDL(cout, "3 - 8 plies");
	DISPMSG1_ENDL(cout, "4 - 9 plies");
	DISPMSG1_ENDL(cout, "5 - 10 plies");
	DISPMSG1_ENDL(cout, "6 - 11 plies");
	DISPMSG1_ENDL(cout, "7 - 12 plies");
	DISPMSG1_ENDL(cout, "8 - 13 plies");
	DISPMSG1_ENDL(cout, "9 - max ply");
	
	DISPMSG1_ENDL(cout, "UNDO 1");
	DISPMSG1_ENDL(cout, "HINTS 0");
	DISPMSG1_ENDL(cout, "RULES 0");
	DISPMSG1_ENDL(cout, "BGTHINK 0");
	DISPMSG1_ENDL(cout, "TIMEOUT 1");
	DISPMSG1_ENDL(cout, "QianHong AI Engine for QianHong "QIANHONGPVER);
	DISPMSG1_ENDL(cout, "By ahei, 2005.3 - Now");
	DISPMSG1_ENDL(cout, "ENDINFO");
}

/*******************************************************************************************************
函数名 : GetQianHongCmd
描  述 : 
参  数 : 
        FSCHAR *& szQianHongCmd : 
返回值 : 类型为 QianHongCmd
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年11月6日 15:51:39
*******************************************************************************************************/
QianHongCmd CSearchEngine::GetQianHongCmd(FSCHAR *& szQianHongCmd)
{
	FSINT nFSCHARSize = sizeof(FSCHAR);

	if (strlen(szQianHongCmd) >= 3)
	{
		FSCHAR	szTemp[MAXBUFFERLEN];
		
		szQianHongCmd = JumpNoMatterChar(szQianHongCmd);	
		
		strncpy(szTemp, szQianHongCmd, 3);
		szTemp[3] = '\0';
		strlwr(szTemp);
		
		if (!strncmp(szTemp, "fen", 3))
		{
			szQianHongCmd += 3;
			return QHCFEN;
		}
	}

	strlwr(szQianHongCmd);

	if (!strncmp(szQianHongCmd, "level", 5))
	{
		szQianHongCmd += 5;
		return QHCLevel;
	}
	
	if (!strncmp(szQianHongCmd, "play", 4))
	{
		szQianHongCmd += 4;
		return QHCPlay;
	}

	if (!strncmp(szQianHongCmd, "load", 4))
	{
		szQianHongCmd += 4;
		return QHCLoad;
	}

	if (!strncmp(szQianHongCmd, "ai", 2))
	{
		szQianHongCmd += 2;
		return QHCAI;
	}

	if (!strncmp(szQianHongCmd, "abort", 5))
	{
		szQianHongCmd += 5;
		return QHCAbort;
	}

	//此命令要放在quit命令前面处理
	if (!strncmp(szQianHongCmd, "quitqh", 6))
	{
		szQianHongCmd += 6;
		return QHCQuitQH;
	}

	if (!strncmp(szQianHongCmd, "quit", 4))
	{
		szQianHongCmd += 4;
		return QHCQuit;
	}

	if (!strncmp(szQianHongCmd, "exit", 4))
	{
		szQianHongCmd += 4;
		return QHCQuit;
	}

	if (!strncmp(szQianHongCmd, "undo", 4))
	{
		szQianHongCmd += 4;
		return QHCUndo;
	}

	if (!strncmp(szQianHongCmd, "hints", 5))
	{
		szQianHongCmd += 5;
		return QHCHints;
	}

	if (!strncmp(szQianHongCmd, "ban", 3))
	{
		szQianHongCmd += 3;
		return QHCBan;
	}

	if (!strncmp(szQianHongCmd, "bgthink", 7))
	{
		szQianHongCmd += 7;
		return QHCBGThink;
	}

	if (!strncmp(szQianHongCmd, "timeout", 7))
	{
		szQianHongCmd += 7;
		return QHCTimeOut;
	}

	if (strncmp(szQianHongCmd, "", 1))
	{
		return QHCInvalid;
	}

	return QHCNone;
}

/*******************************************************************************************************
函数名 : Initial
描  述 : 
参  数 : 无
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年11月6日 15:51:27
*******************************************************************************************************/
FSVOID CSearchEngine::Initial()
{
	FSINT t;
	FSINT tStart = clock();


//	cout << "正在分配内存..." << endl;
	AllocateMem();	//分配内存
	m_bAllocateMem = TRUE;

//	cout << "正在初始化变量..." << endl;
	InitVar();		//初始化变量

	GenPreMoves();	//产生走法预产生数组

//	cout << "正在产生将军预判断数组..." << endl;
	t = clock();
	GenPreCheck();	//产生将军预判断数组
//	cout << "产生将军预判断数组时间:" << clock() - t << "ms" << endl;

	switch(m_EgnProtocol)
	{
	case EgnPUCCI:
	case EgnPAhei:
		DISPMSG1_ENDL(cout, "产生走法有效性预判断数组...");
		break;

	case EgnPQianHong:
		STATEMENT_TESTQHP(DISPMSG1_ENDL(cout, "产生走法有效性预判断数组..."););
		break;
					
	default:
		break;
	}
	t = clock();
	GenPreValidMove();	//产生走法有效性预判断数组
//	cout << "产生走法有效性预判断数组时间:" << clock() - t << "ms" << endl;

	SWITCHEGN(
	DISPMSG3_ENDL(cout,	"启动时间:", clock() - tStart, "ms");
	DISPMSG3_ENDL(cout,	"引擎", ENGINENAME" "ENGINEVERSION, "开始工作"););
}

/*******************************************************************************************************
函数名 : InitPointer
描  述 : 
参  数 : 无
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年11月6日 15:51:19
*******************************************************************************************************/
FSVOID CSearchEngine::InitPointer()
{
	//初始化
	//走法预产生数组
	//车
	m_pcmRookFilePreMoves = 0;
	m_pcmRookRankPreMoves = 0;

	m_pcmRookFileNoCapPreMoves = 0;
	m_pcmRookRankNoCapPreMoves = 0;

	m_pcmRookFileCapPreMoves = 0;
	m_pcmRookRankCapPreMoves = 0;

	//马
	//不吃子走法
	m_pcmKnightNoCapPreMoves = 0;
	//吃子走法
	m_pcmKnightCapPreMoves = 0;

	//炮
	m_pcmCannonFilePreMoves = 0;
	m_pcmCannonRankPreMoves = 0;

	m_pcmCannonFileNoCapPreMoves = 0;
	m_pcmCannonRankNoCapPreMoves = 0;

	m_pcmCannonFileCapPreMoves = 0;
	m_pcmCannonRankCapPreMoves = 0;
	
	//将
	m_pcmKingPreMoves = 0;

	//士
	m_pcmAdvisorPreMoves = 0;

	//象
	m_pcmBishopPreMoves = 0;
	m_pcmBishopCapPreMoves = 0;

	//卒
	m_PawnPreMoves = 0;
	m_PawnNoCapPreMoves = 0;
	m_PawnCapPreMoves = 0;

	m_nRookFoeFileRankCheckNum = 0;

	m_nCannonFoeFileRankCheckNum = 0;

	m_bIsTwoKingMeet= 0;
	
	//将军预判断数组
	//车
	m_bIsRookFilePreCheck = 0;
	m_bIsRookRankPreCheck = 0;

	//马
	m_bIsKnightPreCheck = 0;

	//炮
	m_bIsCannonFilePreCheck = 0;
	m_bIsCannonRankPreCheck = 0;

	//兵
	m_bIsPawnPreCheck = 0;

	//走法有效性预判断数组
//	//车
//	m_bIsRookFilePreValidMove = 0;
//	m_bIsRookRankPreValidMove = 0;
//
//	//炮
//	m_bIsCannonFilePreValidMove = 0;
//	m_bIsCannonRankPreValidMove = 0;

	//车
	m_bIsRookPreValidMove = 0;

	//炮
	m_bIsCannonPreValidMove = 0;

	//hash表
	m_pHashTab[0] = 0;
	m_pHashTab[1] = 0;

	m_pHashTabByDepth[0] = 0;
	m_pHashTabByDepth[1] = 0;

	m_pQSHashTab[0] = 0;
	m_pQSHashTab[1] = 0;
}

/*******************************************************************************************************
函数名 : CanMemMeet
描  述 : 
参  数 : 无
返回值 : 类型为 FSBOOL
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年11月6日 15:51:09
*******************************************************************************************************/
FSBOOL CSearchEngine::CanMemMeet()
{
	return TRUE;
}

/*******************************************************************************************************
函数名 : InitPreMoveListPointer
描  述 : 
参  数 : 无
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年11月6日 15:50:06
*******************************************************************************************************/
FSVOID CSearchEngine::InitPreMoveListPointer()
{
	//走法预产生数组中的走法链表
	FSINT w,p,i,j,k;
	
	//车
	for (p = 0; p < 91; p++)
	{
		for (i = 0; i < ROOKFILECHECKSUMMAXCNT; i++)
		{
			m_pcmRookFileNoCapPreMoves[p][i].pMoveList = &m_InvalidMovePointer;

			for (j = 0; j < 4; j++)
			{
				m_pcmRookFilePreMoves[p][i][j].pMoveList = &m_InvalidMovePointer;
				m_pcmRookFileCapPreMoves[p][i][j].pMoveList = &m_InvalidMovePointer;
			}
		}
		
		for (i = 0; i < ROOKRANKCHECKSUMMAXCNT; i++)
		{
			m_pcmRookRankNoCapPreMoves[p][i].pMoveList = &m_InvalidMovePointer;

			for (j = 0; j < 4; j++)
			{
				m_pcmRookRankPreMoves[p][i][j].pMoveList = &m_InvalidMovePointer;
				m_pcmRookRankCapPreMoves[p][i][j].pMoveList = &m_InvalidMovePointer;
			}
		}
	}
	
	//马
	//不吃子走法
	for (p = 0; p < 91; p++)
	{
		for (i = 0; i < 256; i++)
		{
			for (j = 0; j < 16; j++)
			{
				m_pcmKnightNoCapPreMoves[p][i][j].pMoveList = &m_InvalidMovePointer;
			}
		}
	}
	//吃子走法
	for (p = 0; p < 91; p++)
	{
		for (i = 0; i < 256; i++)
		{
			for (j = 0; j < 16; j++)
			{
				m_pcmKnightCapPreMoves[p][i][j].pMoveList = &m_InvalidMovePointer;
			}
		}
	}

	//炮
	for (p = 0; p < 91; p++)
	{
		for (i = 0; i < CANNONFILECHECKSUMMAXCNT; i++)
		{
			m_pcmCannonFileNoCapPreMoves[p][i].pMoveList = &m_InvalidMovePointer;

			for (j = 0; j < 4; j++)
			{
				m_pcmCannonFilePreMoves[p][i][j].pMoveList = &m_InvalidMovePointer;
				m_pcmCannonFileCapPreMoves[p][i][j].pMoveList = &m_InvalidMovePointer;
			}
		}
		
		for (i = 0; i < CANNONRANKCHECKSUMMAXCNT; i++)
		{
			m_pcmCannonRankNoCapPreMoves[p][i].pMoveList = &m_InvalidMovePointer;

			for (j = 0; j < 4; j++)
			{
				m_pcmCannonRankPreMoves[p][i][j].pMoveList = &m_InvalidMovePointer;
				m_pcmCannonRankCapPreMoves[p][i][j].pMoveList = &m_InvalidMovePointer;
			}
		}
	}

	//将
	for (i = 0; i < 18; i++)
	{
		for (j = 0; j < 16; j++)
		{
			for (k = 0; k < 16; k++)
			{
				m_pcmKingPreMoves[i][j][k].pMoveList = &m_InvalidMovePointer;
			}
		}
	}
	for (i = 0; i < 18; i++)
	{
		for (k = 0; k < 16; k++)
		{
			m_pcmKingNoCapPreMoves[i][k].pMoveList = &m_InvalidMovePointer;
			m_pcmKingCapPreMoves[i][k].pMoveList = &m_InvalidMovePointer;
		}
	}

	//士
	for (i= 0;i<11;i++)
	{
		for (j = 0; j < 16; j++)
		{
			for (k = 0; k < 16; k++)
			{
				m_pcmAdvisorPreMoves[i][j][k].pMoveList = &m_InvalidMovePointer;
			}
		}
	}
	//吃子走法
	for (i= 0;i<11;i++)
	{
		for (k = 0; k < 16; k++)
		{
			m_pcmAdvisorNoCapPreMoves[i][k].pMoveList = &m_InvalidMovePointer;
			m_pcmAdvisorCapPreMoves[i][k].pMoveList = &m_InvalidMovePointer;
		}
	}

	//象
	for (p = 0; p < 15; p++)
	{
		for (i = 0; i < 16; i++)
		{
			for (j = 0; j < 16; j++)
			{ 
				for (k = 0; k < 16; k++)
				{
					m_pcmBishopPreMoves[p][i][j][k].pMoveList = &m_InvalidMovePointer;
				}
			}
		}
	}
	//不吃子走法
	for (p = 0; p < 15; p++)
	{
		for (j = 0; j < 16; j++)
		{ 
			for (k = 0; k < 16; k++)
			{
				m_pcmBishopNoCapPreMoves[p][j][k].pMoveList = &m_InvalidMovePointer;
				m_pcmBishopCapPreMoves[p][j][k].pMoveList = &m_InvalidMovePointer;
			}
		}
	}

	//兵
	for (w = 0; w < 2; w++)
	{
		for (p = 0; p < 56; p++)
		{
			for (i= 0;i<8;i++)
			{
				for (j = 0; j < 8; j++)
				{
					m_PawnPreMoves[w][p][i][j].pMoveList = &m_InvalidMovePointer;
				}
			}
		}
	}
	for (w = 0; w < 2; w++)
	{
		for (p = 0; p < 56; p++)
		{
			for (j = 0; j < 8; j++)
			{
				m_PawnNoCapPreMoves[w][p][j].pMoveList = &m_InvalidMovePointer;
				m_PawnCapPreMoves[w][p][j].pMoveList = &m_InvalidMovePointer;
			}
		}
	}
}

/*******************************************************************************************************
函数名 : QHStopThink
描  述 : 
参  数 : 无
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年11月6日 15:49:23
*******************************************************************************************************/
#ifdef _WIN32
FSVOID CSearchEngine::QHStopThink()
{
	if (m_hThinkThread == NULL)
	{
		return;
	}
	
	m_bStopThink = TRUE;
	
	//结束搜索线程
	KillThread(m_hThinkThread);
	
	//恢复刚搜索时的局面,这点与UCCI协议的时间控制不同 [2005/11/7 16:47:03]
	LoadFEN(m_szFEN);
	DEBUGASSERT(!memcmp(m_nPiecePosBeforeAI, m_nPiecePos, 32 * sizeof(FSINT)));

	SearchStop();
}
#else
#endif

/*******************************************************************************************************
函数名 : CBChessID2FEN
描  述 : 
参  数 : 
        FSCHAR *szFEN : 
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年11月8日 22:24:00
*******************************************************************************************************/
FSVOID CSearchEngine::CBChessID2FEN(FSCHAR *szFEN)
{
	FSINT i, j;
	FSINT nSpaceCnt;
	FSINT nPos = 4;

	strncpy(szFEN, "fen ", 4);

	for (j = 9; j >= 0; j--)
	{
		i = 0;

		while (i < 9)
		{
			if (m_nCBChessID[m_nPos2CGN[i][j]] == NOCHESS)
			{
				nSpaceCnt = 0;
				while (i < 9 && m_nCBChessID[m_nPos2CGN[i][j]] == NOCHESS)
				{
					nSpaceCnt++;
					i++;
				}

				szFEN[nPos++] = '0' + nSpaceCnt;
			}
			else
			{
				szFEN[nPos++] = g_cChessID2Alpha[m_nCBChessID[m_nPos2CGN[i][j]]];
				i++;
			}
		}

		szFEN[nPos++] = '/';
	}

	szFEN[nPos - 1] = ' ';

	if (m_bIsRed == RED)
	{
		szFEN[nPos] = 'w';
	}
	else
	{
		szFEN[nPos] = 'b';
	}

	strncpy(szFEN + nPos + 1, " - - ", 5);
	nPos += 6;
	szFEN[nPos++] = '0' + m_nBouts;
	szFEN[nPos++] = ' ';
	szFEN[nPos++] = '0' + m_nCurBout;
	if (m_nNoEatMoveCnt_BS > 0)
	{
		FSCHAR szMove[MAXBUFFERLEN];
		FSINT i;

		for (i = 0; i < m_nNoEatMoveCnt_BS; i++)
		{
			szFEN[nPos++] = ' ';
			ChessMove2Str(m_Moves_NoEat[i], szMove);
			strncpy(szFEN + nPos, szMove, 4);
			nPos += 4;
		}
	}

	szFEN[nPos] = '\0';
}

/*******************************************************************************************************
函数名 : GetPVLineFromHash
描  述 : 
参  数 : 无
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年11月8日 22:23:52
*******************************************************************************************************/
FSVOID CSearchEngine::GetPVLineFromHash(FSINT nSearchDepth)
{
	CHESSMOVE cm = m_cmNullMove;
	FSINT nToChessID;
	FSINT nToPieceID;

	ProbeHash(0, nSearchDepth, g_nPerCheckValue, -g_nPerCheckValue, cm);
	if (cm != m_cmNullMove)
	{
		m_PVLine.pMoveList[m_PVLine.nCount++] = cm;

		MakeMove(cm, nToChessID, nToPieceID);

		// 避免重复,免得会栈溢出[2005/11/12 14:54:05]
		if (IsRep(1) == FALSE)
		{
			GetPVLineFromHash(nSearchDepth - 1);
		}

		UnMakeMove(cm, nToChessID, nToPieceID);
	}
}

/*******************************************************************************************************
函数名 : ClearStack
描  述 : 
参  数 : 
        T stackRedoMove : 
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年11月8日 22:23:43
*******************************************************************************************************/
template<class T> FSVOID CSearchEngine::ClearStack(T s)
{
	while (s.size() > 0)
	{
		s.pop();
	}
}

/*******************************************************************************************************
函数名 : ChangeSide
描  述 : 
参  数 : 无
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年11月12日 14:58:42
*******************************************************************************************************/
inline FSVOID CSearchEngine::ChangeSide()
{
	m_bIsRed = 1 - m_bIsRed;

	m_nHashTabIndex ^= m_nHashKeyPlayer32;
	m_nCBCheckSum ^= m_nHashKeyPlayer64;
}

/*******************************************************************************************************
函数名 : KillThread
描  述 : 
参  数 : 
        HANDLE h : 
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年11月12日 14:58:36
*******************************************************************************************************/
FSVOID CSearchEngine::KillThread(HANDLE h)
{
	if (h == 0)
	{
		return;
	}

	if (::TerminateThread(h, 0) == FALSE)
	{
		DISPMSG1_ENDL(cout,	"Terminate Thread Fail!");

		return;
	}
	::WaitForSingleObject(h, INFINITE);
	::CloseHandle(h);
}

/*******************************************************************************************************
函数名 : IsSpecialPos
描  述 : 
参  数 : 
        FSCHAR *& szStr : 
返回值 : 类型为 BOOL
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年11月12日 14:58:30
*******************************************************************************************************/
BOOL CSearchEngine::IsSpecialPos(FSCHAR *& szStr)
{
	szStr = JumpNoMatterChar(szStr);

	// 开局局面
	if (!strncmp(szStr, "startpos", 8) || strncmp(szStr, "opengamepos", 11) == 0)
	{
		LoadFEN((FSCHAR*)g_szOpenGamePos);
		return TRUE;
	}

	// 中局局面
	if (!strncmp(szStr, "midgamepos", 10))
	{
		LoadFEN((FSCHAR*)g_szMidGamePos);
		return TRUE;
	}

	// 残局局面
	if (!strncmp(szStr, "endgamepos", 10))
	{
		LoadFEN((FSCHAR*)g_szEndGamePos);
		return TRUE;
	}

	// 连杀排局
	if (!strncmp(szStr, "checkmatepos", 12))
	{
		LoadFEN((FSCHAR*)g_szCheckMatePos);
		return TRUE;
	}

	// zugzwang局面
	if (!strncmp(szStr, "zugzwangpos", 11))
	{
		LoadFEN((FSCHAR*)g_szZugzwangPos);
		return TRUE;
	}

	// 检测Null Move的局面
	if (!strncmp(szStr, "detectnm", 8))
	{
		LoadFEN((FSCHAR*)g_szDetectNMPos);
		return TRUE;
	}

	// 马对单士
	if (!strncmp(szStr, "knightvssgladvisor", 18))
	{
		LoadFEN((FSCHAR*)g_szKnightVSSglAdvisorPos);
		return TRUE;
	}

	// 海底捞月
	if (!strncmp(szStr, "fishformooninsea", 15))
	{
		LoadFEN((FSCHAR*)g_szFishforMoonInSeaPos);
		return TRUE;
	}

	// 测试循环
	if (!strncmp(szStr, "detectrep", 15))
	{
		LoadFEN((FSCHAR*)g_szRepDetectPos);
		return TRUE;
	}

	return FALSE;
}

/*******************************************************************************************************
函数名 : RepValue
描  述 : 
参  数 : 
        FSINT nRep : 
返回值 : 类型为 FSINT
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年11月12日 14:58:22
*******************************************************************************************************/
inline FSINT CSearchEngine::RepValue(FSINT nRep)
{
	//己方长将
	if (nRep == 2)
	{
		return m_nCurDepth + g_nPerCheckValue;
	}

	//对方长将
	if (nRep == 4)
	{
		return -g_nPerCheckValue - m_nCurDepth;
	}

	//重复
	return DrawValue();
}

/*******************************************************************************************************
函数名 : DrawValue
描  述 : 和棋分值
参  数 : 无
返回值 : 类型为 inline
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年11月12日 14:58:16
*******************************************************************************************************/
inline FSINT CSearchEngine::DrawValue()
{
	return (m_nCurDepth & 1) == 0 ? -m_nContemptFactor : m_nContemptFactor;
}

/*******************************************************************************************************
函数名 : IsDraw
描  述 : 是否为和棋
参  数 : 无
返回值 : 类型为 FSBOOL
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年11月12日 14:58:05
*******************************************************************************************************/
FSBOOL CSearchEngine::IsDraw()
{
	DEBUGASSERT(m_nChessCnt[BROOK] <= 2);
	DEBUGASSERT(m_nChessCnt[BKNIGHT] <= 2);
	DEBUGASSERT(m_nChessCnt[BCANNON] <= 2);
	DEBUGASSERT(m_nChessCnt[RROOK] <= 2);
	DEBUGASSERT(m_nChessCnt[RKNIGHT] <= 2);
	DEBUGASSERT(m_nChessCnt[RCANNON] <= 2);

	return m_nNoEatMoveCnt_IS >= m_nDrawMoves || 
		   (m_nChessCnt[BROOK] == 0 && m_nChessCnt[BKNIGHT]  == 0 && m_nChessCnt[BCANNON] == 0 && m_nChessCnt[BPAWN] == 0 && 
		    m_nChessCnt[RROOK] == 0 && m_nChessCnt[RKNIGHT]  == 0 && m_nChessCnt[RCANNON] == 0 && m_nChessCnt[RPAWN] == 0);

//	return (m_nChessCnt[BROOK] == 0 && m_nChessCnt[BKNIGHT]  == 0 && m_nChessCnt[BCANNON] == 0 && m_nChessCnt[BPAWN] == 0 && 
//		    m_nChessCnt[RROOK] == 0 && m_nChessCnt[RKNIGHT]  == 0 && m_nChessCnt[RCANNON] == 0 && m_nChessCnt[RPAWN] == 0) || 
//			m_nNoEatMoveCnt_IS >= m_nDrawMoves;

//	return m_nNoEatMoveCnt_IS >= m_nDrawMoves || 
//		   (m_nChessCnt[BROOK] + m_nChessCnt[BKNIGHT] + m_nChessCnt[BCANNON] + m_nChessCnt[BPAWN] + 
//		    m_nChessCnt[RROOK] + m_nChessCnt[RKNIGHT] + m_nChessCnt[RCANNON] + m_nChessCnt[RPAWN] == 0);
}

/*******************************************************************************************************
函数名 : SearchGo
描  述 : 
参  数 : 无
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年11月18日 19:29:33
*******************************************************************************************************/
FSVOID CSearchEngine::SearchGo()
{
#ifdef _WIN32
	FSDWORD	(WINAPI CSearchEngine::*p1)() = SearchMain;
	FSDWORD	(WINAPI *p2)(LPVOID);

	memcpy(&p2, &p1, sizeof(p1));

	m_hThinkThread = ::CreateThread(0, 0, p2, this, 0, 0);//创建线程执行搜索
	if(m_hThinkThread == NULL)
	{
		cout << "创建搜索线程时出现错误!" << endl;
	}
#else
#endif

	m_bPonderHit = FALSE;
}

/*******************************************************************************************************
函数名 : PrintEgnStatus
描  述 : 
参  数 : 无
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年11月18日 19:29:26
*******************************************************************************************************/
FSVOID CSearchEngine::PrintEgnStatus()
{
	switch(m_esUCCIEgnStatus)
	{
	case EgnSIdle:
		DISPMSG1_ENDL(cout,	"idle");
		break;
	
	case EgnSBusy:
		DISPMSG1_ENDL(cout,	"busy");
		break;

	default:
		break;
	}
}

/*******************************************************************************************************
函数名 : OnlyLegalNum
描  述 : 
参  数 : 无
返回值 : 类型为 FSINT
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年11月18日 19:29:19
*******************************************************************************************************/
FSINT CSearchEngine::LegalMoves(FSINT& nLegalNum)
{
	FSINT i;
	FSINT nCnt;
	FSINT nToChessID;
	FSINT nToPieceID;
	FSINT nLegalCnt = 0;

	CHESSMOVE cm;

	// [2006/1/7 21:32:50]
//	if (IsChecked(1 - m_bIsRed) == TRUE)
//	{
//		m_nCheckExtensionPlies = 2;
//	}
	
	GenMoves(0);
	nCnt = m_nMovesCnt;

	for (i = 0; i < nCnt; i++)
	{
		cm = m_MoveList[0][i];

		MakeMove(cm, nToChessID, nToPieceID);

		if (m_nPiecePos[m_nKingID2PieceID[m_bIsRed]] == INVALIDPIECEPOS && cm.nKind == EAT)
		{
			nLegalNum = i;
			nLegalCnt = 1;
			return -1;
		}

		if (IsLegalMove(cm) == TRUE)
		{
			nLegalNum = i;
			nLegalCnt++;

			// [2006/1/7 21:42:44]
//			if (IsChecked(1 - m_bIsRed) == TRUE)
//			{
//				m_nCheckExtensionPlies = 2;
//			}
		}

		UnMakeMove(cm, nToChessID, nToPieceID);
	}

	return nLegalCnt;
}

/*******************************************************************************************************
函数名 : GetMoveFromBook
描  述 : 
参  数 : 无
返回值 : 类型为 CHESSMOVE
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年12月10日 14:16:53
*******************************************************************************************************/
CHESSMOVE CSearchEngine::GetMoveFromBook()
{
	FILE *fp = fopen(m_szBookFile, "r");
	if (fp == NULL)
	{
		SWITCHEGN(DISPMSG4_ENDL(cout, "Open book ", m_szDir, m_szBookFile, " fail"););

		return m_cmNullMove;
	}

	FSCHAR szTemp[1024];
	CHESSMOVE cmTemp;
	FSCHAR szMove[8];
	FSINT nToChessID;
	FSINT nToPieceID;
#ifdef _WIN32
	vector<CHESSMOVE> vectorBookMove;
#endif

	while (feof(fp) == 0)
	{
		fgets(szTemp, 1024, fp);

		if (szTemp[0] != '\0' && strlen(szTemp) > 4)
		{
			if ((cmTemp = LoadBookFEN(szTemp)) != m_cmNullMove)
			{
//				if (vectorBookMove.size() > 0)
//				{
//					if (cmTemp.nScore > vectorBookMove[0].nScore)
//					{
//						vectorBookMove.clear();
//						vectorBookMove.push_back(cmTemp);
//					}
//					else if (cmTemp.nScore == vectorBookMove[0].nScore)
//					{
//						vectorBookMove.push_back(cmTemp);
//					}
//				}
//				else
//				{
//					vectorBookMove.push_back(cmTemp);
//				}

				MakeMove(cmTemp, nToChessID, nToPieceID);
				if (IsRep(3) == FALSE)
				{
					vectorBookMove.push_back(cmTemp);
					
					ChessMove2Str(cmTemp, szMove);
					
					SWITCHEGN(DISPMSG4_ENDL(cout, "info depth 0 score ", cmTemp.nScore, " pv ", szMove););
				}
				UnMakeMove(cmTemp, nToChessID, nToPieceID);
			}
		}
	}
	fclose(fp);

	FSINT nSize = vectorBookMove.size();
	FSINT i;
	FSINT nAllScore = 0;
	vector<FSFLOAT> vectorProb;

	if (nSize == 0)
	{
		SWITCHEGN(DISPMSG4_ENDL(cout,	"Load move from book ", m_szDir, m_szBookFile, " fail"););

		m_nGetOpenBookMoveErrCnt++;

		return m_cmNullMove;
	}

	for (i = 0; i < nSize; i++)
	{
		nAllScore += vectorBookMove[i].nScore;
	}

	if (nAllScore == 0)
	{
		for (i = 0; i < nSize; i++)
		{
			vectorBookMove[i].nScore = 1;
		}

		nAllScore = nSize;
	}
	
	srand(time(0));
	nAllScore = Rand32() % nAllScore;
	
	//轮盘法按概率选择走法
	for (i = 0; i < nSize; i++)
	{
		nAllScore -= vectorBookMove[i].nScore;
		if (nAllScore < 0)
		{
			return vectorBookMove[i];
		}
	}
	
	return vectorBookMove[nSize - 1];
}

/*******************************************************************************************************
函数名 : LoadBookFEN
描  述 : 
参  数 : 
        FSCHAR *szFEN : 
返回值 : 类型为 inline
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年12月10日 14:16:43
*******************************************************************************************************/
inline CHESSMOVE CSearchEngine::LoadBookFEN(FSCHAR *szFEN)
{
	if (szFEN == 0)
	{
		return m_cmNullMove;
	}

	FSINT i;
	FSINT nCuri = 0;
	FSINT nCurj = 9;
	FSINT nCount;
	FSINT nPos;
	FSINT nCBChessID[90];
	FSBOOL bSide;
	FSCHAR szMove[5];
	CHESSMOVE cmTemp;
	FSINT nScore;

	memset(nCBChessID, NOCHESS, MAXCHESSGRIDNUM * sizeof(FSINT));

	//跳过前面的空格,Tab键
	szFEN = JumpNoMatterChar(szFEN);
	strncpy(szMove, szFEN, 4);
	szFEN += 4;
	szFEN = JumpNoMatterChar(szFEN);
	nScore = atoi(szFEN);
	szFEN = JumpDigit(szFEN);
	szFEN = JumpNoMatterChar(szFEN);

	nPos = 0;
	while(szFEN[nPos] != ' ')
	{
		if (isdigit(szFEN[nPos]))
		{
			nCount = atoi(szFEN + nPos);
			for (i = nCuri; i < nCuri + nCount; i++)
			{
				nCBChessID[10 * i + nCurj] = NOCHESS;
			}
			nCuri += nCount;
		}
		else if (szFEN[nPos] == '/')
		{
			nCuri = 0;
			nCurj--;
		}
		else if (szFEN[nPos] >= 'a' && szFEN[nPos] <= 'z')//为小写字母
		{
			nCBChessID[10 * nCuri + nCurj] = g_nAlpha2Chess[szFEN[nPos] -'a'];
			nCuri++;
		}
		else if (szFEN[nPos] >= 'A' && szFEN[nPos] <= 'Z')//为大写字母
		{
			nCBChessID[10 * nCuri + nCurj] = g_nAlpha2Chess[szFEN[nPos] -'A'] + 7;
			nCuri++;
		}
		else//错误处理
		{
			return m_cmNullMove;
		}

		nPos++;
	}

	szFEN = JumpNoMatterChar(szFEN + nPos);
	bSide = (*szFEN != 'b');

	UCCIStr2ChessMove(szMove, cmTemp);
//	Str2ChessMove(szMove, cmTemp);
	cmTemp.nScore = nScore;

	if (GetCBCheckSum(nCBChessID, bSide) == m_nCBCheckSum)
	{
		return cmTemp;
	}

	//注释掉的部分为左右变换
	TXTBOOKRECORD txtBookRecord;
#ifndef LRTEXTBOOK
	TXTBOOKRECORD txtBookRecord2;
#endif

	txtBookRecord.cm = cmTemp;
	memcpy(txtBookRecord.nCBChessID, nCBChessID, 90 * sizeof(FSINT));

#ifndef LRTEXTBOOK
	txtBookRecord2.cm = cmTemp;
	memcpy(txtBookRecord2.nCBChessID, nCBChessID, 90 * sizeof(FSINT));
	LRTransform(txtBookRecord2);

	if (GetCBCheckSum(txtBookRecord2.nCBChessID, bSide) == m_nCBCheckSum)
	{
		return txtBookRecord2.cm;
	}
#endif

#ifndef UDTEXTBOOK
	bSide = 1 - bSide;

	UDTransform(txtBookRecord);
	if (GetCBCheckSum(txtBookRecord.nCBChessID, bSide) == m_nCBCheckSum)
	{
		return txtBookRecord.cm;
	}
#endif

#ifndef LRTEXTBOOK
#ifndef UDTEXTBOOK
	UDTransform(txtBookRecord2);
	if (GetCBCheckSum(txtBookRecord2.nCBChessID, bSide) == m_nCBCheckSum)
	{
		return txtBookRecord2.cm;
	}
#endif
#endif

	return m_cmNullMove;
}

/*******************************************************************************************************
函数名 : GetCBCheckSum
描  述 : 
参  数 : 
        const FSINT nCBChessID[] : 
		FSBOOL bSide             : 
返回值 : 类型为 inline
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年12月10日 14:16:30
*******************************************************************************************************/
inline FSUINT64 CSearchEngine::GetCBCheckSum(const FSINT nCBChessID[], FSBOOL bSide)
{
	FSINT p;
	FSUINT64 nCBCheckSum = 0;

	for (p = 0; p < 90; p++)
	{
		nCBCheckSum ^= m_nHashKey64[nCBChessID[p]][m_CGN2Pos[p].x][m_CGN2Pos[p].y];
	}

	if (bSide == RED)
	{
		nCBCheckSum ^= m_nHashKeyPlayer64;
	}

	return nCBCheckSum;
}

/*******************************************************************************************************
函数名 : LRTransform
描  述 : 
参  数 : 
        TXTBOOKRECORD& txtBookRecord : 
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年12月10日 14:16:23
*******************************************************************************************************/
FSVOID CSearchEngine::LRTransform(TXTBOOKRECORD& txtBookRecord)
{
	FSINT *nCBChessID = txtBookRecord.nCBChessID;
	CHESSMOVE *pCM = &txtBookRecord.cm;

	pCM->nFrom += (8 - 2 * m_CGN2Pos[pCM->nFrom].x) * 10;
	pCM->nTo += (8 - 2 * m_CGN2Pos[pCM->nTo].x) * 10;

	FSINT nChessID;
	FSINT i, j;

	for (j = 0; j < 10; j++)
	{
		for (i = 0; i < 4; i++)
		{
			nChessID = nCBChessID[m_nPos2CGN[i][j]];
			nCBChessID[m_nPos2CGN[i][j]] = nCBChessID[m_nPos2CGN[8 - i][j]];
			nCBChessID[m_nPos2CGN[8 - i][j]] = nChessID;
		}
	}
}

/*******************************************************************************************************
函数名 : UDTransform
描  述 : 
参  数 : 
        TXTBOOKRECORD& txtBookRecord : 
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年12月10日 14:16:15
*******************************************************************************************************/
FSVOID CSearchEngine::UDTransform(TXTBOOKRECORD& txtBookRecord)
{
	FSINT *nCBChessID = txtBookRecord.nCBChessID;
	CHESSMOVE *pCM = &txtBookRecord.cm;

	pCM->nFrom += 9 - 2 * m_CGN2Pos[pCM->nFrom].y;
	pCM->nTo += 9 - 2 * m_CGN2Pos[pCM->nTo].y;

	FSINT nChessID;
	FSINT i, j;

	for (i = 0; i < 9; i++)
	{
		for (j = 0; j < 5; j++)
		{
			nChessID = nCBChessID[m_nPos2CGN[i][j]];
			nCBChessID[m_nPos2CGN[i][j]] = nCBChessID[m_nPos2CGN[i][9 - j]];
			nCBChessID[m_nPos2CGN[i][9 - j]] = nChessID;

			nCBChessID[m_nPos2CGN[i][j]] = m_nConvertChessID[nCBChessID[m_nPos2CGN[i][j]]];
			nCBChessID[m_nPos2CGN[i][9 - j]] = m_nConvertChessID[nCBChessID[m_nPos2CGN[i][9 - j]]];
		}
	}
}

//#ifdef _WIN32
///*******************************************************************************************************
//函数名 : DispTime
//描  述 : 
//参  数 : 
//        FSUINT uTimerID : 
//		FSUINT uMsg     : 
//		FSDWORD dwUser  : 
//		FSDWORD dw1     : 
//		FSDWORD dw2     : 
//返回值 : 类型为 FSVOID
//版  本 : 0.2.0.0
//作  者 : ahei
//时  间 : 2005年12月10日 14:16:03
//*******************************************************************************************************/
//FSVOID CALLBACK CSearchEngine::DispTime(FSUINT uTimerID, FSUINT uMsg, FSDWORD dwUser, FSDWORD dw1, FSDWORD dw2)
//{
////	MMRESULT mmResult = timeKillEvent(uTimerID);
//
////	if (mmResult == MMSYSERR_INVALPARAM)
//	{
//		DISPMSG3_ENDL(cout,		"定时器", uTimerID, "不存在!");
//	}
//}
//#else
//#endif

/*******************************************************************************************************
函数名 : SetChessStyle
描  述 : 
参  数 : 
        FSCHAR *szStyle : 
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年12月22日 16:03:03
*******************************************************************************************************/
FSVOID CSearchEngine::SetChessStyle(FSCHAR *szStyle)
{
	if (szStyle == 0)
	{
		return;
	}

	szStyle = JumpNoMatterChar(szStyle);

	if (szStyle == 0)
	{
		return;
	}

	strlwr(szStyle);

	FSINT nFSCHARSize = sizeof(FSCHAR);

	if (strncmp(szStyle, "keep", 5) == 0)
	{
		m_ChessStyle = Keep;
		m_nContemptFactor = g_nContemptFactor_Keep;

		return;
	}

	if (strncmp(szStyle, "normal", 7) == 0)
	{
		m_ChessStyle = Normal;
		m_nContemptFactor = g_nContemptFactor_Normal;

		return;
	}

	if (strncmp(szStyle, "risky", 6) == 0)
	{
		m_ChessStyle = Risky;
		m_nContemptFactor = g_nContemptFactor_Risky;

		return;
	}
}

/*******************************************************************************************************
函数名 : ClearHash
描  述 : 
参  数 : 无
返回值 : 类型为 inline
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年12月22日 16:02:53
*******************************************************************************************************/
inline FSVOID CSearchEngine::ClearHash()
{
	FSINT i;
#ifdef TIMECALC
	FSINT tStart = clock();
	FSINT tEnd;
#endif

	for (i = 0; i < m_nHashTabMask + 1; i++)
	{
		m_pHashTab[BLACK][i].nCBCheckSum = 0;
		m_pHashTab[RED][i].nCBCheckSum = 0;

//		m_pHashTabByDepth[BLACK][i].nCBCheckSum = 0;
//		m_pHashTabByDepth[RED][i].nCBCheckSum = 0;

		// 静态搜索的hash
		m_pQSHashTab[BLACK][i].nCBCheckSum = 0;
		m_pQSHashTab[RED][i].nCBCheckSum = 0;
	}

#ifdef TIMECALC
	tEnd = clock();
	DISPMSG3_ENDL(cout,	"清空hash表时间:", tEnd - tStart, "ms");
#endif
}

/*******************************************************************************************************
函数名 : PrintBetterPV
描  述 : 
参  数 : 
        FSINT nDepth            : 
		FSINT nSearchDepth      : 
		FSINT nEval             : 
		FSINT nHashFlag         : 
		CHESSMOVE &cmBetterMove : 
返回值 : 类型为 FSVOID
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年12月22日 16:02:45
*******************************************************************************************************/
inline FSVOID CSearchEngine::PrintBetterPV(FSINT nDepth, FSINT nSearchDepth, FSINT nEval, FSINT nHashFlag, CHESSMOVE &cmBetterMove)
{
	m_PVLine.nCount = 0;
	RecordHash(nDepth, nSearchDepth, nEval, nHashFlag, cmBetterMove);
	GetPVLineFromHash(m_nCurSearchDepth_ID);
	m_PVLine.pMoveList[0] = cmBetterMove;
	PrintPVLine();
}

/*******************************************************************************************************
函数名 : GetHash
描  述 : 
参  数 : 
        HASHITEM **pHashTab   : 
		FSINT nDepth          : 
		FSINT nSearchDepth    : 
		FSINT alpha           : 
		FSINT beta            : 
		CHESSMOVE &cmBestMove : 
返回值 : 类型为 inline
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年12月26日 20:52:11
*******************************************************************************************************/
inline FSINT CSearchEngine::GetHash(HASHITEM **pHashTab, FSINT nDepth, FSINT nSearchDepth, FSINT alpha, FSINT beta, CHESSMOVE &cmBestMove)
{
	HASHITEM hashItem;

	DEBUGASSERT((m_nHashTabIndex & m_nHashTabMask) >= 0 && (m_nHashTabIndex & m_nHashTabMask) <= m_nHashTabMask);
	
	STATEMENT_STAT(m_nProbeHashCnt++;);

	hashItem = pHashTab[m_bIsRed][m_nHashTabIndex & m_nHashTabMask];
	if (hashItem.nCBCheckSum == m_nCBCheckSum)
	{
		FSINT nHashVal = hashItem.nEval;

		// 说明此hash项是长将时的局面,不能要[2005/12/22 18:23:51]
		if (nHashVal > g_nMateValue)
		{
			return g_nInvalidValue;
		}

		//返回最佳走法
		cmBestMove = hashItem.cmBestMove;

		if (hashItem.nDepth < nSearchDepth - nDepth)
		{
			return g_nInvalidValue;
		}

		if (hashItem.nHashFlag == HASHEXACT)
		{
			m_PVLineThink.pMoveList[nDepth] = hashItem.cmBestMove;
			STATEMENT_STAT(m_nHitHashCnt++;);
			return nHashVal;
		}

		//hashItem.nHashFlag != HASHEXACT
		if (hashItem.nHashFlag == HASHALPHA && nHashVal <= alpha)
		{
			STATEMENT_STAT(m_nHitHashCnt++;);
			return alpha;
		}

		if (hashItem.nHashFlag == HASHBETA && nHashVal >= beta)
		{
			STATEMENT_STAT(m_nHitHashCnt++;);
			return beta;
		}

		return g_nInvalidValue;
	}

	return g_nInvalidValue;
}

/*******************************************************************************************************
函数名 : GetFlexValue
描  述 : 
参  数 : 无
返回值 : 类型为 inline
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年12月26日 20:51:46
*******************************************************************************************************/
inline FSINT CSearchEngine::GetFlexValue()
{
	return GetFlexValue(m_bIsRed) - GetFlexValue(1 - m_bIsRed);
}

/*******************************************************************************************************
函数名 : GetFlexValue
描  述 : 
参  数 : 
        FSBOOL bIsRed : 
返回值 : 类型为 inline
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年12月26日 20:51:40
*******************************************************************************************************/
inline FSINT CSearchEngine::GetFlexValue(FSBOOL bIsRed)
{
	FSINT nPiecePos;

	FSINT nRook;
	FSINT nKnight;
	FSINT nCannon;
	FSINT nAdvisor;
	FSINT nBishop;
	FSINT nPawn;

	FSINT nRet = 0;

	nRook = m_nRook[bIsRed];
	nKnight = m_nKnight[bIsRed];
	nCannon = m_nCannon[bIsRed];
	nAdvisor = m_nAdvisor[bIsRed];
	nBishop = m_nBishop[bIsRed];
	nPawn = m_nPawn[bIsRed];

	//车
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nRook][0]]) != INVALIDPIECEPOS)
	{
		nRet += GetRookFlexValue(nPiecePos);
	}
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nRook][1]]) != INVALIDPIECEPOS)
	{
		nRet += GetRookFlexValue(nPiecePos);
	}
	
	//马
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nKnight][0]]) != INVALIDPIECEPOS)
	{
		nRet += GetKnightFlexValue(nPiecePos);
	}
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nKnight][1]]) != INVALIDPIECEPOS)
	{
		nRet += GetKnightFlexValue(nPiecePos);
	}
	
	//炮
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nCannon][0]]) != INVALIDPIECEPOS)
	{
		nRet += GetCannonFlexValue(nPiecePos);
	}
	if ((nPiecePos = m_nPiecePos[m_nChessID2PieceID[nCannon][1]]) != INVALIDPIECEPOS)
	{
		nRet += GetCannonFlexValue(nPiecePos);
	}

	return nRet;
}

/*******************************************************************************************************
函数名 : GetRookFlexValue
描  述 : 
参  数 : 
        FSINT p : 
返回值 : 类型为 inline
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年12月26日 20:51:32
*******************************************************************************************************/
inline FSINT CSearchEngine::GetRookFlexValue(FSINT p)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);

	FSINT i, j;
	FSINT nCount;
	FSINT nRookCheckNum;//车校验序号
	PRECHESSMOVE pcmFile;
	PRECHESSMOVE pcmRank;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;
	
	//行
	nRookCheckNum = m_nRookFileRankCheckNum[i][m_nBitFile[j]];
	pcmFile = m_pcmRookFileNoCapPreMoves[p][nRookCheckNum];
	nCount = pcmFile.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= MAXROOKFILENOCAPMOVECNT);

	//列
	nRookCheckNum = m_nRookFileRankCheckNum[j][m_nBitRank[i]];
	pcmRank = m_pcmRookRankNoCapPreMoves[p][nRookCheckNum];
	nCount += pcmRank.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= MAXROOKRANKNOCAPMOVECNT + MAXROOKFILENOCAPMOVECNT);

	return m_nRookFlexValue[nCount];
}

/*******************************************************************************************************
函数名 : GetKnightFlexValue
描  述 : 
参  数 : 
        FSINT p : 
返回值 : 类型为 inline
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年12月26日 20:51:24
*******************************************************************************************************/
inline FSINT CSearchEngine::GetKnightFlexValue(FSINT p)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);

	FSINT i, j;
	FSINT nKnightLegCheckSum;
	FSINT nKnightFile0AroundCheckSum;
	FSINT nKnightFile1AroundCheckSum;
	FSINT nKnightFile2AroundCheckSum;
	FSINT nKnightFile3AroundCheckSum;
	FSINT nKnightFileAroundCheckSum;

	PRECHESSMOVE pcm;
	
	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;
	
	nKnightFile0AroundCheckSum = m_nFileRankAroundCheckSum[i][m_nBitFile[m_nAbs[j][2]]];
	nKnightFile1AroundCheckSum = m_nFileRankAroundCheckSum2[i][m_nBitFile[m_nAbs[j][1]]];
	nKnightFile2AroundCheckSum = m_nFileRankAroundCheckSum2[i][m_nBitFile[j + 1]];
	nKnightFile3AroundCheckSum = m_nFileRankAroundCheckSum[i][m_nBitFile[j + 2]];

	nKnightFileAroundCheckSum = 
		m_nKnightAroundCheckSum[m_nAroundCheckSum[nKnightFile0AroundCheckSum][nKnightFile1AroundCheckSum]][m_nAroundCheckSum[nKnightFile2AroundCheckSum][nKnightFile3AroundCheckSum]];
	nKnightLegCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBitFile[j]]][m_nFileRankAroundCheckSum[j][m_nBitRank[i]]];
	
	//不吃子走法
	pcm = m_pcmKnightNoCapPreMoves[p][nKnightFileAroundCheckSum][nKnightLegCheckSum];
	DEBUGASSERT(pcm.nCount >= 0 && pcm.nCount <= MAXKNIGHTNOCAPMOVECNT);
	return m_nKnightFlexValue[pcm.nCount];
}

/*******************************************************************************************************
函数名 : GetCannonFlexValue
描  述 : 
参  数 : 
        FSINT p : 
返回值 : 类型为 inline
版  本 : 0.2.0.0
作  者 : ahei
时  间 : 2005年12月26日 20:51:13
*******************************************************************************************************/
inline FSINT CSearchEngine::GetCannonFlexValue(FSINT p)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);

	FSINT i, j;
	FSINT nCount;
	FSINT nCannonCheckNum;
	PRECHESSMOVE pcmFile;
	PRECHESSMOVE pcmRank;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	//行
	nCannonCheckNum = m_nCannonFileRankCheckNum[i][m_nBitFile[j]];
	pcmFile = m_pcmCannonFileNoCapPreMoves[p][nCannonCheckNum];
	nCount = pcmFile.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= MAXCANNONFILENOCAPMOVECNT);

	//列
	nCannonCheckNum = m_nCannonFileRankCheckNum[j][m_nBitRank[i]];
	pcmRank = m_pcmCannonRankNoCapPreMoves[p][nCannonCheckNum];
	nCount += pcmRank.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= MAXCANNONRANKNOCAPMOVECNT + MAXCANNONFILENOCAPMOVECNT);

	return m_nCannonFlexValue[nCount];
}

inline FSBOOL CSearchEngine::IsEmptyHeartCannon(FSBOOL bIsRed)
{
	FSINT nFoeKingP;
	FSINT nFoeKingI;
	FSINT nFoeKingJ;
	FSINT nFoeKingPosCheckNum;
	
	FSINT nRookPos1;
	FSINT nRookPos2;

	FSINT nKnightPos1;
	FSINT nKnightPos2;

	FSINT nCannonPos1;
	FSINT nCannonPos2;

	FSINT nFoeKingBitFile;
	FSINT nFoeKingBitRank;

	FSINT nFoeAdvisorPos1;
	FSINT nFoeAdvisorPos2;

	FSINT nRook;
	FSINT nKnight;
	FSINT nCannon;
	FSINT nFoeAdvisor;

	//对方士
	nFoeAdvisor = m_nAdvisor[1 - bIsRed];

	nFoeAdvisorPos1 = m_nPiecePos[m_nChessID2PieceID[nFoeAdvisor][0]];
	nFoeAdvisorPos2 = m_nPiecePos[m_nChessID2PieceID[nFoeAdvisor][1]];

	//无双士
	if (nFoeAdvisorPos1 == INVALIDPIECEPOS && nFoeAdvisorPos1 == INVALIDPIECEPOS)
	{
		return FALSE;
	}

	nRook = m_nRook[bIsRed];
	nKnight = m_nKnight[bIsRed];

	//车
	nRookPos1 = m_nPiecePos[m_nChessID2PieceID[nRook][0]];
	nRookPos2 = m_nPiecePos[m_nChessID2PieceID[nRook][1]];

	//马
	nKnightPos1 = m_nPiecePos[m_nChessID2PieceID[nKnight][0]];
	nKnightPos2 = m_nPiecePos[m_nChessID2PieceID[nKnight][1]];

	//无车马
	if (nRookPos1 == INVALIDPIECEPOS && nRookPos2 == INVALIDPIECEPOS && 
		nKnightPos1 == INVALIDPIECEPOS && nKnightPos2 == INVALIDPIECEPOS)
	{
		return FALSE;
	}

	nCannon = m_nCannon[bIsRed];

	//对方将
	nFoeKingP = m_nPiecePos[m_nKingID2PieceID[1 - bIsRed]];

	nFoeKingPosCheckNum = m_nBRKingPosCheckNum[1 - bIsRed][nFoeKingP];
	
	nFoeKingI = m_CGN2Pos[nFoeKingP].x;
	nFoeKingJ = m_CGN2Pos[nFoeKingP].y;

	//炮
	nCannonPos1 = m_nPiecePos[m_nChessID2PieceID[nCannon][0]];
	nCannonPos2 = m_nPiecePos[m_nChessID2PieceID[nCannon][1]];

	nFoeKingBitFile = m_nBitFile[nFoeKingJ];
	nFoeKingBitRank = m_nBitRank[nFoeKingI];

	return m_bIsRookFilePreCheck[bIsRed][nCannonPos1][nFoeKingPosCheckNum][nFoeKingBitFile] ||	//车行1
		   m_bIsRookFilePreCheck[bIsRed][nCannonPos2][nFoeKingPosCheckNum][nFoeKingBitFile] ||	//车行2
		   m_bIsRookRankPreCheck[bIsRed][nCannonPos1][nFoeKingPosCheckNum][nFoeKingBitRank] ||	//车列1
		   m_bIsRookRankPreCheck[bIsRed][nCannonPos2][nFoeKingPosCheckNum][nFoeKingBitRank];		//车列2
}
/*
inline FSINT CSearchEngine::GenRookMoves(FSINT p, CHESSMOVE* pMoveList)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);

	FSINT i, j;
	FSINT nCount;
	FSINT nRet;
	FSINT nRookCheckNum;//车校验序号
	FSINT nRookFoeCheckNum;
	PRECHESSMOVE pcmFile;
	PRECHESSMOVE pcmRank;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;
	
	//行
	nRookCheckNum = m_nRookFileRankCheckNum[i][m_nBitFile[j]];
	nRookFoeCheckNum = m_nRookFoeFileRankCheckNum[i][nRookCheckNum][m_nBRBitFile[1 - m_bIsRed][j]];
	pcmFile = m_pcmRookFilePreMoves[p][nRookCheckNum][nRookFoeCheckNum];
	nCount = pcmFile.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= 8);
	memcpy(pMoveList, pcmFile.pMoveList, m_nBytes[nCount]);
	nRet = nCount;

	//列
	nRookCheckNum = m_nRookFileRankCheckNum[j][m_nBitRank[i]];
	nRookFoeCheckNum = m_nRookFoeFileRankCheckNum[j][nRookCheckNum][m_nBRBitRank[1 - m_bIsRed][i]];
	pcmRank = m_pcmRookRankPreMoves[p][nRookCheckNum][nRookFoeCheckNum];
	nCount = pcmRank.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= 9);
	memcpy(pMoveList + nRet, pcmRank.pMoveList, m_nBytes[nCount]);
	nRet += nCount;

	return nRet;
}

inline FSINT CSearchEngine::GenKnightMoves(FSINT p, CHESSMOVE* pMoveList)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);

	FSINT i, j;
	FSINT nCount;
	FSINT nRet;
	FSINT nKnightLegCheckSum;
	FSINT nKnightFile0AroundCheckSum;
	FSINT nKnightFile1AroundCheckSum;
	FSINT nKnightFile2AroundCheckSum;
	FSINT nKnightFile3AroundCheckSum;
	FSINT nKnightFileAroundCheckSum;

	FSINT nKnightFoeFile0AroundCheckSum;
	FSINT nKnightFoeFile1AroundCheckSum;
	FSINT nKnightFoeFile2AroundCheckSum;
	FSINT nKnightFoeFile3AroundCheckSum;
	FSINT nKnightFoeFileAroundCheckSum;

	PRECHESSMOVE pcm;
	
	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;
	
	nKnightFile0AroundCheckSum = m_nFileRankAroundCheckSum[i][m_nBitFile[m_nAbs[j][2]]];
	nKnightFile1AroundCheckSum = m_nFileRankAroundCheckSum2[i][m_nBitFile[m_nAbs[j][1]]];
	nKnightFile2AroundCheckSum = m_nFileRankAroundCheckSum2[i][m_nBitFile[j + 1]];
	nKnightFile3AroundCheckSum = m_nFileRankAroundCheckSum[i][m_nBitFile[j + 2]];

	nKnightFoeFile0AroundCheckSum = m_nFileRankAroundCheckSum[i][m_nBRBitFile[1 - m_bIsRed][m_nAbs[j][2]]];
	nKnightFoeFile1AroundCheckSum = m_nFileRankAroundCheckSum2[i][m_nBRBitFile[1 - m_bIsRed][m_nAbs[j][1]]];
	nKnightFoeFile2AroundCheckSum = m_nFileRankAroundCheckSum2[i][m_nBRBitFile[1 - m_bIsRed][j + 1]];
	nKnightFoeFile3AroundCheckSum = m_nFileRankAroundCheckSum[i][m_nBRBitFile[1 - m_bIsRed][j + 2]];

	nKnightFileAroundCheckSum = 
		m_nKnightAroundCheckSum[m_nAroundCheckSum[nKnightFile0AroundCheckSum][nKnightFile1AroundCheckSum]][m_nAroundCheckSum[nKnightFile2AroundCheckSum][nKnightFile3AroundCheckSum]];
	nKnightFoeFileAroundCheckSum = 
		m_nKnightAroundCheckSum[m_nAroundCheckSum[nKnightFoeFile0AroundCheckSum][nKnightFoeFile1AroundCheckSum]][m_nAroundCheckSum[nKnightFoeFile2AroundCheckSum][nKnightFoeFile3AroundCheckSum]];
	nKnightLegCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBitFile[j]]][m_nFileRankAroundCheckSum[j][m_nBitRank[i]]];
	
	//不吃子走法
	pcm = m_pcmKnightNoCapPreMoves[p][nKnightFileAroundCheckSum][nKnightLegCheckSum];
	nCount = pcm.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= 8);
	memcpy(pMoveList, pcm.pMoveList, m_nBytes[nCount]);
	nRet = nCount;

	//吃子走法
	pcm = m_pcmKnightCapPreMoves[p][nKnightFoeFileAroundCheckSum][nKnightLegCheckSum];
	nCount = pcm.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= 8);
	memcpy(pMoveList + nRet, pcm.pMoveList, m_nBytes[nCount]);
	nRet += nCount;

	return nRet;
}

inline FSINT CSearchEngine::GenCannonMoves(FSINT p, CHESSMOVE* pMoveList)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);

	FSINT i, j;
	FSINT nCount;
	FSINT nRet;
	FSINT nCannonCheckNum;
	FSINT nCannonFoeCheckNum;
	PRECHESSMOVE pcmFile;
	PRECHESSMOVE pcmRank;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	//行
	nCannonCheckNum = m_nCannonFileRankCheckNum[i][m_nBitFile[j]];
	nCannonFoeCheckNum =
		m_nCannonFoeFileRankCheckNum[i][nCannonCheckNum][m_nBRBitFile[1 - m_bIsRed][j]];
	pcmFile = m_pcmCannonFilePreMoves[p][nCannonCheckNum][nCannonFoeCheckNum];
	nCount = pcmFile.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= 8);
	memcpy(pMoveList, pcmFile.pMoveList, m_nBytes[nCount]);
	nRet = nCount;

	//列
	nCannonCheckNum = m_nCannonFileRankCheckNum[j][m_nBitRank[i]];
	nCannonFoeCheckNum = 
		m_nCannonFoeFileRankCheckNum[j][nCannonCheckNum][m_nBRBitRank[1 - m_bIsRed][i]];
	pcmRank = m_pcmCannonRankPreMoves[p][nCannonCheckNum][nCannonFoeCheckNum];
	nCount = pcmRank.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= 9);
	memcpy(pMoveList + nRet, pcmRank.pMoveList, m_nBytes[nCount]);
	nRet += nCount;

	return nRet;
}

inline FSINT CSearchEngine::GenKingMoves(FSINT p, CHESSMOVE* pMoveList)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);

	FSINT i, j;
	FSINT nCount;
	FSINT nPosCheckNum;
	FSINT nAroundCheckSum;
	FSINT nFoeAroundCheckSum;
	PRECHESSMOVE pcm;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	nPosCheckNum = m_nKingPosCheckNum[p];
	DEBUGASSERT(nPosCheckNum >= MINKINGPOSCHECKNUM && nPosCheckNum <= MAXKINGPOSCHECKNUM);
	nAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBitFile[j]]][m_nFileRankAroundCheckSum[j][m_nBitRank[i]]];
	nFoeAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBRBitFile[1 - m_bIsRed][j]]][m_nFileRankAroundCheckSum[j][m_nBRBitRank[1 - m_bIsRed][i]]];
	
	pcm = m_pcmKingPreMoves[nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum];
	nCount = pcm.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= 4);
	memcpy(pMoveList + nCount, pcm.pMoveList, m_nBytes[nCount]);

	return nCount;
}

inline FSINT CSearchEngine::GenAdvisorMoves(FSINT p, CHESSMOVE* pMoveList)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);

	FSINT i, j;
	FSINT nCount;
	FSINT nPosCheckNum;
	FSINT nAroundCheckSum;
	FSINT nFoeAroundCheckSum;
	PRECHESSMOVE pcm;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	nPosCheckNum = m_nAdvisorPosCheckNum[p];
	nAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBitFile[m_nAbs[j][1]]]][m_nFileRankAroundCheckSum[i][m_nBitFile[j + 1]]];
	nFoeAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBRBitFile[1 - m_bIsRed][m_nAbs[j][1]]]][m_nFileRankAroundCheckSum[i][m_nBRBitFile[1 - m_bIsRed][j + 1]]];
	
	pcm = m_pcmAdvisorPreMoves[nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum];
	nCount = pcm.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= 4);
	memcpy(pMoveList + nCount, pcm.pMoveList, m_nBytes[nCount]);

	return nCount;
}

inline FSINT CSearchEngine::GenBishopMoves(FSINT p, CHESSMOVE* pMoveList)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);

	FSINT i, j;
	FSINT nCount;
	FSINT nPosCheckNum;
	FSINT nElephantEyeCheckSum;
	FSINT nAroundCheckSum;
	FSINT nFoeAroundCheckSum;
	PRECHESSMOVE pcm;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	nPosCheckNum = m_nBishopPosCheckNum[p];
	nAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum2[i][m_nBitFile[m_nAbs[j][2]]]][m_nFileRankAroundCheckSum2[i][m_nBitFile[j + 2]]];
	nFoeAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum2[i][m_nBRBitFile[1 - m_bIsRed][m_nAbs[j][2]]]][m_nFileRankAroundCheckSum2[i][m_nBRBitFile[1 - m_bIsRed][j + 2]]];
	nElephantEyeCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBitFile[m_nAbs[j][1]]]][m_nFileRankAroundCheckSum[i][m_nBitFile[j + 1]]];

	pcm = 
		m_pcmBishopPreMoves[nPosCheckNum][nElephantEyeCheckSum][nAroundCheckSum][nFoeAroundCheckSum];
	nCount = pcm.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= 4);
	memcpy(pMoveList + nCount, pcm.pMoveList, m_nBytes[nCount]);

	return nCount;
}

inline FSINT CSearchEngine::GenPawnMoves(FSINT p, CHESSMOVE* pMoveList)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);

	FSINT i, j;
	FSINT nCount;
	FSINT nPosCheckNum;
	FSINT nAroundCheckSum;
	FSINT nFoeAroundCheckSum;
	PRECHESSMOVE pcm;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	nPosCheckNum = m_nPawnPosCheckNum[m_bIsRed][p];
	nAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBitFile[j]]][m_nPawnRankAroundCheckSum[m_bIsRed][j][m_nBitRank[i]]];
	nFoeAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBRBitFile[1 - m_bIsRed][j]]][m_nPawnRankAroundCheckSum[m_bIsRed][j][m_nBRBitRank[1 - m_bIsRed][i]]];

	pcm = m_PawnPreMoves[m_bIsRed][nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum];
	nCount = pcm.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= MAXPAWNCAPMOVECNT);
	memcpy(pMoveList, pcm.pMoveList, m_nBytes[nCount]);

	return nCount;
}

inline FSINT CSearchEngine::GenRookMoves(FSINT p)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);

	FSINT i, j;
	FSINT nCount;
	FSINT nRet;
	FSINT nRookCheckNum;//车校验序号
	FSINT nRookFoeCheckNum;
	PRECHESSMOVE pcmFile;
	PRECHESSMOVE pcmRank;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;
	
	//行
	nRookCheckNum = m_nRookFileRankCheckNum[i][m_nBitFile[j]];
	nRookFoeCheckNum = m_nRookFoeFileRankCheckNum[i][nRookCheckNum][m_nBRBitFile[1 - m_bIsRed][j]];
	pcmFile = m_pcmRookFilePreMoves[p][nRookCheckNum][nRookFoeCheckNum];
	nCount = pcmFile.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= 8);
	nRet = nCount;

	//列
	nRookCheckNum = m_nRookFileRankCheckNum[j][m_nBitRank[i]];
	nRookFoeCheckNum = m_nRookFoeFileRankCheckNum[j][nRookCheckNum][m_nBRBitRank[1 - m_bIsRed][i]];
	pcmRank = m_pcmRookRankPreMoves[p][nRookCheckNum][nRookFoeCheckNum];
	nCount = pcmRank.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= 9);
	nRet += nCount;

	return nRet;
}

inline FSINT CSearchEngine::GenKnightMoves(FSINT p)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);

	FSINT i, j;
	FSINT nCount;
	FSINT nRet;
	FSINT nKnightLegCheckSum;
	FSINT nKnightFile0AroundCheckSum;
	FSINT nKnightFile1AroundCheckSum;
	FSINT nKnightFile2AroundCheckSum;
	FSINT nKnightFile3AroundCheckSum;
	FSINT nKnightFileAroundCheckSum;

	FSINT nKnightFoeFile0AroundCheckSum;
	FSINT nKnightFoeFile1AroundCheckSum;
	FSINT nKnightFoeFile2AroundCheckSum;
	FSINT nKnightFoeFile3AroundCheckSum;
	FSINT nKnightFoeFileAroundCheckSum;

	PRECHESSMOVE pcm;
	
	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;
	
	nKnightFile0AroundCheckSum = m_nFileRankAroundCheckSum[i][m_nBitFile[m_nAbs[j][2]]];
	nKnightFile1AroundCheckSum = m_nFileRankAroundCheckSum2[i][m_nBitFile[m_nAbs[j][1]]];
	nKnightFile2AroundCheckSum = m_nFileRankAroundCheckSum2[i][m_nBitFile[j + 1]];
	nKnightFile3AroundCheckSum = m_nFileRankAroundCheckSum[i][m_nBitFile[j + 2]];

	nKnightFoeFile0AroundCheckSum = m_nFileRankAroundCheckSum[i][m_nBRBitFile[1 - m_bIsRed][m_nAbs[j][2]]];
	nKnightFoeFile1AroundCheckSum = m_nFileRankAroundCheckSum2[i][m_nBRBitFile[1 - m_bIsRed][m_nAbs[j][1]]];
	nKnightFoeFile2AroundCheckSum = m_nFileRankAroundCheckSum2[i][m_nBRBitFile[1 - m_bIsRed][j + 1]];
	nKnightFoeFile3AroundCheckSum = m_nFileRankAroundCheckSum[i][m_nBRBitFile[1 - m_bIsRed][j + 2]];

	nKnightFileAroundCheckSum = 
		m_nKnightAroundCheckSum[m_nAroundCheckSum[nKnightFile0AroundCheckSum][nKnightFile1AroundCheckSum]][m_nAroundCheckSum[nKnightFile2AroundCheckSum][nKnightFile3AroundCheckSum]];
	nKnightFoeFileAroundCheckSum = 
		m_nKnightAroundCheckSum[m_nAroundCheckSum[nKnightFoeFile0AroundCheckSum][nKnightFoeFile1AroundCheckSum]][m_nAroundCheckSum[nKnightFoeFile2AroundCheckSum][nKnightFoeFile3AroundCheckSum]];
	nKnightLegCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBitFile[j]]][m_nFileRankAroundCheckSum[j][m_nBitRank[i]]];
	
	//不吃子走法
	pcm = m_pcmKnightNoCapPreMoves[p][nKnightFileAroundCheckSum][nKnightLegCheckSum];
	nCount = pcm.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= 8);
	nRet = nCount;

	//吃子走法
	pcm = m_pcmKnightCapPreMoves[p][nKnightFoeFileAroundCheckSum][nKnightLegCheckSum];
	nCount = pcm.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= 8);
	nRet += nCount;

	return nRet;
}

inline FSINT CSearchEngine::GenCannonMoves(FSINT p)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);

	FSINT i, j;
	FSINT nCount;
	FSINT nRet;
	FSINT nCannonCheckNum;
	FSINT nCannonFoeCheckNum;
	PRECHESSMOVE pcmFile;
	PRECHESSMOVE pcmRank;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	//行
	nCannonCheckNum = m_nCannonFileRankCheckNum[i][m_nBitFile[j]];
	nCannonFoeCheckNum =
		m_nCannonFoeFileRankCheckNum[i][nCannonCheckNum][m_nBRBitFile[1 - m_bIsRed][j]];
	pcmFile = m_pcmCannonFilePreMoves[p][nCannonCheckNum][nCannonFoeCheckNum];
	nCount = pcmFile.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= 8);
	nRet = nCount;

	//列
	nCannonCheckNum = m_nCannonFileRankCheckNum[j][m_nBitRank[i]];
	nCannonFoeCheckNum = 
		m_nCannonFoeFileRankCheckNum[j][nCannonCheckNum][m_nBRBitRank[1 - m_bIsRed][i]];
	pcmRank = m_pcmCannonRankPreMoves[p][nCannonCheckNum][nCannonFoeCheckNum];
	nCount = pcmRank.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= 9);
	nRet += nCount;

	return nRet;
}

inline FSINT CSearchEngine::GenKingMoves(FSINT p)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);

	FSINT i, j;
	FSINT nPosCheckNum;
	FSINT nAroundCheckSum;
	FSINT nFoeAroundCheckSum;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	nPosCheckNum = m_nKingPosCheckNum[p];
	DEBUGASSERT(nPosCheckNum >= MINKINGPOSCHECKNUM && nPosCheckNum <= MAXKINGPOSCHECKNUM);
	nAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBitFile[j]]][m_nFileRankAroundCheckSum[j][m_nBitRank[i]]];
	nFoeAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBRBitFile[1 - m_bIsRed][j]]][m_nFileRankAroundCheckSum[j][m_nBRBitRank[1 - m_bIsRed][i]]];
	
	return m_pcmKingPreMoves[nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].nCount;
}

inline FSINT CSearchEngine::GenAdvisorMoves(FSINT p)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);

	FSINT i, j;
	FSINT nPosCheckNum;
	FSINT nAroundCheckSum;
	FSINT nFoeAroundCheckSum;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	nPosCheckNum = m_nAdvisorPosCheckNum[p];
	nAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBitFile[m_nAbs[j][1]]]][m_nFileRankAroundCheckSum[i][m_nBitFile[j + 1]]];
	nFoeAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBRBitFile[1 - m_bIsRed][m_nAbs[j][1]]]][m_nFileRankAroundCheckSum[i][m_nBRBitFile[1 - m_bIsRed][j + 1]]];
	
	return m_pcmAdvisorPreMoves[nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].nCount;
}

inline FSINT CSearchEngine::GenBishopMoves(FSINT p)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);

	FSINT i, j;
	FSINT nPosCheckNum;
	FSINT nElephantEyeCheckSum;
	FSINT nAroundCheckSum;
	FSINT nFoeAroundCheckSum;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	nPosCheckNum = m_nBishopPosCheckNum[p];
	nAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum2[i][m_nBitFile[m_nAbs[j][2]]]][m_nFileRankAroundCheckSum2[i][m_nBitFile[j + 2]]];
	nFoeAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum2[i][m_nBRBitFile[1 - m_bIsRed][m_nAbs[j][2]]]][m_nFileRankAroundCheckSum2[i][m_nBRBitFile[1 - m_bIsRed][j + 2]]];
	nElephantEyeCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBitFile[m_nAbs[j][1]]]][m_nFileRankAroundCheckSum[i][m_nBitFile[j + 1]]];

	return m_pcmBishopPreMoves[nPosCheckNum][nElephantEyeCheckSum][nAroundCheckSum][nFoeAroundCheckSum].nCount;
}

inline FSINT CSearchEngine::GenPawnMoves(FSINT p)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);

	FSINT i, j;
	FSINT nPosCheckNum;
	FSINT nAroundCheckSum;
	FSINT nFoeAroundCheckSum;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	nPosCheckNum = m_nPawnPosCheckNum[m_bIsRed][p];
	nAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBitFile[j]]][m_nPawnRankAroundCheckSum[m_bIsRed][j][m_nBitRank[i]]];
	nFoeAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBRBitFile[1 - m_bIsRed][j]]][m_nPawnRankAroundCheckSum[m_bIsRed][j][m_nBRBitRank[1 - m_bIsRed][i]]];

	return m_PawnPreMoves[m_bIsRed][nPosCheckNum][nAroundCheckSum][nFoeAroundCheckSum].nCount;
}

inline FSINT CSearchEngine::GenRookCapMoves(FSINT p)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);

	FSINT i, j;
	FSINT nCount;
	FSINT nRet;
	FSINT nRookCheckNum;//车校验序号
	FSINT nRookFoeCheckNum;
	PRECHESSMOVE pcmFile;
	PRECHESSMOVE pcmRank;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;
	
	//行
	nRookCheckNum = m_nRookFileRankCheckNum[i][m_nBitFile[j]];
	nRookFoeCheckNum = m_nRookFoeFileRankCheckNum[i][nRookCheckNum][m_nBRBitFile[1 - m_bIsRed][j]];
	pcmFile = m_pcmRookFileCapPreMoves[p][nRookCheckNum][nRookFoeCheckNum];
	nCount = pcmFile.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= MAXROOKFILECAPMOVECNT);
	nRet = nCount;

	//列
	nRookCheckNum = m_nRookFileRankCheckNum[j][m_nBitRank[i]];
	nRookFoeCheckNum = m_nRookFoeFileRankCheckNum[j][nRookCheckNum][m_nBRBitRank[1 - m_bIsRed][i]];
	pcmRank = m_pcmRookRankCapPreMoves[p][nRookCheckNum][nRookFoeCheckNum];
	nCount = pcmRank.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= MAXROOKRANKCAPMOVECNT);
	nRet += nCount;

	return nRet;
}
/*
inline FSINT CSearchEngine::GenKnightCapMoves(FSINT p)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);

	FSINT i, j;
	FSINT nKnightLegCheckSum;

	FSINT nKnightFoeFile0AroundCheckSum;
	FSINT nKnightFoeFile1AroundCheckSum;
	FSINT nKnightFoeFile2AroundCheckSum;
	FSINT nKnightFoeFile3AroundCheckSum;
	FSINT nKnightFoeFileAroundCheckSum;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	nKnightFoeFile0AroundCheckSum = m_nFileRankAroundCheckSum[i][m_nBRBitFile[1 - m_bIsRed][m_nAbs[j][2]]];
	nKnightFoeFile1AroundCheckSum = m_nFileRankAroundCheckSum2[i][m_nBRBitFile[1 - m_bIsRed][m_nAbs[j][1]]];
	nKnightFoeFile2AroundCheckSum = m_nFileRankAroundCheckSum2[i][m_nBRBitFile[1 - m_bIsRed][j + 1]];
	nKnightFoeFile3AroundCheckSum = m_nFileRankAroundCheckSum[i][m_nBRBitFile[1 - m_bIsRed][j + 2]];

	nKnightFoeFileAroundCheckSum = 
		m_nKnightAroundCheckSum[m_nAroundCheckSum[nKnightFoeFile0AroundCheckSum][nKnightFoeFile1AroundCheckSum]][m_nAroundCheckSum[nKnightFoeFile2AroundCheckSum][nKnightFoeFile3AroundCheckSum]];
	nKnightLegCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBitFile[j]]][m_nFileRankAroundCheckSum[j][m_nBitRank[i]]];

	//吃子走法
	return m_pcmKnightCapPreMoves[p][nKnightFoeFileAroundCheckSum][nKnightLegCheckSum].nCount;
}*/
/*
inline FSINT CSearchEngine::GenCannonCapMoves(FSINT p)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);

	FSINT i, j;
	FSINT nCount;
	FSINT nRet;
	FSINT nCannonCheckNum;
	FSINT nCannonFoeCheckNum;
	PRECHESSMOVE pcmFile;
	PRECHESSMOVE pcmRank;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	//行
	nCannonCheckNum = m_nCannonFileRankCheckNum[i][m_nBitFile[j]];
	nCannonFoeCheckNum =
		m_nCannonFoeFileRankCheckNum[i][nCannonCheckNum][m_nBRBitFile[1 - m_bIsRed][j]];
	pcmFile = m_pcmCannonFileCapPreMoves[p][nCannonCheckNum][nCannonFoeCheckNum];
	nCount = pcmFile.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= 2);
	nRet= nCount;

	//列
	nCannonCheckNum = m_nCannonFileRankCheckNum[j][m_nBitRank[i]];
	nCannonFoeCheckNum = 
		m_nCannonFoeFileRankCheckNum[j][nCannonCheckNum][m_nBRBitRank[1 - m_bIsRed][i]];
	pcmRank = m_pcmCannonRankCapPreMoves[p][nCannonCheckNum][nCannonFoeCheckNum];
	nCount = pcmRank.nCount;
	DEBUGASSERT(nCount >= 0 && nCount <= 2);
	nRet += nCount;

	return nRet;
}

inline FSINT CSearchEngine::GenKingCapMoves(FSINT p)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);

	FSINT i, j;
	FSINT nPosCheckNum;
	FSINT nFoeAroundCheckSum;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	nPosCheckNum = m_nKingPosCheckNum[p];
	nFoeAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBRBitFile[1 - m_bIsRed][j]]][m_nFileRankAroundCheckSum[j][m_nBRBitRank[1 - m_bIsRed][i]]];
	
	return m_pcmKingCapPreMoves[nPosCheckNum][nFoeAroundCheckSum].nCount;
}

inline FSINT CSearchEngine::GenAdvisorCapMoves(FSINT p)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);

	FSINT i, j;
	FSINT nPosCheckNum;
	FSINT nFoeAroundCheckSum;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	nPosCheckNum = m_nAdvisorPosCheckNum[p];
	DEBUGASSERT(nPosCheckNum >= 0);
	nFoeAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBRBitFile[1 - m_bIsRed][m_nAbs[j][1]]]][m_nFileRankAroundCheckSum[i][m_nBRBitFile[1 - m_bIsRed][j + 1]]];
	
	return m_pcmAdvisorCapPreMoves[nPosCheckNum][nFoeAroundCheckSum].nCount;
}

inline FSINT CSearchEngine::GenBishopCapMoves(FSINT p)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);

	FSINT i, j;
	FSINT nPosCheckNum;
	FSINT nElephantEyeCheckSum;
	FSINT nFoeAroundCheckSum;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	nPosCheckNum = m_nBishopPosCheckNum[p];
	nFoeAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum2[i][m_nBRBitFile[1 - m_bIsRed][m_nAbs[j][2]]]][m_nFileRankAroundCheckSum2[i][m_nBRBitFile[1 - m_bIsRed][j + 2]]];
	nElephantEyeCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBitFile[m_nAbs[j][1]]]][m_nFileRankAroundCheckSum[i][m_nBitFile[j + 1]]];

	return m_pcmBishopCapPreMoves[nPosCheckNum][nElephantEyeCheckSum][nFoeAroundCheckSum].nCount;
}

inline FSINT CSearchEngine::GenPawnCapMoves(FSINT p)
{
	DEBUGASSERT(p >= 0 && p < MAXCHESSGRIDNUM);

	FSINT i, j;
	FSINT nPosCheckNum;
	FSINT nFoeAroundCheckSum;

	i = m_CGN2Pos[p].x;
	j = m_CGN2Pos[p].y;

	nPosCheckNum = m_nPawnPosCheckNum[m_bIsRed][p];
	nFoeAroundCheckSum = 
		m_nAroundCheckSum[m_nFileRankAroundCheckSum[i][m_nBRBitFile[1 - m_bIsRed][j]]][m_nPawnRankAroundCheckSum[m_bIsRed][j][m_nBRBitRank[1 - m_bIsRed][i]]];

	return m_PawnCapPreMoves[m_bIsRed][nPosCheckNum][nFoeAroundCheckSum].nCount;
}
*/
inline FSBOOL CSearchEngine::IsCheckMove(CHESSMOVE &cm)
{
	FSINT nToChessID;
	FSINT nToPieceID;
	FSBOOL bRet;

	MakeMove(cm, nToChessID, nToPieceID);
	bRet = IsChecked(1 - m_bIsRed);
	UnMakeMove(cm, nToChessID, nToPieceID);

	return bRet;
}

const char* CSearchEngine::GetSpecialFEN(char *str)
{
	if (strncmp(str, "opengamepos", 11) == 0)
	{
		return g_szOpenGamePos;
	}

	if (strncmp(str, "midgamepos", 10) == 0)
	{
		return g_szMidGamePos;
	}

	if (strncmp(str, "endgamepos", 10) == 0)
	{
		return g_szEndGamePos;
	}

	if (strncmp(str, "checkmatepos", 12) == 0)
	{
		return g_szCheckMatePos;
	}

	if (strncmp(str, "zugzwangpos", 12) == 0)
	{
		return g_szZugzwangPos;
	}

	if (strncmp(str, "detectnmpos", 11) == 0)
	{
		return g_szDetectNMPos;
	}

	if (strncmp(str, "knightvssingleadvisor", 21) == 0)
	{
		return g_szKnightVSSglAdvisorPos;
	}

	if (strncmp(str, "fishformooninsea", 16) == 0)
	{
		return g_szFishforMoonInSeaPos;
	}

	if (strncmp(str, "repdetectpos", 12) == 0)
	{
		return g_szRepDetectPos;
	}

	return "nothatpos";
}