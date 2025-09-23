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
	// �����˵���
	BOOL CreateMenuBar(CWnd *pParent, int iMenuID, CRect &rctFirstBtn, Image *pBgImg, Image *pBtnImg); 

	// ���Ʋ˵���
	void OnPaint(Graphics *pGraphics);

	// �ڲ˵���������Ӳ˵���ť
	CNCButton *AddMenuButton(LPRECT lpRect, Image *pImage, UINT nCount = 3, UINT nID = 0, LPCTSTR lpszCaption = NULL);
	bool OnLButtonDown(CPoint point);
	bool OnMouseMove(CPoint point);

	// ��ȡ��ǰ������ڵ��Ӳ˵�
	int TButtonHitTest(CPoint point);
	void OnEnterIdle();

	// ��ȡ�����˵�������
	void GetMenuBarRect(CRect &rct); 
	void GetMenuBtnRect(CRect &rct, int iID);

	// �ͷű����»�������Ӳ˵�
	void ReleaseMenuBtn();

public:
	typedef map<int, CNCButton> MapBtn;

private:
	int			m_iMenuID;			// �˵�ID
	CRect		m_rctBtn;			// ��һ���Ӳ˵�������
	Image*		m_pBgImg;			// �˵�������ͼƬ
	Image*		m_pBtnImg;			// �Ӳ˵���ť������ͼƬ
	CWnd*		m_pParent;			// �˵���������

	int			m_iXButtonHovering;
	MapBtn		m_mMenuBtn;			// �Ӳ˵���ť�б�
	Graphics*	m_pGraphics;

	int			m_iMenuIndex;
	BOOL		m_bMenuContinue;
	int			m_iBtnCnt;

	CRect		m_rcMenuBar;		// �˵���������
};

#endif // !defined(AFX_MENUBAR_H__BB7D3A60_A212_4DC5_A8A0_FB1C34DB2E04__INCLUDED_)
