
/* ============================================================================================ *\
 *	Author		:	ahei																		*
 *	Email		:	ahei0802@126.com															*
 *	Msn			:	ahei080210114@hotmail.com													*
 *	QQ			:	8261525																		*
 *	Version		:	0.2 																		*
 *	Created		:	2005/03/01 9:10:13															*
 *	Modified	:	2006/01/01 17:15:46															*
 *	FileName	: 	SearchEngine.h																*
 *	Description	:	This file is using for declaring the class CSearchEngine,and this class is	*
 *					using for generating moves,evaluating and searching.						*
 * 	Warning     : 	Copyright (c) ahei 2004 All rights reserved.  								*
\* ============================================================================================ */

#ifndef _SEARCHENGINE_H_
#define _SEARCHENGINE_H_

#include <fstream>
#include <stack>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

//********棋子类型编号********
#define NOCHESS		0	//无棋子

#define BKING		1	//黑将
#define BADVISOR	2	//黑士
#define BBISHOP		3	//黑象
#define BROOK		4	//黑车
#define BKNIGHT		5	//黑马
#define BCANNON		6	//黑炮
#define BPAWN		7	//黑卒
#define BBEGIN		1
#define BEND		7

#define RKING		8	//红帅
#define RADVISOR	9	//红士
#define RBISHOP		10	//红相
#define RROOK		11	//红车
#define RKNIGHT		12	//红马
#define RCANNON		13	//红炮
#define RPAWN		14	//红兵
#define RBEGIN		8
#define REND		14
//****************************

// [2005/10/20 22:55:11]
//******总的棋子类型编号******
#define KING		1	//将
#define ADVISOR		2	//士
#define BISHOP		3	//象
#define ROOK		4	//车
#define KNIGHT		5	//马
#define CANNON		6	//炮
#define PAWN		7	//兵
//****************************

#define BLACK	0	//黑方
#define RED		1	//红方

//走法性质
#define NORMAL	1	//一般
#define ESCAPE	2	//逃
#define CAP		4	//捉
#define EAT		8	//吃子
#define CHECK	16	//将军

#define ENGINEVERSION	"0.2"
#define ENGINENAME		"ZWM"
#define ENGINEAUTHOR	"陶善文"

#define BOOKFILE	"Book.dat"

//浅红协议版本
#define QIANHONGPVER	"QHPLUGIN V1.3"

#define QIANHONGLEVELCNT	9	//浅红级别数
#define MINQHLEVEL	1
#define MAXQHLEVEL	QIANHONGLEVELCNT

#define INVALIDCHECKSUM		-1	//无效校验和

#define	MAXSEARCHDEPTH				128		//搜索的最大深度 [2005/11/2 13:15:19]
#define MAXMOVECNTCAPAICITY			128		//所能容纳的最多走法数
#define MAXNOCAPMOVECNTCAPAICITY	128		//所能容纳的最多吃子走法数
#define MAXCAPMOVECNTCAPAICITY		128		//所能容纳的最多吃子走法数

#define MINVALIDCHESSID	0	//最小的棋子类型编号
#define MAXVALIDCHESSID	14	//最大的有效棋子类型编号

#define MINVALIDPIECEID		0						//最小的棋子编号
#define MAXVALIDPIECEID		31						//最大的有效棋子编号
#define MAXPIECEID			(MAXVALIDPIECEID + 1)	//最大的棋子编号

#define MAXXSCALENUM		9	//X轴最大编号
#define MAXYSCALENUM		10	//Y轴最大编号
#define MAXCHESSGRIDNUM		90	//最大的棋格编号

#define ERRCHESSID		-1					//错误的棋子类型编号
#define ERRPIECEID		-1					//错误的棋子编号
#define INVALIDPIECEID	MAXPIECEID			//无效棋子编号
#define INVALIDPIECEPOS	MAXCHESSGRIDNUM		//无效的棋子位置

//校验和最大值
//车
#define ROOKFILECHECKSUMMAXCNT	25	//车行校验和总数最大值,RookFileCheckSumMaxCnt
#define ROOKRANKCHECKSUMMAXCNT	30	//车列校验和总数最大值,RookRankCheckSumMaxCnt
#define ROOKFRCHECKSUMMAXCNT	ROOKRANKCHECKSUMMAXCNT

//炮
#define CANNONFILECHECKSUMMAXCNT	121	//炮行校验和总数最大值,CannonFileCheckSumMaxCnt
#define CANNONRANKCHECKSUMMAXCNT	176	//炮列校验和总数最大值,CannonRankCheckSumMaxCnt
#define CANNONFRCHECKSUMMAXCNT		CANNONRANKCHECKSUMMAXCNT

#define MAXBUFFERLEN	1024	//缓冲区最大长度
#define MAXMOVESTRLEN	8		//走法字符串最大长度

#define MAXMOVESCNT_NOEAT	120 //双方没有吃子的走棋步数(半回合数)
								//通常该值达到120就要判和(六十回合自然限着)

#define INVALIDPLY -1	//无效层

#define MINHASHTABSIZE		(1 << 19)	//hash表最大大小
#define MAXHASHTABSIZE		(1 << 21)	//hash表最小大小

// 不要改变,如若改变,也要改变杀手走法表中的杀手走法数的更新方法[2005/10/24 14:54:58]
#define MAXKILLERMOVECNT	2	//杀手走法最多的个数

//走法最多个数 [2005/10/20 12:46:33]
#define MAXALLMOVESCNT				119	//所有走法

#define MAXMOVECNT					17	//棋子走法最大数

#define MAXROOKMOVECNT				17	//车
#define MAXROOKFILEMOVECNT			8	//车行
#define MAXROOKRANKMOVECNT			9	//车列

#define MAXCANNONMOVECNT			17	//炮
#define MAXCANNONFILEMOVECNT		8	//炮行
#define MAXCANNONRANKMOVECNT		9	//炮列

#define MAXKNIGHTMOVECNT			8	//马
#define MAXKINGMOVECNT				4	//将
#define MAXADVISORMOVECNT			4	//士
#define MAXBISHOPMOVECNT			4	//象
#define MAXDOWNRIVERPAWNMOVECNT		1	//未过河兵
#define MAXUPRIVERPAWNMOVECNT		3	//过河兵

// 单个棋子最多的走法数[2005/10/21 22:24:44]
#define MAXONECHESSMOVECNT			MAXROOKMOVECNT

//不吃子走法最多个数 [2005/10/20 12:46:40]
#define MAXNOCAPMOVECNT				119	//所有不吃子走法

#define MAXROOKNOCAPMOVECNT			17	//车
#define MAXROOKFILENOCAPMOVECNT		8	//车行
#define MAXROOKRANKNOCAPMOVECNT		9	//车列

#define MAXCANNONNOCAPMOVECNT		17	//炮
#define MAXCANNONFILENOCAPMOVECNT	8	//炮行
#define MAXCANNONRANKNOCAPMOVECNT	9	//炮列

#define MAXKNIGHTNOCAPMOVECNT			8	//马
#define MAXKINGNOCAPMOVECNT				4	//将
#define MAXADVISORNOCAPMOVECNT			4	//士
#define MAXBISHOPNOCAPMOVECNT			4	//象
#define MAXDOWNRIVERPAWNNOCAPMOVECNT	1	//未过河兵
#define MAXUPRIVERPAWNNOCAPMOVECNT		3	//过河兵
#define MAXPAWNNOCAPMOVECNT			MAXUPRIVERPAWNNOCAPMOVECNT

//吃子走法最多个数 [2005/10/20 12:46:40]
#define MAXCAPMOVECNT				67	//所有吃子走法

#define MAXROOKCAPMOVECNT			4	//车
#define MAXROOKFILECAPMOVECNT		2	//车行
#define MAXROOKRANKCAPMOVECNT		2	//车列

#define MAXCANNONCAPMOVECNT			4	//炮
#define MAXCANNONFILECAPMOVECNT		2	//炮行
#define MAXCANNONRANKCAPMOVECNT		2	//炮列

#define MAXKNIGHTCAPMOVECNT			8	//马
#define MAXKINGCAPMOVECNT			4	//将
#define MAXADVISORCAPMOVECNT		4	//士
#define MAXBISHOPCAPMOVECNT			4	//象
#define MAXDOWNRIVERPAWNCAPMOVECNT	1	//未过河兵
#define MAXUPRIVERPAWNCAPMOVECNT	3	//过河兵
#define MAXPAWNCAPMOVECNT			MAXUPRIVERPAWNCAPMOVECNT

//马腿,象眼最多个数
#define MAXKNIGHTLEGCNT				4	//马腿
#define MAXBISHOPEYECNT				4	//象眼

//棋子基本价值BV:BasicValue [2005/10/20 11:52:19]
#define BV_ROOK					200	//车

#define BV_KNIGHT_OPEN_GAME		88	//开局马
#define BV_KNIGHT_MID_GAME		92	//中局马
#define BV_KNIGHT_END_GAME		96	//残局马

#define BV_CANNON_OPEN_GAME		96	//开局炮
#define BV_CANNON_MID_GAME		92	//中局炮
#define BV_CANNON_END_GAME		88	//残局炮

#define BV_KING					0	//将
#define BV_ADVISOR				40	//士
#define BV_BISHOP				40	//象

#define BV_PAWN_OPEN_GAME		9	//开局兵
#define BV_PAWN_MID_GAME		10	//中局兵
#define BV_PAWN_END_GAME		20	//残局兵

#define BV_NOCHESS				0	//无子

// 棋子灵活性价值:FlexValue[2005/12/26 16:15:52]
#define FV_ROOK		2
#define FV_KNIGHT	3
#define FV_CANNON	1

//空心炮价值[2006/1/1 14:29:02]
#define EMPTYHEARTVALUE		BV_BISHOP

// 棋局阶段[2005/10/25 11:26:46]
#define OPEN_GAME	0	//开局
#define MID_GAME	1	//中局
#define END_GAME	2	//残局

//预走法文件名 [2005/10/21 21:32:14]
#define KNIGHTPREMOVESFILE	"knightPreMoves.dat"	//马预走法

//将位置校验序号极值
#define MINKINGPOSCHECKNUM		0
#define MAXKINGPOSCHECKNUM		17

//兵位置校验序号极值
#define MINPAWNPOSCHECKNUM		0
#define MAXPAWNPOSCHECKNUM		55

#define BEGAINGAMEMOVESCNT	10	//开局走法数

//#define TESTHH	//测试历史启发

//测试浅红协议
//#define TESTQHP

//安全释放内存
#define SAFEDEL

//得到最大搜索深度
#define GETMAXSCHDPT

//得到最大选择性搜索深度
#define GETMAXSELSCHDPT

//日志文件
#define LOGFILE

//错误文件
#define ERRFILE

//统计
//#define STAT

//计算内存
//#define MEMCALC

//计算运行时间
//#define TIMECALC

//调试
//#define DEBUG

//测试
//#define TEST

//有左右文本开局库
#define LRTEXTBOOK

//有上下文本开局库
//#define UDTEXTBOOK

#ifdef _DEBUG
#define DEBUG
#endif

//Debug状态下的调试输出
#ifdef _DEBUG
#define DEBUGCOUT(str) (cout << (str))
#else 
#define DEBUGCOUT(str) 
#endif

// Release状态下的调试输出
#ifndef _DEBUG
#define RELEASECOUT(str) (cout << (str))
#else 
#define RELEASECOUT(str) 
#endif

// _ASSERT语句的采用与否
#ifdef DEBUG 
#define DEBUGASSERT(exp) _ASSERT(exp)
#else 
#define DEBUGASSERT(exp) 
#endif

// 带try的new
#ifdef DEBUG

#define TRYNEW(NewStatement)		\
	try								\
	{								\
		NewStatement				\
	}								\
	catch(std::bad_alloc const &)	\
	{								\
		AllocateMemErrProc();		\
	}								
#else
#define TRYNEW(NewStatement) NewStatement
#endif

//带try的delete
#ifdef DEBUG

#define TRYDELETE(DeleteStatement)		\
	try									\
	{									\
		DeleteStatement					\
	}									\
	catch(...)							\
	{									\
		FreeMemErrProc();				\
	}									
#else
#define TRYDELETE(DeleteStatement) DeleteStatement
#endif

// 释放有效指针[2005/10/23 14:53:21]
#define VALIDDELETE(PointerToDel, InvalidPointer)	\
	if (PointerToDel != InvalidPointer)				\
	{												\
		TRYDELETE(SAFEDELETE(PointerToDel));		\
	}

//安全释放内存
//TwoDPointer:二维指针
#ifdef SAFEDEL
#define SAFEDELETE(TwoDPointer)		{delete[] TwoDPointer; TwoDPointer = NULL;}
#else
#define SAFEDELETE(TwoDPointer)		{delete[] TwoDPointer;}
#endif

// ms转换成s[2005/10/22 15:48:38]
#define MS2S(t)		(t) * 1.0 / 1000

// 字节转换为M[2005/10/23 14:33:32]
#define B2M(s)		(s) * 1.0 / (1 << 20)

//棋子二维坐标转换为一维坐标 [2005/10/20 19:56:11]
#define POS2CGN(i, j)	MAXYSCALENUM * (i) + (j)

//横坐标i是否在X轴上 [2005/10/20 20:24:17]
#define ISONXSCALE(i)	((i) >=0 && (i) < MAXXSCALENUM)

//横坐标j是否在Y轴上 [2005/10/20 20:24:17]
#define ISONYSCALE(j)	((j) >= 0 && (j) < MAXYSCALENUM)

// 是否在棋盘上[2005/10/24 15:32:36]
#define ISINBOARD(pos)	((pos) >= 0 && (pos) < MAXCHESSGRIDNUM)

//填充CHESSMOVE [2005/10/20 16:10:25]
#define FILLCHESSMOVE(cm, from, to, score, eatChess, kind)		\
	{															\
		DEBUGASSERT(from >= 0 && from < MAXCHESSGRIDNUM);		\
		DEBUGASSERT(to >= 0 && to < MAXCHESSGRIDNUM);			\
		DEBUGASSERT(eatChess == NOCHESS ||						\
					m_bIsBlack_ChessID[eatChess] ||				\
					m_bIsRed_ChessID[eatChess]);				\
																\
		cm.nFrom = from;										\
		cm.nTo = to;											\
																\
		cm.nScore = score;										\
		cm.nEatChess = eatChess;								\
		cm.nKind = kind;										\
	}

#define FILLUNDOMOVE(undoMove, chessMove, toChessID, toPieceID)		\
	{																\
		undoMove.cm = chessMove;									\
		undoMove.nToChessID = toChessID;							\
		undoMove.nToPieceID = toPieceID;							\
	}

//自加
#ifdef STAT
#define INCREMENT_STAT(a)	(a)++
#else
#define INCREMENT_STAT(a)	
#endif

// 比率[2005/11/2 23:19:48]
#define RATE(a, b)	(a) * 100.0 / (b)

#ifdef STAT
#define STATEMENT_STAT(statement)	statement
#else
#define STATEMENT_STAT(statement)	
#endif

#ifdef TEST
#define STATEMENT_TEST(statement)	statement
#else
#define STATEMENT_TEST(statement)	
#endif

#ifdef DEBUG
#define STATEMENT_DEBUG(statement)	statement
#else
#define STATEMENT_DEBUG(statement)	
#endif

#ifdef TESTQHP
#define STATEMENT_TESTQHP(statement)	statement
#else
#define STATEMENT_TESTQHP(statement)	
#endif

#ifdef TESTQHP
#define STATEMENT_NOTESTQHP(statement)	
#else
#define STATEMENT_NOTESTQHP(statement)	statement
#endif

#ifdef GETMAXSCHDPT
#define STATEMENT_GETMAXSCHDPT(statement)	statement
#else
#define STATEMENT_GETMAXSCHDPT(statement)	
#endif

#ifdef GETMAXSELSCHDPT
#define STATEMENT_GETMAXSELSCHDPT(statement)	statement
#else
#define STATEMENT_GETMAXSELSCHDPT(statement)	
#endif

#ifdef LOGFILE
#define STATEMENT_LOGFILE(statement)	statement
#else
#define STATEMENT_LOGFILE(statement)	
#endif

#ifdef ERRFILE
#define STATEMENT_ERRFILE(statement)	statement
#else
#define STATEMENT_ERRFILE(statement)	
#endif

//并行操作
#ifdef _WIN32
#define PARALLELDO(statement)						\
	{												\
		EnterCriticalSection(&g_CriticalSection);	\
		statement;									\
		LeaveCriticalSection(&g_CriticalSection);	\
	}
#else
#define PARALLELDO(statement) statement
#endif

#ifdef _WIN32
#define STRIDEPLATFORM(statement1, statement2)	statement1
#else
#define STRIDEPLATFORM(statement1, statement2)	statement2
#endif

#define SWITCHEGN(statement)			\
	switch(m_EgnProtocol)				\
	{									\
	case EgnPUCCI:						\
	case EgnPAhei:						\
		statement						\
		break;							\
										\
	case EgnPQianHong:					\
		STATEMENT_TESTQHP(statement);	\
		break;							\
										\
	default:							\
		break;							\
	}

//数据类型定义,便于跨平台移植 [2005/10/19 11:50:59]
#ifndef _WIN32
#define __int64 long long
#endif

typedef void		FSVOID;		//void
typedef char		FSCHAR;		//char
typedef bool		FSbool;		//bool
typedef long		FSBOOL;		//BOOL
typedef short		FSSHORT;	//short
typedef long		FSLONG;		//long
typedef long		FSINT;		//long
typedef DWORD		FSDWORD;	//DWORD
typedef float		FSFLOAT;	//float
typedef short		FRINT16;	//16位整型
typedef long		FRINT32;	//32位整型
typedef __int64 	FRINT64;	//64位整型

typedef unsigned char		FRUCHAR;	//无符号char
typedef unsigned long		FRUBOOL;	//无符号bool
typedef unsigned short		FRUSHORT;	//无符号short
typedef unsigned long		FSULONG;	//无符号long
typedef unsigned long		FSUINT;		//无符号long
typedef unsigned short		FRUINT16;	//16位无符号整型
typedef unsigned long		FRUINT32;	//32位无符号整型
typedef unsigned __int64	FSUINT64;	//64位无符号整型

typedef FSCHAR	 FEATCHAR;	//合适的char类型
typedef FSSHORT	 FEATSHORT; //合适的BYTE类型
typedef FSCHAR	 FEATBOOL;  //合适的BOOL类型

typedef FRUCHAR		FEATUCHAR;	//合适的无符号char类型
typedef FRUSHORT	FEATUSHORT;	//合适的无符号short类型
typedef FRUCHAR		FEATUBOOL;  //合适的无符号BOOL类型

typedef FSLONG 		SCORETYPE;	//走法的历史得分的类型

//走法
struct CHESSMOVE
{
	SCORETYPE	nScore;		//走法得分

	FSCHAR		nFrom;		//源位置
	FSCHAR		nTo;		//目的位置

	FSCHAR		nEatChess;	//吃掉的棋子
	FSCHAR		nKind;		//走法性质

	CHESSMOVE()
	{
		nScore = 0;

		nFrom = 0;
		nTo = 0;

		nEatChess = NOCHESS;
		nKind = NORMAL;
	}

	FSBOOL operator == (CHESSMOVE cm)
	{
		return cm.nScore == nScore && cm.nEatChess == nEatChess && 
			cm.nFrom == nFrom && cm.nTo == nTo && cm.nKind == nKind;
	}

	FSBOOL operator != (CHESSMOVE cm)
	{
		return cm.nScore != nScore || cm.nEatChess != nEatChess || 
			cm.nFrom != nFrom || cm.nTo != nTo || cm.nKind != nKind;
	}
};

//浅红悔棋时用的数据结构
struct UNDOMOVE
{
	CHESSMOVE cm;

	FSINT nToChessID;
	FSINT nToPieceID;
};

typedef UNDOMOVE REDOMOVE;

#ifdef _WIN32
typedef stack<UNDOMOVE> UNDOMOVESTACK;
typedef stack<REDOMOVE> REDOMOVESTACK;
#endif

//预产生走法(注意:pMoveList和nCount声明的顺序不要改变)
struct PRECHESSMOVE
{
	CHESSMOVE	*pMoveList;	//走法链
	FSINT		nCount;		//走法总数
};

// 走法路线[2005/11/12 16:33:38]
struct CMLINE
{
	CHESSMOVE	pMoveList[MAXSEARCHDEPTH + MAXMOVESCNT_NOEAT];
	FSINT		nCount;
};

// 主要变例路线[2005/11/12 16:34:06]
struct PVLINE
{
	CHESSMOVE	pMoveList[MAXSEARCHDEPTH];
	FSINT		nCount;
};

// 杀手走法[2005/10/24 14:21:22]
struct KILLERMOVETAB
{
	CHESSMOVE	cmKiller[MAXKILLERMOVECNT];
	FSINT		nKillerCount;
	FSINT		nPointer;

	KILLERMOVETAB()
	{
		nKillerCount = 0;
		nPointer = 0;
	}
};

// hash表中节点类型
#define HASHALPHA	0
#define HASHEXACT	1
#define HASHBETA	2

// 棋盘局面的hash表项
struct HASHITEM
{
	FSUINT64	nCBCheckSum;	//此局面的校验和
	CHESSMOVE	cmBestMove;		//此局面对应的最好的走法
	FSSHORT		nEval;			//此局面的估值
	FSCHAR		nHashFlag;		//节点类型
	FSCHAR		nDepth;			//此局面所在博弈树中的层数
//	FSINT		nEval;			//此局面的估值
//	FSINT		nHashFlag;		//节点类型
//	FSINT		nDepth;			//此局面所在博弈树中的层数

	HASHITEM()
	{
		nDepth = INVALIDPLY;
	}
};

//棋盘局面的hash表项,用于静态搜索中
struct QSHASHITEM
{
	FSUINT64	nCBCheckSum;	//此局面的校验和
	CHESSMOVE	cmBestMove;		//此局面对应的最好的走法
	FSSHORT		nEval;			//此局面的估值
	FSSHORT		nHashFlag;		//节点类型
};

//文本开局库结构
struct TXTBOOKRECORD
{
	FSINT nCBChessID[90];	//棋盘
	CHESSMOVE cm;			//走法
};

// 走法排序[2005/12/24 19:14:39]
struct MOVESORT
{
	FSINT nPhase;
	FSINT nMoveIndex;
	CHESSMOVE pMoveList[MAXSEARCHDEPTH];

	MOVESORT(CHESSMOVE cmIterative, KILLERMOVETAB killerMoveTab)
	{
		nPhase = 0;
		nMoveIndex = 0;

		pMoveList[0] = cmIterative;

		FSINT i;
		FSINT nKillerCnt = killerMoveTab.nKillerCount;

		for (i = 1; i < nKillerCnt + 1; i++)
		{
			pMoveList[i] = killerMoveTab.cmKiller[i - 1];
		}
	}

	CHESSMOVE Next()
	{
		CHESSMOVE cm;
		return cm;
	}
};

// 引擎协议
enum EgnProtocol
{
	EgnPNone,		//无
	EgnPUCCI,		//UCCI协议
	EgnPAhei,		//ahei协议
	EgnPQianHong,	//浅红协议
};

// 引擎状态
enum EgnStatus
{
	EgnSIdle,		// 空闲
	EgnSBusy,		// 忙
	EgnSSuspend,	// 挂起
};

// UCCI协议命令
enum UCCICmd
{
	UCCICNone,			// 空命令
	UCCICInvalid,		// 无效命令

	UCCICPonderHit,		// 后台思考命中, "ponderhit"

	UCCILoadBook,		// 告诉引擎开始使用开局库, "loadbook"

	UCCINullMove,		// 是否启用Null Move, "null move on/off"
	UCCIOpenBook,		// 是否使用开局库, "openbook on/off"
	UCCIEndBook,		// 是否使用残局库, "endbook on/off"
	
	UCCIClearHash,		// 清空hash, "clearhash"

	UCCIHistroyShit,	// 设置历史裁剪的阈值, "historyshit param"

	UCCIDrawValue,		// 设定和棋分值

	UCCIDrawMoves,		// 设定自然限着

	UCCICEPlies,		// 设定将军延伸层数

	UCCIOnlyMove,		// 唯一解时直接出棋,不再思考, "onlymove on/off"

	UCCITime,			// 查看思考已思考时间

	UCCICStop,			// 停止引擎思考, "stop"
	UCCIPause,			// 暂停引擎思考, "pause"
	UCCIResume,			// 继续引擎思考, "resume"

	UCCICIsReady,		// 引擎是否准备好, "isready"
	UCCICSetOption,		// 设置选项, "setoption ..."
	UCCICPosition,		// 设置引擎将要思考的局面, "position ..."
	UCCICGo,			// 让引擎思考, "go ..."
	UCCICAddPosition, 
	UCCICDebug,			// 调试引擎, "debug..."

	UCCIStatus,			// 得到引擎状态, "status"

	UCCICQuitUCCI,		// 退出UCCI协议, "quitucci"
	UCCICQuitEgn,		// 退出引擎, "quitegn"

	UCCIGetSpecialFEN,	// 得到特殊局面的FEN
};

//浅红协议命令
enum QianHongCmd
{
	QHCNone,		//空命令
	QHCInvalid,		//无效命令

	QHCLevel,		//设置级别
	QHCFEN,			//传局面
	QHCPlay,		
	QHCLoad,		
	QHCAI,			//走棋
	QHCAbort,		//中断

	QHCQuit,		//退出
	QHCQuitQH,		//退出浅红

	QHCUndo,		//悔棋
	QHCHints,		//提示
	QHCBan,			//禁着
	QHCBGThink,		//背景思考
	QHCTimeOut,		//超时
};

//思考方式
enum THINKTYPE
{
	TT_DEPTH,	//按深度思考
	TT_TIME,	//按时间思考
};

//下棋风格
enum CHESSSTYLE
{
	Keep,		//保守
	Normal,		//均衡
	Risky,		//冒进
};

const FSCHAR g_szLogFile[MAXBUFFERLEN] = ENGINENAME"Log.txt";	//日志文件名
const FSCHAR g_szErrFile[MAXBUFFERLEN] = ENGINENAME"Err.txt";	//错误文件名:存放可能错误的指令及不可识别的指令

const FSCHAR g_szEngineName[MAXBUFFERLEN] = "ZWM";	//引擎名

//FEN字符串中的小写字母与黑方棋子的对应表,小写字母a~r
const FSINT g_nAlpha2Chess[18] = 
{
	ADVISOR, BISHOP, CANNON, -1,   -1,   -1,
	-1,		 -1,	 -1,     -1,   KING, -1,
	-1,      KNIGHT, -1,     PAWN, -1,	 ROOK,
};

const FSCHAR g_cChessID2Alpha[15] = 
{
	'0', //无

	'k', //黑将
	'a', //黑士
	'b', //黑象
	'r', //黑车
	'n', //黑马
	'c', //黑炮
	'p', //黑兵

	'K', //红将
	'A', //红士
	'B', //红象
	'R', //红车
	'N', //红马
	'C', //红炮
	'P', //红兵
};

//初始棋盘
const FSINT g_nChessBoard[90] = 
{
	RROOK,	 NOCHESS,NOCHESS,RPAWN,  NOCHESS,NOCHESS,BPAWN,  NOCHESS,NOCHESS,BROOK,
	RKNIGHT, NOCHESS,RCANNON,NOCHESS,NOCHESS,NOCHESS,NOCHESS,BCANNON,NOCHESS,BKNIGHT,
	RBISHOP, NOCHESS,NOCHESS,RPAWN,  NOCHESS,NOCHESS,BPAWN,  NOCHESS,NOCHESS,BBISHOP,
	RADVISOR,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,BADVISOR,
	RKING,   NOCHESS,NOCHESS,RPAWN,  NOCHESS,NOCHESS,BPAWN,  NOCHESS,NOCHESS,BKING,
	RADVISOR,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,BADVISOR,
	RBISHOP, NOCHESS,NOCHESS,RPAWN,  NOCHESS,NOCHESS,BPAWN,  NOCHESS,NOCHESS,BBISHOP,
	RKNIGHT, NOCHESS,RCANNON,NOCHESS,NOCHESS,NOCHESS,NOCHESS,BCANNON,NOCHESS,BKNIGHT,
	RROOK,	 NOCHESS,NOCHESS,RPAWN,  NOCHESS,NOCHESS,BPAWN,  NOCHESS,NOCHESS,BROOK,
};

//初始棋子位置
const FSINT g_nPiecePos[32]=
{
	 0,10,20,30,40,50,60,70,
	80,12,72, 3,23,43,63,83,
	 6,26,46,66,86,17,77, 9,
	19,29,39,49,59,69,79,89,
};

//初始各位置上的棋子
const FSINT g_nCBPieceID[90] =
{
	0,-1,-1,11,-1,-1,16,-1,-1,23,
	1,-1,9, -1,-1,-1,-1,21,-1,24,
	2,-1,-1,12,-1,-1,17,-1,-1,25,
	3,-1,-1,-1,-1,-1,-1,-1,-1,26,
	4,-1,-1,13,-1,-1,18,-1,-1,27,
	5,-1,-1,-1,-1,-1,-1,-1,-1,28,
	6,-1,-1,14,-1,-1,19,-1,-1,29,
	7,-1,10,-1,-1,-1,-1,22,-1,30,
	8,-1,-1,15,-1,-1,20,-1,-1,31,
};

//马各走法增量对应的马腿位置
const FSINT g_nKnightLegPos[43] = 
{
	//马腿位置	//马走法增量	Index
	-10,		//-21			21 - 21 = 0
	0,			//无			21 - 20 = 1
	-10,		//-19			21 - 19 = 2
	0,			//无			21 - 18 = 3
	0,			//无			21 - 17 = 4
	0,			//无			21 - 16 = 5
	0,			//无			21 - 15 = 6
	0,			//无			21 - 14 = 7
	0,			//无			21 - 13 = 8
	-1,			//-12			21 - 12 = 9
	0,			//无			21 - 11 = 10
	0,			//无			21 - 10 = 11
	0,			//无			21 - 09 = 12
	1,			//-8			21 - 08 = 13
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//14 ~ 28
	-1,			//8				21 + 08 = 29
	0, 0, 0,										//30 ~ 32
	1,			//12			21 + 12 = 33
	0, 0, 0, 0, 0, 0,								//34 ~ 39
	10,			//19			21 + 19 = 40
	0,			//无			21 + 20 = 8
	10,			//21			21 + 21 = 42
};

const FSINT g_nMateValue = 5000;										// 杀棋分值
const FSINT g_nPerCheckValue = -(g_nMateValue + MAXSEARCHDEPTH + 1);	// 长将分值
const FSINT g_nGoodPositionValue = 300;									// 大优局面分值

const FSINT g_nContemptFactor_Keep = -20;			//风格保守时的轻视因子,即和棋分值
const FSINT g_nContemptFactor_Normal = 0;			//风格均衡时的轻视因子,即和棋分值
const FSINT g_nContemptFactor_Risky = 20;			//风格冒进时的轻视因子,即和棋分值

const FSINT g_nInvalidValue = g_nMateValue + 5000;	//局面无效值,其绝对值必须大于g_nMateValue
const FSINT g_nTimeOutScore = g_nInvalidValue;		//超时时返回的值,其绝对值必须大于g_nMateValue

//走法增量
//马
const FSINT g_nKnightMoveDelta[MAXKNIGHTMOVECNT] = 
{
	-12, 8, -21, 19, 
	-19, 21, -8, 12, 
};
//马腿
const FSINT g_nKnightLegDelta[MAXKNIGHTLEGCNT] = 
{
	-10, 10, -1, 1, 
};

//将
const FSINT g_nKingMoveDelta[MAXKINGMOVECNT] = 
{
	-10, 10, -1, 1, 
};

//士
const FSINT g_nAdvisorMoveDelta[MAXADVISORMOVECNT] = 
{
	-11, 9, -9, 11, 
};

//象
const FSINT g_nBishopMoveDelta[MAXBISHOPMOVECNT] = 
{
	-22, 18, -18, 22, 
};
//象眼
const FSINT g_nBishopEyeDelta[MAXBISHOPEYECNT] = 
{
	-11, 9, -9, 11, 
};

//兵
//未过河兵
const FSINT g_nDownRiverPawnMoveDelta[2][MAXDOWNRIVERPAWNMOVECNT] = 
{
	//黑卒
	{
		-1, 
	}, 
	//红兵
	{
		1, 
	}, 
};
//过河兵
const FSINT g_nUpRiverPawnMoveDelta[2][MAXUPRIVERPAWNMOVECNT] = 
{
	//黑卒
	{
		-10, 10, -1, 
	},
	//红兵
	{
		-10, 10, 1, 
	}, 
};

// 棋子类型编号转换为总的棋子类型编号[2005/10/20 22:53:40]
const FSINT g_nChessID2Chess[15] = 
{
	NOCHESS,	//NOCHESS

	KING,		//BKING
	ADVISOR,	//BADVISOR
	BISHOP,		//BBISHOP
	ROOK,		//BROOK
	KNIGHT,		//BKNIGHT
	CANNON,		//BCANNON
	PAWN,		//BPAWN

	KING,		//RKING
	ADVISOR,	//RADVISOR
	BISHOP,		//RBISHOP
	ROOK,		//RROOK
	KNIGHT,		//RKNIGHT
	CANNON,		//RCANNON
	PAWN,		//RPAWN
};

// 红方炮二平五的开局局面 [2005/10/22 20:45:57](测试开局速度)
const FSCHAR g_szOpenGamePos[128] = 
	"fen rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1";
// 中局局面(测试中局速度)
const FSCHAR g_szMidGamePos[128] = 
	"fen 1rbak1bnr/4a4/n8/p1p1p1p1p/3R4c/P7P/2P1c1P2/NC2B2CN/9/2BAKA2R w - - 0 9 moves f0e1";
// 残局局面(测试循环)(已解)
const FSCHAR g_szEndGamePos[64] = "fen 4k4/4a4/4P4/9/9/9/9/4B4/9/4K4 w - - 0 1";
// 连杀排局(测试杀棋)(已解)
const FSCHAR g_szCheckMatePos[64] = "fen 4kar2/4a2rn/4bc3/RN1c5/2bC5/9/4p4/9/4p4/3p1K3 w - - 0 1";
// zugzwang局面(测试Null Move)
const FSCHAR g_szZugzwangPos[64] = "fen 3k5/4PP3/4r4/3P5/9/9/9/9/9/5K3 w - - 0 1";
// 检测Null Move的局面(测试Null Move)(已解)
const FSCHAR g_szDetectNMPos[64] = "fen 3aka3/9/9/9/1C7/2n6/9/4C4/4K4/9 w - - 0 1";
// 马对单士(测试Null Move)(已解)
const FSCHAR g_szKnightVSSglAdvisorPos[64] = "fen 3k5/9/3a5/9/9/9/9/9/9/4K3N w - - 0 1";
// 海底捞月(测试循环)(已解)
const FSCHAR g_szFishforMoonInSeaPos[64] = "fen 3k5/9/9/4C4/3r5/4R4/9/9/9/4K4 w - - 0 1";
// 测试循环
const FSCHAR g_szRepDetectPos[128] = "fen 4k1b2/4a4/4b3n/R5p1p/2p6/6P2/P1PC4P/9/3K5/6rc1 b - - 0 22 moves g0g1 d1d0 g1g0 d0d1 g0g1 d1d0 g1g0 d0d1 g0g1 d1d0 g1g0 d0d1";

// 浅红按等级得出搜索深度
const FSINT g_nQHSchDptByLvl[QIANHONGLEVELCNT] = {6, 7, 8, 9, 10, 11, 12, 13, MAXSEARCHDEPTH};

// 棋子位置价值 [2005/10/20 16:58:26]
const FSINT g_nPosValues[8][90] = 
{
	// NOCHESS
	{
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0
	},

	// King
	{
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		1, -8, -9,  0,  0,  0,  0,  0,  0,  0,
		5, -8, -9,  0,  0,  0,  0,  0,  0,  0,
		1, -8, -9,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0
	},
	// Advisor
	{
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  3,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0
	},
	// Bishop
	{
		0,  0, -2,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  3,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0, -2,  0,  0,  0,  0,  0,  0,  0
	},
	// Rook
	{
		-6,  5, -2,  4,  8,  8,  6,  6,  6,  6,
		6,   8,  8,  9, 12, 11, 13,  8, 12,  8,
		4,   6,  4,  4, 12, 11, 13,  7,  9,  7,
		12, 12, 12, 12, 14, 14, 16, 14, 16, 13,
		0,   0, 12, 14, 15, 15, 16, 16, 33, 14,
		12, 12, 12, 12, 14, 14, 16, 14, 16, 13,
		4,   6,  4,  4, 12, 11, 13,  7,  9,  7,
		6,   8,  8,  9, 12, 11, 13,  8, 12,  8,
		-6,  5, -2,  4,  8,  8,  6,  6,  6,  6
	},
	// Knight
	{
		0,  -3,  5,  4,  2,  2,  5,  4,  2,  2,
		-3,  2,  4,  6, 10, 12, 20, 10,  8,  2,
		2,   4,  6, 10, 13, 11, 12, 11, 15,  2,
		0,   5,  7,  7, 14, 15, 19, 15,  9,  8,
		2, -10,  4, 10, 15, 16, 12, 11,  6,  2,
		0,  5,   7,  7, 14, 15, 19, 15,  9,  8,
		2,  4,   6, 10, 13, 11, 12, 11, 15,  2,
		-3, 2,   4,  6, 10, 12, 20, 10,  8,  2,
		0, -3,   5,  4,  2,  2,  5,  4,  2,  2
	},
	// Cannon
	{
		0,  0,  1,  0, -1,  0,  0,  1,  2,  4,
		0,  1,  0,  0,  0,  0,  3,  1,  2,  4,
		1,  2,  4,  0,  3,  0,  3,  0,  0,  0,
		3,  2,  3,  0,  0,  0,  2, -5, -4, -5,
		3,  2,  5,  0,  4,  4,  4, -4, -7, -6,
		3,  2,  3,  0,  0,  0,  2, -5, -4, -5,
		1,  2,  4,  0,  3,  0,  3,  0,  0,  0,
		0,  1,  0,  0,  0,  0,  3,  1,  2,  4,
		0,  0,  1,  0, -1,  0,  0,  1,  2,  4
	},
	// Pawn
	{
		0,  0,  0, -2,  3, 10, 20, 20, 20,  0,
		0,  0,  0,  0,  0, 18, 27, 30, 30,  0,
		0,  0,  0, -2,  4, 22, 30, 45, 50,  0,
		0,  0,  0,  0,  0, 35, 40, 55, 65,  2,
		0,  0,  0,  6,  6, 40, 42, 55, 70,  4,
		0,  0,  0,  0,  0, 35, 40, 55, 65,  2,
		0,  0,  0, -2,  4, 22, 30, 45, 50,  0,
		0,  0,  0,  0,  0, 18, 27, 30, 30,  0,
		0,  0,  0, -2,  3, 10, 20, 20, 20,  0,
	}
};

// 搜索引擎类
class CSearchEngine
{
public:
	CSearchEngine();
	virtual ~CSearchEngine();

	// 公用接口
public:
	FSVOID RunEngine(FSCHAR *szParam);		// 运行引擎

protected:
	//=============================走法产生=============================
	//产生所有的走法
	FSVOID GenMoves(FSINT nDepth);
	
	FSVOID GenRookMoves   (FSINT p, FSINT nDepth);	//车
	FSVOID GenKnightMoves (FSINT p, FSINT nDepth);	//马
	FSVOID GenCannonMoves (FSINT p, FSINT nDepth);	//炮
	FSVOID GenKingMoves   (FSINT p, FSINT nDepth);	//将
	FSVOID GenAdvisorMoves(FSINT p, FSINT nDepth);	//士
	FSVOID GenBishopMoves (FSINT p, FSINT nDepth);	//象
	FSVOID GenPawnMoves   (FSINT p, FSINT nDepth);	//卒

	// [2006/1/3 20:07:08]
//	FSINT GenRookMoves   (FSINT p, CHESSMOVE* pMoveList);	//车
//	FSINT GenKnightMoves (FSINT p, CHESSMOVE* pMoveList);	//马
//	FSINT GenCannonMoves (FSINT p, CHESSMOVE* pMoveList);	//炮
//	FSINT GenKingMoves   (FSINT p, CHESSMOVE* pMoveList);	//将
//	FSINT GenAdvisorMoves(FSINT p, CHESSMOVE* pMoveList);	//士
//	FSINT GenBishopMoves (FSINT p, CHESSMOVE* pMoveList);	//象
//	FSINT GenPawnMoves   (FSINT p, CHESSMOVE* pMoveList);	//卒
//
//	// [2006/1/4 21:10:52]
//	FSINT GenRookMoves   (FSINT p);	//车
//	FSINT GenKnightMoves (FSINT p);	//马
//	FSINT GenCannonMoves (FSINT p);	//炮
//	FSINT GenKingMoves   (FSINT p);	//将
//	FSINT GenAdvisorMoves(FSINT p);	//士
//	FSINT GenBishopMoves (FSINT p);	//象
//	FSINT GenPawnMoves   (FSINT p);	//卒
	
	//产生所有的吃子走法
	FSVOID GenCapMoves(FSINT nDepth);

	FSVOID GenRookCapMoves   (FSINT p, FSINT nDepth);	//车
	FSVOID GenKnightCapMoves (FSINT p, FSINT nDepth);	//马
	FSVOID GenCannonCapMoves (FSINT p, FSINT nDepth);	//炮
	FSVOID GenKingCapMoves	 (FSINT p, FSINT nDepth);	//将
	FSVOID GenAdvisorCapMoves(FSINT p, FSINT nDepth);	//士
	FSVOID GenBishopCapMoves (FSINT p, FSINT nDepth);	//象
	FSVOID GenPawnCapMoves	 (FSINT p, FSINT nDepth);	//兵
	
	// [2006/1/5 12:05:41]
//	FSINT GenRookCapMoves   (FSINT p);	//车
//	FSINT GenKnightCapMoves (FSINT p);	//马
//	FSINT GenCannonCapMoves (FSINT p);	//炮
//	FSINT GenKingCapMoves	(FSINT p);	//将
//	FSINT GenAdvisorCapMoves(FSINT p);	//士
//	FSINT GenBishopCapMoves (FSINT p);	//象
//	FSINT GenPawnCapMoves	(FSINT p);	//兵

	//产生所有的不吃子走法
	FSVOID GenNoCapMoves(FSINT nDepth);

	FSVOID GenRookNoCapMoves   (FSINT p, FSINT nDepth);	//车
	FSVOID GenKnightNoCapMoves (FSINT p, FSINT nDepth);	//马
	FSVOID GenCannonNoCapMoves (FSINT p, FSINT nDepth);	//炮
	FSVOID GenKingNoCapMoves   (FSINT p, FSINT nDepth);	//将
	FSVOID GenAdvisorNoCapMoves(FSINT p, FSINT nDepth);	//士
	FSVOID GenBishopNoCapMoves (FSINT p, FSINT nDepth);	//象
	FSVOID GenPawnNoCapMoves   (FSINT p, FSINT nDepth);	//兵
	
	//走法预产生
	FSVOID GenPreMoves();

	FSVOID GenRookPreMoves   ();	//车
	FSVOID GenKnightPreMoves ();	//马
    FSVOID GenCannonPreMoves ();	//炮
	FSVOID GenKingPreMoves   ();	//将
	FSVOID GenAdvisorPreMoves();	//士
	FSVOID GenBishopPreMoves ();	//象
	FSVOID GenPawnPreMoves   ();	//兵

	//加载预走法
//	FSBOOL LoadKnightPreMove();		//马

	//保存预走法
//	FSVOID SaveKnightPreMoves();	//马

	FSVOID InitPreMoveListPointer();	//初始化走法预产生数组链表中的指针

	FSBOOL CanMemMeet();	//内存是否满足要求
	FSVOID InitPointer();	//初始化指针
	FSVOID Initial();		//初始化

	//=============================局面评估=============================
	FSINT Evaluate();	//估值

	//灵活性价值
	FSINT GetFlexValue();
	FSINT GetFlexValue(FSBOOL bIsRed);

	FSINT GetRookFlexValue  (FSINT p);		//车
	FSINT GetKnightFlexValue(FSINT p);		//马
	FSINT GetCannonFlexValue(FSINT p);		//炮

	FSBOOL IsEmptyHeartCannon(FSBOOL bIsRed);	// 是否有空心炮[2006/1/1 11:38:37]

	//=============================搜索引擎=============================
	//将军判断
	FSBOOL IsChecked(BOOL bIsRed);

	//将军预判断
	FSVOID GenPreCheck();

	FSVOID GenRookPreCheck();	//车
	FSVOID GenKnightPreCheck();	//马
	FSVOID GenCannonPreCheck();	//炮
	FSVOID GenPawnPreCheck();	//兵

	FSBOOL IsRep(FSINT nCanRep);		//循环检测:是否发生长将,长捉
	FSBOOL IsLegalMove(CHESSMOVE cm);	//走法cm是否合法:走了cm后是否会产生送将,长将,长捉
	FSBOOL IsValidMove(CHESSMOVE cm);	//走法cm是否是有效的走法

	FSVOID IDSearchInit();
	FSVOID SearchInit();
	FSVOID SearchStop();
#ifdef _WIN32
	DWORD WINAPI SearchMain();
#endif
	FSVOID SearchGo(); //进行搜索
	FSINT QuiesSearch(FSINT nDepth, FSINT alpha, FSINT beta);	//平静搜索
	FSINT Search(FSINT nDepth, FSINT nSearchDepth, FSINT alpha, FSINT beta, KILLERMOVETAB& killerMoveTab);	
																//nDepth为当前搜索层数
//	FSINT SearchTest(FSINT nDepth, FSINT nSearchDepth, FSINT alpha, FSINT beta, KILLERMOVETAB& killerMoveTab);

	FSVOID MakeMove(CHESSMOVE& cm, FSINT& nToChessID, FSINT& nToPieceID);
	FSVOID UnMakeMove(const CHESSMOVE& cm, FSINT nToChessID, FSINT nToPieceID);

	FSVOID UnMakeNullMove();
	FSVOID MakeNullMove();

	FSULONG Rand32();	//产生32位的随机数
	FSUINT64 Rand64();	//产生64位的随机数

	//产生走法有效性预判断数组
	FSVOID GenPreValidMove();

	FSVOID GenRookPreValidMove();		//车
//	FSVOID GenKnightPreValidMove();		//马
	FSVOID GenCannonPreValidMove();		//炮
//	FSVOID GenKingPreValidMove();		//将
//	FSVOID GenAdvisorPreValidMove();	//士
//	FSVOID GenBishopPreValidMove();		//象
//	FSVOID GenPawnPreValidMove();		//兵

	FSINT ProbeHash(FSINT nDepth, FSINT nSearchDepth, FSINT alpha, FSINT beta, CHESSMOVE& cmBestMove);	
																//探测hash表
	FSVOID RecordHash(FSINT nDepth, FSINT nSearchDepth, FSINT nEval, FSINT nHashFlag, CHESSMOVE cmBestMove);
																//录入hash表

	FSINT ProbeQSHash(FSINT alpha, FSINT beta);
	FSVOID RecordQSHash(FSINT nEal, FSINT nHashFlag, CHESSMOVE cmBestMove);

//	FSVOID SetBestMove();
	FSBOOL IsGoodCapMove(const CHESSMOVE& cm);		//走法cm是否为好的吃子走法

	FSVOID ChangeSide();	//改变走子方

	FSVOID GetPVLineFromHash(FSINT nSearchDepth);	//从hash表中取PV

	BOOL IsSpecialPos(FSCHAR *& szStr);	//是否为特殊的局面,如果是,处理之

	FSBOOL	IsDraw();				//是否为和棋
	FSINT	DrawValue();			//和棋分值
	FSINT	RepValue(FSINT nRep);	//重复分值
	
	FSINT LegalMoves(FSINT& nLegalNum); //唯一的合法走法

	FSUINT64 GetCBCheckSum(const FSINT nCBChessID[], FSBOOL bSide);		//得到棋盘的校验和

	CHESSMOVE LoadBookFEN(FSCHAR *szFEN);	//加载开局库中的FEN串
	CHESSMOVE GetMoveFromBook();			//从开局库中得到走法

	FSVOID PrintBetterPV(FSINT nDepth, FSINT nSearchDepth, FSINT nEval, FSINT nHashFlag, CHESSMOVE& cmBetterMove);
	FSVOID ClearHash();
	FSVOID SetChessStyle(FSCHAR *szStyle);

	FSINT GetHash(HASHITEM **pHashTab, FSINT nDepth, FSINT nSearchDepth, FSINT alpha, FSINT beta, CHESSMOVE& cmBestMove);

	FSBOOL IsCheckMove(CHESSMOVE& cm);	//是否为将军走法

	//=============================UCCI协议=============================
	FSVOID UCCIProc();	//UCCI协议处理

	UCCICmd GetUCCICmd(FSCHAR * &szCmd);	//解析命令szCmd

	FSBOOL LoadFEN(FSCHAR *szFEN);		//从字符串szFEN中分离出当前局面

	FSVOID ChessMove2UCCIStr(const CHESSMOVE& cm, FSCHAR *szMove);		//将走法cm转换为字符串szMove
	FSBOOL UCCIStr2ChessMove(FSCHAR *szMove, CHESSMOVE& cm);	//将字符串szMove转换为走法cm

	FSVOID UCCIStopThink();
#ifdef _WIN32
	FSVOID CALLBACK ThinkTimeOut(FSUINT uTimerID, FSUINT uMsg, FSDWORD dwUser, FSDWORD dw1, FSDWORD dw2);
#endif
	FSVOID UCCIBusy();

	FSVOID KillThread(HANDLE h);	//结束线程

	FSVOID PrintEgnStatus();	// 打印引擎状态

	const char* GetSpecialFEN(char *str); // 得到特殊局面的FEN串

	//=============================ahei协议=============================
	FSVOID AheiPProc();	//ahei协议处理

	//=============================浅红协议=============================
	FSVOID QianHongPProc(FSCHAR *szParam);	//浅红协议处理
	QianHongCmd GetQianHongCmd(FSCHAR *& szQianHongCmd);
	FSVOID PrintQHInfo();	//输出浅红信息

	FSBOOL QHStr2ChessMove(FSCHAR *szICCS, CHESSMOVE& cm);			//浅红走法字符串转换为走法
	FSVOID ChessMove2QHStr(const CHESSMOVE& cm, FSCHAR *szICCS);	//走法转换为浅红走法字符串

	FSVOID QHStopThink();

	FSVOID CBChessID2FEN(FSCHAR *szFEN);	//浅红协议悔棋中用到

	template<class T> FSVOID ClearStack(T s);	//清空栈,浅红协议悔棋中用到

	//=============================其他方面=============================
	FSVOID InitVar();	//初始化变量

	FSBOOL IsInBoard(FSINT i,FSINT j);	//(i,j)是否在棋盘内
	FSBOOL IsInBoard(FSINT p);			//p是否在棋盘内

	FSBOOL IsInRBoard(FSINT p);	//p是否在红方地盘内
	FSBOOL IsInBBoard(FSINT p);	//p是否在黑方地盘内

	FSBOOL IsInRBoard(FSINT i,FSINT j);	//(i,j)是否在红方地盘内
	FSBOOL IsInBBoard(FSINT i,FSINT j);	//(i,j)是否在黑方地盘内

	FSBOOL IsInCity(FSINT i,FSINT j);	//(i,j)是否在九宫内
	FSBOOL IsInCity(FSINT p);			//p是否在九宫内

	EgnProtocol GetEgnP(FSCHAR *szStr);	//得到引擎协议

	FSVOID AllocateMem();			//分配内存
	FSVOID FreeMem();				//释放内存
	FSVOID AllocateMemErrProc();	//分配内存错误处理
	FSVOID FreeMemErrProc();		//释放内存错误处理

	FSVOID GetVersion(FSCHAR *szVersion) const;	//得到本引擎版本版本号

	FSCHAR * JumpNoMatterChar(FSCHAR *szStr);	//跳过szStr前面的空格,Tab
	FSCHAR * JumpDigit(FSCHAR *szStr);			//跳过szStr前面的数字

	FSVOID ExitEgn();	//退出引擎

	FSBOOL IsValidPosition();

	FSVOID PrintPVLine();	//输出PVLine

	FSVOID ChessMove2Str(const CHESSMOVE& cm, FSCHAR *szMove);
	FSBOOL Str2ChessMove(FSCHAR *szMoveStr, CHESSMOVE& cm);

	FSVOID UDTransform(TXTBOOKRECORD& txtBookRecord);	// 对棋盘进行上下对称转换,即:交换红黑方
	FSVOID LRTransform(TXTBOOKRECORD& txtBookRecord);	// 对棋盘进行左右对称转换

//#ifdef _WIN32
//	FSVOID CALLBACK DispTime(FSUINT uTimerID, FSUINT uMsg, FSDWORD dwUser, FSDWORD dw1, FSDWORD dw2);
//#endif

	int SelBestMove(vector<float> vectorScore); //选择好的走法

protected:
	//=============================走法产生=============================
	//走法产生
	FSINT m_nMovesCnt;	//在某层产生走法时已向走法队列中插入的走法数

	CHESSMOVE m_MoveList[MAXSEARCHDEPTH][MAXMOVECNTCAPAICITY];	//走法队列,[层数][每一层的走法序号]

	//走法预产生
	//走法预产生数组
	FEATCHAR m_nFileRankAroundCheckSum[10][1024];	//m_nFileRankAroundCheckSum[棋子位置][位行或位列],
													//m_nFileRankAroundCheckSum[i][k]表示与第i位相
													//隔为1的位对应的校验和,它的取值只有4种情况0,1,2,3
													//如:m_nFileRankAroundCheckSum[1][10b]=0,
													//m_nFileRankAroundCheckSum[1][11b]=1,
													//m_nFileRankAroundCheckSum[1][110b]=2,
													//m_nFileRankAroundCheckSum[1][111b]=3
	FEATCHAR m_nFileRankAroundCheckSum2[10][1024];	//m_nFileRankAroundCheckSum2[棋子位置][位行或位列],
													//m_nFileRankAroundCheckSum2[i][k]表示与第i位
													//相隔为2的位对应的校验和,它的取值只有4种情况0,1,2,3
													//如:m_nFileRankAroundCheckSum2[1][100b]=0,
													//m_nFileRankAroundCheckSum2[1][101b]=1,
													//m_nFileRankAroundCheckSum2[1][10100b]=2,
													//m_nFileRankAroundCheckSum2[1][10101b]=3

	FSINT m_nAroundCheckSum[4][4];	//将两个二位的二进制数合成位一个4位的二进制数,
									//第一个参数作为低二位,如:[01][00]=0001

    //车
	//TODO:bug易发区:数组维数的确定
	PRECHESSMOVE (* m_pcmRookFilePreMoves)[ROOKFILECHECKSUMMAXCNT][4];
															//[91][ROOKFILECHECKSUMMAXCNT][4]
															//车的行走法预产生数组:
															//[车的位置][位行校验序号][对方子位行校验序号]
	PRECHESSMOVE (* m_pcmRookRankPreMoves)[ROOKRANKCHECKSUMMAXCNT][4];	
															//[91][ROOKRANKCHECKSUMMAXCNT][4]
															//车的列走法预产生数组:
															//[车的位置][位列校验序号][对方子位列校验序号]
	PRECHESSMOVE (* m_pcmRookFileCapPreMoves)[ROOKFILECHECKSUMMAXCNT][4];
															//[91][ROOKFILECHECKSUMMAXCNT][4]
															//车的行吃子走法预产生数组:
															//[车的位置][位行校验序号][对方子位行校验序号]
	PRECHESSMOVE (* m_pcmRookRankCapPreMoves)[ROOKRANKCHECKSUMMAXCNT][4];	
															//[91][ROOKRANKCHECKSUMMAXCNT][4]
															//车的列吃子走法预产生数组:
															//[车的位置][位列校验序号][对方子位列校验序号]
	PRECHESSMOVE (* m_pcmRookFileNoCapPreMoves)[ROOKFILECHECKSUMMAXCNT];
															//[91][ROOKFILECHECKSUMMAXCNT]
															//车的行不吃子走法预产生数组:
															//[车的位置][位行校验序号]
	PRECHESSMOVE (* m_pcmRookRankNoCapPreMoves)[ROOKRANKCHECKSUMMAXCNT];	
															//[91][ROOKRANKCHECKSUMMAXCNT]
															//车的列不吃子走法预产生数组:
															//[车的位置][位列校验序号]

	FEATCHAR m_nRookFileRankCheckNum[10][1024];	//车的位行和位列的校验序号:[车在行或列中的位置][位行或位列]
												//取值范围为:0~29

	FSINT m_nRookFileRankCheckSum[10][ROOKFRCHECKSUMMAXCNT];	//车的位行和位列的校验和:
																//[车在行或列中的位置][校验序号]
																//取值范围为:0~1023

	FEATCHAR (*m_nRookFoeFileRankCheckNum)[ROOKFRCHECKSUMMAXCNT][1024];
														//[10][ROOKFRCHECKSUMMAXCNT][1024]
														//对方子位行和位列对应的校验序号:
														//[车在位行或位列中的位置][车的校验序号][对方子的位行或位列]
														//取值范围为:0,1,2,3
	FEATSHORT m_nRookFoeFileRankCheckSum[10][ROOKFRCHECKSUMMAXCNT][4];
														//车的对方子位行或位列的校验和:
														//[车在位行或位列中的位置][车的校验序号][对方子的位行或位列的校验序号]
														//取值范围为0~1023

	//马
	PRECHESSMOVE (*m_pcmKnightNoCapPreMoves)[256][16];		//[91][256][16]马的不吃子走法预产生数组:
															//[马的位置][马的可走点的校验和][马腿的的校验和]
	PRECHESSMOVE (*m_pcmKnightCapPreMoves)[256][16];		//[91][256][16]马的吃子走法预产生数组:
															//[马的位置][马的可走点为对方子的校验和][马腿的校验和]

    FEATUCHAR m_nKnightAroundCheckSum[16][16];	//马的周围情况的校验和:
												//[马的下面两行的校验序号][马的上面两行的校验序号]
												//取值范围为:0~255

	FEATBOOL m_bIsKnightInvalidArroundCheckSum[90][256];	//是否为马的有效校验和
															//[马的位置][马的周围情况校验和]
															//取值范围为:0,1
	FEATBOOL m_bIsKnightInvalidLegCheckSum[90][256][16];	//是否为马腿的有效校验和
															//[马的位置][马的周围情况校验和][马腿校验和]
															//取值范围为:0,1

	//炮
	PRECHESSMOVE (*m_pcmCannonFilePreMoves)[CANNONFILECHECKSUMMAXCNT][4];
															//[91][CANNONFILECHECKSUMMAXCNT][4]
															//炮的行走法预产生数组:
															//[炮的位置][位行校验序号][对方子位行校验序号]
    PRECHESSMOVE (*m_pcmCannonRankPreMoves)[CANNONRANKCHECKSUMMAXCNT][4];
															//[91][CANNONRANKCHECKSUMMAXCNT][4]
															//炮的列走法预产生数组:
															//[炮的位置][位列校验序号][对方子位列校验序号]
	PRECHESSMOVE (*m_pcmCannonFileCapPreMoves)[CANNONFILECHECKSUMMAXCNT][4];
															//[91][CANNONFILECHECKSUMMAXCNT][4]
															//炮的行吃子走法预产生数组:
															//[炮的位置][位行校验序号][对方子位行校验序号]
    PRECHESSMOVE (*m_pcmCannonRankCapPreMoves)[CANNONRANKCHECKSUMMAXCNT][4];
															//[91][CANNONRANKCHECKSUMMAXCNT][4]
															//炮的列吃子走法预产生数组:
															//[炮的位置][位列校验序号][对方子位列校验序号]
	PRECHESSMOVE (*m_pcmCannonFileNoCapPreMoves)[CANNONFILECHECKSUMMAXCNT];
															//[91][CANNONFILECHECKSUMMAXCNT]
															//炮的行吃子走法预产生数组:
															//[炮的位置][位行校验序号]
    PRECHESSMOVE (*m_pcmCannonRankNoCapPreMoves)[CANNONRANKCHECKSUMMAXCNT];
															//[91][CANNONRANKCHECKSUMMAXCNT]
															//炮的列吃子走法预产生数组:
															//[炮的位置][位列校验序号]

	FEATUCHAR m_nCannonFileRankCheckNum[10][1024];			//炮的位行和位列的校验序号:
															//[炮在行或列中的位置][位行或位列]
															//取值范围为:0~175
	FEATSHORT m_nCannonFileRankCheckSum[10][CANNONFRCHECKSUMMAXCNT];
															//炮的位行和位列的校验序号对应的位行和位列:
															//[车在行或列中的位置][校验序号]
															//取值范围为:0~1023
	FEATCHAR (*m_nCannonFoeFileRankCheckNum)[CANNONFRCHECKSUMMAXCNT][1024];
															//[10][CANNONFRCHECKSUMMAXCNT][1024]
															//对方子位行和位列对应的校验序号,
															//[炮在位行或位列中的位置][炮的校验序号][对方子的位行或位列]
															//取值范围为:0,1,2,3
	FEATSHORT m_nCannonFoeFileRankCheckSum[10][CANNONFRCHECKSUMMAXCNT][4];
															////炮的对方子位行或位列的校验序号所对应的位行或位列
															//[炮在位行或位列中的位置][炮的校验序号][对方子的位行或位列的校验序号]
															//取值范围为:0~1023
	//将
	PRECHESSMOVE (*m_pcmKingPreMoves)[16][16];	//[18][16][16]将的走法预产生数组:
												//[将的位置][将周围情况的校验序号][将周围对方子情况的校验序号]
	PRECHESSMOVE m_pcmKingNoCapPreMoves[18][16];//[18][16]将的不吃子走法预产生数组:
												//[将的位置][将周围情况的校验序号]
	PRECHESSMOVE m_pcmKingCapPreMoves[18][16];	//[18][16]将的吃子走法预产生数组:
												//[将的位置][将周围对方子情况的校验序号]
	
	FSINT m_nKingPosCheckNum[90];	//将的位置校验序号:[将的位置](注意:将必存在,所以不需要扩充为91)
									//取值范围为:0~17
    FSINT m_nKingPosCheckSum[18];	//将的位置校验和:[将的位置的校验序号],取值范围为:0~89

    //士
    PRECHESSMOVE (*m_pcmAdvisorPreMoves)[16][16];	//[11][16][16]士的走法预产生数组:
													//[士的位置的校验和][士的周围情况的位棋盘的校验和][士的周围对方子的位棋盘的校验和]
	PRECHESSMOVE m_pcmAdvisorNoCapPreMoves[11][16];	//[11][16]士的不吃子走法预产生数组:
													//[士的位置的校验和][士的周围情况的校验和]
	PRECHESSMOVE m_pcmAdvisorCapPreMoves[11][16];	//[11][16]士的吃子走法预产生数组:
													//[士的位置的校验和][士的周围为对方子情况的校验和]

	FSINT m_nAdvisorPosCheckNum[91];	//士的位置校验序号:[士的位置],取值范围为:0~10
    FSINT m_nAdvisorPosCheckSum[11];	//士的位置校验和:[士的位置的校验序号],取值范围为:0~89

    //象
    PRECHESSMOVE (*m_pcmBishopPreMoves)[16][16][16];	//[15][16][16][16]象的走法预产生数组:
														//[象的位置的校验序号][象眼的校验序号][象的可走点的校验序号][象的可走点为对方子的校验序号]
	PRECHESSMOVE (*m_pcmBishopNoCapPreMoves)[16][16];	//[15][16][16]象的吃子走法预产生数组:
														//[象的位置的校验序号][象眼的校验序号][象的可走点的校验序号]
	PRECHESSMOVE (*m_pcmBishopCapPreMoves)[16][16];		//[15][16][16]象的吃子走法预产生数组:
														//[象的位置的校验序号][象眼的校验序号][象的可走点为对方子的校验序号]

    FSINT m_nBishopPosCheckNum[91];	//象的位置校验序号:[象的位置],取值范围为:0~14
    FSINT m_nBishopPosCheckSum[15];	//象的位置校验和:[象的位置的校验序号],取值范围为:0~89

	//卒
	PRECHESSMOVE (*m_PawnPreMoves)[56][8][8];	//[2][56][8][8]兵的走法预产生数组:
												//[兵或卒][兵的位置的校验序号][兵的可走点的校验和][兵的可走点为对方子的校验和]
	PRECHESSMOVE (*m_PawnNoCapPreMoves)[56][8];	//[2][56][8]兵的吃子走法预产生数组:
												//[兵或卒][兵的位置的校验序号][兵的可走点的校验和]
	PRECHESSMOVE (*m_PawnCapPreMoves)[56][8];	//[2][56][8]兵的吃子走法预产生数组:
												//[兵或卒][兵的位置的校验序号][兵的可走点为对方子的校验和]

	FEATSHORT m_nPawnRankAroundCheckSum[2][10][1024];	//卒的列周围情况的校验和:
														//[兵或卒][卒所在的列][卒所在的位列]
														///取值范围为:0~1

	FSINT m_nPawnPosCheckNum[2][91];	//卒的位置校验序号:[兵或卒][卒的位置],取值范围为:0~55
    FSINT m_nPawnPosCheckSum[2][56];	//卒的位置校验和:[兵或卒][卒的位置的校验序号],取值范围为:0~89

	//双将照面判断数组
	FEATBOOL (*m_bIsTwoKingMeet)[10][1024];	//[10][10][1024]
											//[黑将位置校验序号][红帅位置校验序号][黑将所在列的位列],
											//=0没有照面,反之照面
											//[x][y][z]:x或y等于9的时候,都返回TRUE
											//取值范围为:0,1

	FSINT m_nBRKingPosCheckNum[2][MAXCHESSGRIDNUM + 1];	//将的位置校验序号:[将的位置],[0]:黑,[1]:红
														//(注意:将必存在,所以不需要扩充为91)
														//返回值范围为:0~8

    FSINT m_nBRKingPosCheckSum[2][MAXXSCALENUM];	//将的位置校验和:[何方][将的位置的校验序号],
													//[0][]:黑,[1][]:红
													//返回值范围为:0~89

	//将军预判断数组
	//车
	FEATBOOL (*m_bIsRookFilePreCheck)[91][9][512];	//[2][91][9][512]车行将军判断
													//[何方车][车的位置][对方将的位置的校验序号][将所在位置的位行]
													//取值范围为:0,1
	FEATBOOL (*m_bIsRookRankPreCheck)[91][9][1024];	//[2][91][9][1024]车列将军判断
													//[何方车][车的位置][对方将的位置的校验序号][将所在位置的位列]
													//取值范围为:0,1

	//马
	FEATBOOL (*m_bIsKnightPreCheck)[91][9][16];	//[2][91][9][16]马将军判断
													//[何方马][马的位置][对方将的位置的校验序号][马腿校验和]
													//取值范围为:0,1

	//炮
	FEATBOOL (*m_bIsCannonFilePreCheck)[91][9][512];	//[2][91][9][512]炮行将军判断
														//[何方炮][炮的位置][对方将的位置的校验序号][将所在位置的位行]
														//取值范围为:0,1
	FEATBOOL (*m_bIsCannonRankPreCheck)[91][9][1024];	//[2][91][9][1024]炮列将军判断
														//[何方炮][炮的位置][对方将的位置的校验序号][将所在位置的位列]
														//取值范围为:0,1
	
	//兵
	FEATBOOL (*m_bIsPawnPreCheck)[56][9];	//[2][55][9]兵将军判断
											//[何方兵][兵的位置][对方将的位置的校验序号]
											//取值范围为:0,1

	CHESSMOVE m_InvalidMovePointer;	//无效的走法指针,生成走法中拷贝走法时用到

	//=============================局面评估=============================
	FSINT m_nHave[2];		//m_nHave[0] = 0xFFFF, m_nHave[1] = 0
	FSINT m_nSign[2];		//m_nSign[0] = 1, m_nSign[1] = -1	

	FSINT m_nRookFlexValue[17];		//车灵活性价值:m_nRookFlexValue[车的不吃子走法个数]
	FSINT m_nKnightFlexValue[8];	//马灵活性价值:m_nKnightFlexValue[马的不吃子走法个数]
	FSINT m_nCannonFlexValue[17];	//炮灵活性价值:m_nCannonFlexValue[炮的不吃子走法个数]

	FSINT m_nEmptyHeartCannonValue[2]; //空心炮的价值

	FSINT m_nKnightBasicValue[3];	//马的基本分值
	FSINT m_nCannonBasicValue[3];	//炮的基本分值
	FSINT m_nPawnBasicValue[3];		//兵的基本分值

	//=============================搜索引擎=============================
	CHESSMOVE m_Moves_NoEat[MAXMOVESCNT_NOEAT];		//双方没有吃子的走法,判断长将长捉时用
	FSINT m_nNoEatMoveCnt_BS;						//搜索前双方没有吃子的走法数
	FSINT m_nNoEatMoveCnt_IS;						//搜索中没有吃子走法的走法数
	FSINT m_nNoEatMoveCnt_IS_Backup;				//搜索中没有吃子走法的走法数的备份
	FSUINT64 m_nHistoryCBCheckSum_NoEat[MAXSEARCHDEPTH + MAXMOVESCNT_NOEAT];

	FSULONG m_nHashKey32[15][9][10];	//32位随机数组,用以生成32位哈希值
	FSUINT64 m_nHashKey64[15][9][10];	//64位随机数组,用以生成64位哈希值

	FSULONG m_nHashKeyPlayer32;		//用于ChangeSide
	FSUINT64 m_nHashKeyPlayer64;	//用于ChangeSide

	FSULONG m_nHashTabIndex;	//hash表索引:用于将局面存入hash表
	FSUINT64 m_nCBCheckSum;		//棋盘局面的64位校验和

	HASHITEM *m_pHashTab[2];		//hash表头指针
	HASHITEM *m_pHashTabByDepth[2];	//深度替换策略的hash表头指针
	QSHASHITEM *m_pQSHashTab[2];	//qshash表头指针
	FSINT m_nHashTabMask;			//hash表屏蔽位,hash表大小为其加1

	FSINT m_nRepHashTabMask;	//重复局面检测中用到,屏蔽位
	FSINT *m_nRepHashTab;		//重复局面检测用的迷你hash表

	SCORETYPE m_nHistoryScore[2][MAXCHESSGRIDNUM + 1][MAXCHESSGRIDNUM + 1];	//走法的历史得分表

	FSUINT m_nMoveSearchedCnt[MAXCHESSGRIDNUM][MAXCHESSGRIDNUM];	//走法被搜索的次数
	FSUINT m_nMoveCutoffCnt[MAXCHESSGRIDNUM][MAXCHESSGRIDNUM];		//走法被截断的次数

	FSINT m_nDRF_NM;	//Null Move中用的深度减少因子(DepthReductionFactor)

	FSINT m_nHistoryShift; // 历史表裁剪的比例阈值，1 = 50%, 2 = 25%, 3 = 12.5%, 等等

	CHESSMOVE m_cmKiller[MAXSEARCHDEPTH][MAXKILLERMOVECNT];	//杀手走法
	FSINT m_nKillerMoveCnt[MAXSEARCHDEPTH];					//每层已有杀手走法个数
	KILLERMOVETAB m_KillerMoveTab;							//杀手走法表

	PVLINE m_PVLineThink;	//搜索中的PV路线
	PVLINE m_PVLine;		//搜索好的PV路线

	FSINT m_nTimeThinkLimit;	//限制的思考时间
	FSINT m_nTimeStartThink;	//刚开始时思考的时间

	THINKTYPE m_ThinkType;	//思考方式

	FSBOOL m_bNullMoveOn;	//是否启用Null Move

	FSBOOL m_bOpenBookOn;	//是否使用开局库
	FSBOOL m_bEndBookOn;	//是否使用残局库

	FSBOOL m_bOnlyMoveOn;	//有唯一解时,直接走棋,不再思考

	FSINT m_nGetOpenBookMoveErrCnt;		//从开局库中加载走法错误次数

	FSINT m_nContemptFactor;	//轻视因子
	CHESSSTYLE m_ChessStyle;	//下棋风格

	FSINT m_nDrawMoves;		//双方没有吃子的走棋步数(半回合数)
							//通常该值达到120就要判和(六十回合自然限着)

	FSINT m_nCheck[2][CHECK + 1]; // m_nCheck[FALSE][] = [], m_nCheck[TRUE][] = CHECK [2006/1/2 19:28:38]

//	FSVOID (CSearchEngine::*m_GenMovesFun[8])(FSINT, FSINT);		//产生走法的函数
//	FSINT (CSearchEngine::*m_GenMovesFun2[8])(FSINT, CHESSMOVE*);	//产生走法的函数
//	FSINT (CSearchEngine::*m_GenCapMovesFun[8])(FSINT);				//产生吃子走法的函数
	
	FSINT m_nCheckExtensionPlies;	// 将军延伸的层数[2006/1/7 21:02:29]

	FSBOOL m_bIsHaveCheckMateMove;	// 是否已经搜索到杀棋
	FSBOOL m_bIsHaveGoodPosition;	// 局面是否已经占大优 [2006/2/24 9:22:02]

	//走法有效性预判断数组
	//车
//	FEATBOOL (*m_bIsRookFilePreValidMove)[90][512];		//[90][90][512]:[车位置][车要到达的目的位置][车所在的位行]
//	FEATBOOL (*m_bIsRookRankPreValidMove)[90][1024];	//[90][90][1024]:[车位置][车要到达的目的位置][车所在的位列]
//	
//	//马
//	FEATBOOL m_bIsKnightPreValidMove[90][90][16];	//[马位置][马要到达的目的位置][马腿情况校验和]
//
//	//炮
//	FEATBOOL (*m_bIsCannonFilePreValidMove)[90][512][2];	//[90][90][512][2]:[炮位置][炮要到达的目的位置][炮所在的位行][目标位置是否有子]
//	FEATBOOL (*m_bIsCannonRankPreValidMove)[90][1024][2];	//[90][90][1024][2]:[炮位置][炮要到达的目的位置][炮所在的位列][目标位置是否有子]
//
//	//将
//	FEATBOOL m_bIsKingPreValidMove[90][90];		//[90][90]:[车位置][车要到达的目的位置][车所在的位行/位列]

	//车
	FEATBOOL (*m_bIsRookPreValidMove)[10][1024];	//[10][10][1024]:[车位置][车要到达的目的位置][车所在的位列/位列]
	
	//炮
	FEATBOOL (*m_bIsCannonPreValidMove)[10][1024][2];	//[10][10][1024][2]:[炮位置][炮要到达的目的位置][炮所在的位列/位列][目标位置是否有子]

	//=============================UCCI协议=============================
	FSBOOL m_bStopThink;

#ifdef _WIN32

	HANDLE m_hThinkThread;	//思考线程

	CRITICAL_SECTION g_CriticalSection;	//临界区

#else
#endif

	FSBOOL m_bPonder;			//是否进行后台思考
	FSBOOL m_bPonderHit;		//后台思考是否命中

	FSINT m_nBouts;
	FSINT m_nCurBout;

	//=============================浅红协议=============================
	FSINT m_nQHLevel;	//浅红等级

	//浅红协议中悔棋时用到
#ifdef _WIN32
	UNDOMOVESTACK m_UndoMoveStack;
	REDOMOVESTACK m_RedoMoveStack;
#else
#endif

#ifdef DEBUG
	FSINT m_nPiecePosBeforeAI[33];	//运行AI命令之前的PiecePos
#endif

	//=============================其他方面=============================
	UINT m_nMask[10];			//m_nMask[i]表示二进制时只有第i位为1,其余为0,如:m_nMask[3]=1000b
	UINT m_nReverseMask[10];	//[i]表示二进制时只有第i位为0,其余为1,如:m_nMask[3]=11111...11110111b

	FSCHAR m_szWhoN2WhoStr[2][4]; //m_szWhoN2WhoStr[0] = "黑", m_szWhoN2WhoStr[1] = "红"

	FSBOOL m_bAllocateMem;	//是否已分配内存

	EgnProtocol m_EgnProtocol;	//当前使用的协议

	//用于计算搜索时间
	FSINT m_nTotalSearchTime;	//总搜索时间
	FSINT m_nSearchCnt;			//搜索次数
	FSINT m_nMaxSearchTime;		//最长的搜索时间

	CHESSMOVE m_cmNullMove;

	//字节计算
	FSINT m_nBytes[MAXSEARCHDEPTH];	//[i]:返回i *sizeof(CHESSMOVE)

#ifdef DEBUG
	FSINT m_nTestTimer;		//供测试用的计数器	
#endif
	
	STATEMENT_GETMAXSCHDPT(FSINT m_nMaxSearchDepth;);	//最大搜索深度

	STATEMENT_GETMAXSCHDPT(FSINT m_nMaxSelSchDpt;);		//选择性搜索达到的最大深度

	//重复局面检测中用到
	CHESSMOVE m_cmFight[MAXSEARCHDEPTH + MAXMOVESCNT_NOEAT];			//双方走的走法
	FSUINT64 m_nHistoryCBCheckSum[MAXSEARCHDEPTH + MAXMOVESCNT_NOEAT];	//沿着走法路线记录下来的局面校验和

	FSINT m_nCurDepth;	//当前搜索深度

	FSCHAR m_szBookFile[MAXBUFFERLEN];	//开局库

	FSCHAR m_szDir[MAXBUFFERLEN];	//当前目录

	FSINT m_nPieceID2Bit[33];	//棋子编号转换为bit,
								//[nPieceID]:nPieceID == INVALIDPIECEID ret 0 else ret 1

	POINT m_CGN2Pos[91];		//棋格编号转换为位置m_CGN2Pos[i]返回(i / 10, i % 10)
	FSINT m_nPos2CGN[9][10];	//位置转换为棋格编号m_nPos2CGN[i][j] = i * 10 + j

	FSINT m_nPieceID2ChessID[32];		//[棋子编号]:返回棋子类型编号
	FSINT m_nChessID2PieceID[15][5];	//[棋子类型编号][棋子类型中的编号]棋子类型编号转换为棋子编号

	FSINT m_nInitChessCnt[15];	//初始各类棋子数目

	FSINT m_nSearchDepth;			//搜索深度
	FSINT m_nCurSearchDepth_ID;		//迭代加深时,当前的搜索深度
	
	EgnStatus m_esUCCIEgnStatus;	//引擎状态

	FSINT m_nAbs[10][3];	//[i][j]:if(i - j < 0) ret 0 else ret i - j;

	FSINT m_nRook[2];		//[0] = BROOK,		[1] = RROOK
	FSINT m_nKnight[2];		//[0] = BKnight,	[1] = RKnight
	FSINT m_nCannon[2];		//[0] = BROOK,		[1] = RROOK
	FSINT m_nKing[2];		//[0] = BKing,		[1] = RKing
	FSINT m_nAdvisor[2];	//[0] = BAdvisor,	[1] = RAdvisor
	FSINT m_nBishop[2];		//[0] = BBishop,	[1] = RBishop
	FSINT m_nPawn[2];		//[0] = BPawn,		[1] = RPawn

	FSINT m_nKingID2PieceID[2];	//m_nKingID2PieceID[0] = m_nChessID2PieceID[BKING][0]
								//m_nKingID2PieceID[1] = m_nChessID2PieceID[RKING][0]

	//棋子方判断数组
    FSBOOL m_bIsRed_PieceID[33];	//[棋子编号]:返回1表示该位置上的棋子为红子
	FSBOOL m_bIsBlack_PieceID[33];	//[棋子编号]:返回1表示该位置上的棋子为黑子
	FSBOOL m_bIsRed_ChessID[15];	//[棋子类型编号]:返回1表示该位置上的棋子为红子
	FSBOOL m_bIsBlack_ChessID[15];	//[棋子类型编号]:返回1表示该位置上的棋子为黑子

	FSINT m_nConvertChessID[15];

#ifdef STAT

	FSUINT m_nVistNodeCnt;	//访问的节点数:用于计算NPS

	FSUINT m_nNonQuiesNodeCnt;	//非静态搜索的节点数
	FSUINT m_nAlphaNodeCnt;		//alpha节点
	FSUINT m_nPVNodeCnt;		//PV节点
	FSUINT m_nBetaNodeCnt;		//beta节点
	FSUINT m_nNullNodeCnt;		//空着成功的节点数

	FSINT m_nHashBetaNodeCnt;		//hash move截断次数
	FSINT m_nCapMoveBetaNodeCnt;	//Cap Move截断次数
	FSINT m_nKillerMoveBetaNodeCnt;	//Killer Move截断次数
	FSINT m_nNoCapMoveBetaNodeCnt;	//NoCap Move截断次数

	FSUINT m_nQuiesNodeCnt;			//静态搜索的节点数
	FSUINT m_nQuiesAlphaNodeCnt;	//静态搜索中的alpha节点数
	FSUINT m_nQuiesPVNodeCnt;		//静态搜索中的PV节点
	FSUINT m_nQuiesBetaNodeCnt;		//静态搜索中的beta节点
	FSUINT m_nQuiesNullNodeCnt;		//静态搜索中的空着成功的节点数

	FSUINT m_nProbeHashCnt;		//探测hash表次数
	FSUINT m_nRecordHashCnt;	//记录hash次数
	FSUINT m_nHitHashCnt;		//hash命中次数
	FSUINT m_nProbeQSHashCnt;	//探测qshash表次数
	FSUINT m_nRecordQSHashCnt;	//记录qshash次数
	FSUINT m_nHitQSHashCnt;		//qshash命中次数

#endif
	
	//当前棋局特征
	FSCHAR	m_szFEN[MAXBUFFERLEN];	//FEN串

	FSINT	m_nBoardValue;	//当前局面分
	FSINT	m_nChessAge;	//棋局所处阶段:开中残局
	FSBOOL	m_bIsRed;		//走子方:0:Black,1:Red

	FSINT m_nAllChessCnt;	//总棋子数
	FSINT m_nBigChessCnt;	//大子数,即车马炮数
	FSINT m_nChessCnt[15];	//各棋子数

	FSINT m_nCBChessID[90];	//棋盘(按纵线编号:左小右大,下小上大):[棋子位置],返回该位置的棋子类型编号
	FSINT m_nCBPieceID[90];	//[棋子位置]返回棋子编号,返回-1表示这个位置没有棋子

	FSINT m_nPiecePos[33];	//棋子位置数组:[棋子编号],返回90表示这个编号的棋子已经被吃掉

	//注意:位行和位列扩充到12
	FSINT m_nBitFile[12];	//棋子位行
	FSINT m_nBitRank[12];	//棋子位列

	FSINT m_nBRBitFile[3][12];	//黑红子位行:m_nBRBitFile[0]黑子位行,m_nBRBitFile[x][10 or 11] = 0
								//m_nBRBitFile[2]:无效,MakeMove用到
	FSINT m_nBRBitRank[3][12];	//黑红子位列:m_nBRBitRank[0]黑子位列,m_nBRBitRank[x][9 or 10] = 0
								//m_nBRBitRank[2]:无效,MakeMove用到
};

#endif