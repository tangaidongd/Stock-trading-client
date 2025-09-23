/*********************************************************
* Multi-Page Interface
* Version: 1.2
* Date: September 2, 2003
* Author: Michal Mecinski
* E-mail: mimec@mimec.w.pl
* WWW: http://www.mimec.w.pl
*
* You may freely use and modify this code, but don't remove
* this copyright note.
*
* There is no warranty of any kind, express or implied, for this class.
* The author does not take the responsibility for any damage
* resulting from the use of it.
*
* Let me know if you find this code useful, and
* send me any modifications and bug reports.
*
* Copyright (C) 2003 by Michal Mecinski
*********************************************************/

#pragma once

//#include "XPTabCtrl.h"
#include "GuiTabWnd.h"
#include "tinyxml.h"
#include "dllexport.h"
//#include "IoViewBase.h"

typedef struct T_DoViewElementItem
{
	TiXmlElement *pChild;
	CWnd *pSender;
	int iDlgItemId;
}T_DoViewElementItem;

class CONTROL_EXPORT CTabSplitWnd : public CSplitterWnd,public CGuiTabWndCB
{
	DECLARE_DYNAMIC(CTabSplitWnd);
public:
	CTabSplitWnd();
	virtual ~CTabSplitWnd();

public:
	// Show the given page
	void SetPage(int nPage);
	static void BindMainFram(CWnd *pMainFram);
	// Load tabs from toolbar resource
	int CreateTabs(int nResourceID);
	int CreateEmptyTabs();

public:
	CString			ToXml();
	bool32			FromXml(TiXmlElement * pTiXmlElement);

	void			SetActiveChildView(int iPage, CView *pActiveView);
	CView*			GetActiveChildView(int iPage);

	CView*			FindFirstChildView(int iPage);	
	void			DelTab(int iPage);	

	bool32			GetDelAll(){ return m_bDelAll; }
	void			SetDelAll(bool32 bDelAll){ m_bDelAll = bDelAll;}

	virtual void	RecalcLayout();
	static const char *	GetXmlElementValue();

	void			SetPageByIoView(CWnd * pIoView);
	int32			GetIoViewPage(CWnd * pIoView);

	void			AddGroupID(int32 iID);
	void			DelGroupID(int32 iTab);
	int32			GetGroupID();

protected:
	virtual int		HitTest(CPoint pt) const;
	virtual void	DrawAllSplitBars(CDC* pDC, int cxInside, int cyInside);

private:
	bool32			DoTabItemElement(TiXmlElement *pElement);
	void			OnRButtonDown2( CPoint pt, int32 iTab);
	 
public:
	CGuiTabWnd		m_wndTabCtrl;
	int				m_nCurPage;
	bool32			m_bDelAll;
	bool32			m_bfromxml;
	static CWnd *m_pMainFram;
	
	CArray<CView *, CView *> m_ActiveViews;
	CArray<int32,int32>		 m_aGroupIDs;
	int32					 m_iCurGroupID;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSelChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTabMenu(UINT nID);
 //	afx_msg void OnSize(UINT nType, int cx, int cy);
};

