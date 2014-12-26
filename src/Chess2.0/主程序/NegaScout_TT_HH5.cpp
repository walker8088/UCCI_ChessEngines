// NegaScout_TT_HH.cpp: implementation of the CNegaScout_TT_HH5 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NegaScout_TT_HH5.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNegaScout_TT_HH5::CNegaScout_TT_HH5()
{

}

CNegaScout_TT_HH5::~CNegaScout_TT_HH5()
{

}

CNegaScout_TT_HH5::SearchAGoodMove(BYTE byChessBoard[][9])
{
	memcpy(byCurChessBoard,byChessBoard,90);//传入棋盘

	m_iMaxDepth=m_iSearchDepth;
	CalculateInitHashKey(byCurChessBoard);
	ResetHistoryTable();
	NegaScout(m_iMaxDepth,-20000,20000);
	m_umUndoMove.cmChessMove=m_cmBestMove;
	m_umUndoMove.nChessID=MakeMove(&m_cmBestMove);	

	memcpy(byChessBoard,byCurChessBoard,90);//传出棋盘
}

void CNegaScout_TT_HH5::NegaScout(int iDepth, int iAlpha, int iBeta)
{
	int iCount;
	BYTE byChess;
	int a,b,t;
	int iSide;
	int i;
	
	iSide=(m_iMaxDepth-iDepth)%2;//计算当前节点的类型，极大0/极小1
	
	iCount=m_pMG->CreatePossibleMove(byCurChessBoard,iDepth,iSide,m_nUserChessColor);

	m_pThinkProgress->SetRange(0,iCount);
	m_pThinkProgress->SetStep(1);

	int bestmove=-1;
	
    a=iAlpha;
    b=iBeta;

    int eval_is_exact=0;

    for(i=0;i<iCount;i++)
	{
		m_pThinkProgress->StepIt();//走进度条

		Hash_MakeMove(&m_pMG->m_MoveList[iDepth][i],byCurChessBoard);
		byChess=MakeMove(&m_pMG->m_MoveList[iDepth][i]);
		
		t=-SubNegaScout(iDepth-1,-b,-a);
		
		if(t>a && t<iBeta && i>0) 
		{
			//对于第一个后的节点，如果上面的搜索failhigh
			a=-SubNegaScout(iDepth-1,-iBeta,-t);//递归搜索子节点
			eval_is_exact=1;//设数据类型为精确值
			m_cmBestMove=m_pMG->m_MoveList[iDepth][i];
			bestmove=i;
		}
		Hash_UnMakeMove(&m_pMG->m_MoveList[iDepth][i],byChess,byCurChessBoard); 
		UnMakeMove(&m_pMG->m_MoveList[iDepth][i],byChess); 
		if(a<t)
		{
			eval_is_exact=1;
			a=t;
			m_cmBestMove=m_pMG->m_MoveList[iDepth][i];
		}
		b=a+1;//set new null window
	}
}

int CNegaScout_TT_HH5::SubNegaScout(int iDepth, int iAlpha, int iBeta)
{
	int iCount,iGameOver;
	BYTE byChess;
	BYTE byTempChess;
	int a,b,t;
	int iSide;
	int iScore;
	int i;
	
	iGameOver=IsGameOver(byCurChessBoard, iDepth);
	if(iGameOver!=0)
		return iGameOver;
	
	iSide=(m_iMaxDepth-iDepth)%2;//计算当前节点的类型，极大0/极小1
	
	iScore=LookUpHashTable(iAlpha,iBeta,iDepth,iSide);
	if(iScore!=66666)
		return iScore;
	
	if(!iDepth)//叶子节点取估值
	{
		iScore=m_pEval->Eveluate(byCurChessBoard,iSide);
		EnterHashTable(Exact,iScore,iDepth,iSide);//将估值存入置换表
		return iScore;
	}
	
	iCount=m_pMG->CreatePossibleMove(byCurChessBoard,iDepth,iSide,m_nUserChessColor);

	for(i=0;i<iCount;i++)
		m_pMG->m_MoveList[iDepth][i].iScore=GetHistoryScore(&m_pMG->m_MoveList[iDepth][i]);
	MergeSort(m_pMG->m_MoveList[iDepth],iCount,0);
	
	int bestmove=-1;
	
    a=iAlpha;
    b=iBeta;

    int eval_is_exact=0;

    for(i=0;i<iCount;i++)
	{
		Hash_MakeMove(&m_pMG->m_MoveList[iDepth][i],byCurChessBoard);
		byChess=MakeMove(&m_pMG->m_MoveList[iDepth][i]);
		//搜索6层后进行静止期搜索
		if(m_iMaxDepth-iDepth>=6)//搜索第7层时
		{
			byTempChess=m_pMG->m_MoveList[iDepth][i].nChessID;
//			if(byChess==NOCHESS)//无吃子时不继续搜索
//			if(byChess!=R_KING && byChess!=B_KING)//不将军时不继续搜索
			//作用较小的子不继续
			if(byTempChess==B_PAWN || byTempChess==R_PAWN || byTempChess==B_ELEPHANT || byTempChess==R_ELEPHANT || byTempChess==B_BISHOP || byTempChess==R_BISHOP)
			{
				Hash_UnMakeMove(&m_pMG->m_MoveList[iDepth][i],byChess,byCurChessBoard);
				UnMakeMove(&m_pMG->m_MoveList[iDepth][i],byChess);
				iScore=m_pEval->Eveluate(byCurChessBoard,iSide);
				EnterHashTable(Exact,iScore,iDepth,iSide);//将估值存入置换表
				return iScore;
			}
		}
		t=-SubNegaScout(iDepth-1,-b,-a);
		
		if(t>a && t<iBeta && i>0)
		{
			//对于第一个后的节点，如果上面的搜索failhigh
			a=-SubNegaScout(iDepth-1,-iBeta,-t);//递归搜索子节点
			eval_is_exact=1;//设数据类型为精确值
			bestmove=i;
		}
		Hash_UnMakeMove(&m_pMG->m_MoveList[iDepth][i],byChess,byCurChessBoard);
		UnMakeMove(&m_pMG->m_MoveList[iDepth][i],byChess);
		if(a<t)
		{
			eval_is_exact=1;
			a=t;
		}
		if(a>=iBeta)
		{
			EnterHashTable(LowerBound,a,iDepth,iSide);
			EnterHistoryScore(&m_pMG->m_MoveList[iDepth][i],iDepth);
			return a;
		}
		b=a+1;//set new null window
	}
	if(bestmove!=-1)
		EnterHistoryScore(&m_pMG->m_MoveList[iDepth][bestmove],iDepth);
	if(eval_is_exact)
		EnterHashTable(Exact,a,iDepth,iSide);
	else
		EnterHashTable(UpperBound,a,iDepth,iSide);

	return a;
}