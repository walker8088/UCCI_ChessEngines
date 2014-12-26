// MemDC.cpp: implementation of the CMemDC class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MemDC.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemDC::CMemDC(CDC* pDC):CDC()
{

	ASSERT(pDC!=NULL);

	m_pDC=pDC;
	m_pOldBitmap=NULL;
	m_bMemDC=!pDC->IsPrinting();
	//
	if(m_bMemDC)
	{
		pDC->GetClipBox(&m_rect);
		CreateCompatibleDC(pDC);
		m_bitmap.CreateCompatibleBitmap(pDC,m_rect.Width(),m_rect.Height());
		m_pOldBitmap=SelectObject(&m_bitmap);
		SetWindowOrg(m_rect.left,m_rect.top);
	}
	else		//为相关的现有设备准备打印
	{
		m_bPrinting=pDC->m_bPrinting;
		m_hDC=pDC->m_hDC;
		m_hAttribDC=pDC->m_hAttribDC;
	}

}

CMemDC::~CMemDC()
{
	if(m_bMemDC)
	{
		m_pDC->BitBlt(m_rect.left,m_rect.top,m_rect.Width(),m_rect.Height(),
			this,m_rect.left,m_rect.top,SRCCOPY);
		SelectObject(m_pOldBitmap);
	}
	else
	{
		m_hDC=m_hAttribDC=NULL;
	}

}
//	CMemDC* operator->(){return this;}
//	operator CMemDC*(){return this;}
