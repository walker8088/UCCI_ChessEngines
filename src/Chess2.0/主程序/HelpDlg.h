#if !defined(AFX_HELPDLG_H__2604B1FF_E844_4160_80BC_3C60622236E9__INCLUDED_)
#define AFX_HELPDLG_H__2604B1FF_E844_4160_80BC_3C60622236E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HelpDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHelpDlg dialog
#include "Chess.h"
#include "XPButton.h"

class CHelpDlg : public CDialog
{
// Construction
public:
	CHelpDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHelpDlg)
	enum { IDD = IDD_HELP };
	CString	m_strHelp;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHelpDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CBitmap m_bitOk;
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CHelpDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HELPDLG_H__2604B1FF_E844_4160_80BC_3C60622236E9__INCLUDED_)
