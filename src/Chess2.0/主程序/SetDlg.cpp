// NewGameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Chess.h"
#include "SetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetDlg dialog


CSetDlg::CSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetDlg)
	m_bMoveSound = FALSE;
	m_fTimeLimit = 5.0f;
	m_iDepth = 6;
	//}}AFX_DATA_INIT
	m_hSetIcon=::AfxGetApp()->LoadIcon(IDI_SET);
	m_bitmapOk.LoadBitmap(IDB_OK);
	m_bitmapCancel.LoadBitmap(IDB_CANCEL);

	m_strTip="搜索引擎,双击可以快捷“确定”";

	m_nUserChessColor=REDCHESS;
	m_nChessSort=CS_PCCHESS;
}


void CSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetDlg)
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_PLY, m_SetPly);
	DDX_Check(pDX, IDC_CHECK_SOUND, m_bMoveSound);
	DDX_Text(pDX, IDC_EDIT_TIMELIMIT, m_fTimeLimit);
	DDX_Text(pDX, IDC_EDIT_DEPTH, m_iDepth);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetDlg, CDialog)
	//{{AFX_MSG_MAP(CSetDlg)
	ON_BN_CLICKED(IDC_RADIOBLACKCHESS, OnRadioblackchess)
	ON_BN_CLICKED(IDC_RADIOREDCHESS, OnRadioredchess)
	ON_BN_CLICKED(IDC_RADIOPC, OnRadiopc)
	ON_BN_CLICKED(IDC_RADIOCC, OnRadiocc)
	ON_BN_CLICKED(IDC_RADIOPP, OnRadiopp)
	ON_BN_CLICKED(IDC_RADIOHASH, OnRadiohash)
	ON_BN_DOUBLECLICKED(IDC_RADIOPC, OnDoubleclickedRadiopc)
	ON_BN_DOUBLECLICKED(IDC_RADIOCC, OnDoubleclickedRadiocc)
	ON_BN_DOUBLECLICKED(IDC_RADIOPP, OnDoubleclickedRadiopp)
	ON_BN_DOUBLECLICKED(IDC_RADIOHASH, OnDoubleclickedRadiohash)
	ON_BN_DOUBLECLICKED(IDC_RADIOREDCHESS, OnDoubleclickedRadioredchess)
	ON_BN_DOUBLECLICKED(IDC_RADIOBLACKCHESS, OnDoubleclickedRadioblackchess)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetDlg message handlers

void CSetDlg::OnOK() 
{
	// TODO: Add extra validation here	
	UpdateData(1);
	if(m_fTimeLimit<1)
	{
		MessageBox("限制时间也太短了吧,你以为它是深蓝啊,不行,至少为1秒","友情提醒",MB_ICONINFORMATION);
		return;
	}

	if(m_fTimeLimit>7200)
	{
		MessageBox("限制时间也太长了吧,你也太小瞧它了吧,不行,至多至多为7200秒","友情提醒",MB_ICONINFORMATION);
		return;
	}

	CDialog::OnOK();
}

BOOL CSetDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetIcon(m_hSetIcon,1);
	m_btnOk.SetBitmap(m_bitmapOk);
	m_btnCancel.SetBitmap(m_bitmapCancel);

	m_tooltip.Create(this);
	m_tooltip.Activate(1);
	m_tooltip.AddTool(GetDlgItem(IDC_EDIT_TIMELIMIT),m_strTip);

	m_SetPly.SetRange(1,7200);

	if(m_nUserChessColor==REDCHESS)
		((CButton*)GetDlgItem(IDC_RADIOREDCHESS))->SetCheck(TRUE);
	else
		((CButton*)GetDlgItem(IDC_RADIOBLACKCHESS))->SetCheck(TRUE);

	switch(m_nChessSort)
	{
	case 1:
		((CButton*)GetDlgItem(IDC_RADIOPC))->SetCheck(TRUE);
		break;

	case 2:
		((CButton*)GetDlgItem(IDC_RADIOPP))->SetCheck(TRUE);
		break;

	case 3:
		((CButton*)GetDlgItem(IDC_RADIOCC))->SetCheck(TRUE);
		break;

	case 4:
		((CButton*)GetDlgItem(IDC_RADIOHASH))->SetCheck(TRUE);
		break;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetDlg::OnRadioblackchess() 
{
	// TODO: Add your control notification handler code here
	m_nUserChessColor=BLACKCHESS;
}

void CSetDlg::OnRadioredchess() 
{
	// TODO: Add your control notification handler code here
	m_nUserChessColor=REDCHESS;
}

void CSetDlg::OnDblclkListengine() 
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CSetDlg::OnRadiopc() 
{
	// TODO: Add your control notification handler code here
	m_nChessSort=CS_PCCHESS;
}

void CSetDlg::OnRadiocc() 
{
	// TODO: Add your control notification handler code here
	m_nChessSort=CS_CCCHESS;
}

void CSetDlg::OnRadiopp() 
{
	// TODO: Add your control notification handler code here
	m_nChessSort=CS_PPCHESS;
}

void CSetDlg::OnRadiohash() 
{
	// TODO: Add your control notification handler code here
	m_nChessSort=CS_HASHCHESS;
}

BOOL CSetDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	m_tooltip.RelayEvent(pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}

void CSetDlg::OnDoubleclickedRadiopc() 
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CSetDlg::OnDoubleclickedRadiocc()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CSetDlg::OnDoubleclickedRadiopp() 
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CSetDlg::OnDoubleclickedRadiohash() 
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CSetDlg::OnDoubleclickedRadioredchess() 
{
	// TODO: Add your control notification handler code here
	m_nUserChessColor=REDCHESS;
	OnOK();
}

void CSetDlg::OnDoubleclickedRadioblackchess() 
{
	// TODO: Add your control notification handler code here
	m_nUserChessColor=BLACKCHESS;
	OnOK();
}

void CSetDlg::OnDoubleclickedRadioDefaultset() 
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CSetDlg::OnDoubleclickedRadioUserdefine() 
{
	// TODO: Add your control notification handler code here
	OnOK();
}
