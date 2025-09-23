#if !defined(AFX_GUITABWND_H__4C966B28_CE7A_407D_9A7A_9BE7A80B395D__INCLUDED_)
#define AFX_GUITABWND_H__4C966B28_CE7A_407D_9A7A_9BE7A80B395D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabWnd.h : header file
//

////////////////////////////////////////////////////////////////////////////
// CTabWnd window
#include "GdiplusTS.h"
#include "GuiDrawlayer.h"
#include "XTipWnd.h"
#include "dllexport.h"

#define WM_DELETETAB (WM_USER + 5000)

class CGuiTabWnd;
class CGuiTab;

class CONTROL_EXPORT CGuiTabWndCB
{
protected:
	virtual void	OnRButtonDown2(CPoint pt, int32 iTab) = 0;	
	virtual bool32  OnLButtonDown2(CPoint pt, int32 iTab) { return false; }
	virtual bool32	BlindLButtonBeforeRButtonDown(int32 iTab) { return false; }

friend class CGuiTabWnd;
};
//*********************************************************
class CONTROL_EXPORT CGuiTabWnd : public CWnd 
{
public:
	enum Style{S3D=0,FLAT=1,SHADOW=2};
	enum AlingTab{ALN_TOP=0,ALN_BOTTOM=1,ALN_LEFT=2,ALN_RIGHT=3};
	enum E_LayoutStyle {ELSNormal=0, ELSGraphic=1, ELSCustomSize};
	enum E_GrapicStyle{EGSStyle1=0, EGSStyle2=1}; // EGSStyle1,3种状态，bitmap // EGSStyle2,2中状态，png ,文字颜色和对齐方式不同

public:
	CGuiTabWnd();
	virtual ~CGuiTabWnd();
	
public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	
public:
	//***********************************************************************
	void	FillWithBitmap(CDC* pDC,UINT uiBitmapID,const CRect& Rect);
	void	SetBkGround(bool32 bBitmap,COLORREF clrBkGround,UINT uiBitmapIDNormal,UINT uiBitmapIDSelected,int nType=1,UINT uiBitmapIDBK=0);	// 设置背景.可能是颜色,也可能是图片
	void	SetPngBkGround(Gdiplus::Image *pngIDNormal, Gdiplus::Image *pngIDSelected, uint32 uiType=1,UINT uiBitmapIDBK=0);	// 设置背景png图片
	void	SetStyle(Style estyle);
	void	virtual Drawtabs(CDC* dc);
	void	DrawTab(CDC* dc, int32 iIndexTab);
	void	DrawImageTab(CDC* dc, int32 iIndexTab);
	void	Addtab(CString lpMsg, CString lpShortMsg, CString lpTipMsg, CString lpValue=L"");
	void	SetImageList(UINT nBitmapID, int cx, int nGrow, COLORREF crMask);
	void	RecalLayout();
	virtual void AjustTabs(CRect &RectTabs);
	void    SetCurtab(int m_numtab, bool bSilence = false);
	int     GetCurtab();
	void    DeleteTab(int m_numTab);
	void	DeleteAll();
	int     GetCount();
	void    SetALingTabs(AlingTab alnTab);
	void    SetTabsTitle(int iTabNr, CString sNewTitle);
	CString GetTabsTitle(int iTabNr);
	void    SetTabMenu(int iNumTab, UINT uiMenuId);
	void	SetTabIcon(int iNumTab, UINT uiIconIndex);
	BOOL	GetTabRect(OUT CRect &RectTab, int32 iIndexTab);
	void    SetTabLength(int iTabLen);
	
	int		GetFitHorW();
	int		GetFitFontW();	// 一个字的宽度
	void	SetTipTitle(CString strTipTitle);

	CString GetTabsValue(int iTabNr);
	void	SetTabsValue(int iTabNr,CString cstrValue);

	CSize	GetWindowWishSize();		// 获取整个窗口希望的大小

	bool		SetFontHeight(int iHeight);
	bool		SetFontByName(const CString &StrFaceName, BYTE lfCharSet);
	bool		SetFontByLF(const LOGFONT &lf);

	int			SetTabSpace(int iSpace);	// 设置tab的高度(如果是align top/b)空余 或者 
	void		SetTabBkColor(COLORREF clrNormal = CLR_DEFAULT, COLORREF clrSelect = CLR_DEFAULT);
	void		GetTabBkColor(COLORREF &clrNormal, COLORREF &clrSelect);
	void		SetTabTextColor(COLORREF clrNormal = CLR_DEFAULT, COLORREF clrSelect = CLR_DEFAULT);
	void		GetTabTextColor(COLORREF &clrNormal, COLORREF &clrSelect);
	void		SetTabFrameColor(COLORREF clrFrame = CLR_DEFAULT);
	void		GetTabFrameColor(COLORREF& clrFrame);
	
	void    SetLayoutStyle(E_LayoutStyle style){ m_eLayoutStyle = style; }
	void    SetGraphicStyle(E_GrapicStyle style){ m_eGraphicStyle = style; }
	BOOL	IsValid();
	void	SetUserCB(CGuiTabWndCB* pUserCB);
	virtual void StyleDispl(DWORD dwDsp=GUISTYLE_XP)  {m_StyleDisplay=dwDsp;SendMessage(WM_NCPAINT);Invalidate(); UpdateWindow();}
	DWORD   GetStyleDispl(){ return m_StyleDisplay;}														
	DWORD			m_StyleDisplay;

	CPoint	GetLeftTopPoint(int32 iTab);	// 得到某个Tab 的左上角的Windows 坐标
	//***********************************************************************

	void	SetShowBeginIndex(int32 index, int32 iEnd);

private:	
	void	GetSuitableDisplayShortName(IN CDC *pDC, IN const CRect &RectMax, IN const CString &StrLongName, OUT CString &StrShortName);
	CSize	GetTabWishSize(int32 iTab);		// 获取tab的wish大小，每个tab根据所要显示的文字和一个最小合适大小进行计算

protected:
	CPtrArray		m_pArray;
	CSize			m_sizeImag;
	int				m_Numtabs;
	int				m_iMinValRec;
	int				m_ActualVar;
	int				m_iSelectTab;
	CRect			m_rectCliente;
	CRect			m_rectTabs;
	CImageList		Image;
	Style			m_style;
	GuiDrawLayer	m_wd;
	CWnd*			pParent;
	AlingTab		m_alnTab;
	E_LayoutStyle	m_eLayoutStyle;
	E_GrapicStyle	m_eGraphicStyle;
	BOOL			m_InTab;
	
	CXTipWnd		m_TipWnd;
	CString			m_strTipTitle;

	CPoint			m_PointLast;
	
	CFont			m_FontHorz;
	CFont			m_FontHorzBold;

	bool32			m_bBkGroundBitmap;
	COLORREF		m_ClrBkGround;
	UINT			m_uiBitmapIDNormal;
	UINT			m_uiBitmapIDSelected;
	UINT			m_uiBitmapIDBK; // 背景图片

	Gdiplus::Image	*m_pngIDNormal;
	Gdiplus::Image	*m_pngIDSelected;

	COLORREF		m_clrTabBkNormal;
	COLORREF		m_clrTabBkSelect;
	COLORREF		m_clrTabTextNormal;
	COLORREF		m_clrTabTextSelect;
	COLORREF		m_clrTabFrame;

	int				m_iTabSpace;

	uint32			m_ibmpWidth;
	uint32			m_ibmpHeight;

	int				m_iDefTabLen;

	int				m_iHotTab;
	int				m_iIndexBegin;
	int				m_iIndexEnd;

	CGuiTabWndCB*	m_pUserCB;
protected:
	
	//{{AFX_MSG(CGuiTabWnd)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSysColorChange( );
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu);
	afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GUITABWND_H__4C966B28_CE7A_407D_9A7A_9BE7A80B395D__INCLUDED_)
