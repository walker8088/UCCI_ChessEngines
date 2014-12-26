// MoveGenerator.cpp: implementation of the CMoveGenerator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "chess.h"
#include "MoveGenerator.h"

#ifdef _DEBUG
#undef THIS_FILE
static BYTE THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMoveGenerator::CMoveGenerator()
{
	
}

CMoveGenerator::~CMoveGenerator()
{
	
}

int CMoveGenerator::AddMove(int nFromX,int nFromY,int nToX,int nToY,int nPly,int nChessID)
{
	m_MoveList[nPly][m_nMoveCount].From.x=nFromX;
	m_MoveList[nPly][m_nMoveCount].From.y=nFromY;
	m_MoveList[nPly][m_nMoveCount].To.x=nToX;
	m_MoveList[nPly][m_nMoveCount].To.y=nToY;
	m_MoveList[nPly][m_nMoveCount].nChessID=nChessID;
	m_nMoveCount++;

	return m_nMoveCount;
}

//nPly指明当前搜索的层数，每层将走法存在不同的位置，以免覆盖
//nSide指明产生哪一方的走法，TRUE为红方，FALSE是黑方
int CMoveGenerator::CreatePossibleMove(BYTE byChessBoard[][9],int nPly,int nSide,int nUserChessColor)
{
	int nChessID;
	int i,j;

	m_nMoveCount=0;
	m_nUserChessColor=nUserChessColor;

	for(j=0;j<9;j++)
		for(i=0;i<10;i++)
		{
			if(byChessBoard[i][j]!=NOCHESS)
			{
				nChessID=byChessBoard[i][j];

				if(nUserChessColor==REDCHESS)
				{
					if(!nSide && IsRed(nChessID))
						continue;//如要产生黑棋走法，跳过红棋
					
					if(nSide && IsBlack(nChessID))
						continue;//如要产生红棋走法，跳过黑棋
				}
				else
				{
					if(nSide && IsRed(nChessID))
						continue;//如要产生黑棋走法，跳过红棋
					
					if(!nSide && IsBlack(nChessID))
						continue;//如要产生红棋走法，跳过黑棋
				}

				switch(nChessID)
				{
				case R_KING://红帅
				case B_KING://黑将
					Gen_KingMove(byChessBoard,i,j,nPly);
					break;

				case R_BISHOP://红士
					Gen_RBishopMove(byChessBoard,i,j,nPly);
					break;
				
				case B_BISHOP://黑士
					Gen_BBishopMove(byChessBoard,i,j,nPly);
					break;
				
				case R_ELEPHANT://红相
				case B_ELEPHANT://黑象
					Gen_ElephantMove(byChessBoard,i,j,nPly);
					break;
				
				case R_HORSE://红马
				case B_HORSE://黑马
					Gen_HorseMove(byChessBoard,i,j,nPly);
					break;
				
				case R_CAR://红车
				case B_CAR://黑车
					Gen_CarMove(byChessBoard,i,j,nPly);
					break;

				case R_PAWN://红兵
					Gen_RPawnMove(byChessBoard,i,j,nPly);
					break;
				
				case B_PAWN://黑卒
					Gen_BPawnMove(byChessBoard,i,j,nPly);
					break;
				
				case B_CANON://黑炮
				case R_CANON://红炮
					Gen_CanonMove(byChessBoard,i,j,nPly);
					break;
				
				default:
					break;
				}
			}
		}

	return m_nMoveCount;
}

void CMoveGenerator::Gen_KingMove(BYTE byChessBoard[10][9],int i,int j,int nPly)
{
	int x,y;
	
	for(y=0;y<3;y++)
	{
		for(x=3;x<6;x++)
			if(IsValidMove(byChessBoard,j,i,x,y,m_nUserChessColor))
				AddMove(j,i,x,y,nPly,byChessBoard[i][j]);
	}

	for(y=7;y<10;y++)
	{
		for(x=3;x<6;x++)
			if(IsValidMove(byChessBoard,j,i,x,y,m_nUserChessColor))
				AddMove(j,i,x,y,nPly,byChessBoard[i][j]);
	}
}

//红士
void CMoveGenerator::Gen_RBishopMove(BYTE byChessBoard[10][9],int i,int j,int nPly)
{
	int x,y;
	
	for(y=7;y<10;y++)
	{
		for(x=3;x<6;x++)
			if(IsValidMove(byChessBoard,j,i,x,y,m_nUserChessColor))
				AddMove(j,i,x,y,nPly,byChessBoard[i][j]);
	}
}

//黑士
void CMoveGenerator::Gen_BBishopMove(BYTE byChessBoard[10][9],int i,int j,int nPly)
{
	int x,y;

	for(y=0;y<3;y++)
	{
		for(x=3;x<6;x++)
			if(IsValidMove(byChessBoard,j,i,x,y,m_nUserChessColor))
				AddMove(j,i,x,y,nPly,byChessBoard[i][j]);
	}
}

//象
void CMoveGenerator::Gen_ElephantMove(BYTE byChessBoard[10][9],int i,int j,int nPly)
{
	int x,y;
	
	//插入右下方的有效走法
	x=j+2;
	y=i+2;
	if(x<9 && y<10 && IsValidMove(byChessBoard,j,i,x,y,m_nUserChessColor))
		AddMove(j,i,x,y,nPly,byChessBoard[i][j]);
	
	//插入右上方的有效走法
	x=j+2;
	y=i-2;
	if(x<9 && y>=0 && IsValidMove(byChessBoard,j,i,x,y,m_nUserChessColor))
		AddMove(j,i,x,y,nPly,byChessBoard[i][j]);
	
	//插入左下方的有效走法
	x=j-2;
	y=i+2;
	if(x>=0 && y<10 && IsValidMove(byChessBoard,j,i,x,y,m_nUserChessColor))
		AddMove(j,i,x,y,nPly,byChessBoard[i][j]);
	
	//插入左上方的有效走法
	x=j-2;
	y=i-2;
	if(x>=0 && y>=0 && IsValidMove(byChessBoard,j,i,x,y,m_nUserChessColor))
		AddMove(j,i,x,y,nPly,byChessBoard[i][j]);
}

//马
void CMoveGenerator::Gen_HorseMove(BYTE byChessBoard[10][9], int i, int j, int nPly)
{
	int x, y;
	
	//插入右下方的有效走法
	x=j+2;//右2
	y=i+1;//下1
	if((x<9 && y<10) && IsValidMove(byChessBoard,j,i,x,y,m_nUserChessColor))
		AddMove(j,i,x,y,nPly,byChessBoard[i][j]);
	
	//插入右上方的有效走法
	x=j+2;//右2
	y=i-1;//上1
	if((x<9 && y>=0) && IsValidMove(byChessBoard,j,i,x,y,m_nUserChessColor))
		AddMove(j,i,x,y,nPly,byChessBoard[i][j]);
	
	//插入左下方的有效走法
	x=j-2;//左2
	y=i+1;//下1
	if((x>=0 && y<10) && IsValidMove(byChessBoard,j,i,x,y,m_nUserChessColor))
		AddMove(j,i,x,y,nPly,byChessBoard[i][j]);

	//插入左上方的有效走法
	x=j-2;//左2
	y=i-1;//上1
	if((x>=0 && y>=0) && IsValidMove(byChessBoard,j,i,x,y,m_nUserChessColor))
		AddMove(j,i,x,y,nPly,byChessBoard[i][j]);
	
	//插入右下方的有效走法
	x=j+1;//右1
	y=i+2;//下2 
	if((x<9 && y<10) && IsValidMove(byChessBoard,j,i,x,y,m_nUserChessColor))
		AddMove(j,i,x,y,nPly,byChessBoard[i][j]);

	//插入左下方的有效走法
	x=j-1;//左1
	y=i+2;//下2
	if((x>=0 && y<10) && IsValidMove(byChessBoard,j,i,x,y,m_nUserChessColor))
		AddMove(j,i,x,y,nPly,byChessBoard[i][j]);    
	
	//插入右上方的有效走法
	x=j+1;//右1
	y=i-2;//上2
	if((x<9 && y >=0) && IsValidMove(byChessBoard,j,i,x,y,m_nUserChessColor))
		AddMove(j,i,x,y,nPly,byChessBoard[i][j]);

	//插入左上方的有效走法
	x=j-1;//左1
	y=i-2;//上2
	if((x>=0 && y>=0) && IsValidMove(byChessBoard,j,i,x,y,m_nUserChessColor))
		AddMove(j,i,x,y,nPly,byChessBoard[i][j]);
}

//红兵
void CMoveGenerator::Gen_RPawnMove(BYTE byChessBoard[10][9], int i, int j, int nPly)
{
	int x,y;
	int nChessID;
	
	nChessID=byChessBoard[i][j];

	if(m_nUserChessColor==REDCHESS)
	{
		y=i-1;//向前
		x=j;
		if(y>0 && !IsSameSide(nChessID,byChessBoard[y][x]))
			AddMove(j,i,x,y,nPly,byChessBoard[i][j]);//前方无阻碍
		
		if(i<5)//是否已过河
		{
			y=i;
			
			x=j+1;//右边
			if(x<9 && !IsSameSide(nChessID,byChessBoard[y][x]))
				AddMove(j,i,x,y,nPly,byChessBoard[i][j]);
			
			x=j-1;//左边
			if(x>=0 && !IsSameSide(nChessID,byChessBoard[y][x]))
				AddMove(j,i,x,y,nPly,byChessBoard[i][j]);
		}
	}
	else
	{
		y=i+1;//向前
		x=j;
		if(y>0 && !IsSameSide(nChessID,byChessBoard[y][x]))
			AddMove(j,i,x,y,nPly,byChessBoard[i][j]);//前方无阻碍
		
		if(i>4)//是否已过河
		{
			y=i;
			
			x=j+1;//右边
			if(x<9 && !IsSameSide(nChessID,byChessBoard[y][x]))
				AddMove(j,i,x,y,nPly,byChessBoard[i][j]);
			
			x=j-1;//左边
			if(x>=0 && !IsSameSide(nChessID,byChessBoard[y][x]))
				AddMove(j,i,x,y,nPly,byChessBoard[i][j]);
		}
	}
}

//黑卒
void CMoveGenerator::Gen_BPawnMove(BYTE byChessBoard[][9],int i,int j,int nPly)
{
	int x,y;
	int nChessID;
	
	nChessID=byChessBoard[i][j];

	if(m_nUserChessColor==REDCHESS)
	{
		y=i+1;//向前
		x=j;
		if(y<10 && !IsSameSide(nChessID,byChessBoard[y][x]))
			AddMove(j,i,x,y,nPly,byChessBoard[i][j]);//前方无阻碍
		
		if(i>4)//是否已过河
		{
			y=i;
			
			x=j+1;//右边
			if(x<9 && !IsSameSide(nChessID,byChessBoard[y][x]))
				AddMove(j,i,x,y,nPly,byChessBoard[i][j]);
			
			x=j-1;//左边
			if(x>=0 && !IsSameSide(nChessID,byChessBoard[y][x]))
				AddMove(j,i,x,y,nPly,byChessBoard[i][j]);
		}
	}
	else
	{
		y=i-1;//向前
		x=j;
		if(y<10 && !IsSameSide(nChessID,byChessBoard[y][x]))
			AddMove(j,i,x,y,nPly,byChessBoard[i][j]);//前方无阻碍
		
		if(i<5)//是否已过河
		{
			y=i;
			
			x=j+1;//右边
			if(x<9 && !IsSameSide(nChessID,byChessBoard[y][x]))
				AddMove(j,i,x,y,nPly,byChessBoard[i][j]);
			
			x=j-1;//左边
			if(x>=0 && !IsSameSide(nChessID,byChessBoard[y][x]))
				AddMove(j,i,x,y,nPly,byChessBoard[i][j]);
		}
	}
}

//车
void CMoveGenerator::Gen_CarMove(BYTE byChessBoard[10][9], int i, int j, int nPly)
{
	int x,y;
	int nChessID;
	
	nChessID=byChessBoard[i][j];

	//插入向右的有效的走法
	x=j+1;
	y=i;
	while(x<9)
	{
		if(NOCHESS==byChessBoard[y][x])
			AddMove(j,i,x,y,nPly,byChessBoard[i][j]);
		else
		{
			if(!IsSameSide(nChessID,byChessBoard[y][x]))
				AddMove(j,i,x,y,nPly,byChessBoard[i][j]);

			break;
		}
		x++;
	}

	//插入向左的有效的走法
	x=j-1;
	y=i;
	while(x>=0)
	{
		if(NOCHESS==byChessBoard[y][x])
			AddMove(j,i,x,y,nPly,byChessBoard[i][j]);
		else
		{
			if(!IsSameSide(nChessID,byChessBoard[y][x]))
				AddMove(j,i,x,y,nPly,byChessBoard[i][j]);

			break;
		}
		x--;
	}

	//插入向下的有效的走法
	x=j;
	y=i+1;
	while(y<10)
	{
		if(NOCHESS==byChessBoard[y][x])
			AddMove(j,i,x,y,nPly,byChessBoard[i][j]);
		else
		{
			if(!IsSameSide(nChessID,byChessBoard[y][x]))
				AddMove(j,i,x,y,nPly,byChessBoard[i][j]);

			break;
		}
		y++;
	}

	//插入向上的有效的走法
	x=j;
	y=i-1;
	while(y>=0)
	{
		if(NOCHESS==byChessBoard[y][x])
			AddMove(j,i,x,y,nPly,byChessBoard[i][j]);
		else
		{
			if(!IsSameSide(nChessID,byChessBoard[y][x]))
				AddMove(j,i,x,y,nPly,byChessBoard[i][j]);

			break;
		}
		y--;
	}
}

//炮
void CMoveGenerator::Gen_CanonMove(BYTE byChessBoard[10][9], int i, int j, int nPly)
{
	int x,y;
	BOOL flag;
	int nChessID;
	
	nChessID=byChessBoard[i][j];

	//插入向右的有效的走法
	x=j+1;
	y=i;
	flag=FALSE;
	while(x<9)
	{
		if(NOCHESS==byChessBoard[y][x])
		{
			if(!flag)//隔有棋子
				AddMove(j,i,x,y,nPly,byChessBoard[i][j]);
		}
		else
		{
			if(!flag)//没有隔棋子，此棋子是第一个障碍，设置标志
				flag=TRUE;
			else     //隔有棋子，此处如为敌方棋子，则可走
			{
				if(!IsSameSide(nChessID,byChessBoard[y][x]))
					AddMove(j,i,x,y,nPly,byChessBoard[i][j]);
				break;
			}
		}
		x++;//继续下一个位置
	}

	//插入向左的有效的走法
	x=j-1;
	y=i;
	flag=FALSE;
	while(x>=0)
	{
		if(NOCHESS==byChessBoard[y][x])
		{
			if(!flag)//隔有棋子
				AddMove(j,i,x,y,nPly,byChessBoard[i][j]);
		}
		else
		{
			if(!flag)//没有隔棋子，此棋子是第一个障碍，设置标志
				flag=TRUE;
			else     //隔有棋子，此处如为敌方棋子，则可走
			{
				if(!IsSameSide(nChessID,byChessBoard[y][x]))
					AddMove(j,i,x,y,nPly,byChessBoard[i][j]);
				break;
			}
		}
		x--;//继续下一个位置
	}

	//插入向下的有效的走法
	x=j;
	y=i+1;
	flag=FALSE;
	while(y<10)
	{
		if(NOCHESS==byChessBoard[y][x])
		{
			if(!flag)//隔有棋子
				AddMove(j,i,x,y,nPly,byChessBoard[i][j]);
		}
		else
		{
			if(!flag)//没有隔棋子，此棋子是第一个障碍，设置标志
				flag=TRUE;
			else     //隔有棋子，此处如为敌方棋子，则可走
			{
				if(!IsSameSide(nChessID,byChessBoard[y][x]))
					AddMove(j,i,x,y,nPly,byChessBoard[i][j]);
				break;
			}
		}
		y++;//继续下一个位置
	}

	//插入向上的有效的走法
	x=j;
	y=i-1;
	flag=FALSE;
	while(y>=0)
	{
		if(NOCHESS==byChessBoard[y][x])
		{
			if(!flag)//隔有棋子
				AddMove(j,i,x,y,nPly,byChessBoard[i][j]);
		}
		else
		{
			if(!flag)//没有隔棋子，此棋子是第一个障碍，设置标志
				flag=TRUE;
			else     //隔有棋子，此处如为敌方棋子，则可走
			{
				if(!IsSameSide(nChessID,byChessBoard[y][x]))
					AddMove(j,i,x,y,nPly,byChessBoard[i][j]);
				break;
			}
		}
		y--;//继续下一个位置
	}
}

BOOL CMoveGenerator::IsValidMove(BYTE byChessBoard[10][9], int nFromX, int nFromY, int nToX, int nToY,int nUserChessColor)
{
	int i,j;
	int nMoveChessID,nTargetID;

	if(nFromX==nToX && nFromY==nToY)
		return false;//目的与源相同，非法

	nMoveChessID=byChessBoard[nFromY][nFromX];
	nTargetID=byChessBoard[nToY][nToX];

	if(IsSameSide(nMoveChessID,nTargetID))
		return false;

	switch(nMoveChessID)
	{
	case B_KING://黑将
		if(nUserChessColor==REDCHESS)
		{
			if(nTargetID==R_KING)//判断是否将帅见面
			{
				if(nFromX!=nToX)//横坐标不相等
					return false;//将帅不在同一列
				
				for(i=nFromY+1;i<nToY;i++)
					if(byChessBoard[i][nFromX]!=NOCHESS)
						return false;//中间隔有棋子
			}
			else
			{
				if(nToY>2 || nToX>5 || nToX<3)
					return false;//目标点在九宫之外
				
				if(abs(nFromY-nToY)+abs(nFromX-nToX)>1)
					return false;//将帅只走一步直线
			}
		}
		else
		{
			if(nTargetID==R_KING)//判断是否将帅见面
			{
				if(nFromX!=nToX)//横坐标不相等
					return false;//将帅不在同一列
				
				for(i=nFromY-1;i>nToY;i--)
					if(byChessBoard[i][nFromX]!=NOCHESS)
						return false;//中间隔有棋子
			}
			else
			{
				if(nToY<7 || nToX>5 || nToX<3)
					return false;//目标点在九宫之外
				
				if(abs(nFromY-nToY)+abs(nFromX-nToX)>1)
					return false;//将帅只走一步直线
			}
		}

		break;

	case R_KING://红帅
		if(nUserChessColor==REDCHESS)
		{
			if(nTargetID==B_KING)//判断是否将帅见面
			{
				if(nFromX!=nToX)//横坐标不相等
					return false;//将帅不在同一列
				
				for(i=nFromY-1;i>nToY;i--)
					if(byChessBoard[i][nFromX]!=NOCHESS)
						return false;//中间隔有棋子
			}
			else
			{
				if(nToY<7 || nToX>5 || nToX<3)
					return false;//目标点在九宫之外
				
				if(abs(nFromY-nToY)+abs(nFromX-nToX)>1)
					return false;//将帅只走一步直线
			}
		}
		else
		{
			if(nTargetID==B_KING)//判断是否将帅见面
			{
				if(nFromX!=nToX)//横坐标不相等
					return false;//将帅不在同一列
				
				for(i=nFromY+1;i<nToY;i++)
					if(byChessBoard[i][nFromX]!=NOCHESS)
						return false;//中间隔有棋子
			}
			else
			{
				if(nToY>2 || nToX>5 || nToX<3)
					return false;//目标点在九宫之外
				
				if(abs(nFromY-nToY)+abs(nFromX-nToX)>1)
					return false;//将帅只走一步直线
			}
		}

		break;

	case R_BISHOP://红士
		if(nUserChessColor==REDCHESS)
		{
			if(nToY<7 || nToX>5 || nToX<3)
				return false;//士出九宫
		}
		else
		{
			if(nToY>2 || nToX>5 || nToX<3)
				return false;//士出九宫
		}

		if(abs(nFromX-nToX)!=1 || abs(nFromY-nToY)!=1)
				return false;//士走斜线

		break;

	case B_BISHOP://黑士
		if(nUserChessColor==REDCHESS)
		{
			if(nToY>2 || nToX>5 || nToX<3)
				return false;//士出九宫
		}
		else
		{
			if(nToY<7 || nToX>5 || nToX<3)
			return false;//士出九宫
		}

		if(abs(nFromX-nToX)!=1 || abs(nFromY-nToY)!=1)
			return false;//士走斜线

		break;

	case R_ELEPHANT://红相
		if(nUserChessColor==REDCHESS)
		{
			if(nToY<5)
				return false;//相不能过河
		}
		else
		{
			if(nToY>4)
				return false;//相不能过河
		}

		if(abs(nFromX-nToX)!=2 || abs(nFromY-nToY)!=2)
			return false;//相走田字

		if(byChessBoard[(nFromY +nToY)/2][(nFromX +nToX)/2]!=NOCHESS)
			return FALSE;//相眼被塞

		break;

	case B_ELEPHANT://黑象
		if(nUserChessColor==REDCHESS)
		{
			if(nToY>4)
				return false;//象不能过河
		}
		else
		{
			if(nToY<5)
				return false;//象不能过河
		}

		if(abs(nFromX-nToX)!=2 || abs(nFromY-nToY)!=2)
			return false;//象走田字

		if(byChessBoard[(nFromY +nToY)/2][(nFromX +nToX)/2]!=NOCHESS)
			return FALSE;//象眼被塞

		break;

	case B_PAWN://黑卒
		if(nUserChessColor==REDCHESS)
		{
			if(nToY<nFromY)
				return false;//卒不能回头
			
			if(nFromY<5 && nFromY==nToY)
				return FALSE;//卒过河前只能直走
		}
		else
		{
			if(nToY>nFromY)
				return false;//卒不能回头
			
			if(nFromY>4 && nFromY==nToY)
				return FALSE;//卒过河前只能直走
		}

		if(nToY-nFromY+abs(nToX-nFromX)>1)
			return FALSE;//卒只走一步直线

		break;

	case R_PAWN://红兵
		if(nUserChessColor==REDCHESS)
		{
			if(nToY>nFromY)
				return false;//兵不能回头
			
			if(nFromY>4 && nFromY==nToY)
				return FALSE;//兵过河前只能直走
		}
		else
		{
			if(nToY<nFromY)
				return false;//兵不能回头
			
			if(nFromY<5 && nFromY==nToY)
				return FALSE;//兵过河前只能直走
		}

		if(nFromY-nToY+abs(nToX-nFromX)>1)
			return FALSE;//兵只走一步直线

		break;

	case B_CAR://黑车
	case R_CAR://红车
		if(nFromY!=nToY && nFromX!=nToX)
			return FALSE;//车走直线

		if(nFromY==nToY)
		{
			if(nFromX<nToX)
			{
				for(i=nFromX+1;i<nToX;i++)
					if(byChessBoard[nFromY][i]!=NOCHESS)
						return FALSE;
			}
			else
			{
				for(i=nToX+1;i<nFromX;i++)
					if(byChessBoard[nFromY][i]!=NOCHESS)
						return FALSE;
			}
		}
		else
		{
			if(nFromY<nToY)
			{
				for(j=nFromY+1;j<nToY;j++)
					if(byChessBoard[j][nFromX]!=NOCHESS)
						return FALSE;
			}
			else
			{
				for(j=nToY+1;j<nFromY;j++)
					if(byChessBoard[j][nFromX]!=NOCHESS)
						return FALSE;
			}
		}
		
		break;

	case B_HORSE://黑马
	case R_HORSE://红马
		if(!((abs(nToX-nFromX)==1 && abs(nToY -nFromY)==2) || (abs(nToX-nFromX)==2&&abs(nToY -nFromY)==1)))
			return FALSE;//马走日字

		if(nToX-nFromX==2)
		{
			i=nFromX+1;
			j=nFromY;
		}
		else
			if(nFromX-nToX==2)
			{
				i=nFromX-1;
				j=nFromY;
			}
			else
				if(nToY-nFromY==2)
				{
					i=nFromX;
					j=nFromY+1;
				}
				else
					if(nFromY-nToY==2)
					{
						i=nFromX;
						j=nFromY-1;
					}

		if(byChessBoard[j][i]!=NOCHESS)
			return FALSE;//绊马腿
		
		break;

	case B_CANON://黑炮
	case R_CANON://红炮
		if(nFromY!=nToY && nFromX!=nToX)
			return FALSE;//炮走直线

		//炮吃子时经过的路线中不能有棋子
		if(byChessBoard[nToY][nToX]==NOCHESS)
		{
			if(nFromY==nToY)
			{
				if(nFromX<nToX)
				{
					for(i=nFromX+1;i<nToX;i++)
						if(byChessBoard[nFromY][i]!=NOCHESS)
							return FALSE;
				}
				else
				{
					for(i=nToX+1;i<nFromX;i++)
						if(byChessBoard[nFromY][i]!=NOCHESS)
							return FALSE;
				}
			}
			else
			{
				if(nFromY<nToY)
				{
					for(j=nFromY+1;j<nToY;j++)
						if(byChessBoard[j][nFromX]!=NOCHESS)
							return FALSE;
				}
				else
				{
					for(j=nToY+1;j<nFromY;j++)
						if(byChessBoard[j][nFromX]!=NOCHESS)
							return FALSE;
				}
			}
		}		
		else//炮吃子时
		{
			int j=0;
			if(nFromY==nToY)
			{
				if(nFromX<nToX)
				{
					for(i=nFromX+1;i<nToX;i++)
						if(byChessBoard[nFromY][i]!=NOCHESS)
							j++;
						if(j!=1)
							return FALSE;
				}
				else
				{
					for(i=nToX+1;i<nFromX;i++)
						if(byChessBoard[nFromY][i]!=NOCHESS)
							j++;
						if(j!=1)
							return FALSE;
				}
			}
			else
			{
				if(nFromY<nToY)
				{
					for(i=nFromY+1;i<nToY;i++)
						if(byChessBoard[i][nFromX]!=NOCHESS)
							j++;
					if(j!=1)
						return FALSE;
				}
				else
				{
					for(i=nToY+1;i<nFromY;i++)
						if(byChessBoard[i][nFromX]!=NOCHESS)
							j++;
					if(j!=1)
						return FALSE;
				}
			}
		}
		
		break;

	default:
		return false;
	}
		
	return true;
}