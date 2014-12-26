//====================================================================
//==============XP Style Button=============
//Author	Ma811
//Date		2002-7-19
//Version	0.50
//E-mail	ma811@yeah.net
//====================================================================
////////////////////////////////////////////////////////////////////
// XPButton.cpp : implementation file
//

#include "stdafx.h"
#include "XPButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXPButton

CXPButton::CXPButton()
{
	m_bIsMouseMove = 0;

	m_NormalTextColor	= RGB(255, 255, 255);
	m_SelectTextColor	= RGB(255, 255, 255);
	m_ActiveTextColor	= RGB(255, 255, 255);

	m_ActiveColor		= RGB(255, 120, 80);

	m_NormalColorTop	= RGB(255, 255, 255);
	m_NormalColorBottom = RGB(0, 128, 255);

	m_FrameColor		= RGB(0, 64, 128);
	m_FrameHeight		= RGB(230, 230, 230);
	m_FrameShadow		= RGB(128, 128, 128);
}

CXPButton::~CXPButton()
{
}


BEGIN_MESSAGE_MAP(CXPButton, CButton)
	//{{AFX_MSG_MAP(CXPButton)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXPButton message handlers

void CXPButton::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	//*
	CDC* pDC      = CDC::FromHandle(lpDrawItemStruct->hDC);
	DWORD nState  = lpDrawItemStruct->itemState;
	DWORD nAction = lpDrawItemStruct->itemAction; 
	CRect rc	  = lpDrawItemStruct->rcItem;
	UINT uStyle   = DFCS_BUTTONPUSH;

	pDC->SetBkMode(TRANSPARENT);
	CString strText;
	GetWindowText(strText);

	if( nState & ODS_SELECTED )
	{
		m_bIsMouseMove = 0;

		DrawFace(m_NormalColorBottom, m_NormalColorTop, rc, rc, pDC);
		DrawFrame(m_FrameShadow, m_FrameShadow, m_FrameColor, rc, pDC);	
		//pDC->Draw3dRect(rc, RGB(0,0,0), RGB(0,0,0));

		pDC->SetTextColor(m_SelectTextColor);
	}
	else //Normal
	{
		DrawFace(m_NormalColorTop, m_NormalColorBottom, rc, rc, pDC);
		DrawFrame(m_FrameHeight, m_FrameShadow, m_FrameColor, rc, pDC);

		pDC->SetTextColor(m_NormalTextColor);
	}

	if( m_bIsMouseMove )
	{
		CRect rc2(rc);
		rc2.DeflateRect(2, 2, 2, 2);

		DrawFace(RGB(GetRValue(m_ActiveColor), GetGValue(m_ActiveColor)+100, GetBValue(m_ActiveColor)+100),
			m_ActiveColor, rc, rc, pDC);
		DrawFace(m_NormalColorTop, m_NormalColorBottom, rc2, rc, pDC);

		CBrush NullBrush;
		NullBrush.CreateStockObject(NULL_BRUSH);
		CBrush* pOldBrush = pDC->SelectObject(&NullBrush);
		
		CPen Pen;
		Pen.CreatePen(PS_SOLID, 1, m_FrameColor);
		CPen* pOldPen = pDC->SelectObject(&Pen);
		rc.InflateRect(1,1,1,1);
		pDC->RoundRect(rc, CPoint(3, 3));
		//rc.DeflateRect(1, 1, 1, 1); 
		//pDC->Draw3dRect(rc, HeightLight, ShadowLight);
		
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);

		pDC->SetTextColor(m_ActiveTextColor);
	}
	
	pDC->DrawText(strText, strText.GetLength(), 
		&lpDrawItemStruct->rcItem, DT_SINGLELINE|DT_VCENTER|DT_CENTER);
	//*///
}

void CXPButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if( m_bIsMouseMove == 0 )
	{
		m_bIsMouseMove = 1;
		Invalidate();
		
		TRACKMOUSEEVENT trackmouseevent;
		trackmouseevent.cbSize = sizeof(trackmouseevent);
		trackmouseevent.dwFlags = TME_LEAVE;
		trackmouseevent.hwndTrack = GetSafeHwnd();
		trackmouseevent.dwHoverTime = HOVER_DEFAULT;
		_TrackMouseEvent(&trackmouseevent);
	}
	
	CButton::OnMouseMove(nFlags, point);
}

LONG CXPButton::OnMouseLeave(WPARAM, LPARAM)
{
	m_bIsMouseMove = 0;
	Invalidate();

	return 0;
}

void CXPButton::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	UINT nBS = GetButtonStyle();

	// Add BS_OWNERDRAW style
	SetButtonStyle(nBS | BS_OWNERDRAW);

	CButton::PreSubclassWindow();
}

void CXPButton::DrawFace(COLORREF Top, COLORREF Bottom, CRect& rc, CRect CalRc, CDC* pDC)
{
	CPen Pen;
	CPen* pOldPen = pDC->SelectObject(&Pen);
	
	int R, G, B;
	R = (GetRValue(Top) - GetRValue(Bottom)) / CalRc.Height();
	G = (GetGValue(Top) - GetGValue(Bottom)) / CalRc.Height();
	B = (GetBValue(Top) - GetBValue(Bottom)) / CalRc.Height();
	
	//R = R>0 ? R : -R;
	//G = G>0 ? G : -G;
	//B = B>0 ? B : -B;

	int ColR = GetRValue(Top), ColG = GetGValue(Top), ColB = GetBValue(Top);
	COLORREF ColMax = Top > Bottom ? Top : Bottom;
	COLORREF ColMin = Top > Bottom ? Bottom: Top;

	for(int i=0; i<rc.Height(); i++)
	{
		ColR -= R;
		ColG -= G;
		ColB -= B;

		/*
		if( ColR >= GetRValue(ColMax) || ColR <= GetRValue(ColMin) ||
			ColG >= GetGValue(ColMax) || ColG <= GetGValue(ColMin) ||
			ColB >= GetBValue(ColMax) || ColB <= GetBValue(ColMin) )
		{
			R = G = B = 0;
		}///*/


		Pen.DeleteObject();
		Pen.CreatePen(PS_SOLID, 1, RGB(ColR, ColG, ColB));
				
		pDC->SelectObject(&Pen);
		
		pDC->MoveTo(rc.left, rc.top+i);
		pDC->LineTo(rc.right, rc.top+i);
	}

	pDC->SelectObject(pOldPen);
}

void CXPButton::DrawFrame(COLORREF HeightLight, COLORREF ShadowLight, COLORREF FrameColor, CRect& rc, CDC* pDC)
{
	CBrush NullBrush;
	NullBrush.CreateStockObject(NULL_BRUSH);
	CBrush* pOldBrush = pDC->SelectObject(&NullBrush);

	CPen Pen;
	Pen.CreatePen(PS_SOLID, 1, FrameColor);
	CPen* pOldPen = pDC->SelectObject(&Pen);
	
	pDC->RoundRect(rc, CPoint(3, 3));
	rc.DeflateRect(1, 1, 1, 1); 
	pDC->Draw3dRect(rc, HeightLight, ShadowLight);

	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
}
