// MenuBar.h: interface for the CMenuBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MENUBAR_H__BB7D3A60_A212_4DC5_A8A0_FB1C34DB2E04__INCLUDED_)
#define AFX_MENUBAR_H__BB7D3A60_A212_4DC5_A8A0_FB1C34DB2E04__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <vector>
#include "NCButton.h"
using namespace std;

class CMenuBar  
{
public:
	CMenuBar();
	virtual ~CMenuBar();

public:
	// 创建菜单栏
	BOOL CreateMenuBar(CWnd *pParent, int iMenuID, CRect &rctFirstBtn, Image *pBgImg, Image *pBtnImg); 

	// 绘制菜单栏
	void OnPaint(Graphics *pGraphics);

	// 在菜单栏上添加子菜单按钮
	CNCButton *AddMenuButton(LPRECT lpRect, Image *pImage, UINT nCount = 3, UINT nID = 0, LPCTSTR lpszCaption = NULL);
	bool OnLButtonDown(CPoint point);
	bool OnMouseMove(CPoint point);

	// 获取当前鼠标所在的子菜单
	int TButtonHitTest(CPoint point);
	void OnEnterIdle();

	// 获取整个菜单栏区域
	void GetMenuBarRect(CRect &rct); 
	void GetMenuBtnRect(CRect &rct, int iID);

	// 释放被按下或高亮的子菜单
	void ReleaseMenuBtn();

public:
	typedef map<int, CNCButton> MapBtn;

private:
	int			m_iMenuID;			// 菜单ID
	CRect		m_rctBtn;			// 第一个子菜单的区域
	Image*		m_pBgImg;			// 菜单栏背景图片
	Image*		m_pBtnImg;			// 子菜单按钮高亮的图片
	CWnd*		m_pParent;			// 菜单栏父窗口

	int			m_iXButtonHovering;
	MapBtn		m_mMenuBtn;			// 子菜单按钮列表
	Graphics*	m_pGraphics;

	int			m_iMenuIndex;
	BOOL		m_bMenuContinue;
	int			m_iBtnCnt;

	CRect		m_rcMenuBar;		// 菜单栏的区域
};

#endif // !defined(AFX_MENUBAR_H__BB7D3A60_A212_4DC5_A8A0_FB1C34DB2E04__INCLUDED_)
