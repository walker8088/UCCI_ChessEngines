// EveluationBase.h: interface for the CBaseEveluation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVELUATIONBASE_H__02E08D9B_918A_4860_A745_4076F632250D__INCLUDED_)
#define AFX_EVELUATIONBASE_H__02E08D9B_918A_4860_A745_4076F632250D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Define.h"
#include <vector>

//存放每个棋子的基本价值，这些价值随着战局的不断进行而发生变化
const int iChessBaseValue[15][10][9]=
{
	//无子
	{
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
	},

	//黑将
	{
		{10000,10000,10000,10000,10000,10000,10000,10000,10000},
		{10000,10000,10000,10000,10000,10000,10000,10000,10000},
		{10000,10000,10000,10000,10000,10000,10000,10000,10000},
		{10000,10000,10000,10000,10000,10000,10000,10000,10000},
		{10000,10000,10000,10000,10000,10000,10000,10000,10000},
		{10000,10000,10000,10000,10000,10000,10000,10000,10000},
		{10000,10000,10000,10000,10000,10000,10000,10000,10000},
		{10000,10000,10000,10000,10000,10000,10000,10000,10000},
		{10000,10000,10000,10000,10000,10000,10000,10000,10000},
		{10000,10000,10000,10000,10000,10000,10000,10000,10000},
	},

	//黑车
	{
		{1300,1300,1300,1300,1300,1300,1300,1300,1300},
		{1300,1300,1300,1350,1300,1350,1300,1300,1300},
		{1300,1300,1300,1300,1300,1300,1300,1300,1300},
		{1300,1300,1300,1300,1300,1300,1300,1300,1300},
		{1325,1350,1350,1350,1350,1350,1350,1350,1325},//河界
		{1325,1350,1350,1350,1350,1350,1350,1350,1325},//河界
		{1300,1350,1400,1300,1300,1300,1400,1350,1300},
		{1300,1300,1300,1300,1300,1300,1300,1300,1300},
		{1400,1300,1300,1300,1300,1300,1300,1400,1300},//压马
		{1300,1300,1300,1300,1300,1300,1300,1300,1300},
	},

	//黑马
	{
		{520,520,540,570,540,570,540,520,520},
		{520,540,560,570,500,570,560,540,520},
		{520,560,580,590,560,590,580,560,520},
		{560,580,620,590,620,590,620,580,560},
		{540,620,650,660,670,660,650,620,540},
		{540,640,630,670,680,670,630,640,540},
		{570,720,630,710,640,710,630,720,570},
		{560,620,630,670,630,670,630,620,560},
		{540,600,670,610,580,610,670,600,540},
		{540,540,540,600,540,600,540,540,540},
	},

	//黑炮
	{
		{600,600,600,600,600,600,600,600,600},
		{600,600,600,600,600,600,600,600,600},
		{600,600,625,600,640,600,625,600,600},
		{600,600,600,600,640,600,600,600,600},
		{550,600,550,600,550,600,550,600,550},
		{600,600,600,600,600,600,600,600,600},
		{600,600,600,600,640,600,600,600,600},
		{600,600,600,600,600,600,600,600,600},
		{600,600,600,600,600,600,600,600,600},
		{600,600,600,600,600,600,600,600,600},
	},

	//黑士
	{
		{0,0,0,250,0,250,0,0,0},
		{0,0,0,0,260,0,0,0,0},
		{0,0,0,240,0,240,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
	},

	//黑象
	{
		{0,0,250,0,0,0,250,0,0},
		{0,0,0,0,0,0,0,0,0},
		{240,0,0,0,260,0,0,0,240},
		{0,0,0,0,0,0,0,0,0},
		{0,0,240,0,0,0,240,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
	},

	//黑卒
	{
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{100,0,100,0,200,0,100,0,100},
		{100,0,125,0,200,0,125,0,100},
		{200,200,200,200,220,200,200,200,200},
		{200,220,240,240,240,240,240,220,200},
		{220,220,240,250,250,250,240,220,220},
		{200,200,220,230,230,230,220,200,200},
		{50 ,75 ,100,100,100,100,100,75 ,50 },
	},

	//红帅
	{
		{10000,10000,10000,10000,10000,10000,10000,10000,10000},
		{10000,10000,10000,10000,10000,10000,10000,10000,10000},
		{10000,10000,10000,10000,10000,10000,10000,10000,10000},
		{10000,10000,10000,10000,10000,10000,10000,10000,10000},
		{10000,10000,10000,10000,10000,10000,10000,10000,10000},
		{10000,10000,10000,10000,10000,10000,10000,10000,10000},
		{10000,10000,10000,10000,10000,10000,10000,10000,10000},
		{10000,10000,10000,10000,10000,10000,10000,10000,10000},
		{10000,10000,10000,10000,10000,10000,10000,10000,10000},
		{10000,10000,10000,10000,10000,10000,10000,10000,10000},
	},

	//红车
	{
		{1300,1300,1300,1300,1300,1300,1300,1300,1300},
		{1300,1400,1300,1300,1300,1300,1300,1300,1400},//压马
		{1300,1300,1300,1300,1300,1300,1300,1300,1300},
		{1300,1350,1400,1300,1300,1300,1400,1350,1300},
		{1325,1350,1350,1350,1350,1350,1350,1350,1325},//河界
		{1325,1350,1350,1350,1350,1350,1350,1350,1325},//河界
		{1300,1300,1300,1300,1300,1300,1300,1300,1300},
		{1300,1300,1300,1300,1300,1300,1300,1300,1300},
		{1300,1300,1300,1350,1300,1350,1300,1300,1300},
		{1300,1300,1300,1300,1300,1300,1300,1300,1300},
	},

	//红马
	{
		{540,540,540,600,540,600,540,540,540},
		{540,600,670,610,580,610,670,600,540},
		{560,620,630,670,630,670,630,620,560},
		{570,720,630,710,640,710,630,720,570},
		{540,640,630,670,680,670,630,640,540},
		{540,620,650,660,670,660,650,620,540},
		{560,580,620,590,620,590,620,580,560},
		{520,560,580,590,560,590,580,560,520},
		{520,540,560,570,500,570,560,540,520},
		{520,520,540,570,540,570,540,520,520},
	},

	//红炮
	{
		{600,600,600,600,600,600,600,600,600},
		{600,600,600,600,600,600,600,600,600},
		{600,600,600,600,600,600,600,600,600},
		{600,600,600,600,640,600,600,600,600},
		{600,600,600,600,600,600,600,600,600},
		{550,600,550,600,550,600,550,600,550},
		{600,600,600,600,640,600,600,600,600},
		{600,600,625,600,640,600,625,600,600},
		{600,600,600,600,600,600,600,600,600},
		{600,600,600,600,600,600,600,600,600},
	},

	//红士
	{
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,240,0,240,0,0,0},
		{0,0,0,0,260,0,0,0,0},
		{0,0,0,250,0,250,0,0,0},
	},

	//红相
	{
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,240,0,0,0,240,0,0},
		{0,0,0,0,0,0,0,0,0},
		{240,0,0,0,260,0,0,0,240},
		{0,0,0,0,0,0,0,0,0},
		{0,0,250,0,0,0,250,0,0},
	},

	//红兵
	{
		{50 ,75 ,100,100,100,100,100,75 ,50 },
		{200,200,220,230,230,230,220,200,200},
		{220,220,240,250,250,250,240,220,220},
		{200,220,240,240,240,240,240,220,200},
		{200,200,200,200,220,200,200,200,200},
		{100,0,125,0,200,0,125,0,100},
		{100,0,100,0,200,0,100,0,100},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
	},
};

//每个棋子的防守能力
const UINT uiRecoveryValue[15]=
{
	0, //无子
	0, //将
	50,//黑车
	25,//黑马
	25,//黑炮
	25,//黑士
	30,//黑象
	0, //卒
	0, //帅
	50,//红车
	25,//红马
	25,//红炮
	25,//红士
	30,//红相
	0, //兵
};

//当前各类棋子数目
const int iChessCount[15]=
{
	0,//无子
	1,//黑将
	2,//黑车
	2,//黑马
	2,//黑炮
	2,//黑士
	2,//黑象
	5,//黑卒
	1,//红帅
	2,//红车
	2,//红马
	2,//红炮
	2,//红士
	2,//红相
	5,//红兵
};

//棋子灵活性
const int iFlexValue[15]=
{
	0, //无子
	0, //黑将
	6, //黑车
	12,//黑马
	6, //黑炮
	1, //黑士
	1, //黑象
	15,//黑卒
	0, //红帅
	6, //红车
	12,//红马
	6, //红炮
	1, //红士
	1, //红相
	15,//红兵
};

using namespace std;

class CBaseEveluation
{
public:
	CBaseEveluation();
	virtual ~CBaseEveluation();

public:
	virtual int Eveluate(BYTE byChessBoard[10][9],BOOL bIsRedTurn);
	int GetAccessCount(){return m_nAccessCount;};  //得到访问节点数
	UINT GetAllChessCnt(){return m_uiAllChessCnt;};//得到所有的棋子数
	void ClearAccessCount(){m_nAccessCount=0;};    //重置访问节点数为0
	void UpdateCurChess(BYTE byChess,bool bIsRed); //更新容器内棋子
	void UnUpdateCurChess(BYTE byChess,bool bIsRed);
	void InitVar(BYTE byChessBoar[][9]);//初始化各表征棋局的变量

protected:
	void AddPoint(int x,int y);//将一个位置加入相关位置队列
	//判断位置From的棋子是否能走到位置To
	bool CanTouch(BYTE byChessBoard[10][9],int nFromX,int nFromY,int nToX,int nToY);
	//列举与指定位置的棋子相关的棋子
	int GetRelatePiece(BYTE byChessBoard[10][9],int j,int i);
	void UpdateValue(BYTE byChessBoard[][9]);//随着战局的变化随时更新棋子的价值和阵形的价值
	int Multiply(int a,int b);//快速相乘

public:
	//------------开始棋局的特征------------
	vector<CHESSMANPOS> m_vectorCurChessPos[15];//当前各种棋子位置栈

	UINT m_uiCurBigChessCnt[15]; //统计过河各子的个数
	UINT m_uiCurLBigChessCnt[15];//过河在左边的棋子个数
	UINT m_uiCurRBigChessCnt[15];//过河在左边的棋子个数
	//--------------------------------------

protected:
	//-------阵形的价值-------
	//黑
	int m_iLineupB5ConnectPawn;//5连兵
	int m_iLineupB4ConnectPawn;//4连兵
	int m_iLineupB3ConnectPawn;//3连兵
	int m_iLineupB2ConnectPawn;//2连兵

	int m_iLineupBConnectElephant; //连环象
	int m_iLineupBConnectBishop;   //连环士
	int m_iLineupBConnectHorse;    //连环马
	int m_iLineupBScndConnectHorse;//亚连环马
	int m_iLineupBSznConnectHorse; //季连环马
	int m_iLineupBConnectCanon;    //连环炮

	//竖着的
	int m_iLineupBVCanonKing;			//空心炮:将的同列有一个炮，且它们之间无子
	int m_iLineupBVHorseCanon;			//马后炮将军，且马和将的距离为2
	int m_iLineupBVScndHorseCanon;		//马后炮将军，且马和将的距离不为2
	int m_iLineupBVCanonCar;		    //炮车将军
	int m_iLineupBVCarCanonKing;		//车炮将
	int m_iLineupBVDblCanon;			//双炮将军
	int m_iLineupBVCanon2Horese;        //炮和将之间有两子，其中有一个是对方的马
	int m_iLineupBCannonKingLieMidHorse;//炮和将之间有一卧心马，且将两边都是士
	int m_iLineupBVCanonCar2;			//炮和将之间有两子,其中有己方的车
	int m_iLineupBVCanon2Else;			//炮和将之间有两子，其中没有对方的马

	//横着的
	int m_iLineupBHCanonKing;	  //将的同行或同列有一个炮，且它们之间无子
	int m_iLineupBHHorseCanon;	  //马后炮将军，且马和将的距离为2
	int m_iLineupBHScndHorseCanon;//马后炮将军，且马和将的距离不为2
	int m_iLineupBHCanonCar;	  //炮车将军
	int m_iLineupBHDblCanon;	  //双炮将军
	int m_iLineupBHCanon2Horese;  //炮和将之间有两子，其中有一个是对方的马
	int m_iLineupBHCanonCar2;	  //炮和将之间有两子,其中有己方的车
	int m_iLineupBHCanon2Else;	  //炮和将之间有两子，其中没有对方的马

	int m_iLineupBCarKing;    //车和将在一行或一列
	int m_iLineupBCarHorse;   //车和马在一行或一列
	int m_iLineupBCarCanon;   //车和炮在一行或一列
	int m_iLineupBCarBishop;  //车和士在一行或一列
	int m_iLineupBCarElephant;//车和象在一行或一列
	int m_iLineupBCarPawn;    //车和兵在一行或一列

	int m_iLineupBCanonKingCar;    //炮牵制将，将后有车
	int m_iLineupBCanonCarCar;     //炮牵制车，将后有车
	int m_iLineupBCanonHorseCar;   //炮牵制马，将后有车
	int m_iLineupBCanonElephantCar;//炮牵制象，将后有车
	int m_iLineupBCanonBishopCar;  //炮牵制士，将后有车
	int m_iLineupBCanonPawnCar;    //炮牵制士，将后有车

	int m_iLineupBLieSlotHorseCar;//卧槽马和车,车在马的槽位:可以抽车吃子

	int m_iLineupBDblCarLoneKing;			   //双车对孤将
	int m_iLineupBDblCarLoneKingElephant;	   //双车对孤将一象
	int m_iLineupBDblCarLoneKing2Elephant;	   //双车对孤将二象
	int m_iLineupBDblCarLoneKingBishop;		   //双车对孤将一士
	int m_iLineupBDblCarLoneKingBishopElephant;//双车对孤将一士一象

	int m_iLineupBLieMidCar;			//窝心车
	int m_iLineupBLieMidCar_LoneKing;   //卧心车对孤将
	int m_iLineupBLieMidCar_BigCar;		//卧心车,有过河车
	int m_iLineupBLieMidCar_Car;		//卧心车,有车,无过河车
	int m_iLineupBLieMidCar_CanonBishop;//卧心车,有炮和士,无马兵
	int m_iLineupBLieMidCar_Horse[3];   //卧心车,有过河马,对方过河马的个数:0~2

	int m_iLineupBLieMidPawn;		     //窝心兵
	int m_iLineupBLieMidPawn_LoneKing;   //卧心兵对孤将
	int m_iLineupBLieMidPawn_BigCar;	 //卧心兵,有过河车
	int m_iLineupBLieMidPawn_Car;		 //卧心兵,有车,无过河车
	int m_iLineupBLieMidPawn_CanonBishop;//卧心兵,有炮和士,无马兵
	int m_iLineupBLieMidPawn_Horse[3];   //卧心兵,有过和兵马,对方过河马的个数:0~2

	int m_iLineupBLieSlotHorse;     //卧槽马
	int m_iLineupBLieMidHorse;      //卧心马
	int m_iLineupBImpertinenceHorse;//鲁莽马:车未行动，它已行

	int m_iLineupBBigAngleHorse;			//大角马
	int m_iLineupBBigAngleHorse_LoneKing;   //大角马对孤将
	int m_iLineupBBigAngleHorse_BigCar;		//大角马,有过河车
	int m_iLineupBBigAngleHorse_Car;		//大角马,有车,无过河车
	int m_iLineupBBigAngleHorse_CanonBishop;//大角马,有炮和士,无马兵
	int m_iLineupBBigAngleHorse_Horse[3];   //大角马,有过河马,对方过河马的个数:0~2

	int m_iLineupBNoProtectKing;			 //将离原位,第一维:车,二:马,三:炮
	int m_iLineupBNoProtectKing_CHC[3][3][3];//将离原位,第一维:对方过河车的个数,二:对方过河马的个数,三:对方过河炮的个数,对方过河车的个数:0~2,对方过河马的个数:0~2,对方过河炮的个数:0~2

	int m_iLineupBLoneKingVsCarHorse;        //孤将对车马
	int m_iLineupBKingVsCarHorse[3][3][3][6];//孤将对车马:将无士保护,对付对方过河一车一马,如果再无己方车保护的话,非常危险
											 //第一维:己方士,二:己方象,三:对方马,四:对方兵

	int m_iLineupBKingAttack;    //将助攻铁门栓
	int m_iLineupBKingCarAttack; //将助攻铁门栓,且车和将在一列,他们所在的列与对方将所在的行的交叉点上无子
	int m_iLineupBScndKingAttack;//将助攻铁门栓,他们所在的列与对方将所在的行的交叉点上有子

	int m_iLineupBAttackTogetherFactor;//协同作战因子

	int m_iLineupBCarHorseCannonPawnAttackTogether;//车马炮兵联合作战
	int m_iLineupBCarHorseCannonAttackTogether;    //车马炮联合作战
	int m_iLineupBCarHorsePawnAttackTogether;	   //车马兵联合作战
	int m_iLineupBCarCannonPawnAttackTogether;	   //车炮兵联合作战
	int m_iLineupBHorseCannonPawnAttackTogether;   //马炮兵联合作战

	int m_iLineupBCarHorseKing;//车马将
	int m_iLineupBCarPawnKing; //车兵将

	int m_iLineupBHCarCanonKing;			 //车炮将:炮碾丹沙
	int m_iLineupBHCarCanonKing_CHC[2][3][3];//车炮将:炮碾丹沙:第一维:己方过河车的个数-1

	//红
	int m_iLineupR5ConnectPawn;//5连兵
	int m_iLineupR4ConnectPawn;//4连兵
	int m_iLineupR3ConnectPawn;//3连兵
	int m_iLineupR2ConnectPawn;//2连兵

	int m_iLineupRConnectElephant; //连环象
	int m_iLineupRConnectBishop;   //连环士
	int m_iLineupRConnectHorse;    //连环马
	int m_iLineupRScndConnectHorse;//亚连环马
	int m_iLineupRSznConnectHorse; //季连环马
	int m_iLineupRConnectCanon;    //连环炮

	//竖着的
	int m_iLineupRVCanonKing;			//将的同行或同列有一个炮，且它们之间无子
	int m_iLineupRVHorseCanon;			//马后炮将军，且马和将的距离为2
	int m_iLineupRVScndHorseCanon;		//马后炮将军，且马和将的距离不为2
	int m_iLineupRVCanonCar;		    //炮车将军
	int m_iLineupRVCarCanonKing;		//车炮将
	int m_iLineupRVDblCanon;			//双炮将军
	int m_iLineupRVCanon2Horese;        //炮和将之间有两子，其中有一个是对方的马
	int m_iLineupRCannonKingLieMidHorse;//炮和将之间有一卧心马，且将两边都是士
	int m_iLineupRVCanonCar2;			//炮和将之间有两子,其中有己方的车
	int m_iLineupRVCanon2Else;			//炮和将之间有两子，其中没有对方的马

	//横着的
	int m_iLineupRHCanonKing;	  //将的同行或同列有一个炮，且它们之间无子
	int m_iLineupRHHorseCanon;	  //马后炮将军，且马和将的距离为2
	int m_iLineupRHScndHorseCanon;//马后炮将军，且马和将的距离不为2
	int m_iLineupRHCanonCar;	  //炮车将军
	int m_iLineupRHDblCanon;	  //双炮将军
	int m_iLineupRHCanon2Horese;  //炮和将之间有两子，其中有一个是对方的马
	int m_iLineupRHCanonCar2;	  //炮和将之间有两子,其中有己方的车
	int m_iLineupRHCanon2Else;	  //炮和将之间有两子，其中没有对方的马
	
	int m_iLineupRCarKing;    //车和将在一行或一列
	int m_iLineupRCarHorse;   //车和马在一行或一列
	int m_iLineupRCarCanon;   //车和炮在一行或一列
	int m_iLineupRCarBishop;  //车和士在一行或一列
	int m_iLineupRCarElephant;//车和象在一行或一列
	int m_iLineupRCarPawn;    //车和兵在一行或一列

	int m_iLineupRCanonKingCar;    //炮牵制将，将后有车
	int m_iLineupRCanonCarCar;     //炮牵制车，将后有车
	int m_iLineupRCanonHorseCar;   //炮牵制马，将后有车
	int m_iLineupRCanonElephantCar;//炮牵制象，将后有车
	int m_iLineupRCanonBishopCar;  //炮牵制士，将后有车
	int m_iLineupRCanonPawnCar;    //炮牵制士，将后有车

	int m_iLineupRLieSlotHorseCar;//卧槽马和车,车在马的槽位:可以抽车吃子

	int m_iLineupRDblCarLoneKing;			   //双车对孤将
	int m_iLineupRDblCarLoneKingElephant;	   //双车对孤将一象
	int m_iLineupRDblCarLoneKing2Elephant;	   //双车对孤将二象
	int m_iLineupRDblCarLoneKingBishop;		   //双车对孤将一士
	int m_iLineupRDblCarLoneKingBishopElephant;//双车对孤将一士一象

	int m_iLineupRLieMidCar;			//窝心车
	int m_iLineupRLieMidCar_LoneKing;   //卧心车对孤将
	int m_iLineupRLieMidCar_BigCar;		//卧心车,有过河车
	int m_iLineupRLieMidCar_Car;	    //卧心车,有车,无过河车
	int m_iLineupRLieMidCar_CanonBishop;//卧心车,有炮和士,无马兵
	int m_iLineupRLieMidCar_Horse[3];   //卧心车,有过河马

	int m_iLineupRLieMidPawn;		     //窝心兵
	int m_iLineupRLieMidPawn_LoneKing;   //卧心兵对孤将
	int m_iLineupRLieMidPawn_BigCar;	 //卧心兵,有过河车
	int m_iLineupRLieMidPawn_Car;		 //卧心兵,有车,无过河车
	int m_iLineupRLieMidPawn_CanonBishop;//卧心兵,有炮和士,无马兵
	int m_iLineupRLieMidPawn_Horse[3];   //卧心兵,有过和兵马

	int m_iLineupRLieSlotHorse;     //卧槽马
	int m_iLineupRLieMidHorse;      //卧心马
	int m_iLineupRImpertinenceHorse;//鲁莽马:车未行动，它已行

	int m_iLineupRBigAngleHorse;			//大角马
	int m_iLineupRBigAngleHorse_LoneKing;   //大角马对孤将
	int m_iLineupRBigAngleHorse_BigCar;		//大角马,有过河车
	int m_iLineupRBigAngleHorse_Car;		//大角马,有车,无过河车
	int m_iLineupRBigAngleHorse_CanonBishop;//大角马,有炮和士,无马兵
	int m_iLineupRBigAngleHorse_Horse[3];   //大角马,有过河马

	int m_iLineupRNoProtectKing;			 //无保护将
	int m_iLineupRNoProtectKing_CHC[3][3][3];//无保护将:将离原位,第一维:车,二:马,三:炮,对方过河车的个数:0~2,对方过河马的个数:0~2,对方过河炮的个数:0~2

	int m_iLineupRLoneKingVsCarHorse;        //孤将对车马
	int m_iLineupRKingVsCarHorse[3][3][3][6];//孤将对车马:将无士保护,对付对方过河一车一马,如果再无己方车保护的话,非常危险
											 //第一维:士,二:象,三:马,四:兵

	int m_iLineupRKingAttack;    //将助攻铁门栓
	int m_iLineupRKingCarAttack; //将助攻铁门栓,且车和将在一列,他们所在的列与对方将所在的行的交叉点上无子
	int m_iLineupRScndKingAttack;//将助攻铁门栓

	int m_iLineupRAttackTogetherFactor;		   //协同作战因子

	int m_iLineupRCarHorseCannonPawnAttackTogether;//车马炮兵联合作战
	int m_iLineupRCarHorseCannonAttackTogether;    //车马炮联合作战
	int m_iLineupRCarHorsePawnAttackTogether;	   //车马兵联合作战
	int m_iLineupRCarCannonPawnAttackTogether;	   //车炮兵联合作战
	int m_iLineupRHorseCannonPawnAttackTogether;   //马炮兵联合作战

	int m_iLineupRCarHorseKing;//车马将
	int m_iLineupRCarPawnKing; //车兵将

	int m_iLineupRHCarCanonKing;			 //车炮将
	int m_iLineupRHCarCanonKing_CHC[2][3][3];//车炮将:炮碾丹沙:第一维:己方过河车的个数-1
	//------------------------

	int m_iChessBaseValue[15][10][9];//存放各个棋子的价值
	int m_nAttackValue[15];			 //攻势价值
	int m_nAttackValue_EB[15][3][3]; //存放每个棋子的攻势价值,第二维:象,第三维:士
	UINT m_uiRecoveryValue[15];      //存放每个棋子的防守能力
	int m_iFlexValue[15];		     //存放棋子灵活性分值
	short m_AttackPos[10][9];	     //存放每一位置被威胁的信息
	BYTE m_GuardPos[10][9];          //存放每一位置被保护的信息
	BYTE m_FlexibilityPos[10][9];    //存放每一位置上棋子的灵活性分值
	int m_chessValue[10][9];	     //存放每一位置上棋子的总价值
	int nPosCount;				     //记录一棋子的相关位置个数
	POINT RelatePos[20];		     //记录一棋子的相关位置
	long m_nAccessCount;             //记录估值函数被访问的次数

	//------------开始棋局的特征------------
	int m_iChessCnt[15];  //当前各类棋子数目

	UINT m_uiAllChessCnt; //当前棋局共有的棋子数
	UINT m_uiAllBChessCnt;//当前棋局黑方共有的棋子数
	UINT m_uiAllRChessCnt;//当前棋局红方共有的棋子数

	UINT m_uiCurChessAge; //当前棋局所处的阶段

	vector<BYTE> m_vectorBCurChess;//黑方当前所拥有的棋子
	vector<BYTE> m_vectorRCurChess;//红方当前所拥有的棋子

	int m_nMainChessCnt; //当前棋局所拥有的主力棋子:车马炮
	int m_nBMainChessCnt;//当前棋局黑方所拥有的主力棋子:车马炮
	int m_nRMainChessCnt;//当前棋局红方所拥有的主力棋子:车马炮
	//--------------------------------------
};

#endif // !defined(AFX_EVELUATIONBASE_H__02E08D9B_918A_4860_A745_4076F632250D__INCLUDED_)
