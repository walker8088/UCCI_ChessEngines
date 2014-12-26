//define.h
#ifndef DEFINE_H_
#define DEFINE_H_

#pragma warning(disable:4786)//禁止waring号为4786的warning消息

#include <stack>
#include <vector>

//象棋规则类型定义
#define CR_LONGGEN 1//长将或长捉
#define CR_SENDGEN 2//送将
#define CR_FAIL    3//已败
#define CR_OBEY    4//遵守

#define BLACKCHESS 1//黑方
#define REDCHESS   2//红方

//棋局不同阶段定义
#define CA_START 1//开局
#define CA_MID   2//中局
#define CA_END   3//残局

//深度方式DepthSort
#define DS_DEFAULTSET 1
#define DS_USERDEFINE 2

#define CS_PCCHESS   1//人机对弈
#define CS_PPCHESS	 2//人人对弈
#define CS_CCCHESS   3//机机对弈
#define CS_HASHCHESS 4//混杂对弈

#define MAX_SEARCHDEPTH 50//最大搜索深度

//--------棋子--------
#define NOCHESS    0 //没有棋子

#define B_KING	   1 //黑帅
#define B_CAR	   2 //黑车
#define B_HORSE	   3 //黑马
#define B_CANON	   4 //黑炮
#define B_BISHOP   5 //黑士
#define B_ELEPHANT 6 //黑象
#define B_PAWN     7 //黑卒
#define B_BEGIN    B_KING
#define B_END      B_PAWN

#define R_KING	   8 //红将
#define R_CAR      9 //红车
#define R_HORSE    10//红马
#define R_CANON    11//红炮
#define R_BISHOP   12//红士
#define R_ELEPHANT 13//红相
#define R_PAWN     14//红兵
#define R_BEGIN    R_KING
#define R_END      R_PAWN
//--------------------

#define IsBlack(x) (x>=B_BEGIN && x<=B_END)//判断某个棋子是不是黑色
#define IsRed(x)   (x>=R_BEGIN && x<=R_END)//判断某个棋子是不是红色

//判断两个棋子是不是同色
#define IsSameSide(x,y) ((IsBlack(x) && IsBlack(y)) || (IsRed(x) && IsRed(y)))

#pragma pack(push,1)

//棋子位置
struct CHESSMANPOS
{
	BYTE x;
	BYTE y;
};

//棋子走法
struct CHESSMOVE
{
	short nChessID;  //表明是什么棋子
	CHESSMANPOS From;//起始位置
	CHESSMANPOS To;  //走到什么位置
	int iScore;      //走法的分数
};

//悔棋时需要的数据结构
struct UNDOMOVE
{
	CHESSMOVE cmChessMove;
	short nChessID;//被吃掉的棋子
};

//开局库树节点定义
struct OPENBOOKTREENODE
{
	int iDepth;			  //该节点所在树中的深度
	int iPassCnt;         //该节点的使用次数
	float fScore;		  //该节点的得分
	CHESSMOVE cmChessMove;//该节点所含内容,即走法
	std::vector<OPENBOOKTREENODE*> vectorChildNode;//孩子节点
	OPENBOOKTREENODE *pObnParent;//父节点
};

//棋盘和走法,判断长将长捉时用到
struct CBANDMOVE
{
	BYTE byChessBoard[10][9];
	CHESSMOVE cm;
};

#pragma pack(push,1)

#endif
