// SearchEngine.h: interface for the CBaseSearchEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEARCHENGINE_H__7A7237B9_0908_45D8_B102_94E342B174A5__INCLUDED_)
#define AFX_SEARCHENGINE_H__7A7237B9_0908_45D8_B102_94E342B174A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseEveluation.h"
#include "Eveluation.h"
#include "MoveGenerator.h"
#include "GradientProgressCtrl.h"
#include <vector>

using namespace std;

//搜索引擎的基类
class CBaseSearchEngine
{
public:
	CBaseSearchEngine();
	virtual ~CBaseSearchEngine();

public:
	virtual SearchAGoodMove(BYTE position[10][9])=0;        //走下一步
	int GetSearchDepth(){return m_iCanSearchDepth;};        //得到搜索的深度
	CHESSMOVE GetBestMove(){return m_cmBestMove;};			//得到最佳走法
	UNDOMOVE GetUndoMove(){return m_umUndoMove;};			//得到悔棋走法
	int GetEvel();											//得到估值
	BYTE GetAteChess(){return m_byAteChess;};               //得到被吃掉的棋子
	void SetSearchDepth(int nDepth){m_iSearchDepth=nDepth;};//设定最大搜索深度
	void SetEveluator(CBaseEveluation* pEval){m_pEval=pEval;};//设定估值引擎
	void SetMoveGenerator(CMoveGenerator* pMG){m_pMG =pMG;};//设定走法产生器
	void SetTimeLimit(float fTimeLimit){m_fTimeLimit=fTimeLimit;};//设置时间限制
	void SetThinkProgress(CGradientProgressCtrl* pThinkProgress){m_pThinkProgress=pThinkProgress;};
															//设定显示思考进度的进度条
	void SetUserChessColor(int nUserChessColor){m_nUserChessColor=nUserChessColor;};
															//设定用户为黑方或红方

	void UndoChessMove(BYTE position[10][9],CHESSMOVE* move,BYTE nChessID);//悔棋
	void RedoChessMove(BYTE position[10][9],CHESSMOVE* move);              //还原

	int IsObeyRule(BYTE byChessBoard[][9],int iDepth,CBANDMOVE cam,bool bIsRed);//是否遵循象棋规则
	bool IsFail(BYTE byChessBoard[][9],bool bIsRed);//判断某棋面已败

protected:
	int IsGameOver(BYTE position[10][9],int nDepth);   //判断是否已分胜负
	int SelMove(vector<int> vectorScore,int iMaxScore);//根据轮盘法选择比较好的走法
	BYTE MakeMove(CHESSMOVE* move);				       //根据某一走法产生走了之后的棋盘
	void UnMakeMove(CHESSMOVE* move,BYTE nChessID);    //恢复为走过之前的棋盘
	bool BadForMe(BYTE byChessBoard[][9],bool bIsRed); //对己方不利
	UINT GetChessAge(BYTE byChessBoard[][9]);

public:
	int m_nUserChessColor;

protected:
	CGradientProgressCtrl* m_pThinkProgress;
								 //用以显示思考进度的进度条指针
	BYTE byCurChessBoard[10][9];//搜索时用于记录当前节点棋盘状态的数组
	CHESSMOVE m_cmBestMove;		//记录最佳走法
	UNDOMOVE m_umUndoMove;
	CMoveGenerator* m_pMG;		//走法产生器
	CBaseEveluation* m_pEval;	//估值核心
	int m_iSearchDepth;			//最大搜索深度
	int m_iMaxDepth;			//当前搜索的最大搜索深度
	int m_iCanSearchDepth;      //能搜索的深度
	float m_fTimeLimit;         //限制时间
	int m_iTimeCount;			//计时变量
	int m_iEvel;
	BYTE m_byAteChess;          //被吃掉的棋子
	vector<int> m_vectorEvel;   //棋盘估值

	vector<CBANDMOVE> m_vectorRedCbAndMove;  //棋盘和走法
	vector<CBANDMOVE> m_vectorBlackCbAndMove;//棋盘和走法
};

#endif // !defined(AFX_SEARCHENGINE_H__7A7237B9_0908_45D8_B102_94E342B174A5__INCLUDED_)
