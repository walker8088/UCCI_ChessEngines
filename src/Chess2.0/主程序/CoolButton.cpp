// CoolButton.cpp : implementation file
//

#include "stdafx.h"
#include "CoolButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCoolButton

CCoolButton::CCoolButton()
{
#ifdef XS_FLAT_BUTTON
	m_MouseOnButton=FALSE;
#endif

	m_hIcon=NULL;
	m_cyIcon=0;
	m_cxIcon=0;
}

CCoolButton::~CCoolButton()
{
}


BEGIN_MESSAGE_MAP(CCoolButton, CButton)
	//{{AFX_MSG_MAP(CCoolButton)
	ON_WM_MOUSEMOVE()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCoolButton message handlers

void CCoolButton::DrawItem(LPDRAWITEMSTRUCT lpDIS) 
{
	// TODO: Add your code to draw the specified item
	CDC * pDC=CDC::FromHandle(lpDIS->hDC);
	
	unsigned int  IsPressed =(lpDIS->itemState&ODS_SELECTED);
	unsigned int  IsFocused =(lpDIS->itemState&ODS_FOCUS);
	unsigned int  IsDisabled =(lpDIS->itemState&ODS_DISABLED);

	CRect itemRect = lpDIS->rcItem;

#ifndef XS_FLAT_BUTTON
	if(IsFocused)
	{
		CBrush br(RGB(0,0,0));
		pDC->FrameRect(&itemRect,&br);
		itemRect.DeflateRect(1,1);
	}
#endif
	//Fill with bkcolor
	CBrush br(GetSysColor(COLOR_BTNFACE));
	pDC->FillRect(&itemRect,&br);

	//Is pressed?
	if(IsPressed)
	{
#ifdef XS_FLAT_BUTTON
		//浅边界笔
		CPen penBtnHiLight(PS_SOLID,0,GetSysColor(COLOR_BTNHILIGHT));
		//阴影笔
		CPen penBtnShadow(PS_SOLID,0,GetSysColor(COLOR_BTNSHADOW));
		
		//绘边界阴影
		pDC->SelectObject(penBtnShadow);
		pDC->MoveTo(itemRect.left,itemRect.bottom-1);
		pDC->LineTo(itemRect.left,itemRect.top);
		pDC->LineTo(itemRect.right,itemRect.top);
		//绘浅边界
		pDC->SelectObject(penBtnHiLight);
		pDC->MoveTo(itemRect.left,itemRect.bottom-1);
		pDC->LineTo(itemRect.right-1,itemRect.bottom-1);
		pDC->LineTo(itemRect.right-1,itemRect.top-1);
#else
		CBrush brBtnShadow(GetSysColor(COLOR_BTNSHADOW));
		pDC->FrameRect(&itemRect,&brBtnShadow);
#endif
	}
	else//没按下
	{
		CPen penBtnHiLight(PS_SOLID,0,GetSysColor(COLOR_BTNHILIGHT));
		CPen pen3DLight(PS_SOLID,0,GetSysColor(COLOR_3DLIGHT));
		CPen penBtnShadow(PS_SOLID,0,GetSysColor(COLOR_BTNSHADOW));
		CPen pen3DDKShadow(PS_SOLID,0,GetSysColor(COLOR_3DDKSHADOW));

#ifdef XS_FLAT_BUTTON
		if(m_MouseOnButton==TRUE)
		{
			pDC->SelectObject(penBtnHiLight);
			pDC->MoveTo(itemRect.left,itemRect.bottom-1);
			pDC->LineTo(itemRect.left,itemRect.top);
			pDC->LineTo(itemRect.right,itemRect.top);
			//
			pDC->SelectObject(penBtnShadow);
			pDC->MoveTo(itemRect.left,itemRect.bottom-1);
			pDC->LineTo(itemRect.right-1,itemRect.bottom-1);
			pDC->LineTo(itemRect.right-1,itemRect.top-1);
		}
#else
		pDC->SelectObject(penBtnHiLight);
		pDC->MoveTo(itemRect.left,itemRect.bottom-1);
		pDC->LineTo(itemRect.left,itemRect.top);
		pDC->LineTo(itemRect.right,itemRect.top);

		pDC->SelectObject(pen3DLight);
		pDC->MoveTo(itemRect.left+1,itemRect.bottom-1);
		pDC->LineTo(itemRect.left+1,itemRect.top+1);
		pDC->LineTo(itemRect.right,itemRect.top+1);

		pDC->SelectObject(pen3DDKShadow);
		pDC->MoveTo(itemRect.left,itemRect.bottom-1);
		pDC->LineTo(itemRect.right-1,itemRect.bottom-1);
		pDC->LineTo(itemRect.right-1,itemRect.top-1);

		pDC->SelectObject(penBtnShadow);
		pDC->MoveTo(itemRect.left+1,itemRect.bottom-2);
		pDC->LineTo(itemRect.right-2,itemRect.bottom-2);
		pDC->LineTo(itemRect.right-2,itemRect.top);
#endif
	}

#ifndef XS_FLAT_BUTTON
	//
	if(IsFocused)
	{
		CRect focusRect = itemRect;
		focusRect.DeflateRect(3,3);
		pDC->DrawFocusRect(&focusRect);
	}
#endif
	
	//获取文本
	CString title;
	GetWindowText(title);

	//绘制图标
	if(m_hIcon!=NULL)
	{
		CRect iconRect = lpDIS->rcItem;
		//根据标题是否存在来设置不同的图标位置
		if(title.IsEmpty()==TRUE)
		{
			iconRect.left+=((iconRect.Width()-m_cxIcon)/2);
		}
		else
		{
			iconRect.left+=6;
		}
		iconRect.top+=((iconRect.Height()-m_cyIcon)/2);
	
		if(IsPressed)iconRect.OffsetRect(1,1);


		pDC->DrawIcon(iconRect.TopLeft(),m_hIcon);
	}
	//绘标题
	CRect captionRect = lpDIS->rcItem;
	captionRect.left+=m_cxIcon;
	
	if(title.IsEmpty()==FALSE)
	{
		pDC->SetBkMode(TRANSPARENT);
		captionRect.OffsetRect(0,-1);
		
		if(IsPressed)
			captionRect.OffsetRect(1,1);

		if(IsDisabled)
		{
			captionRect.OffsetRect(1,1);
			pDC->SetTextColor(GetSysColor(COLOR_BTNHILIGHT));
			pDC->DrawText(title,-1,captionRect,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
			captionRect.OffsetRect(-1,-1);
			pDC->SetTextColor(GetSysColor(COLOR_BTNSHADOW));
			pDC->DrawText(title,-1,captionRect,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
		}
		else
		{
			pDC->DrawText(title,-1,captionRect,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
		}
	}
}

#ifdef XS_FLAT_BUTTON
void CCoolButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CWnd* pWnd;
	CWnd* pParent;

	CButton::OnMouseMove(nFlags, point);

	pWnd=GetActiveWindow();
	pParent=GetOwner();
	if((m_MouseOnButton==FALSE)&&(GetCapture()!=this)&&
		((pWnd!=NULL)&&(pWnd->m_hWnd==pParent->m_hWnd)))
	{
		SetCapture();
		SetFocus();
		m_MouseOnButton=TRUE;
		Invalidate();
		UpdateWindow();
	}
	else
	{
		CRect rc;
		GetClientRect(&rc);
		if(!rc.PtInRect(point))
		{
			m_MouseOnButton=FALSE;
			Invalidate();
			UpdateWindow();
			ReleaseCapture();
		}
	}
}
#endif

#ifdef XS_FLAT_BUTTON
void CCoolButton::OnKillFocus(CWnd* pNewWnd) 
{
	CButton::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here
	if(m_MouseOnButton==TRUE)
	{
		m_MouseOnButton=FALSE;
		Invalidate();
		UpdateWindow();
	}
}
#endif

void CCoolButton::SetIcon(HICON hIcon, BYTE cx, BYTE cy)
{
	m_hIcon  = hIcon;
	m_cxIcon = cx;	
	m_cyIcon = cy;
}

BOOL CCoolButton::SubclassDlgItem(UINT nID, CWnd *pParent)
{
	BOOL retValue=CButton::SubclassDlgItem(nID,pParent);

	LONG bs=::GetWindowLong(m_hWnd,GWL_STYLE);
	bs|=BS_OWNERDRAW;
	::SetWindowLong(m_hWnd,GWL_STYLE,bs);

	return retValue;
}
