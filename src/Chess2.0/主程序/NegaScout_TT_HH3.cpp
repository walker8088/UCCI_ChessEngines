// NegaScout_TT_HH.cpp: implementation of the CNegaScout_TT_HH3 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NegaScout_TT_HH3.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNegaScout_TT_HH3::CNegaScout_TT_HH3()
{
	
}

CNegaScout_TT_HH3::~CNegaScout_TT_HH3()
{

}

CNegaScout_TT_HH3::SearchAGoodMove(BYTE byChessBoard[][9])
{
	CHESSMOVE cmBackupBM;

	memcpy(byCurChessBoard,byChessBoard,90);//传入棋盘

	CalculateInitHashKey(byCurChessBoard);
	ResetHistoryTable();
	m_iTimeCount=GetTickCount();

	m_pThinkProgress->SetRange(0,MAX_SEARCHDEPTH);
	m_pThinkProgress->SetStep(1);

	//迭代深化搜索
	for(m_iMaxDepth=1;m_iMaxDepth<=MAX_SEARCHDEPTH;m_iMaxDepth++)
	{
		if(NegaScout(m_iMaxDepth,-20000,20000)!=66666)//如果本次搜索没有被中止，保存最佳走法
			cmBackupBM=m_cmBestMove;
		else
			break;

		m_pThinkProgress->StepIt();//走进度条
	}
	m_iCanSearchDepth=m_iMaxDepth-1;

	m_cmBestMove=cmBackupBM;
	m_umUndoMove.cmChessMove=m_cmBestMove;
	m_umUndoMove.nChessID=MakeMove(&m_cmBestMove);	

	CBANDMOVE camTemp;
	memcpy(camTemp.byChessBoard,byCurChessBoard,90);
	camTemp.cm=m_cmBestMove;
	m_vectorRedCbAndMove.push_back(camTemp);

	memcpy(byChessBoard,byCurChessBoard,90);//传出棋盘
	m_pThinkProgress->SetPos(0);
}

int CNegaScout_TT_HH3::NegaScout(int iDepth, int iAlpha, int iBeta)
{
	if(GetTickCount()-m_iTimeCount>=m_fTimeLimit)
		return 66666;//返回中止标记

	int iCount;
	BYTE byChess;
	int iSide;
	int iScore;
	int i;
	
	iSide=(m_iMaxDepth-iDepth)%2;//计算当前节点的类型，极大0/极小1

	iCount=m_pMG->CreatePossibleMove(byCurChessBoard,iDepth,iSide,m_nUserChessColor);
	for(i=0;i<iCount;i++)
	{
		Hash_MakeMove(&m_pMG->m_MoveList[iDepth][i],byCurChessBoard);
		byChess=MakeMove(&m_pMG->m_MoveList[iDepth][i]);
		iScore=-SubNegaScout(iDepth-1,-iBeta,-iAlpha);
		if(iScore==-66666)
		{
			Hash_UnMakeMove(&m_pMG->m_MoveList[iDepth][i],byChess,byCurChessBoard);
			UnMakeMove(&m_pMG->m_MoveList[iDepth][i],byChess);
			return 66666;
		}
		m_pMG->m_MoveList[iDepth][i].iScore=iScore;
		Hash_UnMakeMove(&m_pMG->m_MoveList[iDepth][i],byChess,byCurChessBoard);
		UnMakeMove(&m_pMG->m_MoveList[iDepth][i],byChess);
	}

	//根据得分对走法排序
	sort(m_pMG->m_MoveList[iDepth],m_pMG->m_MoveList[iDepth]+iCount-1,UDgreater);
	
	CBANDMOVE camTemp;
	//选择走法,避免长将,长捉
	for(i=0;i<iCount;i++)
	{
		byChess=MakeMove(&m_pMG->m_MoveList[iDepth][i]);
		memcpy(camTemp.byChessBoard,byCurChessBoard,90);
		camTemp.cm=m_pMG->m_MoveList[iDepth][i];
		UnMakeMove(&m_pMG->m_MoveList[iDepth][i],byChess);
		if(IsObeyRule(byCurChessBoard,iDepth,camTemp,1)==CR_OBEY && !BadForMe(byCurChessBoard,(m_iMaxDepth-iDepth)%2))
		{
			m_cmBestMove=m_pMG->m_MoveList[iDepth][i];
			m_vectorEvel.push_back(m_iEvel=m_pEval->Eveluate(byCurChessBoard,(m_iMaxDepth-iDepth)%2));
			break;
		}
	}
	
	//按概率选择走法,且避免长捉和长将
//	m_cmBestMove=m_pMG->m_MoveList[iDepth][SelMove(vectorScore,iMaxScore)];
}

int CNegaScout_TT_HH3::SubNegaScout(int iDepth, int iAlpha, int iBeta)
{
	int iCount,iGameOverRet;
	BYTE byChess;
	int a,b,t;
	int iSide;
	int score;
	int i;
	
	iGameOverRet=IsGameOver(byCurChessBoard,iDepth);
	if(iGameOverRet)
		return iGameOverRet;
	
	iSide=(m_iMaxDepth-iDepth)%2;//计算当前节点的类型，极大0/极小1
	
	score=LookUpHashTable(iAlpha,iBeta,iDepth,iSide);
	if(score!=66666)
		return score;
	
	if(iDepth<=0)//叶子节点取估值
	{
		score=m_pEval->Eveluate(byCurChessBoard,iSide);
		EnterHashTable(Exact,score,iDepth,iSide);//将估值存入置换表
		return score;
	}
	
	iCount=m_pMG->CreatePossibleMove(byCurChessBoard,iDepth,iSide,m_nUserChessColor);
	//此下if语句之内代码，用于调整底层节点排列顺序
	//如果某走法在上次迭代中是BestMove，将其排在第一位
	if(iDepth==m_iMaxDepth && m_iMaxDepth>1)
		for(i=1;i<iCount;i++)
		{
			if(m_pMG->m_MoveList[iDepth][i].From.x==m_cmBestMove.From.x
				&& m_pMG->m_MoveList[iDepth][i].From.y==m_cmBestMove.From.y
				&& m_pMG->m_MoveList[iDepth][i].To.x==m_cmBestMove.To.x	
				&& m_pMG->m_MoveList[iDepth][i].To.y==m_cmBestMove.To.y)	
			{
				//交换两个走法的位置
				m_pMG->m_MoveList[iDepth][i]=m_pMG->m_MoveList[iDepth][0];
				m_pMG->m_MoveList[iDepth][0]=m_cmBestMove;
			}
		}
	for(i=0;i<iCount;i++)
		m_pMG->m_MoveList[iDepth][i].iScore=GetHistoryScore(&m_pMG->m_MoveList[iDepth][i]);
	MergeSort(m_pMG->m_MoveList[iDepth],iCount,0);
	
	int iBestMove=-1;
	
    a=iAlpha;
    b=iBeta;

    int eval_is_exact=0;

    for(i=0;i<iCount;i++) 
	{
		if(GetTickCount()-m_iTimeCount>=m_fTimeLimit)
			return 66666;//返回中止标记

		Hash_MakeMove(&m_pMG->m_MoveList[iDepth][i],byCurChessBoard);
		byChess=MakeMove(&m_pMG->m_MoveList[iDepth][i]);
		
		t=-SubNegaScout(iDepth-1,-b,-a);
		
		if(t>a && t<iBeta && i>0)
		{
			//对于第一个后的节点，如果上面的搜索failhigh
			a=-SubNegaScout(iDepth-1,-iBeta,-t);//递归搜索子节点
			eval_is_exact=1;//设数据类型为精确值
			iBestMove=i;
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
	if(iBestMove!=-1)
		EnterHistoryScore(&m_pMG->m_MoveList[iDepth][iBestMove],iDepth);
	if(eval_is_exact)
		EnterHashTable(Exact,a,iDepth,iSide);
	else
		EnterHashTable(UpperBound,a,iDepth,iSide);

	return a;
}
