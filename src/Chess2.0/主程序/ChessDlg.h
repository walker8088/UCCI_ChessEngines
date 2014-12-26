// ChessDlg.h : header file
//

#if !defined(AFX_CHESSDLG_H__C9065501_927E_437F_8700_63743E930B62__INCLUDED_)
#define AFX_CHESSDLG_H__C9065501_927E_437F_8700_63743E930B62__INCLUDED_

#pragma warning(disable:4786)//禁止waring号为4786的warning消息

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Define.h"
#include "SetDlg.h"
#include "CoolButton.h"
#include "GradientProgressCtrl.h"
#include "HelpDlg.h"
#include "NegaScout_TT_HH.h"
#include "NegaScout_TT_HH2.h"
#include "NegaScout_TT_HH3.h"
#include "NegaScout_TT_HH4.h"
#include "NegaScout_TT_HH5.h"
#include "NegaScout_TT_HH6.h"
#include "SplashDlg.h"
#include "TipDlg.h"
#include <fstream>
#include <stack>
#include <vector>

/////////////////////////////////////////////////////////////////////////////
// CChessDlg dialog

using namespace std;

struct MOVECHESS
{
	BYTE nChessID;
	POINT ptMovePoint;
};

typedef enum{Chessing,SetChessBoarding,Previewing}Status;
typedef vector<OPENBOOKTREENODE*> MOVEVECTOR;

class CChessDlg : public CDialog
{
// Construction
public:
	CChessDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CChessDlg)
	enum { IDD = IDD_CHESS_DIALOG };
	CStatic	m_staticTip;
	CCoolButton	m_btnStop;
	CCoolButton	m_btnUndo;
	CCoolButton	m_btnRedo;
	CCoolButton	m_btnComputer;
	CListBox	m_lstChessRecord;
	CGradientProgressCtrl m_progressThink;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChessDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

private:
	void CoolBeep();
	void OnCancel();

	void ConvertChessMove(CHESSMOVE *pCm);//对称的转换走法
	void LoadOpenBook(CString strFile,OPENBOOKTREENODE *pOpenBookTree,MOVEVECTOR& vectorChessMoveNode);//加载开局库
	void UpdateOpenBook(CString strFile,MOVEVECTOR vectorChessMoveNode,MOVEVECTOR& vectorPassNode);//更新开局库
	bool FindMoveInOB(OPENBOOKTREENODE *pOpenBookTree,CHESSMOVE& cmRival,MOVEVECTOR& vectorPassNode);//在开局库中找对应的走法
	int SelBestMove(vector<float> vectorScore);//根据各节点的得分选择节点

	void DrawTipBox(CDC *pDc, CPoint point,int iBitmap);//对走法画提示框
	void TransparentBlt2(HDC hdcDest,int nXOriginDest,int nYOriginDest,int nWidthDest,int nHeightDest,HDC hdcSrc,int nXOriginSrc,int nYOriginSrc,int nWidthSrc,int nHeightSrc,UINT crTransparent);
	
	CString GetMoveStr(int nFromX,int nFromY,int nToX,int nToY,int nSourceID);
										   //根据走法得到改走法对应的象棋语言
	CString ConvertDigit2Chinese(int nNum);//转换数字为汉字
	bool IsChessOver(int nChessSort);	   //判断某种类型棋子有无剩余
	void InvertChessBoard(BYTE cb[][9]);   //对换红黑双方棋子
	void AddChessRecord(int nFromX,int nFromY,int nToX,int nToY,int nUserChessColor,int nSourceID);
										   //记录下棋步骤
	int IsGameOver(BYTE position[][9]);    //判断游戏是否结束

public:
	void Think();

// Implementation
protected:
	HICON m_hIcon;
	HICON m_hUndoIcon;
	HICON m_hRedoIcon;
	HICON m_hComputerIcon;
	HICON m_hStopIcon;
	CSetDlg m_SetDlg;
	CToolTipCtrl m_tooltip;
	CSplashDlg m_splashDlg;

	//创建多线程时用
	HANDLE m_hHandle;
	DWORD m_dwThreadID;

	CString m_strWelcome;
	bool m_bIsNeedUpdateOB;          //是否需要更新开局库
	bool m_bIsGameOver;              //该盘棋是否结束
	int m_iWhoChess;				 //下步该谁走棋
	int m_nUserChessColor;		     //用户棋子颜色
	BYTE m_byChessBoard[10][9];      //棋盘数组，用于显示棋盘
	BYTE m_byBackupChessBoard[10][9];//备份棋盘数组，用于出错恢复
	BYTE m_byShowChessBoard[10][9];  //用于显示的棋盘
	MOVECHESS m_MoveChess;		     //用于保存当前被拖拽的棋子的结构
	POINT m_ptMoveChess;		     //用于保存当前被拖拽的棋子的位置
	CBitmap m_BoardBmp;			     //bitmap图用于显示棋盘
	CImageList m_Chessman;		     //用于绘制棋子的ImageList对象
	int m_nBoardWidth;			     //棋盘宽度
	int m_nBoardHeight;			     //棋盘高度

	CMoveGenerator* m_pMG;   //走法产生器
	CBaseEveluation* m_pEval;//估值核心
	CBaseSearchEngine* m_pSE;//搜索引擎指针

	CPoint m_MousePoint;   //当前鼠标所在点
	Status m_Status;       //当前状态
	bool m_bIsThinking;    //电脑是否正在想
	bool m_bIsBegin;       //是否已开始下棋
	bool m_bIsNeedOpenBook;//开局库是否已经使用完毕
	bool m_bIsConvert;     //再开局库中匹配走法时是否需要对称的转换一下
	int m_nWillChessColor; //下一步该哪方下
	CHESSMOVE m_cmBestMove;//最佳走法
	CHESSMOVE m_cmShow;    //要显示的走法
	UNDOMOVE m_umUndoMove; //悔棋走法
	int m_iBout;		   //已下的回合数,如有悔棋算减
	int m_iResult;         //这盘棋的结果
	bool bAddFlag[2];      //搜索深度是否增加的标志
	int m_iInitDepth;	   //初始搜索深度
	
	//-------设置-------
	int m_iChessSort;  //下棋方式
	int m_iDepthSort;  //深度方式
	bool m_bMoveSound; //走棋时是否响铃
	float m_fTimeLimit;//时间限制,就是走每步最多用的时间
	int m_iDepth;      //搜索深度
	//-----------------

	stack<UNDOMOVE> m_stackUndoMove;//记录走法的栈，便于悔棋
	stack<UNDOMOVE> m_stackRedoMove;//记录已悔棋的走法的栈，便于还原

	OPENBOOKTREENODE *m_pOpenBookTree;				//开局库树
	OPENBOOKTREENODE *m_pOpenBookTreeBackup;		//开局库树备份
	vector<OPENBOOKTREENODE*> m_vectorPassNode;		//开局时使用的节点,供程序退出时对开局库中的节点评分
	vector<OPENBOOKTREENODE*> m_vectorChessMoveNode;//开局库链表,该链表供节点查找父节点时用
	
	// Generated message map functions
	//{{AFX_MSG(CChessDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSetchessboard();
	afx_msg void OnSet();
	afx_msg void OnAbout();
	afx_msg void OnOpenfile();
	afx_msg void OnSavefile();
	afx_msg void OnScbover();
	afx_msg void OnRpawn();
	afx_msg void OnRcanon();
	afx_msg void OnRcar();
	afx_msg void OnRhorse();
	afx_msg void OnRelephant();
	afx_msg void OnRbishop();
	afx_msg void OnRking();
	afx_msg void OnBpawn();
	afx_msg void OnBcanon();
	afx_msg void OnBcar();
	afx_msg void OnBhorse();
	afx_msg void OnBelephant();
	afx_msg void OnBbishop();
	afx_msg void OnBking();
	afx_msg void OnDelete();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnClose();
	afx_msg void OnClearcb();
	afx_msg void OnNewgame();
	afx_msg void OnBtncomputer();
	afx_msg void OnBtnundo();
	afx_msg void OnBtnredo();
	afx_msg void OnDblclkListchessrecord();
	afx_msg void OnBtnStop();
	afx_msg void OnSelchangeListchessrecord();
	afx_msg void OnHelp();
	afx_msg void OnInversecb();
	afx_msg void OnSave();
	afx_msg void OnExit();
	afx_msg void OnOpen();
	afx_msg void OnUpdatePreview(CCmdUI* pCmdUI);
	afx_msg void OnMakemanual();
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHESSDLG_H__C9065501_927E_437F_8700_63743E930B62__INCLUDED_)
