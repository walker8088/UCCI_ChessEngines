// SplashDlg.cpp : implementation file
//

#include "stdafx.h"
#include "chess.h"
#include "SplashDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSplashDlg dialog


CSplashDlg::CSplashDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSplashDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSplashDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSplashDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSplashDlg)
	DDX_Control(pDX, IDC_PROGRESS, m_progressCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSplashDlg, CDialog)
	//{{AFX_MSG_MAP(CSplashDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSplashDlg message handlers

BOOL CSplashDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	//彩色进度条设置
	m_progressCtrl.SetStartColor(RGB(0xFF,0xFF,0x00));//黄色
	m_progressCtrl.SetEndColor(RGB(0x00,0x93,0x00));  //绿色
	m_progressCtrl.SetBkColor(RGB(0xE6,0xE6,0xFA));   //淡紫色
	m_progressCtrl.SetTextColor(RGB(0,0,255));
	m_progressCtrl.ShowPercent(1);
	m_progressCtrl.SetRange(0,8934);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSplashDlg::ProgressGo()
{
	m_progressCtrl.StepIt();
}
