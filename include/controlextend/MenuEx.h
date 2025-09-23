// MenuEx.h: interface for the CMenuEx class.
//CMenuEx
//接口:
//	void InitMenu(CMenu *pMenu,UINT uToolBar,CToolBar *pToolBar)
//	void InitPopupMenu(CMenu *pMenu,UINT uToolBar,CToolBar *pToolBar)
//		与InitMenu不同的是：InitMenu并不修改第一级菜单为自绘风格，而
//		该函数有包括第一级菜单但必须注意：该类的任一实例都只能调用这
//		两个函数中的一个，不能一同使用
//	void SetHighLightColor(COLORREF crColor)
//	void SetBackColor(COLORREF)
//	void SetTextColor(COLORREF)
//	void SetImageLeft(UINT idBmpLeft)
//									作者：lbird(郑恒)
//										2003.1.10	zh0116@163.net QQ:10992445
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MENUEX_H__FE677F6B_2315_11D7_8869_BB2B2A4F4D45__INCLUDED_)
#define AFX_MENUEX_H__FE677F6B_2315_11D7_8869_BB2B2A4F4D45__INCLUDED_
#include "afxtempl.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "dllexport.h"

typedef struct tagMENUITEM
{
	CString		strText;
	UINT		uID;
#ifdef MENUCHAR
	UINT		uChr;
#endif
	UINT		uIndex;
	int 		uPositionImageLeft;
}MENUITEM;
typedef MENUITEM * LPMENUITEM;
///////////////////////////////////////////
class CONTROL_EXPORT CMenuEx : public CMenu  
{
public:
	void InitPopupMenu(CMenu *pPopupMenu,UINT uToolBar,CToolBar *pToolBar);
	void ChangeStyle(CMenu *pMenu,CToolBar *pToolBar,BOOL bIsMainMenu=FALSE);
	void SetHighLightColor(COLORREF crColor);
	void SetBackColor(COLORREF);
	void SetTextColor(COLORREF);
#ifdef MENUCHAR
	LRESULT	MenuChar(UINT nChar);
#endif
	void SetImageLeft(UINT idBmpLeft);
	void MeasureItem(LPMEASUREITEMSTRUCT  lpMIS);
	void InitMenu(CMenu *pMenu,UINT uToolBar,CToolBar *pToolBar);
	void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	CMenuEx();
	virtual ~CMenuEx();

protected:
	int m_nSeparator;
	CSize m_szImageLeft;
	CBitmap m_bmpImageLeft;	
	BOOL m_bHasImageLeft;	//是否有侧边位图
	BOOL m_bInitial;	//菜单是否已初始化，即设定了自绘风格
	int GetImageFromToolBar(UINT uToolBar,CToolBar *pToolBar,COLORREF	crMask=RGB(192,192,192));
	//lint -sem(CList::AddTail, custodial(1))
	CList<MENUITEM *,MENUITEM *> m_ListMenu;

#ifdef MENUCHAR
	CList<MENUITEM *,MENUITEM *> m_currentListMenu;	//是m_ListMenu的子集，指当前打开的菜单
#endif

	COLORREF m_colMenu;
	COLORREF m_colTextSelected;
	void DrawImageLeft(CDC *pDC,CRect &rect,LPMENUITEM lpItem);
	void TextMenu(CDC *pDC,CRect &rect,CRect rtText,BOOL bSelected,BOOL bGrayed,LPMENUITEM lpItem);
	CImageList m_ImageList;
	COLORREF m_colText;
	CSize m_szImage;	//菜单项位图的大小
	void DrawMenuItemImage(CDC *pDC,CRect &rect,BOOL bSelected,BOOL bChecked,BOOL bGrayed,BOOL bHasImage,LPMENUITEM lpItem);
	void GrayString(CDC *pDC,const CString &str,const CRect rect);
};

#endif // !defined(AFX_MENUEX_H__FE677F6B_2315_11D7_8869_BB2B2A4F4D45__INCLUDED_)
