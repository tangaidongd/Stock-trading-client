// CMyGroupBoxButton.cpp : implementation file
//

#include "stdafx.h"
#include "MyGroupBoxButton.h"


// CMyGroupBoxButton

IMPLEMENT_DYNAMIC(CMyGroupBoxButton, CButton)

CMyGroupBoxButton::CMyGroupBoxButton()
{
	m_bHtBottom = FALSE;
	m_iID = 0;
}

CMyGroupBoxButton::~CMyGroupBoxButton()
{
}


BEGIN_MESSAGE_MAP(CMyGroupBoxButton, CButton)
ON_WM_NCHITTEST()
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()



// CMyGroupBoxButton message handlers



LRESULT CMyGroupBoxButton::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CRect rect;
	GetClientRect(&rect);
	CPoint pt =point;
	ScreenToClient(&pt);
	int nFrame=4;
	rect.DeflateRect(nFrame,nFrame);
	if (!rect.PtInRect(pt))
	{ 
		if (pt.y>=rect.bottom-nFrame)
		{
			m_bHtBottom = TRUE;
			return HTBOTTOM;
		}
	}

	return CButton::OnNcHitTest(point);
}

void CMyGroupBoxButton::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CButton::OnWindowPosChanging(lpwndpos);

	if (m_bHtBottom)
	{
		m_bHtBottom = FALSE;
		GetParent()->PostMessage(WM_POSCHANGED_GROUPBOX,m_iID,0);
	}
	
	// TODO: 在此处添加消息处理程序代码
}

// 注册ID(create之前)
void CMyGroupBoxButton::RegistrID(int iID)
{
	m_iID = iID;
}