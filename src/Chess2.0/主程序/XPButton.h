#if !defined(AFX_XPBUTTON_H__7C6468A7_65D2_4DD8_B7B3_1CD483294610__INCLUDED_)
#define AFX_XPBUTTON_H__7C6468A7_65D2_4DD8_B7B3_1CD483294610__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XPButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CXPButton window

class CXPButton : public CButton
{
// Construction
public:
	CXPButton();

// Attributes
public:

// Operations
public:
	inline void SetNormalTopColor(COLORREF color)
	{
		m_NormalColorTop = color;
	}
	inline void SetNormalBottomColor(COLORREF color)
	{
		m_NormalColorBottom = color;
	}
	inline void SetActiveTextColor(COLORREF color)
	{
		m_ActiveTextColor = color;
	}
	inline void SetNormalTextColor(COLORREF color)
	{
		m_NormalTextColor = color;
	}
	inline void SetSelectTextColor(COLORREF color)
	{
		m_SelectTextColor = color;
	}
	inline void SetFrameColor(COLORREF color)
	{
		m_FrameColor = color;
	}
	inline void SetActiveColor(COLORREF color)
	{
		m_ActiveColor = color;
	}
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXPButton)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CXPButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CXPButton)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	void DrawFace(COLORREF Top, COLORREF Bottom, CRect& rc, CRect CalRc, CDC* pDC);
	void DrawFrame(COLORREF HeightLight, COLORREF ShadowLight, COLORREF FrameColor, CRect& rc, CDC* pDC);
	void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );
	LONG OnMouseLeave(WPARAM, LPARAM);
	BOOL m_bIsMouseMove;

	COLORREF m_FrameColor;
	COLORREF m_ActiveColor;

	COLORREF m_ActiveTextColor;
	COLORREF m_NormalTextColor;
	COLORREF m_SelectTextColor;

	COLORREF m_FrameHeight;
	COLORREF m_FrameShadow;

	COLORREF m_NormalColorTop;
	COLORREF m_NormalColorBottom;

DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XPBUTTON_H__7C6468A7_65D2_4DD8_B7B3_1CD483294610__INCLUDED_)
