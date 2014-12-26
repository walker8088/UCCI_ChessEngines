// NegaScout_TT_HH.h: interface for the CNegaScout_TT_HH2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEST_H__7A81FEF8_312B_4BF8_A7F8_A303A79EC1BB__INCLUDED_)
#define AFX_TEST_H__7A81FEF8_312B_4BF8_A7F8_A303A79EC1BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseSearchEngine.h"
#include "TranspositionTable.h"
#include "HistoryHeuristic.h"
#include <algorithm>

//使用了置换表和历史启发的NegaScout搜索引擎
class CNegaScout_TT_HH2:public CTranspositionTable,public CHistoryHeuristic,public CBaseSearchEngine 
{
public:
	CNegaScout_TT_HH2();
	virtual ~CNegaScout_TT_HH2();

public:
	virtual  SearchAGoodMove(BYTE byChessBoard[10][9]);

protected:
	int NegaScout(int depth, int alpha, int beta);//NegaScout搜索函数
};

#endif // !defined(AFX_NEGASCOUT_TT_HH_H__DB5C0A53_E715_4037_A42B_B8D5704521B4__INCLUDED_)
