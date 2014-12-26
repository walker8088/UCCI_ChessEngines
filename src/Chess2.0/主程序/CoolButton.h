#if !defined(AFX_COOLBUTTON_H__B86C574E_1E72_11D4_9933_E709602BF90C__INCLUDED_)
#define AFX_COOLBUTTON_H__B86C574E_1E72_11D4_9933_E709602BF90C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CoolButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCoolButton window

#define XS_FLAT_BUTTON

class CCoolButton : public CButton
{
// Construction
public:
	CCoolButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCoolButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL SubclassDlgItem(UINT nID, CWnd* pParent);
	void SetIcon(HICON hIcon,BYTE cx,BYTE cy);
	virtual ~CCoolButton();

	// Generated message map functions
protected:
#ifdef XS_FLAT_BUTTON
	//{{AFX_MSG(CCoolButton)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
#endif

	DECLARE_MESSAGE_MAP()
private:
	BYTE m_cyIcon;
	BYTE m_cxIcon;
	HICON m_hIcon;

#ifdef XS_FLAT_BUTTON
	BOOL m_MouseOnButton;
#endif
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COOLBUTTON_H__B86C574E_1E72_11D4_9933_E709602BF90C__INCLUDED_)
