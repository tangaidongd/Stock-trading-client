// MenuBar.cpp: implementation of the CMenuBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GGTong.h"
#include "MenuBar.h"
#include "FontFactory.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define INVALID_ID	-1
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMenuBar::CMenuBar()
{
	m_iMenuID		= IDR_MAINFRAME;
	m_pBgImg		= NULL;
	m_pBtnImg		= NULL;
	m_pParent		= NULL;
	m_pGraphics		= NULL;
	m_iMenuIndex	= INVALID_ID;
	m_bMenuContinue = FALSE;
	m_iBtnCnt		= 0;
	m_iXButtonHovering = INVALID_ID;
}

CMenuBar::~CMenuBar()
{
}

BOOL CMenuBar::CreateMenuBar(CWnd *pParent, int iMenuID, CRect &rctFirstBtn, Image *pBgImg, Image *pBtnImg)
{
	ASSERT(NULL != pBtnImg);
	m_iMenuID = iMenuID;
	m_rctBtn  = rctFirstBtn;
	m_pBgImg  = pBgImg;
	m_pBtnImg = pBtnImg;
	m_pParent = pParent;

	CString strVal;
	CRect rctBtn = m_rctBtn;
	CMenu menu; 
	menu.LoadMenu(m_iMenuID); 
	m_iBtnCnt = menu.GetMenuItemCount();
	for (int i=0; i<m_iBtnCnt; i++)
	{
		menu.GetMenuString(i, strVal, MF_BYPOSITION);
		int iLength = strVal.GetLength();
		if (3 < iLength) 
		{
			rctBtn.right += (iLength - 3)*12;
		}
		AddMenuButton(&rctBtn, pBtnImg, 3, ID_SUBMENU_BTN+i, strVal);

		if (i < (m_iBtnCnt - 1))
		{
			rctBtn.left = rctBtn.right;
			rctBtn.right = rctBtn.left + m_rctBtn.Width();
		}
	}

	m_rcMenuBar = rctBtn;
	return TRUE;
}

void CMenuBar::OnPaint(Graphics *pGraphics)
{
	m_pGraphics = pGraphics;
	ASSERT(NULL != m_pGraphics);

	MapBtn::iterator iter;
	// 遍历菜单栏上所有子菜单
	for (iter=m_mMenuBtn.begin(); iter!=m_mMenuBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;
		btnControl.DrawButton(m_pGraphics);
	}
}

CNCButton *CMenuBar::AddMenuButton(LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption)
{
	ASSERT(pImage);
	CNCButton btnControl;
	CNCButton::T_NcFont stFont;
	CFontFactory fontFactory;
	stFont.m_StrName = fontFactory.GetExistFontName(L"微软雅黑");	//...
	stFont.m_Size	 = 9.5f;
	stFont.m_iStyle = FontStyleRegular;
	btnControl.CreateButton(lpszCaption, lpRect, m_pParent, pImage, nCount, nID);
	
	btnControl.SetTextColor(RGB(0xf1,0xd1,0xd1), RGB(255,255,255), RGB(255,255,255));
	//btnControl.SetTextBkgColor(RGB(44,50,55), RGB(44,50,55), RGB(44,50,55));
	//btnControl.SetTextFrameColor(RGB(44,50,55), RGB(44,50,55), RGB(44,50,55));
	
	btnControl.SetFont(stFont);

	m_mMenuBtn[nID] = btnControl;
	return &m_mMenuBtn[nID];
}

bool CMenuBar::OnLButtonDown(CPoint point)
{
	// 	int iButton = TButtonHitTest(point);
	// 	if (INVALID_ID != iButton)
	// 	{
	// 		m_mMenuBtn[iButton].LButtonDown();
	// 	}

	// 遍历菜单栏上所有子菜单按钮
	int32 i=0;
	for (i=ID_SUBMENU_BTN; i<m_iBtnCnt+ID_SUBMENU_BTN; i++)
	{
		if (m_mMenuBtn[i].PtInButton(point))
		{
			if (!m_mMenuBtn[m_iMenuIndex].PtInButton(point))
			{
				m_mMenuBtn[i].MouseLeave();
			}

			m_iMenuIndex = i;

			do 
			{
				//画菜单的按下效果
				m_mMenuBtn[i].LButtonDown();
				m_pParent->PostMessage(WM_COMMAND, m_mMenuBtn[i].GetControlId());
				m_mMenuBtn[i].SetPressed(FALSE);
				m_bMenuContinue = FALSE;
				//	ShowMenu(); 
			} while(m_bMenuContinue);   

			return TRUE;
		}
	}

	if (i == m_iBtnCnt)
	{
		m_iMenuIndex = INVALID_ID;
	}

	return FALSE;
}

bool CMenuBar::OnMouseMove(CPoint point)
{
	// 	int iButton = TButtonHitTest(point);
	// 	if (iButton != m_iXButtonHovering)
	// 	{
	// 		if (INVALID_ID != m_iXButtonHovering)
	// 		{
	// 			m_mMenuBtn[m_iXButtonHovering].MouseLeave();
	// 			m_iXButtonHovering = INVALID_ID;
	// 		}
	// 		
	// 		if (INVALID_ID != iButton)
	// 		{
	// 			m_iXButtonHovering = iButton;
	// 			m_mMenuBtn[m_iXButtonHovering].MouseHover();
	// 		}
	// 	}

	if (INVALID_ID != m_iMenuIndex)
	{
		if (!m_mMenuBtn[m_iMenuIndex].PtInButton(point))
		{
			m_mMenuBtn[m_iMenuIndex].MouseLeave();
		}

	}

	m_iMenuIndex = INVALID_ID;
	CRect rcMenu;
	for (int i=ID_SUBMENU_BTN; i<m_iBtnCnt+ID_SUBMENU_BTN; i++)
	{
		if (m_mMenuBtn[i].PtInButton(point))
		{
			// 确定当前菜单索引
			m_iMenuIndex = i;
			// 画菜单的抬起效果
			m_mMenuBtn[i].MouseHover();
			if (m_bMenuContinue)
			{
				m_pParent->PostMessage(WM_COMMAND, m_mMenuBtn[i].GetControlId());
				m_bMenuContinue = FALSE;
			}
			return TRUE;
		}
	}
	return FALSE;
}

int CMenuBar::TButtonHitTest(CPoint point)
{
	MapBtn::iterator iter;

	// 遍历菜单栏上所有子菜单按钮
	for (iter=m_mMenuBtn.begin(); iter!=m_mMenuBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;

		// 点point是否在按钮区域内
		if (btnControl.PtInButton(point))
		{
			return btnControl.GetControlId();
		}
	}

	// 点point不在菜单栏区域的子菜单上 
	return INVALID_ID;
}

void CMenuBar::GetMenuBarRect(CRect &rct)
{
	rct = m_rcMenuBar;
}

void CMenuBar::GetMenuBtnRect(CRect &rct, int iID)
{
	MapBtn::iterator iter;

	// 遍历菜单栏上所有子菜单按钮
	for (iter=m_mMenuBtn.begin(); iter!=m_mMenuBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;
		if (iID == btnControl.GetControlId())
		{
			btnControl.GetRect(rct);
			return;
		}
	}
}

void CMenuBar::OnEnterIdle()
{
	CPoint point;
	::GetCursorPos(&point);
	CRect rcWindow;
	GetWindowRect(m_pParent->m_hWnd, &rcWindow);
	point.x -= rcWindow.left;
	point.y -= rcWindow.top;

	for (int i=ID_SUBMENU_BTN; i<ID_SUBMENU_BTN+m_iBtnCnt; i++)
	{
		if (m_mMenuBtn[i].PtInButton(point))
		{
			if (m_iMenuIndex != i)
			{
				if (INVALID_ID != m_iMenuIndex)
				{
					if (!m_mMenuBtn[m_iMenuIndex].PtInButton(point))
					{
						m_mMenuBtn[m_iMenuIndex].MouseLeave();
					}
				}
				m_iMenuIndex = i;
				m_bMenuContinue = TRUE;
				SendMessage(m_pParent->m_hWnd, WM_CANCELMODE, 0, 0);
			}
			break;
		}
	}
}

void CMenuBar::ReleaseMenuBtn()
{
	MapBtn::iterator iter;

	// 遍历菜单栏上所有子菜单按钮
	for (iter=m_mMenuBtn.begin(); iter!=m_mMenuBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;

		// 按钮是否需要弹起
		if (!btnControl.IsNormal())
		{
			btnControl.MouseLeave();
		}
	}
}
