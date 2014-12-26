// SearchEngine.cpp: implementation of the CBaseSearchEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Chess.h"
#include "BaseSearchEngine.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const BYTE byInitChessBoard[10][9]=
{
	{B_CAR,B_HORSE,B_ELEPHANT,B_BISHOP,B_KING,B_BISHOP,B_ELEPHANT,B_HORSE,B_CAR},
	{NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS},
	{NOCHESS,B_CANON,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,B_CANON,NOCHESS},
	{B_PAWN,NOCHESS,B_PAWN,NOCHESS,B_PAWN,NOCHESS,B_PAWN,NOCHESS,B_PAWN},
	{NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS},
	               //楚河                       汉界//
	{NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS},
	{R_PAWN,NOCHESS,R_PAWN,NOCHESS,R_PAWN,NOCHESS,R_PAWN,NOCHESS,R_PAWN},
	{NOCHESS,R_CANON,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,R_CANON,NOCHESS},
	{NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS},
	{R_CAR,R_HORSE,R_ELEPHANT,R_BISHOP,R_KING,R_BISHOP,R_ELEPHANT,R_HORSE,R_CAR}
};

CBaseSearchEngine::CBaseSearchEngine()
{
	m_iEvel=0;
	m_iCanSearchDepth=0;
	m_byAteChess=NOCHESS;
}

CBaseSearchEngine::~CBaseSearchEngine()
{
	delete m_pMG;
	delete m_pEval;
}

BYTE CBaseSearchEngine::MakeMove(CHESSMOVE* cm)
{
	BYTE nChessID;
	BYTE bySrcChess;
	int nSize;

	nChessID=byCurChessBoard[cm->To.y][cm->To.x];//取目标位置棋子
	bySrcChess=byCurChessBoard[cm->From.y][cm->From.x];

	//--------------更新过河棋子数目--------------
	if(nChessID!=NOCHESS)
	{
		if(IsRed(nChessID))
		{
			if(cm->To.y<=4)
			{
				m_pEval->m_uiCurBigChessCnt[nChessID]--;
				if(cm->To.x<=4)
					m_pEval->m_uiCurLBigChessCnt[nChessID]--;
				if(cm->To.x>=4)
					m_pEval->m_uiCurRBigChessCnt[nChessID]--;
			}
		}
		else
		{
			if(cm->To.y>=5)
			{
				m_pEval->m_uiCurBigChessCnt[nChessID]--;
				if(cm->To.x<=4)
					m_pEval->m_uiCurLBigChessCnt[nChessID]--;
				if(cm->To.x>=4)
					m_pEval->m_uiCurRBigChessCnt[nChessID]--;
			}
		}		
	}
	if(bySrcChess!=NOCHESS)
	{
		if(IsRed(bySrcChess))
		{
			if(cm->From.y<=4)
			{
				m_pEval->m_uiCurBigChessCnt[bySrcChess]--;
				
				if(cm->From.x<=4)
					m_pEval->m_uiCurLBigChessCnt[bySrcChess]--;
				if(cm->From.x>=4)
					m_pEval->m_uiCurRBigChessCnt[bySrcChess]--;
			}

			if(cm->To.y<=4)
			{
				m_pEval->m_uiCurBigChessCnt[bySrcChess]++;
				
				if(cm->To.x<=4)
					m_pEval->m_uiCurLBigChessCnt[bySrcChess]++;
				if(cm->To.x>=4)
					m_pEval->m_uiCurRBigChessCnt[bySrcChess]++;
			}
		}
		else
		{
			if(cm->From.y>=5)
			{
				m_pEval->m_uiCurBigChessCnt[bySrcChess]--;
				
				if(cm->From.x<=4)
					m_pEval->m_uiCurLBigChessCnt[bySrcChess]--;
				if(cm->From.x>=4)
					m_pEval->m_uiCurRBigChessCnt[bySrcChess]--;
			}

			if(cm->To.y>=5)
			{
				m_pEval->m_uiCurBigChessCnt[bySrcChess]++;
				
				if(cm->To.x<=4)
					m_pEval->m_uiCurLBigChessCnt[bySrcChess]++;
				if(cm->To.x>=4)
					m_pEval->m_uiCurRBigChessCnt[bySrcChess]++;
			}
		}				
	}
	//--------------------------------------------


	//--------------更新当前各种棋子位置栈--------------
	nSize=m_pEval->m_vectorCurChessPos[bySrcChess].size();
	for(int i=0;i<nSize;i++)
	{
		if(m_pEval->m_vectorCurChessPos[bySrcChess][i].x==cm->From.x && m_pEval->m_vectorCurChessPos[bySrcChess][i].y==cm->From.y)
		{
			m_pEval->m_vectorCurChessPos[bySrcChess][i].x=cm->To.y;
			m_pEval->m_vectorCurChessPos[bySrcChess][i].y=cm->To.x;
		}
	}
	if(nChessID!=NOCHESS)
		m_pEval->m_vectorCurChessPos[nChessID].pop_back();
	//--------------------------------------------------

    byCurChessBoard[cm->To.y][cm->To.x]=bySrcChess; //把棋子移动到目标位置
	byCurChessBoard[cm->From.y][cm->From.x]=NOCHESS;//将原位置清空
	
	return nChessID;//返回被吃掉的棋子
}

void CBaseSearchEngine::RedoChessMove(BYTE byChessBoard[][9],CHESSMOVE* cm)
{
    byChessBoard[cm->To.y][cm->To.x]=byChessBoard[cm->From.y][cm->From.x];
	byChessBoard[cm->From.y][cm->From.x]=NOCHESS;
}

void CBaseSearchEngine::UnMakeMove(CHESSMOVE* cm, BYTE nChessID)
{
	BYTE bySrcChess=byCurChessBoard[cm->To.y][cm->To.x];

	//更新过河棋子数目
	if(bySrcChess!=NOCHESS)
	{
		if(IsRed(bySrcChess))
		{
			if(cm->To.y<=4)
			{
				m_pEval->m_uiCurBigChessCnt[bySrcChess]--;
				
				if(cm->To.x<=4)
					m_pEval->m_uiCurLBigChessCnt[bySrcChess]--;
				if(cm->To.x>=4)
					m_pEval->m_uiCurRBigChessCnt[bySrcChess]--;
			}

			if(cm->From.y<=4)
			{
				m_pEval->m_uiCurBigChessCnt[bySrcChess]++;
				
				if(cm->From.x<=4)
					m_pEval->m_uiCurLBigChessCnt[bySrcChess]++;
				if(cm->From.x>=4)
					m_pEval->m_uiCurRBigChessCnt[bySrcChess]++;
			}
		}
		else
		{
			if(cm->To.y>=5)
			{
				m_pEval->m_uiCurBigChessCnt[bySrcChess]--;
				
				if(cm->To.x<=4)
					m_pEval->m_uiCurLBigChessCnt[bySrcChess]--;
				if(cm->To.x>=4)
					m_pEval->m_uiCurRBigChessCnt[bySrcChess]--;
			}

			if(cm->From.y>=5)
			{
				m_pEval->m_uiCurBigChessCnt[bySrcChess]++;

				if(cm->From.x<=4)
					m_pEval->m_uiCurLBigChessCnt[bySrcChess]++;
				if(cm->From.x>=4)
					m_pEval->m_uiCurRBigChessCnt[bySrcChess]++;
			}
		}				
	}
	if(nChessID!=NOCHESS)
	{
		if(IsRed(nChessID))
		{
			if(cm->To.y<=4)
			{
				m_pEval->m_uiCurBigChessCnt[nChessID]++;
				
				if(cm->To.x<=4)
					m_pEval->m_uiCurLBigChessCnt[nChessID]++;
				if(cm->To.x>=4)
					m_pEval->m_uiCurRBigChessCnt[nChessID]++;
			}
		}
		else
		{
			if(cm->To.y>=5)
			{
				m_pEval->m_uiCurBigChessCnt[nChessID]++;
				
				if(cm->To.x<=4)
					m_pEval->m_uiCurLBigChessCnt[nChessID]++;
				if(cm->To.x>=4)
					m_pEval->m_uiCurRBigChessCnt[nChessID]++;
			}
		}		
	}

	//更新当前棋子位置信息
	for(int i=0;i<m_pEval->m_vectorCurChessPos[bySrcChess].size();i++)
	{
		if(m_pEval->m_vectorCurChessPos[bySrcChess][i].x==cm->To.x && m_pEval->m_vectorCurChessPos[bySrcChess][i].y==cm->To.y)
		{
			m_pEval->m_vectorCurChessPos[bySrcChess][i].x=cm->From.y;
			m_pEval->m_vectorCurChessPos[bySrcChess][i].y=cm->From.x;
		}
	}
	if(nChessID!=NOCHESS)
	{
		CHESSMANPOS cmpTemp;
		cmpTemp.x=cm->To.y;
		cmpTemp.y=cm->To.x;
		m_pEval->m_vectorCurChessPos[nChessID].push_back(cmpTemp);
	}

	byCurChessBoard[cm->From.y][cm->From.x]=byCurChessBoard[cm->To.y][cm->To.x];//将目标位置棋子拷回原位  	
	byCurChessBoard[cm->To.y][cm->To.x]=nChessID;							    //恢复目标位置的棋子
}

void CBaseSearchEngine::UndoChessMove(BYTE byChessBoard[][9],CHESSMOVE* cm, BYTE nChessID)
{
	byChessBoard[cm->From.y][cm->From.x]=byChessBoard[cm->To.y][cm->To.x];//将目标位置棋子拷回原位  	
	byChessBoard[cm->To.y][cm->To.x]=nChessID;							  //恢复目标位置的棋子
}

int CBaseSearchEngine::IsGameOver(BYTE byChessBoard[][9], int nDepth)
{
	int i,j;
	bool bIsRedLive=0,bIsBlackLive=0;

	//检查红方九宫是否有帅
	i=7;
	while(i<10 && !bIsRedLive && !bIsBlackLive)
	{
		j=3;
		while(j<6 && !bIsRedLive && !bIsBlackLive)
		{
			if(byChessBoard[i][j]==B_KING)
			{
				bIsBlackLive=1;
				break;
			}
			if(byChessBoard[i][j]==R_KING)
			{
				bIsRedLive=1;
				break;
			}
			j++;
		}
		i++;
	}

	//检查黑方九宫是否有将
	i=0;
	while(i<3 && !bIsBlackLive)
	{
		j=3;
		while(j<6 && !bIsBlackLive)
		{
			if(byChessBoard[i][j]==B_KING)
			{
				bIsBlackLive=TRUE;
				break;
			}
			if(byChessBoard[i][j]==R_KING)
			{
				bIsRedLive=TRUE;
				break;
			}
			j++;
		}
		i++;
	}

	i=(m_iMaxDepth-nDepth+1)%2;//取当前奇偶标志,奇数层为电脑方,偶数层为用户方
	//红方不在
	if(!bIsRedLive)
		if(i)
			return 19990+nDepth; //奇数层返回极大值
		else
			return -19990-nDepth;//偶数层返回极小值

	//黑方不在
	if(!bIsBlackLive)
		if(i)
			return -19990-nDepth;//奇数层返回极小值
		else
			return 19990+nDepth; //偶数层返回极大值

	return 0;//将帅都在，返回0
}

int CBaseSearchEngine::IsObeyRule(BYTE byChessBoard[][9],int iDepth,CBANDMOVE cam,bool bIsRed)
{
	int iSize;
	int iCount;
	int i;
	BYTE byChess,byChess2;

	if(!bIsRed)
	{
		//判断长将和长捉
		iSize=m_vectorBlackCbAndMove.size();
		for(i=0;i<iSize;i++)
		{
			if(!memcmp(&cam,&m_vectorBlackCbAndMove[i],sizeof(CBANDMOVE)))
				return CR_LONGGEN;
		}

		//判断送将
		byChess2=MakeMove(&cam.cm);
		UnMakeMove(&cam.cm,byChess2);
		for(i=0;i<iCount;i++)
		{
			byChess=MakeMove(&m_pMG->m_MoveList[iDepth][i]);
			UnMakeMove(&m_pMG->m_MoveList[iDepth][i],byChess);
			if(byChess==B_KING)
				return CR_SENDGEN;
		}

		return CR_OBEY;
	}
	else
	{
		//判断长将和长捉
		iSize=m_vectorRedCbAndMove.size();
		for(i=0;i<iSize;i++)
		{
			if(!memcmp(&cam,&m_vectorRedCbAndMove[i],sizeof(CBANDMOVE)))
				return CR_LONGGEN;
		}

		//判断送将
		iCount=m_pMG->CreatePossibleMove(byChessBoard,iDepth,1,m_nUserChessColor);
		for(i=0;i<iCount;i++)
		{
			byChess=MakeMove(&m_pMG->m_MoveList[iDepth][i]);
			UnMakeMove(&m_pMG->m_MoveList[iDepth][i],byChess);
			if(byChess==R_KING)
				return CR_SENDGEN;
		}

		return CR_OBEY;
	}
}

bool CBaseSearchEngine::IsFail(BYTE byChessBoard[][9], bool bIsRed)
{
	int iCount=m_pMG->CreatePossibleMove(byChessBoard,0,bIsRed,m_nUserChessColor);
	BYTE byChess;
	CBANDMOVE cam;
	int i;

//	for(i=0;i<iCount;i++)
//	{
//		byChess=MakeMove(&m_pMG->m_MoveList[0][i]);
//		IsObeyRule(byChessBoard,
//	}

	return 1;
}

int CBaseSearchEngine::GetEvel()
{
	int iTemp=m_iEvel;
	m_iEvel=0;

	return iTemp;
}

int CBaseSearchEngine::SelMove(vector<int> vectorScore,int iMaxScore)
{
	int iPos=0;
	int iAllScore=0;
	vector<float> vectorSelProb;//选择概率
	float fAllSelProb=0;
	float fRand;
	int iSize=vectorScore.size();

	if(iMaxScore<0)
	{
		//求总得分
		for(iPos=0;iPos<iSize;iPos++)
		{
			vectorScore[iPos]+=5-iMaxScore;
			iAllScore+=vectorScore[iPos];
		}
	}
	else
		//求总得分
		for(iPos=0;iPos<iSize;iPos++)
			iAllScore+=vectorScore[iPos];

	//求各节点的选择概率
	for(iPos=0;iPos<iSize;iPos++)
		vectorSelProb.push_back(vectorScore[iPos]*1.0/iAllScore);

	//根据轮盘法选择节点
	iSize=vectorSelProb.size();
	fRand=rand()*1.0/RAND_MAX;
	for(iPos=0;iPos<iSize;iPos++)
	{
		fAllSelProb+=vectorSelProb[iPos];
		if(fRand<fAllSelProb)
			return iPos;
	}
}

bool CBaseSearchEngine::BadForMe(BYTE byChessBoard[][9],bool bIsRed)
{
//	if(IsObeyRule)
//		return 1;

	return 0;
}

UINT CBaseSearchEngine::GetChessAge(BYTE byChessBoard[][9])
{
	int iCnt=0;
	int i,j;

	for(i=0;i<10;i++)
	{
		for(j=0;j<9;j++)
			if(byChessBoard[i][j]!=byInitChessBoard[i][j])
				iCnt++;
	}

	if(iCnt<30)
		return CA_START;

	if(iCnt<90)
		return CA_MID;

	return CA_END;
}
