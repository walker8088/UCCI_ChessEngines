// Eveluation.cpp: implementation of the CEveluation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Chess.h"
#include "Eveluation.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEveluation::CEveluation()
{
}

CEveluation::~CEveluation()
{

}

int CEveluation::Eveluate(BYTE byChessBoard[][9], BOOL bIsRedTurn)
{
	BYTE bySourceChess,byTargetChess;
	int iHorsePos;
	int iKingPos;
	int iRedValue=0;
	int iBlackValue=0;
	int i,j,k,m;//i,j用于对棋盘盘面的扫描
	bool bFlag;
	int iCount=0;
	UINT uiDist;
	UINT uiSize;
	UINT uiLBigChessCnt=0;//左边有攻击力的棋子
	UINT uiRBigChessCnt=0;//右边有攻击力的棋子

	m_nAccessCount++;//每调用一次就增加一次

	UpdateValue(byChessBoard);//更新棋子价值及阵形价值

	//初始化
	memset(m_chessValue,0,360);
	memset(m_AttackPos,0,180);
	memset(m_GuardPos,0,90);
	memset(m_FlexibilityPos,0,90);

	//扫描棋盘，找出每一个棋子，及其威胁/保护的棋子，还有其灵活性
	for(i=0;i<10;i++)
	{
		for(j=0;j<9;j++)
		{
			if(byChessBoard[i][j]!=NOCHESS)
			{
				bySourceChess=byChessBoard[i][j];//取棋子类型
				GetRelatePiece(byChessBoard,j,i);//找出该棋子所有相关位置
				for(k=0;k<nPosCount;k++)//对每一目标位置
				{
					byTargetChess=byChessBoard[RelatePos[k].y][RelatePos[k].x];//取目标棋子类型
					if(byTargetChess==NOCHESS)//如果是空白，灵活性增加
						m_FlexibilityPos[i][j]++;
					else//有棋子
					{
						if(IsSameSide(bySourceChess,byTargetChess))
							//己方棋子，目标受保护
							m_GuardPos[RelatePos[k].y][RelatePos[k].x]++;
						else
						{
							//敌方棋子，目标受威胁
							m_AttackPos[RelatePos[k].y][RelatePos[k].x]++;
							m_FlexibilityPos[i][j]++;//灵活性增加
							switch(byTargetChess)
							{
								case R_KING://红帅
									if(!bIsRedTurn)//轮到黑棋走
										return 18888;//返回失败极值
								
								case B_KING://黑将
									if(bIsRedTurn)//轮到红棋走
										return 18888;//返回失败极值
									
								default:
									//根据威胁的棋子加上威胁分值
									m_AttackPos[RelatePos[k].y][RelatePos[k].x]+=
										(30+((m_iChessBaseValue[byTargetChess][i][j]-m_iChessBaseValue[bySourceChess][i][j])>>3))>>3;
									//m_AttackPos[RelatePos[k].y][RelatePos[k].x]+=
									//	(30+(m_iChessBaseValue[byTargetChess][i][j]-m_iChessBaseValue[bySourceChess][i][j])/10)/10;
									break;
							}
						}
					}
				}
			}
		}
	}

	//下面的循环统计扫描到的数据
	for(i=0;i<10;i++)
	{
		for(j=0;j<9;j++)
		{
			if(byChessBoard[i][j]!=NOCHESS)
			{
				bySourceChess=byChessBoard[i][j];
				m_chessValue[i][j]++;
				//如果棋子存在其价值不为0，把每一棋子的灵活性价值加进棋子价值
				m_chessValue[i][j]+=m_iFlexValue[bySourceChess]*m_FlexibilityPos[i][j];
				//m_chessValue[i][j]+=Multiply(m_iFlexValue[bySourceChess],m_FlexibilityPos[i][j]);
			}
		}
	}

	//下面的循环继续统计扫描到的数据
	int nHalfvalue;
	for(i=0;i<10;i++)
	{
		for(j=0;j<9;j++)
		{
			if(byChessBoard[i][j]!=NOCHESS)
			{
				bySourceChess=byChessBoard[i][j];	
				//nHalfvalue=m_iChessBaseValue[bySourceChess][i][j]/16;      //棋子基本价值的1/16作为威胁/保护增量
				nHalfvalue=m_iChessBaseValue[bySourceChess][i][j]>>4;      //棋子基本价值的1/16作为威胁/保护增量
				m_chessValue[i][j]+=m_iChessBaseValue[bySourceChess][i][j];//每个棋子的基本价值加入其总价值
				if(IsRed(bySourceChess))//红棋
				{
					if(m_AttackPos[i][j])//当前红棋如果被威胁
					{
						if(bIsRedTurn)//轮到红棋走
						{
							if(bySourceChess==R_KING)//如果是红将
								m_chessValue[i][j]-=20;//价值降低20
							else
							{
								//价值减去2倍nHalfvalue
								//m_chessValue[i][j]-=nHalfvalue*2;
								m_chessValue[i][j]-=nHalfvalue<<1;
								if(m_GuardPos[i][j])//是否被己方棋子保护
									m_chessValue[i][j]+=nHalfvalue;//被保护再加上nHalfvalue
							}
						}
						else//当前红棋被威胁，轮到黑棋走
						{
							if(bySourceChess==R_KING)//是否是红帅
								return 18888;//返回失败极值
							//m_chessValue[i][j]-=nHalfvalue*10;//减去10倍的nHalfvalue,表示威胁程度高
							m_chessValue[i][j]-=(nHalfvalue<<1)+(nHalfvalue<<3);//减去10倍的nHalfvalue,表示威胁程度高
							if(m_GuardPos[i][j])//如果被保护
								//m_chessValue[i][j]+=nHalfvalue*9;//被保护再加上9倍的nHalfvalue
								m_chessValue[i][j]+=nHalfvalue+(nHalfvalue<<3);//被保护再加上9倍的nHalfvalue
						}
						//被威胁的棋子加上威胁差，防止一个兵威胁
						//一个被保护的车，而估值函数没有反映此类问题
						m_chessValue[i][j]-=m_AttackPos[i][j];
					}
					else
					{
						//没受威胁
						if(m_GuardPos[i][j])
							m_chessValue[i][j]+=5;//受保护，加一点分
					}
				}
				else
				{
					//如果是黑棋
					if(m_AttackPos[i][j])
					{
						//受威胁
						if(!bIsRedTurn)
						{
							//轮到黑棋走
							if(bySourceChess==B_KING)//如果是黑将
								m_chessValue[i][j]-=20;//棋子价值降低20
							else
							{
								//棋子价值降低2倍nHalfvalue
								//m_chessValue[i][j]-=nHalfvalue*2;
								m_chessValue[i][j]-=nHalfvalue<<1;
								if(m_GuardPos[i][j])//如果受保护								
									m_chessValue[i][j]+=nHalfvalue;//棋子价值增加nHalfvalue
							}
						}
						else
						{
							//轮到红棋走
							if(bySourceChess==B_KING)//是黑将
								return 18888;//返回失败极值							
							//m_chessValue[i][j]-=nHalfvalue*10;//棋子价值减少10倍nHalfvalue
							m_chessValue[i][j]-=(nHalfvalue<<1)+(nHalfvalue<<3);//棋子价值减少10倍nHalfvalue
							if(m_GuardPos[i][j])//受保护
								//m_chessValue[i][j]+=nHalfvalue*9;//被保护再加上9倍nHalfvalue
								m_chessValue[i][j]+=nHalfvalue+(nHalfvalue<<3);//被保护再加上9倍nHalfvalue
						}
						//被威胁的棋子再加上威胁差
						//防止一个兵威胁一个被保护的车,而估值函数没有反映此类的问题
						m_chessValue[i][j]-=m_AttackPos[i][j];
					}
					else
					{
						//不受威胁
						if(m_GuardPos[i][j])
							m_chessValue[i][j]+=5;//受保护，加一点分
					}
				}
			}
		}
	}

	//以下代码皆为判断形状的价值和攻势
	for(k=0;k<10;k++)
	{
		m_iChessBaseValue[B_CAR][k][3]=1300;
		m_iChessBaseValue[B_CAR][k][5]=1300;

		m_iChessBaseValue[R_CAR][k][3]=1300;
		m_iChessBaseValue[R_CAR][k][5]=1300;
	}
	if(m_uiCurChessAge==CA_START)
	{
		m_iChessBaseValue[B_CAR][4][3]=1350;
		m_iChessBaseValue[B_CAR][5][3]=1350;
		m_iChessBaseValue[B_CAR][4][5]=1350;
		m_iChessBaseValue[B_CAR][5][5]=1350;

		m_iChessBaseValue[R_CAR][4][3]=1350;
		m_iChessBaseValue[R_CAR][5][3]=1350;
		m_iChessBaseValue[R_CAR][4][5]=1350;
		m_iChessBaseValue[R_CAR][5][5]=1350;
	}

	//-------------------双车孤将或多加一士，极有可能形成双车错杀-------------------
	//黑
	if(m_iChessCnt[B_CAR]==2)
	{
		if(m_iChessCnt[R_BISHOP]==0)
		{
			if(m_iChessCnt[R_ELEPHANT]==0)
				iBlackValue+=m_iLineupBDblCarLoneKing;
			if(m_iChessCnt[R_ELEPHANT]==1)
				iBlackValue+=m_iLineupBDblCarLoneKingElephant;
		}
		if(m_iChessCnt[R_BISHOP]==1)
		{
			if(m_iChessCnt[R_ELEPHANT]==0)
				iBlackValue+=m_iLineupBDblCarLoneKingBishop;
			if(m_iChessCnt[R_ELEPHANT]==1)
				iBlackValue+=m_iLineupBDblCarLoneKingBishopElephant;
		}
	}

	//红
	if(m_iChessCnt[R_CAR]==2)
	{
		if(m_iChessCnt[B_BISHOP]==0)
		{
			if(m_iChessCnt[B_ELEPHANT]==0)
				iRedValue+=m_iLineupBDblCarLoneKing;
			if(m_iChessCnt[B_ELEPHANT]==1)
				iRedValue+=m_iLineupBDblCarLoneKingElephant;
		}
		if(m_iChessCnt[B_BISHOP]==1)
		{
			if(m_iChessCnt[B_ELEPHANT]==0)
				iRedValue+=m_iLineupBDblCarLoneKingBishop;
			if(m_iChessCnt[B_ELEPHANT]==1)
				iRedValue+=m_iLineupBDblCarLoneKingBishopElephant;
		}
	}
	//------------------------------------------------------------------------------

	//---------------检测鲁莽马---------------
	//黑
	if(m_uiCurBigChessCnt[B_HORSE]==1 && !m_uiCurBigChessCnt[B_CAR] && !m_uiCurBigChessCnt[B_PAWN] && !m_uiCurBigChessCnt[B_CANON])
		iBlackValue+=m_iLineupBImpertinenceHorse;

	//红
	if(m_uiCurBigChessCnt[R_HORSE]==1 && !m_uiCurBigChessCnt[R_CAR] && !m_uiCurBigChessCnt[R_PAWN] && !m_uiCurBigChessCnt[R_CANON])
		iRedValue+=m_iLineupRImpertinenceHorse;
	//----------------------------------------

	//-----------判断协同作战的攻势价值-----------
	//黑
	uiLBigChessCnt=m_uiCurLBigChessCnt[B_HORSE]+m_uiCurLBigChessCnt[B_CAR]+m_uiCurLBigChessCnt[B_CANON]+m_uiCurLBigChessCnt[B_PAWN];
	uiRBigChessCnt=m_uiCurRBigChessCnt[B_HORSE]+m_uiCurRBigChessCnt[B_CAR]+m_uiCurRBigChessCnt[B_CANON]+m_uiCurRBigChessCnt[B_PAWN];
	iBlackValue+=(uiLBigChessCnt*uiLBigChessCnt+uiRBigChessCnt*uiRBigChessCnt)*m_iLineupBAttackTogetherFactor;

	//红
	uiLBigChessCnt=m_uiCurLBigChessCnt[R_HORSE]+m_uiCurLBigChessCnt[R_CAR]+m_uiCurLBigChessCnt[R_CANON]+m_uiCurLBigChessCnt[R_PAWN];
	uiRBigChessCnt=m_uiCurRBigChessCnt[R_HORSE]+m_uiCurRBigChessCnt[R_CAR]+m_uiCurRBigChessCnt[R_CANON]+m_uiCurRBigChessCnt[R_PAWN];
	iRedValue+=(uiLBigChessCnt*uiLBigChessCnt+uiRBigChessCnt*uiRBigChessCnt)*m_iLineupRAttackTogetherFactor;
	//--------------------------------------------

	//-----------判断多兵种联合作战的攻势价值-----------
	//黑
	if(m_uiCurBigChessCnt[B_CAR])
	{
		if(m_uiCurBigChessCnt[B_HORSE])
		{
			if(m_uiCurBigChessCnt[B_CANON])
			{
				if(m_uiCurBigChessCnt[B_PAWN])
					iBlackValue+=m_iLineupBCarHorseCannonPawnAttackTogether;
				else
					iBlackValue+=m_iLineupBCarHorseCannonAttackTogether;
			}
			else
			{
				if(m_uiCurBigChessCnt[B_PAWN])
					iBlackValue+=m_iLineupBCarHorsePawnAttackTogether;
			}
		}
		else
		{
			if(m_uiCurBigChessCnt[B_CANON] && m_uiCurBigChessCnt[B_PAWN])
				iBlackValue+=m_iLineupBCarCannonPawnAttackTogether;
		}
	}
	else
	{
		if(m_uiCurBigChessCnt[B_HORSE] && m_uiCurBigChessCnt[B_CANON] && m_uiCurBigChessCnt[B_PAWN])
			iBlackValue+=m_iLineupBHorseCannonPawnAttackTogether;
	}

	//红
	if(m_uiCurBigChessCnt[R_CAR])
	{
		if(m_uiCurBigChessCnt[R_HORSE])
		{
			if(m_uiCurBigChessCnt[R_CANON])
			{
				if(m_uiCurBigChessCnt[R_PAWN])
					iRedValue+=m_iLineupRCarHorseCannonPawnAttackTogether;
				else
					iRedValue+=m_iLineupRCarHorseCannonAttackTogether;
			}
			else
			{
				if(m_uiCurBigChessCnt[R_PAWN])
					iRedValue+=m_iLineupRCarHorsePawnAttackTogether;
			}
		}
		else
		{
			if(m_uiCurBigChessCnt[R_CANON] && m_uiCurBigChessCnt[R_PAWN])
				iRedValue+=m_iLineupRCarCannonPawnAttackTogether;
		}
	}
	else
	{
		if(m_uiCurBigChessCnt[R_HORSE] && m_uiCurBigChessCnt[R_CANON] && m_uiCurBigChessCnt[R_PAWN])
			iRedValue+=m_iLineupRHorseCannonPawnAttackTogether;
	}
	//--------------------------------------------------

	//-------------------孤将对车马-------------------
	//黑
	if(m_iChessCnt[B_BISHOP]<2 && m_uiCurBigChessCnt[R_HORSE]>0)
	{
		m_iLineupBLoneKingVsCarHorse=m_iLineupBKingVsCarHorse[m_iChessCnt[B_BISHOP]][m_iChessCnt[B_ELEPHANT]][m_uiCurBigChessCnt[R_HORSE]][m_uiCurBigChessCnt[R_PAWN]];
		m_iLineupBLoneKingVsCarHorse+=m_uiCurBigChessCnt[R_CAR]*50+(m_iChessCnt[R_CAR]-m_uiCurBigChessCnt[R_CAR])*30
			+m_uiCurBigChessCnt[R_CANON]*20+(m_iChessCnt[R_CANON]-m_uiCurBigChessCnt[R_CANON])*10;
		iBlackValue+=m_iLineupBLoneKingVsCarHorse;
	}

	//红
	if(m_iChessCnt[R_BISHOP]<2 && m_uiCurBigChessCnt[B_HORSE]>0)
	{
		m_iLineupRLoneKingVsCarHorse=m_iLineupRKingVsCarHorse[m_iChessCnt[R_BISHOP]][m_iChessCnt[R_ELEPHANT]][m_uiCurBigChessCnt[B_HORSE]][m_uiCurBigChessCnt[B_PAWN]];
		m_iLineupRLoneKingVsCarHorse+=m_uiCurBigChessCnt[B_CAR]*50+(m_iChessCnt[B_CAR]-m_uiCurBigChessCnt[B_CAR])*30
			+m_uiCurBigChessCnt[B_CANON]*20+(m_iChessCnt[B_CANON]-m_uiCurBigChessCnt[B_CANON])*10;
		iRedValue+=m_iLineupRLoneKingVsCarHorse;
	}
	//------------------------------------------------

	//-------------开局时防止车被对方车炮牵制-------------
	//黑
	uiSize=m_vectorCurChessPos[B_CAR].size();
	for(i=0;i<uiSize;i++)
	{
		if(m_vectorCurChessPos[B_CAR][i].y==1 && m_vectorCurChessPos[B_CAR][i].x<6)
		{
			if(byChessBoard[6][1]==B_CANON && (byChessBoard[8][1]==R_CAR || byChessBoard[9][1]==R_CAR))
				iBlackValue+=50;
			break;
		}
	}
	for(i=0;i<uiSize;i++)
	{
		if(m_vectorCurChessPos[B_CAR][i].y==7 && m_vectorCurChessPos[B_CAR][i].x<6)
		{
			if(byChessBoard[6][7]==B_CANON && (byChessBoard[8][7]==R_CAR || byChessBoard[9][7]==R_CAR))
				iBlackValue+=50;
			break;
		}
	}
	
	if(byChessBoard[6][1]==B_CAR && byChessBoard[7][1]==R_CANON && (byChessBoard[8][1]==R_CAR || byChessBoard[9][1]==R_CAR))
		iBlackValue+=100;
	if(byChessBoard[6][7]==B_CAR && byChessBoard[7][7]==R_CANON && (byChessBoard[8][7]==R_CAR || byChessBoard[9][7]==R_CAR))
		iBlackValue+=100;
	
	//红
	uiSize=m_vectorCurChessPos[R_CAR].size();
	for(i=0;i<uiSize;i++)
	{
		if(m_vectorCurChessPos[R_CAR][i].y==1 && m_vectorCurChessPos[R_CAR][i].x>3)
		{
			if(byChessBoard[3][1]==R_CANON && (byChessBoard[1][1]==B_CAR || byChessBoard[0][1]==B_CAR))
				iRedValue+=50;
			break;
		}
	}
	for(i=0;i<uiSize;i++)
	{
		if(m_vectorCurChessPos[R_CAR][i].y==7 && m_vectorCurChessPos[R_CAR][i].x>3)
		{
			if(byChessBoard[3][7]==R_CANON && (byChessBoard[1][7]==B_CAR || byChessBoard[0][7]==B_CAR))
				iRedValue+=50;
			break;
		}
	}
	
	if(byChessBoard[3][1]==R_CAR && byChessBoard[2][1]==B_CANON && (byChessBoard[1][1]==B_CAR || byChessBoard[0][1]==B_CAR))
		iRedValue+=100;
	if(byChessBoard[3][7]==R_CAR && byChessBoard[2][7]==B_CANON && (byChessBoard[1][7]==B_CAR || byChessBoard[0][7]==B_CAR))
		iRedValue+=100;
	//----------------------------------------------------

	//----------------开局时防止对方马跳出牵制己方车----------------
	//黑
	uiSize=m_vectorCurChessPos[B_CANON].size();
	for(i=0;i<uiSize;i++)
	{
		if(m_vectorCurChessPos[B_CANON][i].y==1 && m_vectorCurChessPos[B_CANON][i].x<4 && byChessBoard[4][1]==B_HORSE && byChessBoard[0][0]==B_CAR)
		{
			iBlackValue+=50;
			break;
		}
	}
	for(i=0;i<uiSize;i++)
	{
		if(m_vectorCurChessPos[B_CANON][i].y==7 && m_vectorCurChessPos[B_CANON][i].x<4 && byChessBoard[4][7]==B_HORSE && byChessBoard[0][8]==B_CAR)
		{
			iBlackValue+=50;
			break;
		}
	}

	//红
	uiSize=m_vectorCurChessPos[R_CANON].size();
	for(i=0;i<uiSize;i++)
	{
		if(m_vectorCurChessPos[R_CANON][i].y==1 && m_vectorCurChessPos[R_CANON][i].x>5 && byChessBoard[5][1]==R_HORSE && byChessBoard[9][0]==R_CAR)
		{
			iRedValue+=50;
			break;
		}
	}
	for(i=0;i<uiSize;i++)
	{
		if(m_vectorCurChessPos[R_CANON][i].y==7 && m_vectorCurChessPos[R_CANON][i].x>5 && byChessBoard[5][7]==R_HORSE && byChessBoard[9][8]==R_CAR)
		{
			iRedValue+=50;
			break;
		}
	}
	//--------------------------------------------------------------

	//--------------------开局时，挺兵让马行--------------------
	//黑
	//己方已上马或对方已上马然后要挺兵
	if(byChessBoard[2][2]==B_HORSE || byChessBoard[7][2]==R_HORSE)
	{
		if(byChessBoard[3][2]!=NOCHESS)
		{
			if(IsRed(byChessBoard[3][2]))
				iBlackValue-=50;
			if(byChessBoard[3][2]==B_PAWN)
			{
				if(byChessBoard[5][2]==R_PAWN)
					iBlackValue-=25;
				else
					iBlackValue-=50;
			}
		}
		else
			iBlackValue+=50;
	}
	if(byChessBoard[2][6]==B_HORSE || byChessBoard[7][6]==R_HORSE)
	{
		if(byChessBoard[3][6]!=NOCHESS)
		{
			if(IsRed(byChessBoard[3][6]))
				iBlackValue-=50;
			if(byChessBoard[3][6]==B_PAWN)
			{
				if(byChessBoard[5][6]==R_PAWN)
					iBlackValue-=25;
				else
					iBlackValue-=50;
			}
		}
		else
			iBlackValue+=50;
	}
	//对方已上兵,己方在有车的保护下也要挺兵
	if(byChessBoard[5][2]==R_PAWN)
	{
		bFlag=0;
		if(byChessBoard[4][0]==B_CAR || byChessBoard[4][1]==B_CAR)
			bFlag=1;
		if(!bFlag)
		{
			for(j=3;j<9;j++)
			{
				if(byChessBoard[4][j]!=NOCHESS)
				{
					if(byChessBoard[4][j]==B_CAR)
						bFlag=1;
					break;
				}
			}
		}
		if(bFlag && byChessBoard[4][2]==B_PAWN)
			iBlackValue+=50;
	}
	if(byChessBoard[5][6]==R_PAWN)
	{
		bFlag=0;
		if(byChessBoard[4][8]==B_CAR || byChessBoard[4][7]==B_CAR)
			bFlag=1;
		if(!bFlag)
		{
			for(j=5;j>=0;j--)
			{
				if(byChessBoard[4][j]!=NOCHESS)
				{
					if(byChessBoard[4][j]==B_CAR)
						bFlag=1;
					break;
				}
			}
		}
		if(bFlag && byChessBoard[4][6]==B_PAWN)
			iBlackValue+=50;
	}

	//红
	if(byChessBoard[7][2]==R_HORSE || byChessBoard[2][2]==B_HORSE)
	{
		if(byChessBoard[6][2]!=NOCHESS)
		{
			if(IsBlack(byChessBoard[6][2]))
				iRedValue-=50;
			if(byChessBoard[6][2]==R_PAWN)
			{
				if(byChessBoard[4][2]==B_PAWN)
					iRedValue-=25;
				else
					iRedValue-=50;
			}
		}
		else
			iRedValue+=50;
	}
	if(byChessBoard[6][6]==R_HORSE || byChessBoard[2][6]==B_HORSE)
	{
		if(byChessBoard[6][6]!=NOCHESS)
		{
			if(IsBlack(byChessBoard[6][6]))
				iRedValue-=50;
			if(byChessBoard[6][6]==R_PAWN)
			{
				if(byChessBoard[4][6]==B_PAWN)
					iRedValue-=25;
				else
					iRedValue-=50;
			}
		}
		else
			iRedValue+=50;
	}
	//对方已上兵,己方在有车的保护下也要挺兵
	if(byChessBoard[4][2]==B_PAWN)
	{
		bFlag=0;
		if(byChessBoard[5][0]==R_CAR || byChessBoard[5][1]==R_CAR)
			bFlag=1;
		if(!bFlag)
		{
			for(j=3;j<9;j++)
			{
				if(byChessBoard[5][j]!=NOCHESS)
				{
					if(byChessBoard[5][j]==R_CAR)
						bFlag=1;
					break;
				}
			}
		}
		if(bFlag && byChessBoard[5][2]==R_PAWN)
			iRedValue+=50;
	}
	if(byChessBoard[4][6]==B_PAWN)
	{
		bFlag=0;
		if(byChessBoard[5][8]==R_CAR || byChessBoard[5][7]==R_CAR)
			bFlag=1;
		if(!bFlag)
		{
			for(j=5;j>=0;j--)
			{
				if(byChessBoard[5][j]!=NOCHESS)
				{
					if(byChessBoard[5][j]==R_CAR)
						bFlag=1;
					break;
				}
			}
		}
		if(bFlag && byChessBoard[5][6]==R_PAWN)
			iRedValue+=50;
	}
	//----------------------------------------------------------

	for(i=0;i<10;i++)
	{
		for(j=0;j<9;j++)
		{
			if(byChessBoard[i][j]!=NOCHESS)
			{
				if(IsBlack(byChessBoard[i][j]))
				{
					iBlackValue+=m_chessValue[i][j];//把黑棋的值加总
					
					if(i>=5)
					{
						if(byChessBoard[i][j]==B_PAWN)
						{
							if(i<=7)
							{
								//根据兵与象的距离动态改变其攻势价值
								uiDist=7-i+abs(j-4);
								if(uiDist==0)
									m_nAttackValue[B_PAWN]+=20;
								else
								{
									if(uiDist<=2)
										m_nAttackValue[B_PAWN]+=10;
									else
									{
										if(uiDist<=4)
											m_nAttackValue[B_PAWN]+=5;
									}
								}
							}
						}
						iBlackValue+=m_nAttackValue[byChessBoard[i][j]];//判断黑方的攻势
					}
					else
						iBlackValue+=uiRecoveryValue[byChessBoard[i][j]];//防守能力
				}
				else
				{
					iRedValue+=m_chessValue[i][j];//把红棋的值加总
					if(i<=4)
					{
						if(byChessBoard[i][j]==R_PAWN)
						{
							if(i>=2)
							{
								//根据兵的距离动态改变其攻势价值
								uiDist=i-2+abs(j-4);
								m_nAttackValue[R_PAWN]=35;
								if(uiDist==0)
									m_nAttackValue[R_PAWN]+=20;
								else
								{
									if(uiDist<=2)
										m_nAttackValue[R_PAWN]+=10;
									else
									{
										if(uiDist<=4)
											m_nAttackValue[R_PAWN]+=5;
									}
								}
							}
						}
						iRedValue+=m_nAttackValue[byChessBoard[i][j]];//判断红方的攻势
					}
					else
						iRedValue+=uiRecoveryValue[byChessBoard[i][j]];
				}
			}

			//判断形状的价值
			switch(byChessBoard[i][j])
			{
			case B_PAWN://黑卒
				//卧心兵
				if(!m_iChessCnt[R_BISHOP] && i==8 && j==4 && m_vectorCurChessPos[R_KING][0].x==9)
					iBlackValue+=m_iLineupBLieMidPawn;

				switch(m_iChessCnt[B_PAWN])
				{
				case 5:
					if(j-4>=0)
					{
						if(byChessBoard[i][j-1]==B_PAWN && byChessBoard[i][j-2]==B_PAWN && byChessBoard[i][j-3]==B_PAWN && byChessBoard[i][j-4]==B_PAWN)
							iBlackValue+=m_iLineupB5ConnectPawn;
					}
					break;

				case 4:
					if(j-3>=0)
					{
						if(byChessBoard[i][j-1]==B_PAWN && byChessBoard[i][j-2]==B_PAWN && byChessBoard[i][j-3]==B_PAWN)
							iBlackValue+=m_iLineupB4ConnectPawn;
					}
					break;

				case 3:
					if(j-2>=0)
					{
						if(byChessBoard[i][j-1]==B_PAWN && byChessBoard[i][j-2]==B_PAWN)
							iBlackValue+=m_iLineupB3ConnectPawn;
					}
					break;

				case 2:
					if(j-1>=0)
					{
						if(byChessBoard[i][j-1]==B_PAWN)
							iBlackValue+=m_iLineupB2ConnectPawn;
					}
					break;
				}

				break;

			case B_ELEPHANT://黑象
				//连环象
				if(i-2>=0)
				{
					if(j-2>=0)
					{
						if(byChessBoard[i-2][j-2]==B_ELEPHANT)
						{
							if(byChessBoard[i-1][j-1]==NOCHESS)
								iBlackValue+=m_iLineupBConnectElephant;

							break;
						}
					}
				}
				break;

			case B_BISHOP://黑士
				//连环士
				if(i-1>=0)
				{
					if(j-1>=3)
					{
						if(byChessBoard[i-1][j-1]==B_BISHOP)
						{
							iBlackValue+=m_iLineupBConnectBishop;
							break;
						}
					}
				}
				break;

			case B_KING:
				//将离原位
				if(i==1 || i==2 || j==3 || j==5)
					iBlackValue+=m_iLineupBNoProtectKing;
				break;

			case B_CAR:
				//卧心车
				if(!m_iChessCnt[R_BISHOP] && i==8 && j==4)
					iBlackValue+=m_iLineupBLieMidCar;

				//黑车可能与自己子同行
				bFlag=0;
				k=j-1;
				while(!bFlag && k>=0)
				{
					switch(byChessBoard[i][k])
					{
					case B_KING:
						iBlackValue+=m_iLineupBCarKing;
						bFlag=1;
						break;

					case B_HORSE:
						//车马将
						k--;
						while(k>=0)
						{
							if(byChessBoard[i][k]!=NOCHESS)
							{
								if(byChessBoard[i][k]==R_KING)
								{
									iBlackValue+=m_iLineupBCarHorseKing;
									bFlag=1;
								}
								break;
							}

							k--;
						}

						if(bFlag)
							break;

						iBlackValue+=m_iLineupBCarHorse;
						bFlag=1;
						break;

					case B_CANON:
						//车炮将:炮碾丹砂
						iKingPos=k;
						k--;
						while(k>=0)
						{
							if(byChessBoard[i][k]!=NOCHESS)
							{
								if(byChessBoard[i][k]==R_KING)
								{
									if(byChessBoard[i][iKingPos-1]==R_BISHOP && byChessBoard[0][2]==R_ELEPHANT && !i)
										m_iLineupBHCarCanonKing=300;
									else
									{
										if(byChessBoard[i][iKingPos-1]==R_BISHOP || (byChessBoard[0][2]==R_ELEPHANT && !i))
											m_iLineupBHCarCanonKing=250;
										else
											m_iLineupBHCarCanonKing=200;
									}
									iBlackValue+=m_iLineupBHCarCanonKing;
									bFlag=1;
								}

								break;
							}

							k--;
						}

						if(bFlag)
							break;
						iBlackValue+=m_iLineupBCarCanon;
						bFlag=1;
						break;

					case B_BISHOP:
						iBlackValue+=m_iLineupBCarBishop;
						bFlag=1;
						break;

					case B_ELEPHANT:
						iBlackValue+=m_iLineupBCarElephant;
						bFlag=1;
						break;

					case B_PAWN:
						//车兵将
						k--;
						while(k>=0)
						{
							if(byChessBoard[i][k]!=NOCHESS)
							{
								if(byChessBoard[i][k]==R_KING)
								{
									iBlackValue+=m_iLineupBCarPawnKing;
									bFlag=1;
								}
								break;
							}

							k--;
						}

						if(bFlag)
							break;

						iBlackValue+=m_iLineupBCarPawn;
						bFlag=1;
						break;
					}

					k--;
				}
				k=j+1;
				bFlag=0;
				while(k<9 && !bFlag)
				{
					switch(byChessBoard[i][k])
					{
					case B_KING:
						iBlackValue+=m_iLineupBCarKing;
						bFlag=1;
						break;

					case B_HORSE:
						//车马将
						k++;
						while(k<9)
						{
							if(byChessBoard[i][k]!=NOCHESS)
							{
								if(byChessBoard[i][k]==R_KING)
								{
									iBlackValue+=m_iLineupBCarHorseKing;
									bFlag=1;
								}
								break;
							}

							k++;
						}

						if(bFlag)
							break;

						iBlackValue+=m_iLineupBCarHorse;
						bFlag=1;
						break;

					case B_CANON:
						//车炮将:炮碾丹砂
						iKingPos=k;
						k++;
						while(k<9)
						{
							if(byChessBoard[i][k]!=NOCHESS)
							{
								if(byChessBoard[i][k]==R_KING)
								{
									if(byChessBoard[i][iKingPos+1]==R_BISHOP && byChessBoard[0][6]==R_ELEPHANT && !i)
										m_iLineupBHCarCanonKing=300;
									else
									{
										if(byChessBoard[i][iKingPos+1]==R_BISHOP || (byChessBoard[0][6]==R_ELEPHANT && !i))
											m_iLineupBHCarCanonKing=250;
										else
											m_iLineupBHCarCanonKing=200;
									}
									iBlackValue+=m_iLineupBHCarCanonKing;
									bFlag=1;
								}

								break;
							}

							k++;
						}

						if(bFlag)
							break;
						iBlackValue+=m_iLineupBCarCanon;
						bFlag=1;
						break;

					case B_BISHOP:
						iBlackValue+=m_iLineupBCarBishop;
						bFlag=1;
						break;

					case B_ELEPHANT:
						iBlackValue+=m_iLineupBCarElephant;
						bFlag=1;
						break;

					case B_PAWN:
						//车兵将
						k++;
						while(k<9)
						{
							if(byChessBoard[i][k]!=NOCHESS)
							{
								if(byChessBoard[i][k]==R_KING)
								{
									iBlackValue+=m_iLineupBCarPawnKing;
									bFlag=1;
								}
								break;
							}

							k++;
						}

						if(bFlag)
							break;

						iBlackValue+=m_iLineupBCarPawn;
						bFlag=1;
						break;
					}

					k++;
				}

				//黑车可能与自己子同列
				bFlag=0;
				k=i-1;
				while(k>=0 && !bFlag)
				{
					switch(byChessBoard[k][j])
					{
					case B_KING:
						iBlackValue+=m_iLineupBCarKing;
						bFlag=1;
						break;

					case B_HORSE:
						//车马将
						k--;
						while(k>=0)
						{
							if(byChessBoard[k][j]!=NOCHESS)
							{
								if(byChessBoard[k][j]==R_KING)
								{
									iBlackValue+=m_iLineupBCarHorseKing;
									bFlag=1;
								}

								break;
							}

							k--;
						}

						if(bFlag)
							break;

						iBlackValue+=m_iLineupBCarHorse;
						bFlag=1;
						break;

					case B_CANON:
						//车炮将
						k--;
						while(k>=0)
						{
							if(byChessBoard[k][j]!=NOCHESS)
							{
								if(byChessBoard[k][j]==R_KING)
								{
									iBlackValue+=m_iLineupBVCarCanonKing;
									bFlag=1;
								}

								break;
							}

							k--;
						}

						if(bFlag)
							break;

						iBlackValue+=m_iLineupBCarCanon;
						bFlag=1;
						break;

					case B_BISHOP:
						iBlackValue+=m_iLineupBCarBishop;
						bFlag=1;
						break;

					case B_ELEPHANT:
						iBlackValue+=m_iLineupBCarElephant;
						bFlag=1;
						break;

					case B_PAWN:
						//车兵将
						k--;
						while(k>=0)
						{
							if(byChessBoard[k][j]!=NOCHESS)
							{
								if(byChessBoard[k][j]==R_KING)
								{
									iBlackValue+=m_iLineupBCarPawnKing;
									bFlag=1;
								}

								break;
							}

							k--;
						}

						if(bFlag)
							break;

						iBlackValue+=m_iLineupBCarPawn;
						bFlag=1;
						break;
					}

					k--;
				}
				bFlag=0;
				k=i+1;
				while(k<10 && !bFlag)
				{
					switch(byChessBoard[k][j])
					{
					case B_KING:
						iBlackValue+=m_iLineupBCarKing;
						bFlag=1;
						break;

					case B_HORSE:
						//车马将
						k++;
						while(k<10)
						{
							if(byChessBoard[k][j]!=NOCHESS)
							{
								if(byChessBoard[k][j]==R_KING)
								{
									iBlackValue+=m_iLineupBCarHorseKing;
									bFlag=1;
								}

								break;
							}

							k++;
						}

						if(bFlag)
							break;

						iBlackValue+=m_iLineupBCarHorse;
						bFlag=1;
						break;

					case B_CANON:
						//车炮将
						k++;
						while(k<10)
						{
							if(byChessBoard[k][j]!=NOCHESS)
							{
								if(byChessBoard[k][j]==R_KING)
								{
									iBlackValue+=m_iLineupBVCarCanonKing;
									bFlag=1;
								}

								break;
							}

							k++;
						}

						if(bFlag)
							break;

						iBlackValue+=m_iLineupBCarCanon;
						bFlag=1;
						break;

					case B_BISHOP:
						iBlackValue+=m_iLineupBCarBishop;
						bFlag=1;
						break;

					case B_ELEPHANT:
						iBlackValue+=m_iLineupBCarElephant;
						bFlag=1;
						break;

					case B_PAWN:
						//车兵将
						k++;
						while(k<10)
						{
							if(byChessBoard[k][j]!=NOCHESS)
							{
								if(byChessBoard[k][j]==R_KING)
								{
									iBlackValue+=m_iLineupBCarPawnKing;
									bFlag=1;
								}

								break;
							}

							k++;
						}

						if(bFlag)
							break;

						iBlackValue+=m_iLineupBCarPawn;
						bFlag=1;
						break;
					}

					k++;
				}
				break;

			case B_HORSE://黑马
				//卧槽马
				if((i==8 && (j==2 || j==6)) && byChessBoard[9][4]==R_KING)
					iBlackValue+=m_iLineupBLieSlotHorse;

				//大角马
				if(!m_iChessCnt[R_BISHOP])
				{
					if((i==7 && j==3) && byChessBoard[9][5]==R_KING && byChessBoard[7][4]==NOCHESS && byChessBoard[8][3]==NOCHESS)
						iBlackValue+=m_iLineupBBigAngleHorse;
					if((i==7 && j==5) && byChessBoard[9][3]==R_KING && byChessBoard[7][4]==NOCHESS && byChessBoard[8][5]==NOCHESS)
						iBlackValue+=m_iLineupBBigAngleHorse;
					if((i==9 && j==3) && byChessBoard[7][5]==R_KING && byChessBoard[9][4]==NOCHESS && byChessBoard[8][3]==NOCHESS)
						iBlackValue+=m_iLineupBBigAngleHorse;
					if((i==9 && j==5) && byChessBoard[7][3]==R_KING && byChessBoard[9][4]==NOCHESS && byChessBoard[8][5]==NOCHESS)
						iBlackValue+=m_iLineupBBigAngleHorse;
				}

				//卧心马
				if(i==1 && j==4)
				{
					iBlackValue+=m_iLineupBLieMidHorse;

					for(m=0;m<10;m++)
					{
						m_iChessBaseValue[R_CAR][m][3]=1400;
						m_iChessBaseValue[R_CAR][m][5]=1400;
					}
				}

				//连环马
				if(i-2>=0)
				{
					if(j-1>=0)
					{
						if(byChessBoard[i-2][j-1]==B_HORSE)
						{
							if(byChessBoard[i-1][j-1]==NOCHESS)
							{
								if(byChessBoard[i-1][j]==NOCHESS)
									iBlackValue+=m_iLineupBConnectHorse;
								else
									iBlackValue+=m_iLineupBScndConnectHorse;
							}
							else
							{
								if(byChessBoard[i-1][j]==NOCHESS)
									iBlackValue+=m_iLineupBScndConnectHorse;
								else
									iBlackValue+=m_iLineupBSznConnectHorse;
							}

							break;
						}
					}
				}
				if(i-1>=0)
				{
					if(j-2>=0)
					{
						if(byChessBoard[i-1][j-2]==B_HORSE)
						{
							if(byChessBoard[i-1][j-1]==NOCHESS)
							{
								if(byChessBoard[i][j-1]==NOCHESS)
									iBlackValue+=m_iLineupBConnectHorse;
								else
									iBlackValue+=m_iLineupBScndConnectHorse;
							}
							else
							{
								if(byChessBoard[i][j-1]==NOCHESS)
									iBlackValue+=m_iLineupBScndConnectHorse;
								else
									iBlackValue+=m_iLineupBSznConnectHorse;
							}

							break;
						}
					}
				}
				break;

			case B_CANON://黑炮
				//连环炮
				bFlag=0;
				iCount=0;
				for(k=0;k<9;k++)
				{
					if(byChessBoard[i][k]==B_CANON && k!=j)
					{
						if(k<j)
						{
							k++;
							while(k<j)
							{
								if(byChessBoard[i][k]!=NOCHESS)
									iCount++;
								
								k++;
							}

							if(iCount==1)
								iBlackValue+=m_iLineupBConnectCanon;
						}
						else
						{
							k--;
							while(k>j)
							{
								if(byChessBoard[i][k]!=NOCHESS)
									iCount++;

								k--;
							}

							if(iCount==1)
								iBlackValue+=m_iLineupBConnectCanon;
						}

						bFlag=1;
						break;
					}
				}
				if(!bFlag)
				{
					iCount=0;
					for(k=0;k<10;k++)
					{
						if(byChessBoard[k][j]==B_CANON && k!=i)
						{
							if(k<i)
							{
								k++;
								while(k<i)
								{
									if(byChessBoard[k][j]!=NOCHESS)
										iCount++;

									k++;
								}

								if(iCount==1)
									iBlackValue+=m_iLineupBConnectCanon;
							}
							else
							{
								k--;
								while(k>i)
								{
									if(byChessBoard[k][j]!=NOCHESS)
										iCount++;

									k--;
								}

								if(iCount==1)
									iBlackValue+=m_iLineupBConnectCanon;
							}

							break;
						}
					}
				}

				//炮牵制车系列
				iCount=0;
				uiSize=m_vectorCurChessPos[R_CAR].size();
				for(m=0;m<uiSize;m++)
				{
					if(m<m_vectorCurChessPos[R_CAR][m].x==i)
					{
						if(m_vectorCurChessPos[R_CAR][m].y<j)
						{
							for(k=m_vectorCurChessPos[R_CAR][m].y+1;k<j;k++)
							{
								if(byChessBoard[i][k]!=NOCHESS)
									iCount++;
							}
						}
						else
						{
							for(k=m_vectorCurChessPos[R_CAR][m].y-1;k>j;k--)
							{
								if(byChessBoard[i][k]!=NOCHESS)
									iCount++;
							}
						}
						if(iCount==2)
						{
							if(m_vectorCurChessPos[R_CAR][m].y<j)
							{
								for(k=m_vectorCurChessPos[R_CAR][m].y+1;k<j;k++)
								{
									if(IsRed(byChessBoard[i][k]))
									{
										switch(byChessBoard[i][k])
										{
										case R_KING:
											iBlackValue+=m_iLineupBCanonKingCar;
											break;

										case R_CAR:
											iBlackValue+=m_iLineupBCanonCarCar;
											break;

										case R_HORSE:
											iBlackValue+=m_iLineupBCanonHorseCar;
											break;

										case R_ELEPHANT:
											iBlackValue+=m_iLineupBCanonElephantCar;
											break;

										case R_BISHOP:
											iBlackValue+=m_iLineupBCanonBishopCar;
											break;

										case R_PAWN:
											iBlackValue+=m_iLineupBCanonPawnCar;
											break;
										}
										
										break;
									}									
								}
							}
							else
							{
								for(k=m_vectorCurChessPos[R_CAR][m].y-1;k>j;k--)
								{
									if(IsRed(byChessBoard[i][k]))
									{
										switch(byChessBoard[i][k])
										{
										case R_KING:
											iBlackValue+=m_iLineupBCanonKingCar;
											break;

										case R_CAR:
											iBlackValue+=m_iLineupBCanonCarCar;
											break;

										case R_HORSE:
											iBlackValue+=m_iLineupBCanonHorseCar;
											break;

										case R_ELEPHANT:
											iBlackValue+=m_iLineupBCanonElephantCar;
											break;

										case R_BISHOP:
											iBlackValue+=m_iLineupBCanonBishopCar;
											break;

										case R_PAWN:
											iBlackValue+=m_iLineupBCanonPawnCar;
											break;
										}

										break;
									}
								}
							}
						}
					}
					else
					{
						if(m<m_vectorCurChessPos[R_CAR][m].y==j)
						{
							if(m_vectorCurChessPos[R_CAR][m].x<i)
							{
								for(k=m_vectorCurChessPos[R_CAR][m].x+1;k<i;k++)
								{
									if(byChessBoard[k][j]!=NOCHESS)
										iCount++;
								}
							}
							else
							{
								for(k=m_vectorCurChessPos[R_CAR][m].x-1;k>i;k--)
								{
									if(byChessBoard[k][j]!=NOCHESS)
										iCount++;
								}
							}
							if(iCount==2)
							{
								if(m_vectorCurChessPos[R_CAR][m].x<i)
								{
									for(k=m_vectorCurChessPos[R_CAR][m].x+1;k<i;k++)
									{
										if(IsRed(byChessBoard[k][j]))
										{
											switch(byChessBoard[k][j])
											{
											case R_KING:
												iBlackValue+=m_iLineupBCanonKingCar;
												break;
												
											case R_CAR:
												iBlackValue+=m_iLineupBCanonCarCar;
												break;
												
											case R_HORSE:
												iBlackValue+=m_iLineupBCanonHorseCar;
												break;
												
											case R_ELEPHANT:
												iBlackValue+=m_iLineupBCanonElephantCar;
												break;
												
											case R_BISHOP:
												iBlackValue+=m_iLineupBCanonBishopCar;
												break;
												
											case R_PAWN:
												iBlackValue+=m_iLineupBCanonPawnCar;
												break;
											}

											break;
										}
									}
								}
								else
								{
									for(k=m_vectorCurChessPos[R_CAR][m].x-1;k>i;k--)
									{
										if(IsRed(byChessBoard[k][j]))
										{
											switch(byChessBoard[k][j])
											{
											case R_KING:
												iBlackValue+=m_iLineupBCanonKingCar;
												break;
												
											case R_CAR:
												iBlackValue+=m_iLineupBCanonCarCar;
												break;
												
											case R_HORSE:
												iBlackValue+=m_iLineupBCanonHorseCar;
												break;
												
											case R_ELEPHANT:
												iBlackValue+=m_iLineupBCanonElephantCar;
												break;
												
											case R_BISHOP:
												iBlackValue+=m_iLineupBCanonBishopCar;
												break;
												
											case R_PAWN:
												iBlackValue+=m_iLineupBCanonPawnCar;
												break;
											}
											
											break;
										}
									}
								}
							}
						}
					}
				}

				iCount=0;
				if(i>=7 && i<=9)//黑炮有可能与帅同行
				{
					for(k=3;k<6;k++)
						if(byChessBoard[i][k]==R_KING)
						{
							iKingPos=k;
							if(k<j)
							{
								k++;
								while(k<j)
								{
									if(byChessBoard[i][k]!=NOCHESS)
									{
										iHorsePos=k;
										iCount++;
									}

									k++;
								}
							}
							else
							{
								k--;
								while(k>j)
								{
									if(byChessBoard[i][k]!=NOCHESS)
									{
										iHorsePos=k;
										iCount++;
									}

									k--;
								}
							}

							switch(iCount)
							{
							case 0://炮和帅之间无子,可能形成将军带车之势
								iBlackValue+=m_iLineupBHCanonKing;
								break;

							case 1:
								switch(byChessBoard[i][iHorsePos])
								{
								case B_CAR:
									iBlackValue+=m_iLineupBHCanonCar;
									break;

								case B_HORSE:
									if(abs(iHorsePos-iKingPos)==2)
										iBlackValue+=m_iLineupBHHorseCanon;
									else
										iBlackValue+=m_iLineupBHScndHorseCanon;
									
									break;

								case B_CANON:
									iBlackValue+=m_iLineupBHDblCanon;
									break;
								}

								break;

							case 2:
								for(m=0;m<m_vectorCurChessPos[R_HORSE].size();m++)
								{
									if(m_vectorCurChessPos[R_HORSE][m].x==i && 
										((m_vectorCurChessPos[R_HORSE][m].y>iKingPos && m_vectorCurChessPos[R_HORSE][m].y<j) || (m_vectorCurChessPos[R_HORSE][m].y<iKingPos && m_vectorCurChessPos[R_HORSE][m].y>j)))
									{
										iBlackValue+=m_iLineupBHCanon2Horese;//炮和将之间有两子，其中有一个是对方的马
										for(m=0;m<m_vectorCurChessPos[B_CAR].size();m++)
										{
											if(m_vectorCurChessPos[B_CAR][m].x==i && 
												((m_vectorCurChessPos[B_CAR][m].y>iKingPos && m_vectorCurChessPos[B_CAR][m].y<j) || (m_vectorCurChessPos[B_CAR][m].y<iKingPos && m_vectorCurChessPos[B_CAR][m].y>j)))
											{
												iBlackValue+=m_iLineupBHCanonCar2;//炮和将之间有两子，其中有己方的车
												break;
											}
										}
										break;
									}
								}
								for(m=0;m<m_vectorCurChessPos[B_CAR].size();m++)
								{
									if(m_vectorCurChessPos[B_CAR][m].x==i && 
										((m_vectorCurChessPos[B_CAR][m].y>iKingPos && m_vectorCurChessPos[B_CAR][m].y<j) || (m_vectorCurChessPos[B_CAR][m].y<iKingPos && m_vectorCurChessPos[B_CAR][m].y>j)))
									{
										iBlackValue+=m_iLineupBHCanonCar2;//炮和将之间有两子，其中有己方的车
										break;
									}
								}
								iBlackValue+=m_iLineupBHCanon2Else;//炮和将之间有两子，其中没有对方的马
								break;
							}

							break;
						}
				}

				if(j>=3 && j<=5)//黑炮有可能与帅同列
				{
					for(k=7;k<10;k++)
						if(byChessBoard[k][j]==R_KING)
						{
							iKingPos=k;
							if(k<i)
							{
								k++;
								while(k<i)
								{
									if(byChessBoard[k][j]!=NOCHESS)
									{
										iHorsePos=k;
										iCount++;
									}

									k++;
								}
							}
							else
							{
								k--;
								while(k>i)
								{
									if(byChessBoard[k][j]!=NOCHESS)
										iCount++;

									k--;
								}
							}

							switch(iCount)
							{
							case 0:
								iBlackValue+=m_iLineupBVCanonKing;
								break;

							case 1:
								switch(byChessBoard[iHorsePos][j])
								{
								case B_CAR:
									iBlackValue+=m_iLineupBVCanonCar;
									break;

								case B_HORSE:
									if(abs(iHorsePos-iKingPos)==2)
										iBlackValue+=m_iLineupBVHorseCanon;
									else
										iBlackValue+=m_iLineupBVScndHorseCanon;
									break;

								case B_CANON:
									iBlackValue+=m_iLineupBVDblCanon;
									break;
								}

								break;

							case 2:
								//将助攻
								if(byChessBoard[9][3]==NOCHESS)
								{
									for(m=0;m<10;m++)
									{
										m_iChessBaseValue[B_CAR][m][3]=1425;
										m_iChessBaseValue[B_CAR][m][5]=1400;
									}

									if(byChessBoard[0][3]==B_KING)
									{
										for(m=0;m<m_vectorCurChessPos[B_CAR].size();m++)
										{
											if(m_vectorCurChessPos[B_CAR][m].y==3)
												iBlackValue+=m_iLineupBKingCarAttack;
										}
										iBlackValue+=m_iLineupBKingAttack;
									}
									if(byChessBoard[0][5]==B_KING)
										iBlackValue+=m_iLineupBScndKingAttack;
								}
								else
								{
									for(m=0;m<10;m++)
									{
										m_iChessBaseValue[B_CAR][m][3]=1400;
										m_iChessBaseValue[B_CAR][m][5]=1425;
									}

									if(byChessBoard[0][3]==B_KING)
										iBlackValue+=m_iLineupBScndKingAttack;
									if(byChessBoard[0][5]==B_KING)
									{
										for(m=0;m<m_vectorCurChessPos[B_CAR].size();m++)
										{
											if(m_vectorCurChessPos[B_CAR][m].y==5)
												iBlackValue+=m_iLineupBKingCarAttack;
										}
										iBlackValue+=m_iLineupBKingAttack;
									}
								}

								//炮和将之间有一卧心马，且将两边都是士
								if(byChessBoard[9][4]==R_KING && byChessBoard[8][4]==R_HORSE)
								{
									m_iLineupBCannonKingLieMidHorse=150;
									for(m=0;m<m_vectorCurChessPos[B_CAR].size();m++)
									{
										if(m_vectorCurChessPos[B_CAR][m].x>=5)
											m_iLineupBCannonKingLieMidHorse+=100;
										else
											m_iLineupBCannonKingLieMidHorse+=70;
									}
									for(m=0;m<m_vectorCurChessPos[B_PAWN].size();m++)
									{
										if(m_vectorCurChessPos[B_PAWN][m].x>=8)
											m_iLineupBCannonKingLieMidHorse+=100;
										else
										{
											if(m_vectorCurChessPos[R_PAWN][m].x>=6)
												m_iLineupBCannonKingLieMidHorse+=70;
											else
											{
												if(m_vectorCurChessPos[R_PAWN][m].x>=5)
												m_iLineupBCannonKingLieMidHorse+=30;
											}
										}
									}
									iBlackValue+=m_iLineupBCannonKingLieMidHorse;
									break;
								}

								//限制当对方中兵还在时，在己方兵前放中炮
								if(byChessBoard[6][4]==R_PAWN && m_iChessCnt[R_ELEPHANT]==2)
									m_iLineupBVCanon2Else=50;
								else
									m_iLineupBVCanon2Else=175;

								for(m=0;m<m_vectorCurChessPos[R_HORSE].size();m++)
								{
									if(m_vectorCurChessPos[R_HORSE][m].y==j && 
										((m_vectorCurChessPos[R_HORSE][m].x>iKingPos && m_vectorCurChessPos[R_HORSE][m].y<i) || (m_vectorCurChessPos[R_HORSE][m].x<iKingPos && m_vectorCurChessPos[R_HORSE][m].y>i)))
									{
										iBlackValue+=m_iLineupBVCanon2Horese;
										for(m=0;m<m_vectorCurChessPos[B_CAR].size();m++)
										{
											if(m_vectorCurChessPos[B_CAR][m].x==i && 
												((m_vectorCurChessPos[B_CAR][m].y>iKingPos && m_vectorCurChessPos[B_CAR][m].y<j) || (m_vectorCurChessPos[B_CAR][m].y<iKingPos && m_vectorCurChessPos[B_CAR][m].y>j)))
											{
												iBlackValue+=m_iLineupBVCanonCar2;//炮和将之间有两子，其中有己方的车
												break;
											}
										}
										break;
									}
								}

								for(m=0;m<m_vectorCurChessPos[B_CAR].size();m++)
								{
									if(m_vectorCurChessPos[B_CAR][m].x==i && 
										((m_vectorCurChessPos[B_CAR][m].y>iKingPos && m_vectorCurChessPos[B_CAR][m].y<j) || (m_vectorCurChessPos[B_CAR][m].y<iKingPos && m_vectorCurChessPos[B_CAR][m].y>j)))
									{
										iBlackValue+=m_iLineupBVCanonCar2;//炮和将之间有两子，其中有己方的车
										break;
									}
								}

								iBlackValue+=m_iLineupBVCanon2Else;
								break;
							}
							break;
						}
				}
				break;

			case R_PAWN://红兵
				//卧心兵
				if(!m_iChessCnt[B_BISHOP] && i==1 && j==4 && m_vectorCurChessPos[B_KING][0].x==0)
					iBlackValue+=m_iLineupBLieMidPawn;

				switch(m_iChessCnt[R_PAWN])
				{
				case 5:
					if(j-4>=0)
					{
						if(byChessBoard[i][j-1]==R_PAWN && byChessBoard[i][j-2]==R_PAWN && byChessBoard[i][j-3]==R_PAWN && byChessBoard[i][j-4]==R_PAWN)
							iRedValue+=m_iLineupB5ConnectPawn;
					}
					break;

				case 4:
					if(j-3>=0)
					{
						if(byChessBoard[i][j-1]==R_PAWN && byChessBoard[i][j-2]==R_PAWN && byChessBoard[i][j-3]==R_PAWN)
							iRedValue+=m_iLineupB4ConnectPawn;
					}
					break;

				case 3:
					if(j-2>=0)
					{
						if(byChessBoard[i][j-1]==R_PAWN && byChessBoard[i][j-2]==R_PAWN)
							iRedValue+=m_iLineupB3ConnectPawn;
					}
					break;

				case 2:
					if(j-1>=0)
					{
						if(byChessBoard[i][j-1]==R_PAWN)
							iRedValue+=m_iLineupB2ConnectPawn;
					}
					break;
				}

				break;

			case R_ELEPHANT://红相
				//连环相
				if(i-2>=5)
				{
					if(j-2>=0)
					{
						if(byChessBoard[i-2][j-2]==R_ELEPHANT)
						{
							if(byChessBoard[i-1][j-1]==NOCHESS)
								iRedValue+=m_iLineupRConnectElephant;

							break;
						}
					}
				}
				break;

			case R_BISHOP://红士
				//连环士
				if(i-1>=7)
				{
					if(j-1>=3)
					{
						if(byChessBoard[i-1][j-1]==R_BISHOP)
						{
							iRedValue+=m_iLineupRConnectBishop;
							break;
						}
					}
				}
				break;

			case R_KING:
				//将离原位
				if(i==7 || i==8 || j==3 || j==5)
					iRedValue+=m_iLineupRNoProtectKing;
				break;

			case R_CAR:
				//卧心车
				if(!m_iChessCnt[B_BISHOP] && i==1 && j==4)
					iRedValue+=m_iLineupRLieMidCar;

				//红车可能与自己子同行
				bFlag=0;
				k=j-1;
				while(k>0 && !bFlag)
				{
					switch(byChessBoard[i][k])
					{
					case R_KING:
						iRedValue+=m_iLineupRCarKing;
						bFlag=1;
						break;

					case R_HORSE:
						//车马将
						k--;
						while(k>=0)
						{
							if(byChessBoard[i][k]!=NOCHESS)
							{
								if(byChessBoard[i][k]==B_KING)
								{
									iRedValue+=m_iLineupRCarHorseKing;
									bFlag=1;
								}

								break;
							}

							k--;
						}

						if(bFlag)
							break;

						iRedValue+=m_iLineupRCarHorse;
						bFlag=1;
						break;

					case R_CANON:
						//车炮将
						iKingPos=k;
						k--;
						while(k>=0)
						{
							if(byChessBoard[i][k]!=NOCHESS)
							{
								if(byChessBoard[i][k]==B_KING)
								{
									if(byChessBoard[i][iKingPos-1]==B_BISHOP && byChessBoard[9][2]==B_ELEPHANT && i==9)
										m_iLineupRHCarCanonKing=300;
									else
									{
										if(byChessBoard[i][iKingPos-1]==B_BISHOP || (byChessBoard[9][2]==B_ELEPHANT && i==9))
											m_iLineupRHCarCanonKing=250;
										else
											m_iLineupRHCarCanonKing=200;
									}
									iRedValue+=m_iLineupRHCarCanonKing;
									bFlag=1;
								}

								break;
							}

							k--;
						}

						if(bFlag)
							break;

						iRedValue+=m_iLineupRCarCanon;
						bFlag=1;
						break;

					case R_BISHOP:
						iRedValue+=m_iLineupRCarBishop;
						bFlag=1;
						break;

					case R_ELEPHANT:
						iRedValue+=m_iLineupRCarElephant;
						bFlag=1;
						break;

					case R_PAWN:
						//车兵将
						k--;
						while(k>=0)
						{
							if(byChessBoard[i][k]!=NOCHESS)
							{
								if(byChessBoard[i][k]==B_KING)
								{
									iRedValue+=m_iLineupRCarPawnKing;
									bFlag=1;
								}

								break;
							}

							k--;
						}

						if(bFlag)
							break;

						iRedValue+=m_iLineupRCarPawn;
						bFlag=1;
						break;
					}

					k--;
				}
				bFlag=0;
				k=j+1;
				while(k<9 && !bFlag)
				{
					switch(byChessBoard[i][k])
					{
					case R_KING:
						iRedValue+=m_iLineupRCarKing;
						bFlag=1;
						break;

					case R_HORSE:
						//车马将
						k++;
						while(k<9)
						{
							if(byChessBoard[i][k]!=NOCHESS)
							{
								if(byChessBoard[i][k]==B_KING)
								{
									iRedValue+=m_iLineupRCarHorseKing;
									bFlag=1;
								}

								break;
							}

							k++;
						}

						if(bFlag)
							break;

						iRedValue+=m_iLineupRCarHorse;
						bFlag=1;
						break;

					case R_CANON:
						//车炮将
						iKingPos=k;
						k++;
						while(k<9)
						{
							if(byChessBoard[i][k]!=NOCHESS)
							{
								if(byChessBoard[i][k]==B_KING)
								{
									if(byChessBoard[i][iKingPos+1]==B_BISHOP && byChessBoard[9][6]==B_ELEPHANT && i==9)
										m_iLineupRHCarCanonKing=300;
									else
									{
										if(byChessBoard[i][iKingPos+1]==B_BISHOP || (byChessBoard[9][6]==B_ELEPHANT && i==9))
											m_iLineupRHCarCanonKing=250;
										else
											m_iLineupRHCarCanonKing=200;
									}
									iRedValue+=m_iLineupRHCarCanonKing;
									bFlag=1;
								}

								break;
							}

							k++;
						}

						if(bFlag)
							break;

						iRedValue+=m_iLineupRCarCanon;
						bFlag=1;
						break;

					case R_BISHOP:
						iRedValue+=m_iLineupRCarBishop;
						bFlag=1;
						break;

					case R_ELEPHANT:
						iRedValue+=m_iLineupRCarElephant;
						bFlag=1;
						break;

					case R_PAWN:
						//车兵将
						k++;
						while(k<9)
						{
							if(byChessBoard[i][k]!=NOCHESS)
							{
								if(byChessBoard[i][k]==B_KING)
								{
									iRedValue+=m_iLineupRCarPawnKing;
									bFlag=1;
								}

								break;
							}

							k++;
						}

						if(bFlag)
							break;

						iRedValue+=m_iLineupRCarPawn;
						bFlag=1;
						break;
					}

					k++;
				}

				//红车可能与自己子同列
				bFlag=0;
				k=i-1;
				while(k>=0 && !bFlag)
				{
					switch(byChessBoard[k][j])
					{
					case R_KING:
						iRedValue+=m_iLineupRCarKing;
						bFlag=1;
						break;

					case R_HORSE:
						//车马将
						k--;
						while(k>=0)
						{
							if(byChessBoard[k][j]!=NOCHESS)
							{
								if(byChessBoard[k][j]==B_KING)
								{
									iRedValue+=m_iLineupRCarHorseKing;
									bFlag=1;
								}

								break;
							}

							k--;
						}

						if(bFlag)
							break;

						iRedValue+=m_iLineupRCarHorse;
						bFlag=1;
						break;

					case R_CANON:
						//车炮将
						k--;
						while(k>=0)
						{
							if(byChessBoard[k][j]!=NOCHESS)
							{
								if(byChessBoard[k][j]==B_KING)
								{
									iRedValue+=m_iLineupRVCarCanonKing;
									bFlag=1;
								}

								break;
							}

							k--;
						}

						if(bFlag)
							break;

						iRedValue+=m_iLineupRCarCanon;
						bFlag=1;
						break;

					case R_BISHOP:
						iRedValue+=m_iLineupRCarBishop;
						bFlag=1;
						break;

					case R_ELEPHANT:
						iRedValue+=m_iLineupRCarElephant;
						bFlag=1;
						break;

					case R_PAWN:
						//车兵将
						k--;
						while(k>=0)
						{
							if(byChessBoard[k][j]!=NOCHESS)
							{
								if(byChessBoard[k][j]==B_KING)
								{
									iRedValue+=m_iLineupRCarPawnKing;
									bFlag=1;
								}

								break;
							}

							k--;
						}

						if(bFlag)
							break;

						iRedValue+=m_iLineupRCarPawn;
						bFlag=1;
						break;
					}

					k--;
				}
				bFlag=0;
				k=i+1;
				while(k<10 && !bFlag)
				{
					switch(byChessBoard[k][j])
					{
					case R_KING:
						iRedValue+=m_iLineupRCarKing;
						bFlag=1;
						break;

					case R_HORSE:
						//车马将
						k++;
						while(k<10)
						{
							if(byChessBoard[k][j]!=NOCHESS)
							{
								if(byChessBoard[k][j]==B_KING)
								{
									iRedValue+=m_iLineupRCarHorseKing;
									bFlag=1;
								}

								break;
							}

							k++;
						}

						if(bFlag)
							break;

						iRedValue+=m_iLineupRCarHorse;
						bFlag=1;
						break;

					case R_CANON:
						//车炮将
						k++;
						while(k<10)
						{
							if(byChessBoard[k][j]!=NOCHESS)
							{
								if(byChessBoard[k][j]==B_KING)
								{
									iRedValue+=m_iLineupRVCarCanonKing;
									bFlag=1;
								}

								break;
							}

							k++;
						}

						if(bFlag)
							break;

						iRedValue+=m_iLineupRCarCanon;
						bFlag=1;
						break;

					case R_BISHOP:
						iRedValue+=m_iLineupRCarBishop;
						bFlag=1;
						break;

					case R_ELEPHANT:
						iRedValue+=m_iLineupRCarElephant;
						bFlag=1;
						break;

					case R_PAWN:
						//车兵将
						k++;
						while(k<10)
						{
							if(byChessBoard[k][j]!=NOCHESS)
							{
								if(byChessBoard[k][j]==B_KING)
								{
									iRedValue+=m_iLineupRCarPawnKing;
									bFlag=1;
								}

								break;
							}

							k++;
						}

						if(bFlag)
							break;

						iRedValue+=m_iLineupRCarPawn;
						bFlag=1;
						break;
					}

					k++;
				}
				break;

			case R_HORSE://红马
				//卧槽马
				if((i==8 && (j==2 || j==6)) && byChessBoard[9][4]==R_KING)
					iBlackValue+=m_iLineupBLieSlotHorse;

				//大角马
				if(!m_iChessCnt[B_BISHOP])
				{
					if((i==2 && j==3) && byChessBoard[0][5]==B_KING && byChessBoard[2][4]==NOCHESS && byChessBoard[1][3]==NOCHESS)
						iRedValue+=m_iLineupRBigAngleHorse;
					if((i==2 && j==5) && byChessBoard[0][3]==B_KING && byChessBoard[2][4]==NOCHESS && byChessBoard[1][5]==NOCHESS)
						iRedValue+=m_iLineupRBigAngleHorse;
					if((i==0 && j==3) && byChessBoard[2][5]==B_KING && byChessBoard[0][4]==NOCHESS && byChessBoard[1][3]==NOCHESS)
						iRedValue+=m_iLineupRBigAngleHorse;
					if((i==0 && j==5) && byChessBoard[2][3]==B_KING && byChessBoard[0][4]==NOCHESS && byChessBoard[1][5]==NOCHESS)
						iRedValue+=m_iLineupRBigAngleHorse;
				}

				//卧心马
				if(i==8 && j==4)
				{
					iRedValue+=m_iLineupRLieMidHorse;

					for(m=0;m<10;m++)
					{
						m_iChessBaseValue[B_CAR][m][3]=1400;
						m_iChessBaseValue[B_CAR][m][5]=1400;
					}
				}

				//连环马
				if(i-2>=0)
				{
					if(j-1>=0)
					{
						if(byChessBoard[i-2][j-1]==R_HORSE)
						{
							if(byChessBoard[i-1][j-1]==NOCHESS)
							{
								if(byChessBoard[i-1][j]==NOCHESS)
									iRedValue+=m_iLineupRConnectHorse;
								else
									iRedValue+=m_iLineupRScndConnectHorse;
							}
							else
							{
								if(byChessBoard[i-1][j]==NOCHESS)
									iRedValue+=m_iLineupRScndConnectHorse;
								else
									iRedValue+=m_iLineupRSznConnectHorse;
							}

							break;
						}
					}
				}
				if(i-1>=0)
				{
					if(j-2>=0)
					{
						if(byChessBoard[i-1][j-2]==R_HORSE)
						{
							if(byChessBoard[i-1][j-1]==NOCHESS)
							{
								if(byChessBoard[i][j-1]==NOCHESS)
									iRedValue+=m_iLineupRConnectHorse;
								else
									iRedValue+=m_iLineupRScndConnectHorse;
							}
							else
							{
								if(byChessBoard[i][j-1]==NOCHESS)
									iRedValue+=m_iLineupRScndConnectHorse;
								else
									iRedValue+=m_iLineupRSznConnectHorse;
							}

							break;
						}
					}
				}
				break;

			case R_CANON://红炮
				//连环炮
				bFlag=0;
				iCount=0;
				for(k=0;k<9;k++)
				{
					if(byChessBoard[i][k]==R_CANON && k!=j)
					{
						if(k<j)
						{
							k++;
							while(k<j)
							{
								if(byChessBoard[i][k]!=NOCHESS)
									iCount++;
								
								k++;
							}

							if(iCount==1)
								iRedValue+=m_iLineupRConnectCanon;
						}
						else
						{
							k--;
							while(k>j)
							{
								if(byChessBoard[i][k]!=NOCHESS)
									iCount++;

								k--;
							}

							if(iCount==1)
								iRedValue+=m_iLineupRConnectCanon;
						}

						bFlag=1;
						break;
					}
				}
				if(!bFlag)
				{
					iCount=0;
					for(k=0;k<10;k++)
					{
						if(byChessBoard[k][j]==R_CANON && k!=i)
						{
							if(k<i)
							{
								k++;
								while(k<i)
								{
									if(byChessBoard[k][j]!=NOCHESS)
										iCount++;

									k++;
								}

								if(iCount==1)
									iRedValue+=m_iLineupRConnectCanon;
							}
							else
							{
								k--;
								while(k>i)
								{
									if(byChessBoard[k][j]!=NOCHESS)
										iCount++;

									k--;
								}

								if(iCount==1)
									iRedValue+=m_iLineupRConnectCanon;
							}

							break;
						}
					}
				}

				//炮牵制车系列
				iCount=0;
				uiSize=m_vectorCurChessPos[B_CAR].size();
				for(m=0;m<uiSize;m++)
				{
					if(m<m_vectorCurChessPos[B_CAR][m].x==i)
					{
						if(m_vectorCurChessPos[B_CAR][m].y<j)
						{
							for(k=m_vectorCurChessPos[B_CAR][m].y+1;k<j;k++)
							{
								if(byChessBoard[i][k]!=NOCHESS)
									iCount++;
							}
						}
						else
						{
							for(k=m_vectorCurChessPos[B_CAR][m].y-1;k>j;k--)
							{
								if(byChessBoard[i][k]!=NOCHESS)
									iCount++;
							}
						}
						if(iCount==2)
						{
							if(m_vectorCurChessPos[B_CAR][m].y<j)
							{
								for(k=m_vectorCurChessPos[B_CAR][m].y+1;k<j;k++)
								{
									if(IsBlack(byChessBoard[i][k]))
									{
										switch(byChessBoard[i][k])
										{
										case B_KING:
											iRedValue+=m_iLineupRCanonKingCar;
											break;

										case B_CAR:
											iRedValue+=m_iLineupRCanonCarCar;
											break;

										case B_HORSE:
											iRedValue+=m_iLineupRCanonHorseCar;
											break;

										case B_ELEPHANT:
											iRedValue+=m_iLineupRCanonElephantCar;
											break;

										case B_BISHOP:
											iRedValue+=m_iLineupRCanonBishopCar;
											break;

										case B_PAWN:
											iRedValue+=m_iLineupRCanonPawnCar;
											break;
										}

										break;
									}
								}
							}
							else
							{
								for(k=m_vectorCurChessPos[B_CAR][m].y-1;k>j;k--)
								{
									if(IsBlack(byChessBoard[i][k]))
									{
										switch(byChessBoard[i][k])
										{
										case B_KING:
											iRedValue+=m_iLineupRCanonKingCar;
											break;

										case B_CAR:
											iRedValue+=m_iLineupRCanonCarCar;
											break;

										case B_HORSE:
											iRedValue+=m_iLineupRCanonHorseCar;
											break;

										case B_ELEPHANT:
											iRedValue+=m_iLineupRCanonElephantCar;
											break;

										case B_BISHOP:
											iRedValue+=m_iLineupRCanonBishopCar;
											break;

										case B_PAWN:
											iRedValue+=m_iLineupRCanonPawnCar;
											break;
										}

										break;
									}
								}
							}
						}
					}
					else
					{
						if(m<m_vectorCurChessPos[B_CAR][m].y==j)
						{
							if(m_vectorCurChessPos[B_CAR][m].x<i)
							{
								for(k=m_vectorCurChessPos[B_CAR][m].x+1;k<i;k++)
								{
									if(byChessBoard[k][j]!=NOCHESS)
										iCount++;
								}
							}
							else
							{
								for(k=m_vectorCurChessPos[B_CAR][m].x-1;k>i;k--)
								{
									if(byChessBoard[k][j]!=NOCHESS)
										iCount++;
								}
							}
							if(iCount==2)
							{
								if(m_vectorCurChessPos[B_CAR][m].x<i)
								{
									for(k=m_vectorCurChessPos[B_CAR][m].x+1;k<i;k++)
									{
										if(IsBlack(byChessBoard[k][j]))
										{
											switch(byChessBoard[k][j])
											{
											case B_KING:
												iRedValue+=m_iLineupRCanonKingCar;
												break;
												
											case B_CAR:
												iRedValue+=m_iLineupRCanonCarCar;
												break;
												
											case B_HORSE:
												iRedValue+=m_iLineupRCanonHorseCar;
												break;
												
											case B_ELEPHANT:
												iRedValue+=m_iLineupRCanonElephantCar;
												break;
												
											case B_BISHOP:
												iRedValue+=m_iLineupRCanonBishopCar;
												break;
												
											case B_PAWN:
												iRedValue+=m_iLineupRCanonPawnCar;
												break;
											}

											break;
										}
									}
								}
								else
								{
									for(k=m_vectorCurChessPos[B_CAR][m].x-1;k>i;k--)
									{
										if(IsBlack(byChessBoard[k][j]))
										{
											switch(byChessBoard[k][j])
											{
											case B_KING:
												iRedValue+=m_iLineupRCanonKingCar;
												break;
												
											case B_CAR:
												iRedValue+=m_iLineupRCanonCarCar;
												break;
												
											case B_HORSE:
												iRedValue+=m_iLineupRCanonHorseCar;
												break;
												
											case B_ELEPHANT:
												iRedValue+=m_iLineupRCanonElephantCar;
												break;
												
											case B_BISHOP:
												iRedValue+=m_iLineupRCanonBishopCar;
												break;
												
											case B_PAWN:
												iRedValue+=m_iLineupRCanonPawnCar;
												break;
											}

											break;
										}
									}
								}
							}
						}
					}
				}

				iCount=0;
				if(i>=0 && i<=2)//红炮有可能与将同行
				{
					for(k=3;k<6;k++)
						if(byChessBoard[i][k]==B_KING)
						{
							iKingPos=k;
							if(k<j)
							{
								k++;
								while(k<j)
								{
									if(byChessBoard[i][k]!=NOCHESS)
									{
										iHorsePos=k;
										iCount++;
									}
									
									k++;
								}
							}
							else
							{
								k--;
								while(k>j)
								{
									if(byChessBoard[i][k]!=NOCHESS)
									{
										iHorsePos=k;
										iCount++;
									}

									k--;
								}
							}

							switch(iCount)
							{
							case 0://炮和帅之间无子,可能形成将军带车之势
								iRedValue+=m_iLineupRHCanonKing;
								break;

							case 1:
								switch(byChessBoard[i][iHorsePos])
								{
								case R_CAR:
									iRedValue+=m_iLineupRHCanonCar;
									break;

								case R_HORSE:
									if(abs(iHorsePos-iKingPos)==2)
										iRedValue+=m_iLineupRHHorseCanon;
									else
										iRedValue+=m_iLineupRHScndHorseCanon;
									
									break;

								case R_CANON:
									iRedValue+=m_iLineupRHDblCanon;
									break;
								}

								break;

							case 2:
								for(m=0;m<m_vectorCurChessPos[B_HORSE].size();m++)
								{
									if(m_vectorCurChessPos[B_HORSE][m].x==i && 
										((m_vectorCurChessPos[B_HORSE][m].y>iKingPos && m_vectorCurChessPos[B_HORSE][m].y<j) || (m_vectorCurChessPos[B_HORSE][m].y<iKingPos && m_vectorCurChessPos[B_HORSE][m].y>j)))
									{
										iRedValue+=m_iLineupRHCanon2Horese;
										for(m=0;m<m_vectorCurChessPos[R_CAR].size();m++)
										{
											if(m_vectorCurChessPos[R_CAR][m].x==i && 
												((m_vectorCurChessPos[R_CAR][m].y>iKingPos && m_vectorCurChessPos[R_CAR][m].y<j) || (m_vectorCurChessPos[R_CAR][m].y<iKingPos && m_vectorCurChessPos[R_CAR][m].y>j)))
											{
												iRedValue+=m_iLineupRHCanonCar2;//炮和将之间有两子，其中有己方的车
												break;
											}
										}
										break;
									}
								}
								for(m=0;m<m_vectorCurChessPos[R_CAR].size();m++)
								{
									if(m_vectorCurChessPos[R_CAR][m].x==i && 
										((m_vectorCurChessPos[R_CAR][m].y>iKingPos && m_vectorCurChessPos[R_CAR][m].y<j) || (m_vectorCurChessPos[R_CAR][m].y<iKingPos && m_vectorCurChessPos[R_CAR][m].y>j)))
									{
										iRedValue+=m_iLineupRHCanonCar2;//炮和将之间有两子，其中有己方的车
										break;
									}
								}

								iRedValue+=m_iLineupRHCanon2Else;
								break;
							}

							break;
						}
				}

				if(j>=3 && j<=5)//红炮有可能与将同列
				{
					for(k=0;k<3;k++)
					{
						if(byChessBoard[k][j]==B_KING)
						{
							iKingPos=k;
							if(k<i)
							{
								k++;
								while(k<i)
								{
									if(byChessBoard[k][j]!=NOCHESS)
									{
										iHorsePos=k;
										iCount++;
									}

									k++;
								}
							}
							else
							{
								k--;
								while(k>i)
								{
									if(byChessBoard[k][j]!=NOCHESS)
									{
										iHorsePos=k;
										iCount++;
									}

									k--;
								}
							}

							switch(iCount)
							{
							case 0://炮和帅之间无子,可能形成将军带车之势
								iRedValue+=m_iLineupRVCanonKing;
								break;

							case 1:
								switch(byChessBoard[iHorsePos][j])
								{
								case R_CAR:
									iRedValue+=m_iLineupRVCanonCar;
									break;

								case R_HORSE:
									if(abs(iHorsePos-iKingPos)==2)
										iRedValue+=m_iLineupRVHorseCanon;
									else
										iRedValue+=m_iLineupRVScndHorseCanon;
									
									break;

								case R_CANON:
									iRedValue+=m_iLineupRVDblCanon;
									break;
								}

								break;

							case 2:
								//将助攻
								if(byChessBoard[0][3]==NOCHESS)
								{
									for(m=0;m<10;m++)
									{
										m_iChessBaseValue[R_CAR][m][3]=1425;
										m_iChessBaseValue[R_CAR][m][5]=1400;
									}

									if(byChessBoard[9][3]==R_KING)
									{
										for(m=0;m<m_vectorCurChessPos[R_CAR].size();m++)
										{
											if(m_vectorCurChessPos[R_CAR][m].y==3)
												iRedValue+=m_iLineupRKingCarAttack;
										}
										iRedValue+=m_iLineupRKingAttack;
									}
									if(byChessBoard[9][5]==R_KING)
										iRedValue+=m_iLineupRScndKingAttack;
								}
								else
								{
									for(m=0;m<10;m++)
									{
										m_iChessBaseValue[R_CAR][m][3]=1400;
										m_iChessBaseValue[R_CAR][m][5]=1425;
									}

									if(byChessBoard[9][3]==R_KING)
										iRedValue+=m_iLineupRScndKingAttack;
									if(byChessBoard[9][5]==R_KING)
									{
										for(m=0;m<m_vectorCurChessPos[R_CAR].size();m++)
										{
											if(m_vectorCurChessPos[R_CAR][m].y==5)
												iRedValue+=m_iLineupRKingCarAttack;
										}
										iRedValue+=m_iLineupRKingAttack;
									}
								}

								//炮和将之间有一卧心马，且将两边都是士
								if(byChessBoard[0][4]==B_KING && byChessBoard[1][4]==B_HORSE)
								{
									m_iLineupRCannonKingLieMidHorse=150;
									for(m=0;m<m_vectorCurChessPos[R_CAR].size();m++)
									{
										if(m_vectorCurChessPos[R_CAR][m].x<=4)
											m_iLineupRCannonKingLieMidHorse+=100;
										else
											m_iLineupRCannonKingLieMidHorse+=70;
									}
									for(m=0;m<m_vectorCurChessPos[R_PAWN].size();m++)
									{
										if(m_vectorCurChessPos[R_PAWN][m].x<=1)
											m_iLineupRCannonKingLieMidHorse+=100;
										else
										{
											if(m_vectorCurChessPos[R_PAWN][m].x<=3)
												m_iLineupRCannonKingLieMidHorse+=70;
											else
											{
												if(m_vectorCurChessPos[R_PAWN][m].x<=4)
												m_iLineupRCannonKingLieMidHorse+=30;
											}
										}
									}

									iRedValue+=m_iLineupRCannonKingLieMidHorse;
									break;
								}

								//当对方中兵还在时，防止己方在己方中兵前出当头炮
								if(byChessBoard[4][4]==B_PAWN && m_iChessCnt[B_ELEPHANT]==2)
									m_iLineupRVCanon2Else=50;
								else
									m_iLineupRVCanon2Else=175;

								for(m=0;m<m_vectorCurChessPos[B_HORSE].size();m++)
								{
									if(m_vectorCurChessPos[B_HORSE][m].y==j && 
										((m_vectorCurChessPos[B_HORSE][m].x>iKingPos && m_vectorCurChessPos[B_HORSE][m].y<i) || (m_vectorCurChessPos[B_HORSE][m].x<iKingPos && m_vectorCurChessPos[B_HORSE][m].y>i)))
									{
										iRedValue+=m_iLineupRVCanon2Horese;
										for(m=0;m<m_vectorCurChessPos[R_CAR].size();m++)
										{
											if(m_vectorCurChessPos[R_CAR][m].x==i && 
												((m_vectorCurChessPos[R_CAR][m].y>iKingPos && m_vectorCurChessPos[R_CAR][m].y<j) || (m_vectorCurChessPos[R_CAR][m].y<iKingPos && m_vectorCurChessPos[R_CAR][m].y>j)))
											{
												iRedValue+=m_iLineupRVCanonCar2;//炮和将之间有两子，其中有己方的车
												break;
											}
										}
										break;
									}
								}
								for(m=0;m<m_vectorCurChessPos[R_CAR].size();m++)
								{
									if(m_vectorCurChessPos[R_CAR][m].x==i && 
										((m_vectorCurChessPos[R_CAR][m].y>iKingPos && m_vectorCurChessPos[R_CAR][m].y<j) || (m_vectorCurChessPos[R_CAR][m].y<iKingPos && m_vectorCurChessPos[R_CAR][m].y>j)))
									{
										iRedValue+=m_iLineupRVCanonCar2;//炮和将之间有两子，其中有己方的车
										break;
									}
								}

								iRedValue+=m_iLineupRVCanon2Else;//炮和将之间有两子，其中没有对方的马
								break;
							}

							break;
						}
					}
				}

				break;
				
			default:
				break;
			}
		}
	}

	if(bIsRedTurn)
		return iRedValue-iBlackValue;//如果轮到红棋走返回估值
	
	return iBlackValue-iRedValue;    //如果轮到黑棋走返回负估值
}