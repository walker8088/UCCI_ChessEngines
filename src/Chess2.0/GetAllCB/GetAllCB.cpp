// GetAllCB.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "GetAllCB.h"
#include <conio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

//--------棋子--------
#define NOCHESS    0 //没有棋子

#define B_KING	   1 //黑帅
#define B_CAR	   2 //黑车
#define B_HORSE	   3 //黑马
#define B_CANON	   4 //黑炮
#define B_BISHOP   5 //黑士
#define B_ELEPHANT 6 //黑象
#define B_PAWN     7 //黑卒

#define R_KING	   8 //红将
#define R_CAR      9 //红车
#define R_HORSE    10//红马
#define R_CANON    11//红炮
#define R_BISHOP   12//红士
#define R_ELEPHANT 13//红相
#define R_PAWN     14//红兵
//--------------------

struct CHESSINFO
{
	BYTE byChess;
	int iPos;
};

void GetAllCBCnt(int i,int j);  //得到所有合法布局的总数
int CreateAllChess(int i,int j);//产生所有合法的棋子

bool IsLegalChess(int i,int j,BYTE byChess);//在位置(i,j)放棋子byChess是否合法
bool IsHaveGen(bool bIsRed);				//是否已经放置了将帅

BYTE MakeMove(int i,int j,CHESSINFO ciChessInfo);
void UnMakeMove(int i,int j,CHESSINFO ciChessInfo,BYTE byChess);

void PrintChessBoard(BYTE byChessBoard[10][9]);//打印棋盘

CWinApp theApp;

using namespace std;

float iAllCnt=0;
BYTE byCurChess[33]=
{
	NOCHESS,
	B_CAR,B_HORSE,B_ELEPHANT,B_BISHOP,B_KING,B_BISHOP,B_ELEPHANT,B_HORSE,B_CAR,B_CANON,B_CANON,B_PAWN,B_PAWN,B_PAWN,B_PAWN,B_PAWN,
	R_CAR,R_HORSE,R_ELEPHANT,R_BISHOP,R_KING,R_BISHOP,R_ELEPHANT,R_HORSE,R_CAR,R_CANON,R_CANON,R_PAWN,R_PAWN,R_PAWN,R_PAWN,R_PAWN,
};
CHESSINFO ciAllPsbChess[10][9][32];//每一个格子对应的可以放的棋子
BYTE byCurChessBoard[10][9];//当前棋盘

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}

	int i,j;

	//初始化byCurChessBoard
	for(i=0;i<10;i++)
		for(j=0;j<9;j++)
			byCurChessBoard[i][j]=NOCHESS;

	GetAllCBCnt(0,0);
	cout<<iAllCnt<<endl;

	getch();

	return nRetCode;
}

void GetAllCBCnt(int i,int j)
{
	int k;
	int iCnt=CreateAllChess(i,j);
	BYTE byChess;

	for(k=0;k<iCnt;k++)
	{
		byChess=MakeMove(i,j,ciAllPsbChess[i][j][k]);
		if(j+1>8)
		{
			if(i+1>9)//得到一种布局,布局数加1
			{
				PrintChessBoard(byCurChessBoard);
				getch();
				iAllCnt++;
				UnMakeMove(i,j,ciAllPsbChess[i][j][k],byChess);
				return;
			}
			else
				GetAllCBCnt(i+1,0);
		}
		else
			GetAllCBCnt(i,j+1);
		UnMakeMove(i,j,ciAllPsbChess[i][j][k],byChess);
	}
}

int CreateAllChess(int i,int j)
{
	int iCnt=0;
	int k;
	bool bFlag=0;//空棋子是否已经使用

	for(k=0;k<33;k++)
	{
		if(IsLegalChess(i,j,byCurChess[k]))
		{
			if(byCurChess[k]==NOCHESS)
			{
				if(!bFlag)
				{
					ciAllPsbChess[i][j][iCnt].byChess=byCurChess[k];
					ciAllPsbChess[i][j][iCnt].iPos=k;
					iCnt++;
					bFlag=1;
				}
			}
			else
			{
				ciAllPsbChess[i][j][iCnt].byChess=byCurChess[k];
				ciAllPsbChess[i][j][iCnt].iPos=k;
				iCnt++;
			}
		}
	}

	return iCnt;
}

bool IsLegalChess(int i,int j,BYTE byChess)
{
	switch(byChess)
	{
	//黑红方车马炮以及空棋子
	case NOCHESS:
	case B_CAR:
	case B_HORSE:
	case B_CANON:
	case R_CAR:
	case R_HORSE:
	case R_CANON:
		if(i==5 && j==2)
		{
			if(IsHaveGen(0))
				return 1;
		}
		if(i==9 && j==2)
		{
			if(IsHaveGen(1))
				return 1;
		}
		return 1;

	//黑将士
	case B_KING:
	case B_BISHOP:
		if(i>=0 && i<=2 && j>=3 && j<=5)
			return 1;
		return 0;

	//黑象
	case B_ELEPHANT:
		if((i==2 && (j==0 || j==4)) || (i==6 && (j==0 || j==4)))
			return 1;
		return 0;

	//黑卒
	case B_PAWN:
		if(i>=3)
			return 1;
		return 0;

	//红帅士
	case R_KING:
	case R_BISHOP:
		if(i>=7 && i<=9 && j>=3 && j<=5)
			return 1;
		return 0;

	//红相
	case R_ELEPHANT:
		if((i==2 && (j==5 || j==9)) || (i==6 && (j==5 || j==9)))
			return 1;
		return 0;

	//红兵
	case R_PAWN:
		if(i<=6)
			return 1;
		return 0;	
	}
}

bool IsHaveGen(bool bIsRed)
{
	int i,j;

	if(bIsRed)
	{
		for(i=7;i<10;i++)
			for(j=3;j<6;j++)
				if(byCurChessBoard[i][j]==R_KING && !(i==9 && j==5))
					return 1;

		return 0;
	}

	for(i=0;i<3;i++)
			for(j=3;j<6;j++)
				if(byCurChessBoard[i][j]==B_KING && !(i==2 && j==5))
					return 1;

	return 0;
}

BYTE MakeMove(int i,int j,CHESSINFO ciChessInfo)
{
	BYTE byChess=byCurChessBoard[i][j];

	byCurChess[ciChessInfo.iPos]=NOCHESS;
	byCurChessBoard[i][j]=ciChessInfo.byChess;

	return byChess;
}

void UnMakeMove(int i,int j,CHESSINFO ciChessInfo,BYTE byChess)
{
	byCurChess[ciChessInfo.iPos]=ciChessInfo.byChess;
	byCurChessBoard[i][j]=byChess;
}

void PrintChessBoard(BYTE byChessBoard[10][9])
{
	for(int i=0;i<10;i++)
	{
		for(int j=0;j<9;j++)
			cout<<(int)byChessBoard[i][j]<<"  ";
		cout<<endl;
	}
	cout<<endl<<endl;
}