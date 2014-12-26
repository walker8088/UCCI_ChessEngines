// NegaScout_TT_HH.h: interface for the CNegaScout_TT_HH3 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEGASCOUT_TT_HH3_H__C38189ED_4494_44D2_8B9E_4AE9E0F7A8C1__INCLUDED_)
#define AFX_NEGASCOUT_TT_HH3_H__C38189ED_4494_44D2_8B9E_4AE9E0F7A8C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseSearchEngine.h"
#include "TranspositionTable.h"
#include "HistoryHeuristic.h"
#include <vector>
#include <algorithm>

using namespace std;

extern bool UDgreater(CHESSMOVE cm1, CHESSMOVE cm2);

//使用了置换表和历史启发的NegaScout搜索引擎
class CNegaScout_TT_HH3:public CTranspositionTable,public CHistoryHeuristic,public CBaseSearchEngine 
{
public:
	CNegaScout_TT_HH3();
	virtual ~CNegaScout_TT_HH3();

public:
	virtual  SearchAGoodMove(BYTE byChessBoard[10][9]);

protected:
	int NegaScout(int depth, int alpha, int beta);//NegaScout搜索函数
	int SubNegaScout(int iDepth,int iAlpha,int iBeta);
};

#endif // !defined(AFX_NEGASCOUT_TT_HH_H__DB5C0A53_E715_4037_A42B_B8D5704521B4__INCLUDED_)
