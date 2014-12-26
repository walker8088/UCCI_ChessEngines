#if !defined(AFX_SPLASHDLG_H__7B56115F_776B_4128_8945_E2E01DDF3F59__INCLUDED_)
#define AFX_SPLASHDLG_H__7B56115F_776B_4128_8945_E2E01DDF3F59__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SplashDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSplashDlg dialog

#include "GradientProgressCtrl.h"

class CSplashDlg : public CDialog
{
// Construction
public:
	CSplashDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSplashDlg)
	enum { IDD = IDD_SPLASH };
	CGradientProgressCtrl	m_progressCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplashDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void ProgressGo();

	// Generated message map functions
	//{{AFX_MSG(CSplashDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPLASHDLG_H__7B56115F_776B_4128_8945_E2E01DDF3F59__INCLUDED_)
