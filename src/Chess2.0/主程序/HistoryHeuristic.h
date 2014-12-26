// HistoryHeuristic.h: interface for the CHistoryHeuristic class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HISTORYHEURISTIC_H__BE5BC30B_D7AF_44CD_9A3D_CD9E0AFEE37B__INCLUDED_)
#define AFX_HISTORYHEURISTIC_H__BE5BC30B_D7AF_44CD_9A3D_CD9E0AFEE37B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Define.h"

//历史启发类
class CHistoryHeuristic  
{
public:
	CHistoryHeuristic();
	virtual ~CHistoryHeuristic();

public:
	void MergeSort(CHESSMOVE* source, int n,BOOL direction);//对当前走法队列进行归并排序
	void EnterHistoryScore(CHESSMOVE *move,int depth);		//将某一最佳走法汇入历史记录表
	int GetHistoryScore(CHESSMOVE *move);					//取某一走法的历史得分
	void ResetHistoryTable();								//将历史记录表清空

protected:
	void Merge_A(CHESSMOVE* source,CHESSMOVE* target,int l,int m, int r);//从小到大排序，MergePass调用
	void MergePass(CHESSMOVE* source, CHESSMOVE* target,const int s, const int n, const BOOL direction);
																		 //用于合并排序好的相邻数据段，MergeSort调用
	void Merge(CHESSMOVE* source,CHESSMOVE* target,int l,int m, int r);  //从小到大排序，MergePass调用

protected:
	int m_HistoryTable[90][90]; //历史得分表
	CHESSMOVE m_TargetBuff[100];//排序用的缓冲队列
};

#endif // !defined(AFX_HISTORYHEURISTIC_H__BE5BC30B_D7AF_44CD_9A3D_CD9E0AFEE37B__INCLUDED_)
