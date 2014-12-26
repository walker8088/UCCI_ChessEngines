// EveluationBase.cpp: implementation of the CBaseEveluation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BaseEveluation.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBaseEveluation::CBaseEveluation()
{
	int i,j,k,m,n;

	memcpy(m_iChessBaseValue,iChessBaseValue,1350*sizeof(int));
	memcpy(m_iChessCnt,iChessCount,15*sizeof(int));
	memcpy(m_iFlexValue,iFlexValue,15*sizeof(int));
	memcpy(m_uiRecoveryValue,uiRecoveryValue,15*sizeof(UINT));

	//-------------附加攻势价值-------------
	//黑车
	for(i=0;i<3;i++)
	{
		for(j=0;j<3;j++)
			m_nAttackValue_EB[B_CAR][i][j]=32+(2-i)*5+(2-j)*3;
	}

	//黑马
	for(i=0;i<3;i++)
	{
		for(j=0;j<3;j++)
			m_nAttackValue_EB[B_HORSE][i][j]=80+(2-i)*5+(2-j)*3;
	}

	//黑炮
	for(i=0;i<3;i++)
	{
		for(j=0;j<3;j++)
			m_nAttackValue_EB[B_CANON][i][j]=25-((2-i)*3+(2-j)*2);
	}

	//黑兵
	for(i=0;i<3;i++)
	{
		for(j=0;j<3;j++)
			m_nAttackValue_EB[B_PAWN][i][j]=35+(2-i)*5+(2-j)*3;
	}


	//红车
	for(i=0;i<3;i++)
	{
		for(j=0;j<3;j++)
			m_nAttackValue_EB[R_CAR][i][j]=32+(2-i)*5+(2-j)*3;
	}

	//红马
	for(i=0;i<3;i++)
	{
		for(j=0;j<3;j++)
			m_nAttackValue_EB[R_HORSE][i][j]=80+(2-i)*5+(2-j)*3;
	}

	//红炮
	for(i=0;i<3;i++)
	{
		for(j=0;j<3;j++)
			m_nAttackValue_EB[R_CANON][i][j]=25-((2-i)*3+(2-j)*2);
	}

	//红兵
	for(i=0;i<3;i++)
	{
		for(j=0;j<3;j++)
			m_nAttackValue_EB[R_PAWN][i][j]=35+(2-i)*5+(2-j)*3;
	}
	//--------------------------------------

	//--------初始化阵形价值--------
	//黑
	//连兵
	m_iLineupB5ConnectPawn=500;//5连兵
	m_iLineupB4ConnectPawn=400;//4连兵
	m_iLineupB3ConnectPawn=300;//3连兵
	m_iLineupB2ConnectPawn=200;//2连兵

	m_iLineupBConnectElephant=50; //连环象
	m_iLineupBConnectBishop=25;   //连环士
	m_iLineupBConnectHorse=75;    //连环马
	m_iLineupBScndConnectHorse=30;//亚连环马
	m_iLineupBSznConnectHorse=20; //季连环马
	m_iLineupBConnectCanon=30;    //连环炮

	//竖着的
	m_iLineupBVCanonKing=200;			//空心炮:将的同列有一个炮，且它们之间无子
	m_iLineupBVHorseCanon=1000;			//马后炮将军，且马和将的距离为2
	m_iLineupBVScndHorseCanon=500;		//马后炮将军，且马和将的距离不为2
	m_iLineupBVCanonCar=500;			//炮车将军
	m_iLineupBVCarCanonKing=150;		//车炮将
	m_iLineupBVDblCanon=500;			//双炮将军
	m_iLineupBVCanon2Horese=350;		//炮和将之间有两子，其中有一个是对方的马
	m_iLineupBCannonKingLieMidHorse=500;//炮和将之间有一卧心马，且将两边都是士
	m_iLineupBVCanonCar2=250;			//炮和将之间有两子，其中有己方的车
	m_iLineupBVCanon2Else=175;			//炮和将之间有两子，其中没有对方的马

	//横着的
	m_iLineupBHCanonKing=150;	  //空心炮:将的同行有一个炮，且它们之间无子
	m_iLineupBHHorseCanon=9950;	  //马后炮将军，且马和将的距离为2
	m_iLineupBHScndHorseCanon=400;//马后炮将军，且马和将的距离不为2
	m_iLineupBHCanonCar=450;	  //炮车将军
	m_iLineupBHDblCanon=450;	  //双炮将军
	m_iLineupBHCanon2Horese=300;  //炮和将之间有两子，其中有一个是对方的马
	m_iLineupBHCanonCar2=240;	  //炮和将之间有两子，其中有己方的车
	m_iLineupBHCanon2Else=150;	  //炮和将之间有两子，其中没有对方的马

	m_iLineupBCarKing=-500;   //车和将在一行或一列
	m_iLineupBCarHorse=-200;  //车和马在一行或一列
	m_iLineupBCarCanon=-100;  //车和炮在一行或一列
	m_iLineupBCarBishop=-50;  //车和士在一行或一列
	m_iLineupBCarElephant=-50;//车和象在一行或一列
	m_iLineupBCarPawn=-25;    //车和兵在一行或一列

	m_iLineupBCanonKingCar=1300;  //炮牵制将，将后有车
	m_iLineupBCanonCarCar=1300;   //炮牵制车，车后有车
	m_iLineupBCanonHorseCar=200;  //炮牵制马，马后有车
	m_iLineupBCanonElephantCar=50;//炮牵制象，象后有车
	m_iLineupBCanonBishopCar=50;  //炮牵制士，士后有车
	m_iLineupBCanonPawnCar=75;    //炮牵制兵，兵后有车

	m_iLineupBLieMidCar_LoneKing=2000;  //卧心车对孤将
	m_iLineupBLieMidCar_BigCar=1000;	//卧心车,有过河车
	m_iLineupBLieMidCar_Car=500;		//卧心车,有车,无过河车
	m_iLineupBLieMidCar_CanonBishop=200;//卧心车,有炮和士,无马兵
	//卧心车,有过河马 m_iLineupBLieMidCar_Horse[3]
	for(i=0;i<3;i++)
		m_iLineupBLieMidCar_Horse[i]=30*i;

	m_iLineupBLieMidPawn_LoneKing=2000;  //卧心兵对孤将
	m_iLineupBLieMidPawn_BigCar=800;	 //卧心兵,有过河车
	m_iLineupBLieMidPawn_Car=400;		 //卧心兵,有车,无过河车
	m_iLineupBLieMidPawn_CanonBishop=150;//卧心兵,有炮和士,无马兵
	//卧心兵,有过河兵马 m_iLineupBLieMidPawn_Horse[3]
	for(i=0;i<3;i++)
		m_iLineupBLieMidPawn_Horse[i]=25*i;

	m_iLineupBDblCarLoneKing=1000;		       //双车对孤将
	m_iLineupBDblCarLoneKingElephant=950;      //双车对孤将一象
	m_iLineupBDblCarLoneKing2Elephant=900;	   //双车对孤将二象
	m_iLineupBDblCarLoneKingBishop=950;		   //双车对孤将一士
	m_iLineupBDblCarLoneKingBishopElephant=900;//双车对孤将一士一象

	m_iLineupBLieSlotHorse=500;     //卧槽马
	m_iLineupBLieMidHorse=-100;     //卧心马
	m_iLineupBImpertinenceHorse=-80;//鲁莽马:车未行动，它已行

	m_iLineupBBigAngleHorse_LoneKing=2000;  //大角马对孤将
	m_iLineupBBigAngleHorse_BigCar=500;		//大角马,有过河车
	m_iLineupBBigAngleHorse_Car=400;		//大角马,有车,无过河车
	m_iLineupBBigAngleHorse_CanonBishop=200;//大角马,有炮和士,无马兵
    //大角马,有过河马 m_iLineupBBigAngleHorse_Horse[3]
	for(i=0;i<3;i++)
		m_iLineupBBigAngleHorse_Horse[i]=30*i;

	//将离原位 m_iLineupBNoProtectKing_CHC[3][3][3]
	for(i=0;i<3;i++)
	{
		for(j=0;j<3;j++)
			for(k=0;k<3;k++)
				m_iLineupBNoProtectKing_CHC[i][j][k]=i*100+j*80+k*40;
	}

	m_iLineupBLoneKingVsCarHorse=-500;//孤将对车马:将无士保护,对付对方过河一车一马,如果再无己方车保护的话,非常危险
	for(i=0;i<3;i++)
	{
		for(j=0;j<3;j++)
			for(m=0;m<3;m++)
				for(n=0;n<6;n++)
					m_iLineupBKingVsCarHorse[i][j][m][n]=-((2-i)*60+(2-j)*30+m*50+n*10);
	}

	m_iLineupBKingAttack=50;    //将助攻铁门栓
	m_iLineupBKingCarAttack=200;//将助攻铁门栓,且车和将在一列,他们所在的列与对方将所在的行的交叉点上无子
	m_iLineupBScndKingAttack=30;//将助攻铁门栓

	m_iLineupBAttackTogetherFactor=25;//协同作战因子

	m_iLineupBCarHorseCannonPawnAttackTogether=300;//车马炮兵联合作战
	m_iLineupBCarHorseCannonAttackTogether=250;    //车马炮联合作战
	m_iLineupBCarHorsePawnAttackTogether=200;	   //车马兵联合作战
	m_iLineupBCarCannonPawnAttackTogether=150;	   //车炮兵联合作战
	m_iLineupBHorseCannonPawnAttackTogether=100;   //马炮兵联合作战

	//车炮将:炮碾丹沙 m_iLineupBHCarCanonKing_CHC[2][3][3]
	for(i=0;i<2;i++)
	{
		for(j=0;j<3;j++)
			for(k=0;k<3;k++)
				m_iLineupBHCarCanonKing_CHC[i][j][k]=300+i*100+j*50+k*40;
	}

	m_iLineupBCarHorseKing=100;//车马将
	m_iLineupBCarPawnKing=10;  //车兵将

	//红
	m_iLineupR5ConnectPawn=500;//5连兵
	m_iLineupR4ConnectPawn=400;//4连兵
	m_iLineupR3ConnectPawn=300;//3连兵
	m_iLineupR2ConnectPawn=200;//2连兵

	m_iLineupRConnectElephant=50; //连环象
	m_iLineupRConnectBishop=25;   //连环士
	m_iLineupRConnectHorse=75;    //连环马
	m_iLineupRScndConnectHorse=30;//亚连环马
	m_iLineupRSznConnectHorse=20; //季连环马
	m_iLineupRConnectCanon=100;   //连环炮

	//竖着的
	m_iLineupRVCanonKing=200;			 //将的同列有一个炮，且它们之间无子
	m_iLineupRVHorseCanon=1000;			 //马后炮将军，且马和将的距离为2
	m_iLineupRVScndHorseCanon=500;		 //马后炮将军，且马和将的距离不为2
	m_iLineupRVCanonCar=500;			 //炮车将军
	m_iLineupRVCarCanonKing=150;		 //车炮将
	m_iLineupRVDblCanon=500;			 //双炮将军
	m_iLineupRVCanon2Horese=350;		 //炮和将之间有两子，其中有一个是对方的马
	m_iLineupRCannonKingLieMidHorse=500;//炮和将之间有一卧心马，且将两边都是士
	m_iLineupRVCanonCar2=250;			 //炮和将之间有两子，其中有己方的车
	m_iLineupRVCanon2Else=175;			 //炮和将之间有两子，其中没有对方的马

	//横着的
	m_iLineupRHCanonKing=150;	  //将的同行有一个炮，且它们之间无子
	m_iLineupRHHorseCanon=9950;	  //马后炮将军，且马和将的距离为2
	m_iLineupRHScndHorseCanon=400;//马后炮将军，且马和将的距离不为2
	m_iLineupRHCanonCar=450;	  //炮车将军
	m_iLineupRHCarCanonKing=300;  //车炮将
	m_iLineupRHDblCanon=450;	  //双炮将军
	m_iLineupRHCanon2Horese=300;  //炮和将之间有两子，其中有一个是对方的马
	m_iLineupRHCanonCar2=240;	  //炮和将之间有两子，其中有己方的车
	m_iLineupRHCanon2Else=150;	  //炮和将之间有两子，其中没有对方的马

	m_iLineupRCarKing=-500;      //车和将在一行或一列
	m_iLineupRCarHorse=-200;     //车和马在一行或一列
	m_iLineupRCarCanon=-100;     //车和炮在一行或一列
	m_iLineupRCarBishop=-50;     //车和士在一行或一列
	m_iLineupRCarElephant=-50;   //车和象在一行或一列
	m_iLineupRCarPawn=-25;       //车和兵在一行或一列

	m_iLineupRCanonKingCar=1300;  //炮牵制将，将后有车
	m_iLineupRCanonCarCar=1300;   //炮牵制车，将后有车
	m_iLineupRCanonHorseCar=200;  //炮牵制马，将后有车
	m_iLineupRCanonElephantCar=50;//炮牵制象，将后有车
	m_iLineupRCanonBishopCar=50;  //炮牵制士，将后有车
	m_iLineupRCanonPawnCar=75;    //炮牵制兵，将后有车

	m_iLineupRDblCarLoneKing=1000;		       //双车对孤将
	m_iLineupRDblCarLoneKingElephant=950;      //双车对孤将一象
	m_iLineupRDblCarLoneKing2Elephant=900;	   //双车对孤将二象
	m_iLineupRDblCarLoneKingBishop=950;		   //双车对孤将一士
	m_iLineupRDblCarLoneKingBishopElephant=900;//双车对孤将一士一象

	m_iLineupRLieMidCar_LoneKing=2000;  //卧心车对孤将
	m_iLineupRLieMidCar_BigCar=1000;	//卧心车,有过河车
	m_iLineupRLieMidCar_Car=500;		//卧心车,有车,无过河车
	m_iLineupRLieMidCar_CanonBishop=200;//卧心车,有炮和士,无马兵
	//卧心车,有过河马
	for(i=0;i<3;i++)
		m_iLineupRLieMidCar_Horse[i]=30*i;

	m_iLineupRLieMidPawn_LoneKing=2000;  //卧心兵对孤将
	m_iLineupRLieMidPawn_BigCar=800;	 //卧心兵,有过河车
	m_iLineupRLieMidPawn_Car=400;		 //卧心兵,有车,无过河车
	m_iLineupRLieMidPawn_CanonBishop=150;//卧心兵,有炮和士,无马兵
	//卧心兵,有过河兵马
	for(i=0;i<3;i++)
		m_iLineupRLieMidPawn_Horse[i]=25*i+20*j;

	m_iLineupRLieSlotHorse=500;     //卧槽马
	m_iLineupRLieMidHorse=-100;     //卧心马
	m_iLineupRImpertinenceHorse=-80;//鲁莽马:车未行动，它已行
	
	m_iLineupRBigAngleHorse_LoneKing=2000;  //大角马对孤将
	m_iLineupRBigAngleHorse_BigCar=500;		//大角马,有过河车
	m_iLineupRBigAngleHorse_Car=400;		//大角马,有车,无过河车
	m_iLineupRBigAngleHorse_CanonBishop=200;//大角马,有炮和士,无马兵
    //大角马,有过河马
	for(i=0;i<3;i++)
		m_iLineupRBigAngleHorse_Horse[i]=30*i;

	//将离原位 m_iLineupRNoProtectKing_CHC[3][3][3]
	for(i=0;i<3;i++)
	{
		for(j=0;j<3;j++)
			for(k=0;k<3;k++)
				m_iLineupRNoProtectKing_CHC[i][j][k]=i*100+j*80+k*40;
	}

	m_iLineupRLoneKingVsCarHorse=-500;//孤将对车马:将无士保护,对付对方过河一车一马,如果再无己方车保护的话,非常危险
	for(i=0;i<3;i++)
	{
		for(j=0;j<3;j++)
			for(m=0;m<3;m++)
				for(n=0;n<6;n++)
					m_iLineupRKingVsCarHorse[i][j][m][n]=-((2-i)*60+(2-j)*30+m*50+n*10);
	}

	m_iLineupRKingAttack=50;    //将助攻铁门栓
	m_iLineupRKingCarAttack=200;//将助攻铁门栓,且车和将在一列,他们所在的列与对方将所在的行的交叉点上无子
	m_iLineupRScndKingAttack=30;//将助攻铁门栓

	m_iLineupRAttackTogetherFactor=25;//协同作战因子

	m_iLineupRCarHorseCannonPawnAttackTogether=300;//车马炮兵联合作战
	m_iLineupRCarHorseCannonAttackTogether=250;    //车马炮联合作战
	m_iLineupRCarHorsePawnAttackTogether=200;	   //车马兵联合作战
	m_iLineupRCarCannonPawnAttackTogether=150;	   //车炮兵联合作战
	m_iLineupRHorseCannonPawnAttackTogether=100;   //马炮兵联合作战

	//车炮将:炮碾丹沙 m_iLineupRHCarCanonKing_CHC[2][3][3]
	for(i=0;i<2;i++)
	{
		for(j=0;j<3;j++)
			for(k=0;k<3;k++)
				m_iLineupRHCarCanonKing_CHC[i][j][k]=300+i*100+j*50+k*40;
	}

	m_iLineupRCarHorseKing=100;//车马将
	m_iLineupRCarPawnKing=10;  //车兵将
	//------------------------------

	m_nAccessCount=0;
}

CBaseEveluation::~CBaseEveluation()
{

}

int CBaseEveluation::Eveluate(BYTE byChessBoard[10][9],BOOL bIsRedTurn)
{
	return 0;
}

int CBaseEveluation::GetRelatePiece(BYTE byChessBoard[][9], int j, int i)
{
	nPosCount=0;
	BYTE nChessID;
	BYTE flag;
	int x,y;
	
	nChessID=byChessBoard[i][j];

	switch(nChessID)
	{
	case R_KING://红帅
	case B_KING://黑将
		//循环检查九宫之内哪些位置可到达/保护
		//扫描两边就宫包含了照像的情况
		for(y=0;y<3;y++)
			for(x=3;x<6;x++)
				if(CanTouch(byChessBoard,j,i,x,y))//能否到达
					AddPoint(x,y);//可达到/保护的位置加入数组

		//循环检查九宫之内哪些位置可到达/保护
		//扫描两边就宫包含了照像的情况
		for(y=7;y<10;y++)
			for(x=3;x<6;x++)
				if(CanTouch(byChessBoard,j,i,x,y))//能否到达
					AddPoint(x,y);//可达到/保护的位置加入数组

		break;

	case R_BISHOP://红士
		//循环检查九宫之内哪些位置可到达/保护
		for(y=7;y<10;y++)
			for(x=3;x<6;x++)
				if(CanTouch(byChessBoard,j,i,x,y))
					AddPoint(x,y);//可达到/保护的位置加入数组
		
		break;

	case B_BISHOP://黑士
		//循环检查九宫之内哪些位置可到达/保护
		for(y=0;y<3;y++)
			for(x=3;x<6;x++)
				if(CanTouch(byChessBoard,j,i,x,y))
					AddPoint(x,y);//可达到/保护的位置加入数组
		
		break;

	case R_ELEPHANT://红相
	case B_ELEPHANT://黑象
		//右下
		x=j+2;
		y=i+2;
		if(x<9 && y<10 && CanTouch(byChessBoard,j,i,x,y))
			AddPoint(x,y);

		//右上
		x=j+2;
		y=i-2;
		if(x<9 && y>=0 && CanTouch(byChessBoard,j,i,x,y))
			AddPoint(x,y);

		//左下
		x=j-2;
		y=i+2;
		if(x>=0 && y<10 && CanTouch(byChessBoard,j,i,x,y))
			AddPoint(x,y);

		//左上
		x=j-2;
		y=i-2;
		if(x>=0 && y>=0 && CanTouch(byChessBoard,j,i,x,y))
			AddPoint(x,y);

		break;

		case R_HORSE://红马
		case B_HORSE://黑马
			//检查右下方能否到达/保护
			x=j+2;
			y=i+1;
			if((x<9 && y<10)&&CanTouch(byChessBoard,j,i,x,y))
				AddPoint(x,y);

			//检查右上方能否到达/保护
			x=j+2;
			y=i-1;
			if((x<9 && y>=0)&&CanTouch(byChessBoard,j,i,x,y))
				AddPoint(x,y);

			//检查左下方能否到达/保护
			x=j-2;
			y=i+1;
			if((x>=0 && y<10)&&CanTouch(byChessBoard,j,i,x,y))
				AddPoint(x,y);

			//检查左上方能否到达/保护
			x=j-2;
			y=i-1;
			if((x>=0 && y>=0)&&CanTouch(byChessBoard,j,i,x,y))
				AddPoint(x,y);

			//检查右下方能否到达/保护
			x=j+1;
			y=i+2;
			if((x<9 && y<10)&&CanTouch(byChessBoard,j,i,x,y))
				AddPoint(x,y);

			//检查右上方能否到达/保护
			x=j+1;
			y=i-2;
			if((x<9 && y>=0)&&CanTouch(byChessBoard,j,i,x,y))
				AddPoint(x,y);

			//检查左下方能否到达/保护
			x=j-1;
			y=i+2;
			if((x>=0 && y<10)&&CanTouch(byChessBoard,j,i,x,y))
				AddPoint(x,y);

			//检查左上方能否到达/保护
			x=j-1;
			y=i-2;
			if((x>=0 && y>=0)&&CanTouch(byChessBoard,j,i,x,y))
				AddPoint(x,y);

			break;

		case R_CAR://红车
		case B_CAR://黑车
			//检查向右能否到达/保护
			x=j+1;
			y=i;
			while(x<9)
			{
				if(NOCHESS==byChessBoard[y][x])//空白
					AddPoint(x,y);
				else{
					//碰到第一个棋子
					AddPoint(x,y);
					break;//后面的位置不能走了
				}
				x++;
			}

			//检查向左能否到达/保护
			x=j-1;
			y=i;
			while(x>=0)
			{
				if(NOCHESS==byChessBoard[y][x])//空白
					AddPoint(x,y);
				else{
					//碰到第一个棋子
					AddPoint(x,y);
					break;//后面的位置不能走了
				}
				x--;
			}

			//检查向下能否到达/保护
			x=j;
			y=i+1;
			while(y<10)
			{
				if(NOCHESS==byChessBoard[y][x])//空白
					AddPoint(x,y);
				else{
					//碰到第一个棋子
					AddPoint(x,y);
					break;//后面的位置不能走了
				}
				y++;
			}

			//检查向上能否到达/保护
			x=j;
			y=i-1;
			while(y>=0)
			{
				if(NOCHESS==byChessBoard[y][x])//空白
					AddPoint(x,y);
				else{
					//碰到第一个棋子
					AddPoint(x,y);
					break;//后面的位置不能走了
				}
				y--;
			}

			break;

		case R_PAWN://红兵
			//观看向前是否到底
			y=i-1;
			x=j;
			if(y>=0)
				AddPoint(x,y);//没到底，可走
			if(i<5)
			{
				//如已过河
				y=i;
				x=j+1;//向右
				if(x<9)
					AddPoint(x,y);//未到右边，可走
				x=j-1;//向左
				if(x>=0)
					AddPoint(x,y);//未到左边，可走
			}

			break;

		case B_PAWN://黑卒
			//观看向前是否到底
			y=i+1;
			x=j;
			if(y<10)
				AddPoint(x,y);//没到底，可走
			if(i>4)
			{
				//如已过河
				y=i;
				x=j+1;//向右
				if(x<9)
					AddPoint(x,y);//未到右边，可走
				x=j-1;//向左
				if(x>=0)
					AddPoint(x,y);//未到左边，可走
			}

			break;

		case B_CANON://黑炮
		case R_CANON://红炮
			//检查向右能否到达/保护的位置
			x=j+1;
			y=i;
			flag=FALSE;
			while(x<9)
			{
				if(NOCHESS==byChessBoard[y][x])
				{
					//空白位置
					if(!flag)
						AddPoint(x,y);
				}
				else
				{
					if(!flag)
						flag=TRUE;//是第一个棋子
					else
					{
						//是第二个棋子
						AddPoint(x,y);
						break;
					}
				}
				x++;//继续向右
			}

			//检查向左能否到达/保护的位置
			x=j-1;
			y=i;
			flag=FALSE;
			while(x>=0)
			{
				if(NOCHESS==byChessBoard[y][x])
				{
					//空白位置
					if(!flag)
						AddPoint(x,y);
				}
				else
				{
					if(!flag)
						flag=TRUE;//是第一个棋子
					else
					{
						//是第二个棋子
						AddPoint(x,y);
						break;
					}
				}
				x--;//继续向左
			}

			//检查向下能否到达/保护的位置
			x=j;
			y=i+1;
			flag=FALSE;
			while(y<10)
			{
				if(NOCHESS==byChessBoard[y][x])
				{
					//空白位置
					if(!flag)
						AddPoint(x,y);
				}
				else
				{
					if(!flag)
						flag=TRUE;//是第一个棋子
					else
					{
						//是第二个棋子
						AddPoint(x,y);
						break;
					}
				}
				y++;//继续向下
			}

			//检查向上能否到达/保护的位置
			x=j;
			y=i-1;
			flag=FALSE;
			while(y>=0)
			{
				if(NOCHESS==byChessBoard[y][x])
				{
					//空白位置
					if(!flag)
						AddPoint(x,y);
				}
				else
				{
					if(!flag)
						flag=TRUE;//是第一个棋子
					else
					{
						//是第二个棋子
						AddPoint(x,y);
						break;
					}
				}
				y--;//继续向上
			}

			break;

		default:
			break;
		}

	return nPosCount;
}

bool CBaseEveluation::CanTouch(BYTE byChessBoard[][9], int nFromX, int nFromY, int nToX, int nToY)
{
	int i,j;
	int nMoveChessID,nTargetID;

	if(nFromX==nToX && nFromY==nToY)
		return false;//目的与源相同，非法

	nMoveChessID=byChessBoard[nFromX][nFromY];
	nTargetID=byChessBoard[nToX][nToY];

	if(IsSameSide(nMoveChessID,nTargetID))
		return false;//吃自己的棋，非法

	switch(nMoveChessID)
	{
	case B_KING://黑将
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

		break;

	case R_KING://红帅
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

		break;

	case R_BISHOP://红士
		if(nToY<7 || nToX>5 || nToX<3)
			return false;//士出九宫

		if(abs(nFromX-nToX)!=1 || abs(nFromY-nToY)!=1)
			return false;//士走斜线

		break;

	case B_BISHOP://黑士
		if(nToY>2 || nToX>5 || nToX<3)
			return false;//士出九宫

		if(abs(nFromX-nToX)!=1 || abs(nFromY-nToY)!=1)
			return false;//士走斜线

		break;

	case R_ELEPHANT://红相
		if(nToY<5)
			return false;//相不能过河

		if(abs(nFromX-nToX)!=2 || abs(nFromY-nToY)!=2)
			return false;//相走田字

		if(byChessBoard[(nFromY +nToY)/2][(nFromX +nToX)/2]!=NOCHESS)
			return FALSE;//相眼被塞

		break;

	case B_ELEPHANT://黑象
		if(nToY>4)
			return false;//象不能过河

		if(abs(nFromX-nToX)!=2 || abs(nFromY-nToY)!=2)
			return false;//象走田字

		if(byChessBoard[(nFromY +nToY)/2][(nFromX +nToX)/2]!=NOCHESS)
			return FALSE;//象眼被塞

		break;

	case B_PAWN://黑卒
		if(nToY<nFromY)
			return false;//卒不能回头

		if(nFromY<5 && nFromY==nToY)
			return FALSE;//卒过河前只能直走

		if(nToY-nFromY+abs(nToX -nFromX)>1)
			return FALSE;//卒只走一步直线

		break;

	case R_PAWN://红兵
		if(nToY<nFromY)
			return false;//兵不能回头

		if(nFromY>4 && nFromY==nToY)
			return FALSE;//兵过河前只能直走

		if(nToY-nFromY+abs(nToX -nFromX)>1)
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
					for(j=nFromY+1;j<nToY;j++)
						if(byChessBoard[j][nFromX]!=NOCHESS)
							j++;
					if(j!=1)
						return FALSE;
				}
				else
				{
					for(j=nToY+1;j<nFromY;j++)
						if(byChessBoard[j][nFromX]!=NOCHESS)
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

void CBaseEveluation::AddPoint(int x, int y)
{
	RelatePos[nPosCount].x=x;
	RelatePos[nPosCount].y=y;

	nPosCount++;
}

void CBaseEveluation::UpdateCurChess(BYTE byChess, bool bIsRed)
{
	if(byChess==NOCHESS)
		return;

	BYTE byTempChess;
	int iSize;
	int i;

	if(bIsRed)
	{
		//-------------更新红方当前所拥有的棋子-------------
		iSize=m_vectorRCurChess.size();
		for(i=0;i<iSize;i++)
		{
			if(byChess==m_vectorRCurChess[i])
			{
				//把当前棋子与顶棋子交换
				byTempChess=m_vectorRCurChess[i];
				m_vectorRCurChess[i]=m_vectorRCurChess[iSize-1];
				m_vectorRCurChess[iSize-1]=byTempChess;

				//弹出顶棋子
				m_vectorRCurChess.pop_back();
			}
		}
		//------------------------------------------

		//更新红方主力棋子数
		if(byChess==R_CAR || byChess==R_HORSE || byChess==R_CANON)
		{
			m_nMainChessCnt--; //当前棋局所拥有的主力棋子:车马炮
			m_nRMainChessCnt--;//当前棋局红方所拥有的主力棋子:车马炮
		}

		m_uiAllRChessCnt--;//当前棋局红方共有的棋子数
	}
	else
	{
		//-------------更新红方当前所拥有的棋子-------------
		iSize=m_vectorBCurChess.size();
		for(i=0;i<iSize;i++)
		{
			if(byChess==m_vectorBCurChess[i])
			{
				byTempChess=m_vectorBCurChess[i];
				m_vectorBCurChess[i]=m_vectorBCurChess[iSize-1];
				m_vectorBCurChess[iSize-1]=byTempChess;
				m_vectorBCurChess.pop_back();
			}
		}
		//------------------------------------------

		//更新黑方主力棋子数
		if(byChess==B_CAR || byChess==B_HORSE || byChess==B_CANON)
		{
			m_nMainChessCnt--; //当前棋局所拥有的主力棋子:车马炮
			m_nBMainChessCnt--;//当前棋局黑方所拥有的主力棋子:车马炮
		}

		m_uiAllBChessCnt--;//当前棋局黑方共有的棋子数
	}

	m_iChessCnt[byChess]--;//当前各类棋子数目
	m_uiAllChessCnt--;     //当前棋局共有的棋子数

	//判断当前棋局所处阶段
	if(m_uiAllChessCnt>=28)
		m_uiCurChessAge=CA_START;
	else
	{
		if(m_nMainChessCnt>4)
			m_uiCurChessAge=CA_MID;
		else
			m_uiCurChessAge=CA_END;
	}

	//当前各种棋子位置栈,过河个子个数在CBaseSearchEngine的MakeMove中更新
}

void CBaseEveluation::UpdateValue(BYTE byChessBoard[][9])
{
	int i,j;
	int iSize;
	int iDist;
	bool bFlag;

	//--------------车压马--------------
	//黑
	if(byChessBoard[0][0]==B_CAR && byChessBoard[0][1]==B_HORSE)
		m_iChessBaseValue[R_CAR][1][1]=1400;
	else
		m_iChessBaseValue[R_CAR][1][1]=1300;
	if(byChessBoard[0][8]==B_CAR && byChessBoard[0][7]==B_HORSE)
		m_iChessBaseValue[R_CAR][1][7]=1400;
	else
		m_iChessBaseValue[R_CAR][1][7]=1300;

	//红
	if(byChessBoard[9][0]==R_CAR && byChessBoard[9][1]==R_HORSE)
		m_iChessBaseValue[B_CAR][8][1]=1400;
	else
		m_iChessBaseValue[B_CAR][8][1]=1300;
	if(byChessBoard[9][8]==R_CAR && byChessBoard[9][7]==R_HORSE)
		m_iChessBaseValue[B_CAR][8][7]=1400;
	else
		m_iChessBaseValue[B_CAR][8][7]=1300;
	//----------------------------------

	//----------单象/相时象/相价值降低----------
	//黑
	if(m_iChessCnt[B_ELEPHANT]==1)
	{
		m_iChessBaseValue[B_ELEPHANT][0][2]=200;
		m_iChessBaseValue[B_ELEPHANT][0][6]=200;
		m_iChessBaseValue[B_ELEPHANT][2][0]=200;
		m_iChessBaseValue[B_ELEPHANT][2][4]=200;
		m_iChessBaseValue[B_ELEPHANT][2][8]=200;
		m_iChessBaseValue[B_ELEPHANT][4][2]=200;
		m_iChessBaseValue[B_ELEPHANT][4][6]=200;
	}
	else
	{
		m_iChessBaseValue[B_ELEPHANT][0][2]=250;
		m_iChessBaseValue[B_ELEPHANT][0][6]=250;
		m_iChessBaseValue[B_ELEPHANT][2][0]=240;
		m_iChessBaseValue[B_ELEPHANT][2][4]=260;
		m_iChessBaseValue[B_ELEPHANT][2][8]=240;
		m_iChessBaseValue[B_ELEPHANT][4][2]=240;
		m_iChessBaseValue[B_ELEPHANT][4][6]=240;
	}

	//红
	if(m_iChessCnt[R_ELEPHANT]==1)
	{
		m_iChessBaseValue[R_ELEPHANT][5][2]=200;
		m_iChessBaseValue[R_ELEPHANT][5][6]=200;
		m_iChessBaseValue[R_ELEPHANT][7][0]=200;
		m_iChessBaseValue[R_ELEPHANT][7][4]=200;
		m_iChessBaseValue[R_ELEPHANT][7][8]=200;
		m_iChessBaseValue[R_ELEPHANT][9][2]=200;
		m_iChessBaseValue[R_ELEPHANT][9][6]=200;
	}
	else
	{
		m_iChessBaseValue[R_ELEPHANT][5][2]=250;
		m_iChessBaseValue[R_ELEPHANT][5][6]=250;
		m_iChessBaseValue[R_ELEPHANT][7][0]=240;
		m_iChessBaseValue[R_ELEPHANT][7][4]=260;
		m_iChessBaseValue[R_ELEPHANT][7][8]=240;
		m_iChessBaseValue[R_ELEPHANT][9][2]=240;
		m_iChessBaseValue[R_ELEPHANT][9][6]=240;
	}
	//------------------------------------------

	//----------------有中炮时,架边象,防止外敌入侵----------------
	//黑
	if(byChessBoard[2][4]!=NOCHESS)
	{
		if(byChessBoard[6][2]==R_PAWN || byChessBoard[7][2]==R_PAWN)
			m_iChessBaseValue[B_ELEPHANT][2][0]=260;
		if(byChessBoard[6][6]==R_PAWN || byChessBoard[7][6]==R_PAWN)
			m_iChessBaseValue[B_ELEPHANT][2][8]=260;
	}
	else
	{
		m_iChessBaseValue[B_ELEPHANT][2][0]=240;
		m_iChessBaseValue[B_ELEPHANT][2][8]=240;
		m_iChessBaseValue[B_ELEPHANT][2][4]=260;
	}

	//红
	if(byChessBoard[7][4]!=NOCHESS)
	{
		if(byChessBoard[3][2]==B_PAWN || byChessBoard[4][2]==B_PAWN)
			m_iChessBaseValue[R_ELEPHANT][7][0]=260;
		if(byChessBoard[3][6]==B_PAWN || byChessBoard[4][6]==B_PAWN)
			m_iChessBaseValue[R_ELEPHANT][7][8]=260;
	}
	else
	{
		m_iChessBaseValue[R_ELEPHANT][7][0]=240;
		m_iChessBaseValue[R_ELEPHANT][7][8]=240;
		m_iChessBaseValue[R_ELEPHANT][7][4]=260;
	}
	//------------------------------------------------------------

	//---------抑制河界炮打车之无用着---------
	//黑
	if(byChessBoard[7][8]==R_CAR || byChessBoard[8][8]==R_CAR || byChessBoard[9][8]==R_CAR)
		m_iChessBaseValue[B_CANON][4][8]=400;
	else
		m_iChessBaseValue[B_CANON][4][8]=550;
	if(byChessBoard[7][0]==R_CAR || byChessBoard[8][0]==R_CAR || byChessBoard[9][0]==R_CAR)
		m_iChessBaseValue[B_CANON][4][0]=400;
	else
		m_iChessBaseValue[B_CANON][4][0]=550;

	//红
	if(byChessBoard[0][8]==B_CAR || byChessBoard[1][8]==B_CAR || byChessBoard[2][8]==B_CAR)
		m_iChessBaseValue[R_CANON][5][8]=400;
	else
		m_iChessBaseValue[R_CANON][5][8]=550;
	if(byChessBoard[0][0]==B_CAR || byChessBoard[1][0]==B_CAR || byChessBoard[2][0]==B_CAR)
		m_iChessBaseValue[R_CANON][5][0]=400;
	else
		m_iChessBaseValue[R_CANON][5][0]=550;
	//----------------------------------------

	//--------------当己方缺少某些棋子时,由炮构成的一系列将军价值急剧降低--------------
	//黑
	//己方没有双炮,双车,双马,只有一炮
	if(!m_iChessCnt[B_CAR] && !m_iChessCnt[B_HORSE])
	{
		m_iLineupBVCanon2Else=100;
		m_iLineupBHCanon2Else=70;
	}
	else
	{
		m_iLineupBVCanon2Else=175;
		m_iLineupBHCanon2Else=150;
	}

	//红
	//己方没有双炮,双车,双马,只有一炮
	if(!m_iChessCnt[R_CAR] && !m_iChessCnt[R_HORSE])
	{
		m_iLineupRVCanon2Else=100;
		m_iLineupRHCanon2Else=70;
	}
	else
	{
		m_iLineupRVCanon2Else=175;
		m_iLineupRHCanon2Else=150;
	}
	//---------------------------------------------------------------------------------

	//---------------空心炮---------------
	//黑
	switch(m_iChessCnt[R_BISHOP])
	{
	case 0:
		m_iLineupBVCanonKing=5;
		m_iLineupBHCanonKing=0;

		for(i=0;i<m_uiCurBigChessCnt[R_CAR];i++)
		{
			m_iLineupBVCanonKing+=10;
			m_iLineupBHCanonKing+=10;
		}

		for(i=0;i<m_uiCurBigChessCnt[R_HORSE];i++)
		{
			m_iLineupBVCanonKing+=5;
			m_iLineupBHCanonKing+=5;
		}

		for(i=0;i<m_uiCurBigChessCnt[R_CANON];i++)
		{
			m_iLineupBVCanonKing+=5;
			m_iLineupBHCanonKing+=5;
		}

		for(i=0;i<m_uiCurBigChessCnt[R_PAWN];i++)
		{
			m_iLineupBVCanonKing+=1;
			m_iLineupBHCanonKing+=1;
		}


		for(i=0;i<m_uiCurBigChessCnt[B_CAR];i++)
		{
			m_iLineupBVCanonKing+=10;
			m_iLineupBHCanonKing+=10;
		}

		for(i=0;i<m_uiCurBigChessCnt[B_HORSE];i++)
		{
			m_iLineupBVCanonKing+=5;
			m_iLineupBHCanonKing+=5;
		}

		for(i=0;i<m_uiCurBigChessCnt[B_CANON];i++)
		{
			m_iLineupBVCanonKing+=5;
			m_iLineupBHCanonKing+=5;
		}

		for(i=0;i<m_uiCurBigChessCnt[B_PAWN];i++)
		{
			m_iLineupBVCanonKing+=1;
			m_iLineupBHCanonKing+=1;
		}
		break;

	case 1:
		m_iLineupBVCanonKing=40;
		m_iLineupBHCanonKing=30;

		for(i=0;i<m_uiCurBigChessCnt[R_CAR];i++)
		{
			m_iLineupBVCanonKing+=20;
			m_iLineupBHCanonKing+=20;
		}

		for(i=0;i<m_uiCurBigChessCnt[R_HORSE];i++)
		{
			m_iLineupBVCanonKing+=10;
			m_iLineupBHCanonKing+=10;
		}
		
		for(i=0;i<m_uiCurBigChessCnt[R_CANON];i++)
		{
			m_iLineupBVCanonKing+=10;
			m_iLineupBHCanonKing+=10;
		}

		for(i=0;i<m_uiCurBigChessCnt[R_PAWN];i++)
		{
			m_iLineupBVCanonKing+=5;
			m_iLineupBHCanonKing+=5;
		}


		for(i=0;i<m_uiCurBigChessCnt[B_CAR];i++)
		{
			m_iLineupBVCanonKing+=15;
			m_iLineupBHCanonKing+=15;
		}

		for(i=0;i<m_uiCurBigChessCnt[B_HORSE];i++)
		{
			m_iLineupBVCanonKing+=10;
			m_iLineupBHCanonKing+=10;
		}

		for(i=0;i<m_uiCurBigChessCnt[B_CANON];i++)
		{
			m_iLineupBVCanonKing+=10;
			m_iLineupBHCanonKing+=10;
		}

		for(i=0;i<m_uiCurBigChessCnt[B_PAWN];i++)
		{
			m_iLineupBVCanonKing+=5;
			m_iLineupBHCanonKing+=5;
		}
		break;

	default:
		m_iLineupBVCanonKing=80;
		m_iLineupBHCanonKing=60;

		for(i=0;i<m_uiCurBigChessCnt[R_CAR];i++)
		{
			m_iLineupBVCanonKing+=25;
			m_iLineupBHCanonKing+=25;
		}

		for(i=0;i<m_uiCurBigChessCnt[R_HORSE];i++)
		{
			m_iLineupBVCanonKing+=15;
			m_iLineupBHCanonKing+=15;
		}

		for(i=0;i<m_uiCurBigChessCnt[R_CANON];i++)
		{
			m_iLineupBVCanonKing+=15;
			m_iLineupBHCanonKing+=15;
		}

		for(i=0;i<m_uiCurBigChessCnt[R_PAWN];i++)
		{
			m_iLineupBVCanonKing+=5;
			m_iLineupBHCanonKing+=5;
		}


		for(i=0;i<m_uiCurBigChessCnt[B_CAR];i++)
		{
			m_iLineupBVCanonKing+=15;
			m_iLineupBHCanonKing+=15;
		}

		for(i=0;i<m_uiCurBigChessCnt[B_HORSE];i++)
		{
			m_iLineupBVCanonKing+=10;
			m_iLineupBHCanonKing+=10;
		}

		for(i=0;i<m_uiCurBigChessCnt[B_CANON];i++)
		{
			m_iLineupBVCanonKing+=10;
			m_iLineupBHCanonKing+=10;
		}

		for(i=0;i<m_uiCurBigChessCnt[B_PAWN];i++)
		{
			m_iLineupBVCanonKing+=5;
			m_iLineupBHCanonKing+=5;
		}
		break;
	}

	//红
	switch(m_iChessCnt[B_BISHOP])
	{
	case 0:
		m_iLineupRVCanonKing=5;
		m_iLineupRHCanonKing=0;

		for(i=0;i<m_uiCurBigChessCnt[B_CAR];i++)
		{
			m_iLineupRVCanonKing+=10;
			m_iLineupRHCanonKing+=10;
		}

		for(i=0;i<m_uiCurBigChessCnt[B_HORSE];i++)
		{
			m_iLineupRVCanonKing+=5;
			m_iLineupRHCanonKing+=5;
		}

		for(i=0;i<m_uiCurBigChessCnt[B_CANON];i++)
		{
			m_iLineupRVCanonKing+=5;
			m_iLineupRHCanonKing+=5;
		}

		for(i=0;i<m_uiCurBigChessCnt[B_PAWN];i++)
		{
			m_iLineupRVCanonKing+=1;
			m_iLineupRHCanonKing+=1;
		}


		for(i=0;i<m_uiCurBigChessCnt[R_CAR];i++)
		{
			m_iLineupRVCanonKing+=10;
			m_iLineupRHCanonKing+=10;
		}

		for(i=0;i<m_uiCurBigChessCnt[R_HORSE];i++)
		{
			m_iLineupRVCanonKing+=5;
			m_iLineupRHCanonKing+=5;
		}

		for(i=0;i<m_uiCurBigChessCnt[R_CANON];i++)
		{
			m_iLineupRVCanonKing+=5;
			m_iLineupRHCanonKing+=5;
		}

		for(i=0;i<m_uiCurBigChessCnt[R_PAWN];i++)
		{
			m_iLineupRVCanonKing+=1;
			m_iLineupRHCanonKing+=1;
		}
		break;

	case 1:
		m_iLineupRVCanonKing=40;
		m_iLineupRHCanonKing=30;

		for(i=0;i<m_uiCurBigChessCnt[B_CAR];i++)
		{
			m_iLineupRVCanonKing+=20;
			m_iLineupRHCanonKing+=20;
		}

		for(i=0;i<m_uiCurBigChessCnt[B_HORSE];i++)
		{
			m_iLineupRVCanonKing+=10;
			m_iLineupRHCanonKing+=10;
		}

		for(i=0;i<m_uiCurBigChessCnt[B_CANON];i++)
		{
			m_iLineupRVCanonKing+=10;
			m_iLineupRHCanonKing+=10;
		}

		for(i=0;i<m_uiCurBigChessCnt[B_PAWN];i++)
		{
			m_iLineupRVCanonKing+=5;
			m_iLineupRHCanonKing+=5;
		}


		for(i=0;i<m_uiCurBigChessCnt[R_CAR];i++)
		{
			m_iLineupRVCanonKing+=15;
			m_iLineupRHCanonKing+=15;
		}

		for(i=0;i<m_uiCurBigChessCnt[R_HORSE];i++)
		{
			m_iLineupRVCanonKing+=10;
			m_iLineupRHCanonKing+=10;
		}

		for(i=0;i<m_uiCurBigChessCnt[R_CANON];i++)
		{
			m_iLineupRVCanonKing+=10;
			m_iLineupRHCanonKing+=10;
		}

		for(i=0;i<m_uiCurBigChessCnt[R_PAWN];i++)
		{
			m_iLineupRVCanonKing+=5;
			m_iLineupRHCanonKing+=5;
		}
		break;

	default:
		m_iLineupRVCanonKing=80;
		m_iLineupRHCanonKing=60;

		for(i=0;i<m_uiCurBigChessCnt[B_CAR];i++)
		{
			m_iLineupRVCanonKing+=25;
			m_iLineupRHCanonKing+=25;
		}

		for(i=0;i<m_uiCurBigChessCnt[B_HORSE];i++)
		{
			m_iLineupRVCanonKing+=15;
			m_iLineupRHCanonKing+=15;
		}

		for(i=0;i<m_uiCurBigChessCnt[B_CANON];i++)
		{
			m_iLineupRVCanonKing+=15;
			m_iLineupRHCanonKing+=15;
		}

		for(i=0;i<m_uiCurBigChessCnt[B_PAWN];i++)
		{
			m_iLineupRVCanonKing+=5;
			m_iLineupRHCanonKing+=5;
		}


		for(i=0;i<m_uiCurBigChessCnt[R_CAR];i++)
		{
			m_iLineupRVCanonKing+=15;
			m_iLineupRHCanonKing+=15;
		}

		for(i=0;i<m_uiCurBigChessCnt[R_HORSE];i++)
		{
			m_iLineupRVCanonKing+=10;
			m_iLineupRHCanonKing+=10;
		}

		for(i=0;i<m_uiCurBigChessCnt[R_CANON];i++)
		{
			m_iLineupRVCanonKing+=10;
			m_iLineupRHCanonKing+=10;
		}

		for(i=0;i<m_uiCurBigChessCnt[R_PAWN];i++)
		{
			m_iLineupRVCanonKing+=5;
			m_iLineupRHCanonKing+=5;
		}
		break;
	}
	//------------------------------------

	//---------------对方有双车的情况下，仕的价值明显比相的价值高---------------
	//黑
	if(m_iChessCnt[B_CAR]==2 && m_iChessCnt[B_CANON]==0)
	{
		//士
		m_iChessBaseValue[R_BISHOP][9][3]=275;
		m_iChessBaseValue[R_BISHOP][9][5]=275;
		m_iChessBaseValue[R_BISHOP][8][4]=285;
		m_iChessBaseValue[R_BISHOP][7][3]=265;
		m_iChessBaseValue[R_BISHOP][7][5]=265;

		//相
		m_iChessBaseValue[R_ELEPHANT][5][2]=225;
		m_iChessBaseValue[R_ELEPHANT][5][6]=225;
		m_iChessBaseValue[R_ELEPHANT][7][0]=215;
		m_iChessBaseValue[R_ELEPHANT][7][4]=235;
		m_iChessBaseValue[R_ELEPHANT][7][8]=215;
		m_iChessBaseValue[R_ELEPHANT][9][2]=215;
		m_iChessBaseValue[R_ELEPHANT][9][6]=215;
	}
	else
	{
		//士
		m_iChessBaseValue[R_BISHOP][9][3]=250;
		m_iChessBaseValue[R_BISHOP][9][5]=250;
		m_iChessBaseValue[R_BISHOP][8][4]=260;
		m_iChessBaseValue[R_BISHOP][7][3]=240;
		m_iChessBaseValue[R_BISHOP][7][5]=240;

		//相
		m_iChessBaseValue[R_ELEPHANT][5][2]=250;
		m_iChessBaseValue[R_ELEPHANT][5][6]=250;
		m_iChessBaseValue[R_ELEPHANT][7][0]=240;
		m_iChessBaseValue[R_ELEPHANT][7][4]=260;
		m_iChessBaseValue[R_ELEPHANT][7][8]=240;
		m_iChessBaseValue[R_ELEPHANT][9][2]=240;
		m_iChessBaseValue[R_ELEPHANT][9][6]=240;
	}

	//红
	if(m_iChessCnt[R_CAR]==2 && m_iChessCnt[R_CANON]==0)
	{
		//士
		m_iChessBaseValue[B_BISHOP][0][3]=275;
		m_iChessBaseValue[B_BISHOP][0][5]=275;
		m_iChessBaseValue[B_BISHOP][1][4]=285;
		m_iChessBaseValue[B_BISHOP][2][3]=265;
		m_iChessBaseValue[B_BISHOP][2][5]=265;

		//象
		m_iChessBaseValue[B_ELEPHANT][0][2]=225;
		m_iChessBaseValue[B_ELEPHANT][0][6]=225;
		m_iChessBaseValue[B_ELEPHANT][2][0]=215;
		m_iChessBaseValue[B_ELEPHANT][2][4]=235;
		m_iChessBaseValue[B_ELEPHANT][2][8]=215;
		m_iChessBaseValue[B_ELEPHANT][4][2]=215;
		m_iChessBaseValue[B_ELEPHANT][4][6]=215;
	}
	else
	{
		//士
		m_iChessBaseValue[B_BISHOP][0][3]=250;
		m_iChessBaseValue[B_BISHOP][0][5]=250;
		m_iChessBaseValue[B_BISHOP][1][4]=260;
		m_iChessBaseValue[B_BISHOP][2][3]=240;
		m_iChessBaseValue[B_BISHOP][2][5]=240;

		//象
		m_iChessBaseValue[B_ELEPHANT][0][2]=250;
		m_iChessBaseValue[B_ELEPHANT][0][6]=250;
		m_iChessBaseValue[B_ELEPHANT][2][0]=240;
		m_iChessBaseValue[B_ELEPHANT][2][4]=260;
		m_iChessBaseValue[B_ELEPHANT][2][8]=240;
		m_iChessBaseValue[B_ELEPHANT][4][2]=240;
		m_iChessBaseValue[B_ELEPHANT][4][6]=240;
	}
	//--------------------------------------------------------------------------

	//---------------对方有双炮的情况下，双相的价值明显比双仕高---------------
	//黑
	if(m_iChessCnt[B_CAR]==0 && m_iChessCnt[B_CANON]==2)
	{
		//士
		m_iChessBaseValue[R_BISHOP][9][3]=225;
		m_iChessBaseValue[R_BISHOP][9][5]=225;
		m_iChessBaseValue[R_BISHOP][8][4]=235;
		m_iChessBaseValue[R_BISHOP][7][3]=215;
		m_iChessBaseValue[R_BISHOP][7][5]=215;

		//相
		m_iChessBaseValue[R_ELEPHANT][5][2]=275;
		m_iChessBaseValue[R_ELEPHANT][5][6]=275;
		m_iChessBaseValue[R_ELEPHANT][7][0]=265;
		m_iChessBaseValue[R_ELEPHANT][7][4]=285;
		m_iChessBaseValue[R_ELEPHANT][7][8]=265;
		m_iChessBaseValue[R_ELEPHANT][9][2]=265;
		m_iChessBaseValue[R_ELEPHANT][9][6]=265;
	}
	else
	{
		//士
		m_iChessBaseValue[R_BISHOP][9][3]=250;
		m_iChessBaseValue[R_BISHOP][9][5]=250;
		m_iChessBaseValue[R_BISHOP][8][4]=260;
		m_iChessBaseValue[R_BISHOP][7][3]=240;
		m_iChessBaseValue[R_BISHOP][7][5]=240;

		//相
		m_iChessBaseValue[R_ELEPHANT][5][2]=250;
		m_iChessBaseValue[R_ELEPHANT][5][6]=250;
		m_iChessBaseValue[R_ELEPHANT][7][0]=240;
		m_iChessBaseValue[R_ELEPHANT][7][4]=260;
		m_iChessBaseValue[R_ELEPHANT][7][8]=240;
		m_iChessBaseValue[R_ELEPHANT][9][2]=240;
		m_iChessBaseValue[R_ELEPHANT][9][6]=240;
	}

	//红
	if(m_iChessCnt[R_CAR]==2 && m_iChessCnt[R_CANON]==0)
	{
		//士
		m_iChessBaseValue[B_BISHOP][0][3]=225;
		m_iChessBaseValue[B_BISHOP][0][5]=225;
		m_iChessBaseValue[B_BISHOP][1][4]=235;
		m_iChessBaseValue[B_BISHOP][2][3]=215;
		m_iChessBaseValue[B_BISHOP][2][5]=215;

		//象
		m_iChessBaseValue[B_ELEPHANT][0][2]=275;
		m_iChessBaseValue[B_ELEPHANT][0][6]=275;
		m_iChessBaseValue[B_ELEPHANT][2][0]=265;
		m_iChessBaseValue[B_ELEPHANT][2][4]=285;
		m_iChessBaseValue[B_ELEPHANT][2][8]=265;
		m_iChessBaseValue[B_ELEPHANT][4][2]=265;
		m_iChessBaseValue[B_ELEPHANT][4][6]=265;
	}
	else
	{
		//士
		m_iChessBaseValue[B_BISHOP][0][3]=250;
		m_iChessBaseValue[B_BISHOP][0][5]=250;
		m_iChessBaseValue[B_BISHOP][1][4]=260;
		m_iChessBaseValue[B_BISHOP][2][3]=240;
		m_iChessBaseValue[B_BISHOP][2][5]=240;

		//象
		m_iChessBaseValue[B_ELEPHANT][0][2]=250;
		m_iChessBaseValue[B_ELEPHANT][0][6]=250;
		m_iChessBaseValue[B_ELEPHANT][2][0]=240;
		m_iChessBaseValue[B_ELEPHANT][2][4]=260;
		m_iChessBaseValue[B_ELEPHANT][2][8]=240;
		m_iChessBaseValue[B_ELEPHANT][4][2]=240;
		m_iChessBaseValue[B_ELEPHANT][4][6]=240;
	}
	//------------------------------------------------------------------------

	//-------------进入中局后，马的攻势增强，炮的攻势减弱-------------
//	switch(m_uiCurChessAge)
//	{
//	case CA_START:
//		m_iAttackValue[B_HORSE]=30;
//		m_iAttackValue[R_HORSE]=30;
//
//		m_iAttackValue[B_CANON]=10;
//		m_iAttackValue[R_CANON]=10;
//		break;
//
//	case CA_MID:
//		m_iAttackValue[B_HORSE]=50;
//		m_iAttackValue[R_HORSE]=50;
//
//		m_iAttackValue[B_CANON]=20;
//		m_iAttackValue[R_CANON]=20;
//		break;
//
//	case CA_END:
//		m_iAttackValue[B_HORSE]=60;
//		m_iAttackValue[R_HORSE]=60;
//
//		m_iAttackValue[B_CANON]=15;
//		m_iAttackValue[R_CANON]=15;
//
//		//残局兵力量增加
//		for(i=1;i<7;i++)
//		{
//			for(j=0;j<9;j++)
//				m_iChessBaseValue[R_PAWN][i][j]+=50;
//		}
//		for(i=3;i<9;i++)
//		{
//			for(j=0;j<9;j++)
//				m_iChessBaseValue[B_PAWN][i][j]+=50;
//		}
//		break;
//	}
	//------------------------------------------------------------------

	//------------有屏风马时，屏风马上面的一个位置的车价值才略高------------
	//黑
	if(byChessBoard[7][2]==R_HORSE)
	{
		m_iChessBaseValue[B_CAR][6][1]=1350;
		m_iChessBaseValue[B_CAR][6][2]=1400;
	}
	else
	{
		m_iChessBaseValue[B_CAR][6][1]=1300;
		m_iChessBaseValue[B_CAR][6][2]=1300;
	}
	if(byChessBoard[7][6]==R_HORSE)
	{
		m_iChessBaseValue[B_CAR][6][7]=1350;
		m_iChessBaseValue[B_CAR][6][6]=1400;
	}
	else
	{
		m_iChessBaseValue[B_CAR][6][7]=1300;
		m_iChessBaseValue[B_CAR][6][6]=1300;
	}

	//红
	if(byChessBoard[2][2]==B_HORSE)
	{
		m_iChessBaseValue[R_CAR][3][1]=1350;
		m_iChessBaseValue[R_CAR][3][2]=1400;
	}
	else
	{
		m_iChessBaseValue[R_CAR][3][1]=1300;
		m_iChessBaseValue[R_CAR][3][2]=1300;
	}
	if(byChessBoard[2][6]==B_HORSE)
	{
		m_iChessBaseValue[R_CAR][3][7]=1350;
		m_iChessBaseValue[R_CAR][3][6]=1400;
	}
	else
	{
		m_iChessBaseValue[R_CAR][3][7]=1300;
		m_iChessBaseValue[R_CAR][3][6]=1300;
	}
	//----------------------------------------------------------------------

	//-------------------卧槽马-------------------
	//黑
	switch(m_iChessCnt[B_CAR])
	{
	case 0:
		m_iLineupBLieSlotHorse=100;
		break;

	case 1:
		m_iLineupBLieSlotHorse=300;
		break;

	case 2:
		m_iLineupBLieSlotHorse=500;
	}

	//红
	switch(m_iChessCnt[R_CAR])
	{
	case 0:
		m_iLineupRLieSlotHorse=100;
		break;

	case 1:
		m_iLineupRLieSlotHorse=300;
		break;

	case 2:
		m_iLineupRLieSlotHorse=500;
	}
	//--------------------------------------------

	//---------------无双车的情况下孤将价值升高---------------
	//黑
	if(m_iChessCnt[B_CAR]<2)
		m_iLineupBNoProtectKing=-200;
	else
		m_iLineupBNoProtectKing=-600;

	//红
	if(m_iChessCnt[R_CAR]<2)
		m_iLineupRNoProtectKing=-200;
	else
		m_iLineupRNoProtectKing=-600;
	//--------------------------------------------------------

	//--------------双车对孤将系列--------------
	//黑
	switch(m_iChessCnt[R_CAR])
	{
	case 2:
		m_iLineupBDblCarLoneKing=400;		       //双车对孤将
		m_iLineupBDblCarLoneKingElephant=350;      //双车对孤将一象
		m_iLineupBDblCarLoneKing2Elephant=300;	   //双车对孤将二象
		m_iLineupBDblCarLoneKingBishop=350;		   //双车对孤将一士
		m_iLineupBDblCarLoneKingBishopElephant=300;//双车对孤将一士一象
		break;

	case 1:
		m_iLineupBDblCarLoneKing=1000;		       //双车对孤将
		m_iLineupBDblCarLoneKingElephant=950;      //双车对孤将一象
		m_iLineupBDblCarLoneKing2Elephant=900;	   //双车对孤将二象
		m_iLineupBDblCarLoneKingBishop=950;		   //双车对孤将一士
		m_iLineupBDblCarLoneKingBishopElephant=900;//双车对孤将一士一象
		break;

	case 0:
		m_iLineupBDblCarLoneKing=1500;		        //双车对孤将
		m_iLineupBDblCarLoneKingElephant=1450;      //双车对孤将一象
		m_iLineupBDblCarLoneKing2Elephant=1400;	    //双车对孤将二象
		m_iLineupBDblCarLoneKingBishop=1450;		//双车对孤将一士
		m_iLineupBDblCarLoneKingBishopElephant=1400;//双车对孤将一士一象
		break;
	}

	//红
	switch(m_iChessCnt[B_CAR])
	{
	case 2:
		m_iLineupRDblCarLoneKing=400;		       //双车对孤将
		m_iLineupRDblCarLoneKingElephant=350;      //双车对孤将一象
		m_iLineupRDblCarLoneKing2Elephant=300;	   //双车对孤将二象
		m_iLineupRDblCarLoneKingBishop=350;		   //双车对孤将一士
		m_iLineupRDblCarLoneKingBishopElephant=300;//双车对孤将一士一象
		break;

	case 1:
		m_iLineupRDblCarLoneKing=1000;		       //双车对孤将
		m_iLineupRDblCarLoneKingElephant=950;      //双车对孤将一象
		m_iLineupRDblCarLoneKing2Elephant=900;	   //双车对孤将二象
		m_iLineupRDblCarLoneKingBishop=950;		   //双车对孤将一士
		m_iLineupRDblCarLoneKingBishopElephant=900;//双车对孤将一士一象
		break;

	case 0:
		m_iLineupRDblCarLoneKing=1500;		        //双车对孤将
		m_iLineupRDblCarLoneKingElephant=1450;      //双车对孤将一象
		m_iLineupRDblCarLoneKing2Elephant=1400;	    //双车对孤将二象
		m_iLineupRDblCarLoneKingBishop=1450;		//双车对孤将一士
		m_iLineupRDblCarLoneKingBishopElephant=1400;//双车对孤将一士一象
		break;
	}
	//------------------------------------------

	//-------------------窝心马-------------------
	//黑
	m_iLineupBLieMidHorse=0;
	for(i=0;i<m_vectorCurChessPos[R_CAR].size();i++)
	{
		if(m_vectorCurChessPos[R_CAR][i].x<=4)
			m_iLineupBLieMidHorse+=-50;
		else
			m_iLineupBLieMidHorse+=-30;
	}
	for(i=0;i<m_vectorCurChessPos[R_PAWN].size();i++)
	{
		if(m_vectorCurChessPos[R_PAWN][i].x<=1)
			m_iLineupBLieMidHorse+=-25;
		else
		{
			if(m_vectorCurChessPos[R_PAWN][i].x<=3)
				m_iLineupBLieMidHorse+=-15;
			else
			{
				if(m_vectorCurChessPos[R_PAWN][i].x<=4)
					m_iLineupBLieMidHorse+=-10;
			}
		}
	}

	//红
	m_iLineupRLieMidHorse=0;
	for(i=0;i<m_vectorCurChessPos[B_CAR].size();i++)
	{
		if(m_vectorCurChessPos[B_CAR][i].x>=5)
			m_iLineupRLieMidHorse+=-50;
		else
			m_iLineupRLieMidHorse+=-30;
	}
	for(i=0;i<m_vectorCurChessPos[B_PAWN].size();i++)
	{
		if(m_vectorCurChessPos[B_PAWN][i].x>=8)
			m_iLineupRLieMidHorse+=-25;
		else
		{
			if(m_vectorCurChessPos[B_PAWN][i].x>=6)
				m_iLineupRLieMidHorse+=-15;
			else
			{
				if(m_vectorCurChessPos[B_PAWN][i].x>=5)
					m_iLineupRLieMidHorse+=-10;
			}
		}
	}
	//--------------------------------------------

	//----------------根据各方子力决定窝心车的价值----------------
	//黑
	if(!m_iChessCnt[R_BISHOP])
	{
		if(m_uiAllRChessCnt==1)
			m_iLineupBLieMidCar=m_iLineupBLieMidCar_LoneKing;
		else
		{
			if(m_uiCurBigChessCnt[B_CAR]==2)
				m_iLineupBLieMidCar=m_iLineupBLieMidCar_BigCar;
			else
			{
				if(m_iChessCnt[B_CAR]==2)
					m_iLineupBLieMidCar=m_iLineupBLieMidCar_Car;
				else
				{
					m_iLineupBLieMidCar=m_iLineupBLieMidCar_Horse[m_uiCurBigChessCnt[B_HORSE]];
					iSize=m_vectorCurChessPos[B_PAWN].size();
					for(i=0;i<iSize;i++)
					{
						if(m_vectorCurChessPos[B_PAWN][i].x>=5 && m_vectorCurChessPos[B_PAWN][i].x<=8)
						{
							iDist=abs(7-m_vectorCurChessPos[B_PAWN][i].x)+abs(m_vectorCurChessPos[B_PAWN][i].y-4);
							if(iDist==0)
								m_iLineupBLieMidCar+=20;
							else
							{
								if(iDist<=2)
									m_iLineupBLieMidCar+=10;
								else
								{
									if(iDist<=4)
										m_iLineupBLieMidCar+=5;
								}
							}
						}
					}
					if(!m_iLineupBLieMidCar)
					{
						if(m_iChessCnt[B_BISHOP]>=1)
							m_iLineupBLieMidCar=m_iLineupBLieMidCar_CanonBishop;
					}
				}
			}
		}
	}

	//红
	if(!m_iChessCnt[B_BISHOP])
	{
		if(m_uiAllBChessCnt==1)
			m_iLineupRLieMidCar=m_iLineupRLieMidCar_LoneKing;
		else
		{
			if(m_uiCurBigChessCnt[R_CAR]==2)
				m_iLineupRLieMidCar=m_iLineupRLieMidCar_BigCar;
			else
			{
				if(m_iChessCnt[R_CAR]==2)
					m_iLineupRLieMidCar=m_iLineupRLieMidCar_Car;
				else
				{
					m_iLineupRLieMidCar=m_iLineupRLieMidCar_Horse[m_uiCurBigChessCnt[R_HORSE]];
					iSize=m_vectorCurChessPos[R_PAWN].size();
					for(i=0;i<iSize;i++)
					{
						if(m_vectorCurChessPos[R_PAWN][i].x>=1 && m_vectorCurChessPos[R_PAWN][i].x<=4)
						{
							iDist=abs(2-m_vectorCurChessPos[R_PAWN][i].x)+abs(m_vectorCurChessPos[R_PAWN][i].y-4);
							if(iDist==0)
								m_iLineupRLieMidCar+=20;
							else
							{
								if(iDist<=2)
									m_iLineupRLieMidCar+=10;
								else
								{
									if(iDist<=4)
										m_iLineupRLieMidCar+=5;
								}
							}
						}
					}
					if(!m_iLineupRLieMidCar)
					{
						if(m_iChessCnt[R_BISHOP]>=1)
							m_iLineupRLieMidCar=m_iLineupRLieMidCar_CanonBishop;
					}
				}
			}
		}
	}
	//---------------------------------------------------------------------

	//----------------根据各放子力决定窝心兵的价值----------------
	//黑
	if(!m_iChessCnt[R_BISHOP])
	{
		if(m_uiAllRChessCnt==1)
			m_iLineupBLieMidPawn=m_iLineupBLieMidPawn_LoneKing;
		else
		{
			if(m_uiCurBigChessCnt[B_CAR]==2)
				m_iLineupBLieMidPawn=m_iLineupBLieMidPawn_BigCar;
			else
			{
				if(m_iChessCnt[B_CAR]==2)
					m_iLineupBLieMidPawn=m_iLineupBLieMidPawn_Car;
				else
				{
					m_iLineupBLieMidPawn=m_iLineupBLieMidPawn_Horse[m_uiCurBigChessCnt[B_HORSE]];
					iSize=m_vectorCurChessPos[B_PAWN].size();
					for(i=0;i<iSize;i++)
					{
						if(m_vectorCurChessPos[B_PAWN][i].x>=5 && m_vectorCurChessPos[B_PAWN][i].x<=8)
						{
							iDist=abs(7-m_vectorCurChessPos[B_PAWN][i].x)+abs(m_vectorCurChessPos[B_PAWN][i].y-4);
							if(iDist==0)
								m_iLineupBLieMidPawn+=20;
							else
							{
								if(iDist<=2)
									m_iLineupBLieMidPawn+=10;
								else
								{
									if(iDist<=4)
										m_iLineupBLieMidPawn+=5;
								}
							}
						}
					}
					if(!m_iLineupBLieMidPawn)
					{
						if(m_iChessCnt[B_BISHOP]>=1)
							m_iLineupBLieMidPawn=m_iLineupBLieMidPawn_CanonBishop;
					}
				}
			}
		}
	}

	//红
	if(!m_iChessCnt[B_BISHOP])
	{
		if(m_uiAllBChessCnt==1)
			m_iLineupRLieMidPawn=m_iLineupRLieMidPawn_LoneKing;
		else
		{
			if(m_uiCurBigChessCnt[R_CAR]==2)
				m_iLineupRLieMidPawn=m_iLineupRLieMidPawn_BigCar;
			else
			{
				if(m_iChessCnt[R_CAR]==2)
					m_iLineupRLieMidPawn=m_iLineupRLieMidPawn_Car;
				else
				{
					m_iLineupRLieMidPawn=m_iLineupRLieMidPawn_Horse[m_uiCurBigChessCnt[R_HORSE]];
					iSize=m_vectorCurChessPos[R_PAWN].size();
					for(i=0;i<iSize;i++)
					{
						if(m_vectorCurChessPos[R_PAWN][i].x>=1 && m_vectorCurChessPos[R_PAWN][i].x<=4)
						{
							iDist=abs(2-m_vectorCurChessPos[R_PAWN][i].x)+abs(m_vectorCurChessPos[R_PAWN][i].y-4);
							if(iDist==0)
								m_iLineupRLieMidPawn+=20;
							else
							{
								if(iDist<=2)
									m_iLineupRLieMidPawn+=10;
								else
								{
									if(iDist<=4)
										m_iLineupRLieMidPawn+=5;
								}
							}
						}
					}
					if(!m_iLineupRLieMidPawn)
					{
						if(m_iChessCnt[R_BISHOP]>=1)
							m_iLineupRLieMidPawn=m_iLineupRLieMidPawn_CanonBishop;
					}
				}
			}
		}
	}
	//---------------------------------------------------------------------

	//---------------鲁莽马---------------
	//黑
	if(m_iChessCnt[B_CAR]<2 || m_iChessCnt[B_CANON]<2 || m_iChessCnt[B_HORSE]<2 || m_iChessCnt[B_PAWN]<3)
		m_iLineupBImpertinenceHorse=0;
	else
		m_iLineupBImpertinenceHorse=-80;

	//红
	if(m_iChessCnt[R_CAR]<2 || m_iChessCnt[R_CANON]<2 || m_iChessCnt[R_HORSE]<2 || m_iChessCnt[R_PAWN]<3)
		m_iLineupRImpertinenceHorse=0;
	else
		m_iLineupRImpertinenceHorse=-80;
	//------------------------------------

	//----------------卧槽马和车,车在马的槽位:可以抽车吃子----------------
	//黑
	m_iLineupBLieSlotHorseCar=0;
	for(i=0;i<m_iChessCnt[R_CAR];i++)
		m_iLineupBLieSlotHorseCar+=60;
	for(i=0;i<m_iChessCnt[R_HORSE];i++)
		m_iLineupBLieSlotHorseCar+=30;
	for(i=0;i<m_iChessCnt[R_CANON];i++)
		m_iLineupBLieSlotHorseCar+=30;
	for(i=0;i<m_iChessCnt[R_ELEPHANT];i++)
		m_iLineupBLieSlotHorseCar+=15;
	for(i=0;i<m_iChessCnt[R_BISHOP];i++)
		m_iLineupBLieSlotHorseCar+=15;
	for(i=0;i<m_uiCurBigChessCnt[R_PAWN];i++)
		m_iLineupBLieSlotHorseCar+=15;

	//红
	m_iLineupRLieSlotHorseCar=0;
	for(i=0;i<m_iChessCnt[B_CAR];i++)
		m_iLineupRLieSlotHorseCar+=60;
	for(i=0;i<m_iChessCnt[B_HORSE];i++)
		m_iLineupRLieSlotHorseCar+=30;
	for(i=0;i<m_iChessCnt[B_CANON];i++)
		m_iLineupRLieSlotHorseCar+=30;
	for(i=0;i<m_iChessCnt[B_ELEPHANT];i++)
		m_iLineupRLieSlotHorseCar+=15;
	for(i=0;i<m_iChessCnt[B_BISHOP];i++)
		m_iLineupRLieSlotHorseCar+=15;
	for(i=0;i<m_uiCurBigChessCnt[B_PAWN];i++)
		m_iLineupRLieSlotHorseCar+=15;
	//--------------------------------------------------------------------

	//---------------炮和将之间的卧心马---------------
	//黑
	m_iLineupBCannonKingLieMidHorse=0;
	iSize=m_vectorCurChessPos[B_CAR].size();
	for(i=0;i<iSize;i++)
	{
		if((m_vectorCurChessPos[B_CAR][i].y==3 && byChessBoard[0][3]==B_KING) ||
		   (m_vectorCurChessPos[B_CAR][i].y==5 && byChessBoard[0][5]==B_KING))
		   m_iLineupBCannonKingLieMidHorse+=200;
		else
		{
			if(m_vectorCurChessPos[B_CAR][i].x>=5)
				m_iLineupBCannonKingLieMidHorse+=100;
			else
				m_iLineupBCannonKingLieMidHorse+=80;
		}
	}
	iSize=m_vectorCurChessPos[B_PAWN].size();
	for(i=0;i<iSize;i++)
	{
		if(m_vectorCurChessPos[B_PAWN][i].x>=5 && m_vectorCurChessPos[B_PAWN][i].x<=7)
		{
			iDist=7-m_vectorCurChessPos[B_PAWN][i].x+abs(m_vectorCurChessPos[B_PAWN][i].y-4);
			if(iDist==0)
				m_iLineupBCannonKingLieMidHorse+=20;
			else
			{
				if(iDist<=2)
					m_iLineupBCannonKingLieMidHorse+=10;
				else
				{
					if(iDist<=4)
						m_iLineupBCannonKingLieMidHorse+=5;
				}
			}
		}
		if((m_vectorCurChessPos[B_PAWN][i].y==3 && byChessBoard[0][3]==B_KING) ||
		   (m_vectorCurChessPos[B_PAWN][i].y==5 && byChessBoard[0][5]==B_KING))
		   m_iLineupBCannonKingLieMidHorse+=10;
	}

	//红
	m_iLineupRCannonKingLieMidHorse=0;
	iSize=m_vectorCurChessPos[R_CAR].size();
	for(i=0;i<iSize;i++)
	{
		if((m_vectorCurChessPos[R_CAR][i].y==3 && byChessBoard[9][3]==R_KING) ||
		   (m_vectorCurChessPos[R_CAR][i].y==5 && byChessBoard[9][5]==R_KING))
		   m_iLineupRCannonKingLieMidHorse+=200;
		else
		{
			if(m_vectorCurChessPos[R_CAR][i].x<=4)
				m_iLineupRCannonKingLieMidHorse+=100;
			else
				m_iLineupRCannonKingLieMidHorse+=80;
		}
	}
	iSize=m_vectorCurChessPos[R_PAWN].size();
	for(i=0;i<iSize;i++)
	{
		if(m_vectorCurChessPos[R_PAWN][i].x<=4 && m_vectorCurChessPos[R_PAWN][i].x>=2)
		{
			iDist=m_vectorCurChessPos[R_PAWN][i].x-2+abs(m_vectorCurChessPos[R_PAWN][i].y-4);
			if(iDist==0)
				m_iLineupRCannonKingLieMidHorse+=20;
			else
			{
				if(iDist<=2)
					m_iLineupRCannonKingLieMidHorse+=10;
				else
				{
					if(iDist<=4)
						m_iLineupRCannonKingLieMidHorse+=5;
				}
			}
		}
		if((m_vectorCurChessPos[R_PAWN][i].y==3 && byChessBoard[9][3]==R_KING) ||
		   (m_vectorCurChessPos[R_PAWN][i].y==5 && byChessBoard[9][5]==R_KING))
		   m_iLineupRCannonKingLieMidHorse+=10;
	}
	//------------------------------------------------

	//---------------------大角马---------------------
	//黑
	if(!m_iChessCnt[R_BISHOP])
	{
		if(m_uiAllRChessCnt==1)
			m_iLineupBBigAngleHorse=m_iLineupBBigAngleHorse_LoneKing;
		else
		{
			if(m_uiCurBigChessCnt[B_CAR]==2)
				m_iLineupBBigAngleHorse=m_iLineupBBigAngleHorse_BigCar;
			else
			{
				if(m_iChessCnt[B_CAR]==2)
					m_iLineupBBigAngleHorse=m_iLineupBBigAngleHorse_Car;
				else
				{
					m_iLineupBBigAngleHorse=m_iLineupBBigAngleHorse_Horse[m_uiCurBigChessCnt[B_HORSE]];
					iSize=m_vectorCurChessPos[B_PAWN].size();
					for(i=0;i<iSize;i++)
					{
						if(m_vectorCurChessPos[B_PAWN][i].x>=5 && m_vectorCurChessPos[B_PAWN][i].x<=8)
						{
							iDist=abs(7-m_vectorCurChessPos[B_PAWN][i].x)+abs(m_vectorCurChessPos[B_PAWN][i].y-4);
							if(iDist==0)
								m_iLineupBBigAngleHorse+=20;
							else
							{
								if(iDist<=2)
									m_iLineupBBigAngleHorse+=10;
								else
								{
									if(iDist<=4)
										m_iLineupBBigAngleHorse+=5;
								}
							}
						}
					}
					if(!m_iLineupBBigAngleHorse)
					{
						if(m_iChessCnt[B_BISHOP]>=1)
							m_iLineupBBigAngleHorse=m_iLineupBBigAngleHorse_CanonBishop;
					}
				}
			}
		}
	}

	//红
	if(!m_iChessCnt[B_BISHOP])
	{
		if(m_uiAllBChessCnt==1)
			m_iLineupRBigAngleHorse=m_iLineupRBigAngleHorse_LoneKing;
		else
		{
			if(m_uiCurBigChessCnt[R_CAR]==2)
				m_iLineupRBigAngleHorse=m_iLineupRBigAngleHorse_BigCar;
			else
			{
				if(m_iChessCnt[R_CAR]==2)
					m_iLineupRBigAngleHorse=m_iLineupRBigAngleHorse_Car;
				else
				{
					m_iLineupRBigAngleHorse=m_iLineupRBigAngleHorse_Horse[m_uiCurBigChessCnt[R_HORSE]];
					iSize=m_vectorCurChessPos[R_PAWN].size();
					for(i=0;i<iSize;i++)
					{
						if(m_vectorCurChessPos[R_PAWN][i].x>=5 && m_vectorCurChessPos[R_PAWN][i].x<=8)
						{
							iDist=7-m_vectorCurChessPos[R_PAWN][i].x+abs(m_vectorCurChessPos[R_PAWN][i].y-4);
							if(iDist==0)
								m_iLineupRBigAngleHorse+=20;
							else
							{
								if(iDist<=2)
									m_iLineupRBigAngleHorse+=10;
								else
								{
									if(iDist<=4)
										m_iLineupRBigAngleHorse+=5;
								}
							}
						}
					}
					if(!m_iLineupRBigAngleHorse)
					{
						if(m_iChessCnt[R_BISHOP]>=1)
							m_iLineupRBigAngleHorse=m_iLineupRBigAngleHorse_CanonBishop;
					}
				}
			}
		}
	}
	//------------------------------------------------

	//----------------孤将:将离原位----------------
	//黑
	m_iLineupBNoProtectKing=m_iLineupBNoProtectKing_CHC[m_uiCurBigChessCnt[R_CAR]][m_uiCurBigChessCnt[R_HORSE]][m_uiCurBigChessCnt[R_CANON]];
	iSize=m_vectorCurChessPos[R_PAWN].size();
	for(i=0;i<iSize;i++)
	{
		if(m_vectorCurChessPos[R_PAWN][i].x>=2 && m_vectorCurChessPos[R_PAWN][i].x<=4)
		{
			iDist=m_vectorCurChessPos[R_PAWN][i].x-2+abs(m_vectorCurChessPos[R_PAWN][i].y-4);
			if(iDist==0)
				m_iLineupBNoProtectKing+=20;
			else
			{
				if(iDist<=2)
					m_iLineupBNoProtectKing+=10;
				else
				{
					if(iDist<=4)
						m_iLineupBNoProtectKing+=5;
				}
			}
		}
	}
	m_iLineupBNoProtectKing=-m_iLineupBNoProtectKing;

	//红
	m_iLineupRNoProtectKing=m_iLineupRNoProtectKing_CHC[m_uiCurBigChessCnt[B_CAR]][m_uiCurBigChessCnt[B_HORSE]][m_uiCurBigChessCnt[B_CANON]];
	iSize=m_vectorCurChessPos[B_PAWN].size();
	for(i=0;i<iSize;i++)
	{
		if(m_vectorCurChessPos[B_PAWN][i].x>=5 && m_vectorCurChessPos[B_PAWN][i].x<=7)
		{
			iDist=7-m_vectorCurChessPos[B_PAWN][i].x+abs(m_vectorCurChessPos[B_PAWN][i].y-4);
			if(iDist==0)
				m_iLineupRNoProtectKing+=20;
			else
			{
				if(iDist<=2)
					m_iLineupRNoProtectKing+=10;
				else
				{
					if(iDist<=4)
						m_iLineupRNoProtectKing+=5;
				}
			}
		}
	}
	m_iLineupRNoProtectKing=-m_iLineupRNoProtectKing;
	//------------------------------------

	//----------------炮碾丹沙----------------
	//黑
	if(m_uiCurBigChessCnt[B_CAR])
	{
		m_iLineupBHCarCanonKing=m_iLineupBHCarCanonKing_CHC[m_uiCurBigChessCnt[B_CAR]-1][m_uiCurBigChessCnt[B_HORSE]][m_uiCurBigChessCnt[B_CANON]];
		iSize=m_vectorCurChessPos[B_PAWN].size();
		for(i=0;i<iSize;i++)
		{
			if(m_vectorCurChessPos[B_PAWN][i].x>=5 && m_vectorCurChessPos[B_PAWN][i].x<=7)
			{
				iDist=7-m_vectorCurChessPos[B_PAWN][i].x+abs(m_vectorCurChessPos[B_PAWN][i].y-4);
				if(iDist==0)
					m_iLineupBHCarCanonKing+=20;
				else
				{
					if(iDist<=2)
						m_iLineupBHCarCanonKing+=10;
					else
					{
						if(iDist<=4)
							m_iLineupBHCarCanonKing+=5;
					}
				}
			}
		}
		iSize=m_vectorCurChessPos[R_CAR].size();
		for(i=0;i<iSize;i++)
		{
			if(m_vectorCurChessPos[R_CAR][i].x<=2)
				m_iLineupBHCarCanonKing+=100;
		}
	}

	if(m_uiCurBigChessCnt[R_CAR])
	{
		m_iLineupRHCarCanonKing=m_iLineupRHCarCanonKing_CHC[m_uiCurBigChessCnt[R_CAR]-1][m_uiCurBigChessCnt[R_HORSE]][m_uiCurBigChessCnt[R_CANON]];
		iSize=m_vectorCurChessPos[R_PAWN].size();
		for(i=0;i<iSize;i++)
		{
			if(m_vectorCurChessPos[R_PAWN][i].x>=2 && m_vectorCurChessPos[R_PAWN][i].x<=4)
			{
				iDist=m_vectorCurChessPos[R_PAWN][i].x-2+abs(m_vectorCurChessPos[R_PAWN][i].y-4);
				if(iDist==0)
					m_iLineupRHCarCanonKing+=20;
				else
				{
					if(iDist<=2)
						m_iLineupRHCarCanonKing+=10;
					else
					{
						if(iDist<=4)
							m_iLineupRHCarCanonKing+=5;
					}
				}
			}
		}
		iSize=m_vectorCurChessPos[B_CAR].size();
		for(i=0;i<iSize;i++)
		{
			if(m_vectorCurChessPos[B_CAR][i].x<=2)
				m_iLineupRHCarCanonKing+=100;
		}
	}
	//----------------------------------------

	//--------------根据进攻子决定攻势价值--------------
	//黑
	//黑车
	m_nAttackValue[B_CAR]=m_nAttackValue_EB[B_CAR][m_iChessCnt[R_ELEPHANT]][m_iChessCnt[R_BISHOP]];

	//黑马
	m_nAttackValue[B_HORSE]=m_nAttackValue_EB[B_HORSE][m_iChessCnt[R_ELEPHANT]][m_iChessCnt[R_BISHOP]];
	for(i=0;i<m_uiCurBigChessCnt[B_CAR];i++)
		m_nAttackValue[B_HORSE]+=6;

	//黑炮
	m_nAttackValue[B_CANON]=m_nAttackValue_EB[B_CANON][m_iChessCnt[R_ELEPHANT]][m_iChessCnt[R_BISHOP]];

	//黑兵
	m_nAttackValue[B_PAWN]=m_nAttackValue_EB[B_PAWN][m_iChessCnt[R_ELEPHANT]][m_iChessCnt[R_BISHOP]];
	for(i=0;i<m_uiCurBigChessCnt[B_CAR];i++)
		m_nAttackValue[B_PAWN]+=5;
	
	//红
	//红车
	m_nAttackValue[R_CAR]=m_nAttackValue_EB[R_CAR][m_iChessCnt[B_ELEPHANT]][m_iChessCnt[B_BISHOP]];

	//红马
	m_nAttackValue[R_HORSE]=m_nAttackValue_EB[R_HORSE][m_iChessCnt[B_ELEPHANT]][m_iChessCnt[B_BISHOP]];
	for(i=0;i<m_uiCurBigChessCnt[R_CAR];i++)
		m_nAttackValue[R_HORSE]+=6;

	//红炮
	m_nAttackValue[R_CANON]=m_nAttackValue_EB[R_CANON][m_iChessCnt[B_ELEPHANT]][m_iChessCnt[B_BISHOP]];

	//红卒
	m_nAttackValue[R_PAWN]=m_nAttackValue_EB[R_PAWN][m_iChessCnt[B_ELEPHANT]][m_iChessCnt[B_BISHOP]];
	for(i=0;i<m_uiCurBigChessCnt[R_CAR];i++)
		m_nAttackValue[R_PAWN]+=5;
	//--------------------------------------------------

	//---------------------残局设计---------------------
	//--------------------------------------------------
}

void CBaseEveluation::UnUpdateCurChess(BYTE byChess, bool bIsRed)
{
	if(byChess==NOCHESS)
		return;

	m_iChessCnt[byChess]++;//当前各类棋子数目
	m_uiAllChessCnt++;     //当前棋局共有的棋子数
	if(bIsRed)
	{
		m_uiAllRChessCnt++;//当前棋局红方共有的棋子数

		m_vectorRCurChess.push_back(byChess);//黑方当前所拥有的棋子

		//更新红方主力棋子数
		if(byChess==R_CAR || byChess==R_HORSE || byChess==R_CANON)
		{
			m_nMainChessCnt++; //当前棋局所拥有的主力棋子:车马炮
			m_nRMainChessCnt++;//当前棋局红方所拥有的主力棋子:车马炮
		}
	}
	else
	{
		m_uiAllBChessCnt++;//当前棋局黑方共有的棋子数

		m_vectorRCurChess.push_back(byChess);//红方当前所拥有的棋子

		//更新黑方主力棋子数
		if(byChess==B_CAR || byChess==B_HORSE || byChess==B_CANON)
		{
			m_nMainChessCnt++; //当前棋局所拥有的主力棋子:车马炮
			m_nBMainChessCnt++;//当前棋局黑方所拥有的主力棋子:车马炮
		}
	}

	//判断当前棋局所处阶段
	if(m_uiAllChessCnt>=28)
		m_uiCurChessAge=CA_START;
	else
	{
		if(m_nMainChessCnt>4)
			m_uiCurChessAge=CA_MID;
		else
			m_uiCurChessAge=CA_END;
	}
}

int CBaseEveluation::Multiply(int a, int b)
{
//	if(!a || !b)
//		return 0;
//
//	int n=0;
//	int iRet=0;
//
//	while((1<<n)<=a)
//		n++;
//
//	while(n>0)
//	{
//		n--;
//		if((1<<n)<=a)
//		{
//			iRet+=b<<n;
//			a-=1<<n;
//		}
//	}
//
//	return iRet;

	switch(a)
	{
	case 0:
		return 0;

	case 1:
		return b;

	case 2:
		return b<<1;

	case 6:
		return (b<<2)+(b<<1);

	case 12:
		return (b<<3)+(b<<2);

	case 15:
		return (b<<3)+(b<<2)+(b<<1)+b;
	}
	switch(b)
	{
	case 0:
		return 0;

	case 1:
		return a;

	case 2:
		return a<<1;

	case 6:
		return (a<<2)+(a<<1);

	case 12:
		return (a<<3)+(a<<2);

	case 15:
		return (a<<3)+(a<<2)+(a<<1)+a;
	}
}

void CBaseEveluation::InitVar(BYTE byChessBoard[][9])
{
	int i,j;
	CHESSMANPOS cmpTemp;

	memset(m_iChessCnt,0,15*sizeof(int));
	memset(m_uiCurBigChessCnt,0,15*sizeof(UINT));
	memset(m_uiCurLBigChessCnt,0,15*sizeof(UINT));
	memset(m_uiCurRBigChessCnt,0,15*sizeof(UINT));

	m_uiAllChessCnt=0;
	m_uiAllBChessCnt=0;
	m_uiAllRChessCnt=0;
	m_nMainChessCnt=0;
	m_nBMainChessCnt=0;
	m_nRMainChessCnt=0;

	m_vectorBCurChess.clear();
	m_vectorRCurChess.clear();
	for(i=0;i<15;i++)
		m_vectorCurChessPos[i].clear();

	for(i=0;i<10;i++)
	{
		for(j=0;j<9;j++)
		{
			if(byChessBoard[i][j]!=NOCHESS)
			{
				m_iChessCnt[byChessBoard[i][j]]++;
				m_uiAllChessCnt++;

				cmpTemp.x=i;
				cmpTemp.y=j;
				m_vectorCurChessPos[byChessBoard[i][j]].push_back(cmpTemp);

				if(IsBlack(byChessBoard[i][j]))
				{
					m_uiAllBChessCnt++;
					m_vectorBCurChess.push_back(byChessBoard[i][j]);
					if(i>=5)
					{
						m_uiCurBigChessCnt[byChessBoard[i][j]]++;
						if(j<=4)
							m_uiCurLBigChessCnt[byChessBoard[i][j]]++;
						if(j>=4)
							m_uiCurRBigChessCnt[byChessBoard[i][j]]++;
					}

					if(byChessBoard[i][j]==B_CAR || byChessBoard[i][j]==B_HORSE || byChessBoard[i][j]==B_CANON)
					{
						m_nMainChessCnt++;
						m_nBMainChessCnt++;
					}
				}
				else
				{
					m_uiAllRChessCnt++;
					m_vectorRCurChess.push_back(byChessBoard[i][j]);
					if(i<=4)
					{
						m_uiCurBigChessCnt[byChessBoard[i][j]]++;
						if(j<=4)
							m_uiCurLBigChessCnt[byChessBoard[i][j]]++;
						if(j>=4)
							m_uiCurRBigChessCnt[byChessBoard[i][j]]++;
					}

					if(byChessBoard[i][j]==R_CAR || byChessBoard[i][j]==R_HORSE || byChessBoard[i][j]==R_CANON)
					{
						m_nMainChessCnt++;
						m_nRMainChessCnt++;
					}
				}
			}
		}
	}

	//判断当前棋局所处阶段
	if(m_uiAllChessCnt>=28)
		m_uiCurChessAge=CA_START;
	else
	{
		if(m_nMainChessCnt>4)
			m_uiCurChessAge=CA_MID;
		else
			m_uiCurChessAge=CA_END;
	}
}
