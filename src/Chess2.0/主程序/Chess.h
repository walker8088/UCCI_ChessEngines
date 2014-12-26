// Chess.h : main header file for the CHESS application
//

#if !defined(AFX_CHESS_H__07794904_7160_4EE0_8059_37770BB7A0F8__INCLUDED_)
#define AFX_CHESS_H__07794904_7160_4EE0_8059_37770BB7A0F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "Define.h"

/////////////////////////////////////////////////////////////////////////////
// CChessApp:
// See Chess.cpp for the implementation of this class
//

class CChessApp : public CWinApp
{
public:
	CChessApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChessApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CChessApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void ShowTipAtStartup(void);
private:
	void ShowTipOfTheDay(void);
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHESS_H__07794904_7160_4EE0_8059_37770BB7A0F8__INCLUDED_)
