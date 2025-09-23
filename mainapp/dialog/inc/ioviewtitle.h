#if !defined(AFX_IOVIEWTITLE_H__)
#define AFX_IOVIEWTITLE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ioviewtitle.h : header file
//
#include <Gdiplus.h>
#include "Region.h"
#include "XTipWnd.h"
using namespace Gdiplus;


/////////////////////////////////////////////////////////////////////////////
// CIoViewTitle window

class CIoViewBase;


class CIoViewTitleButton;

// ...fangz1016 应该每个按钮有几种不同的画法,并提供标志给外面设置,做到控件与业务无关

class CIoViewTitle : public CStatic
{
// Construction
public:
	CIoViewTitle();
	virtual	~CIoViewTitle();

	void		Reset();															// 清空所有
	CRect		GetRect();															// 得到矩形
	int32		GetButtonNums();													// 得到所有按钮个数
	void		SetShowRect(const CRect& rectSet);									// 设置显示区域
	void		SetOwnerID(const UINT& uID);	
	void		AddButton(bool32 bPublic,CIoViewTitleButton* pButton);				// 添加按钮
	void		DelButton(bool32 bPublic,UINT iGuid);								// 删除按钮
	void		AddLeftRightButton(bool32 bLeft,CIoViewTitleButton* pButton);		// 添加方向键
	void		AddFuctionButton(CIoViewTitleButton* pButton);						// 添加功能按钮	
	void		ClearButtonFocusFlag();												// 去掉高亮
	void		SetFuctionBtnFlag(bool32 bShowSameTime);							// 设置功能按钮显示规则	
	void		SetMaxFlag(bool32 bMax);											// 最大化标志
	void		SetParentDragFlag(bool32 bParentDrag);								// 父亲拖动的标志
	void		ForcePaint();														// 刷新		
	void		SetBkGroundColor(COLORREF clrBk);									// 背景色

	void		SetDC(CMemDCEx* pDC);												// 设置DC
	void		SetDCTransformParam(int32 ix,int32 iy);								// 设置DC 的偏移量
	void		Transform(int32 ix,int32 iy);										// 坐标转换
	

	CIoViewTitleButton*			  GetButtonByFocus();								// 得到高亮按钮
	CIoViewTitleButton*			  GetButtonByPoint(CPoint point);					// 通过坐标得到按钮
	CIoViewTitleButton*			  GetPrivateButtonByID(UINT uID);					// 通过ID得到私有按钮
	CIoViewTitleButton*			  GetFuctionButtonByID(UINT uID);					// 通过ID得到功能按钮

private:
	UINT		m_uIDOwner;															// 标记是哪个IoView 的标题栏			
	CRect 		m_RectSet;															// 父窗口能给的区域
	CRect 		m_RectPublicBtns;													// 公共按钮的区域
	CRect 		m_RectPrivateBtns;													// 私有按钮区域
	CRect		m_RectFuctionBtns;													// 功能按钮区域
	bool32		m_bShowLeftRight;													// 是否画左右箭头
	bool32		m_bShowFuctionBtnSameTime;											// 控制功能按钮是不是同时显示.( true: 能显示下所有的按钮才显示,否则一个都不显示 false: 能显示几个就显示几个)

	bool32		m_bTracking;														// 设置鼠标离开的标志
	bool32		m_bParentMaxed;														// 视图是否最大化了 ...fangz1016 与业务相关了,要改
	bool32		m_bParentDrag;														// 父亲是否正在拖动
	CXTipWnd	m_TipWnd;															// Tip
	
	CMemDCEx*	m_pDCParent;														// 父亲设置的DC
	int32		m_ixDC;																// 父DC 的区域和本身区域的x 偏移	
	int32		m_iyDC;																// 父DC 的区域和本身区域的y 偏移
	COLORREF	m_ColorBk;															// 背景色
	
	CIoViewTitleButton*	m_pLeftButton;												// 左箭头
	CIoViewTitleButton*	m_pRightButton;												// 右箭头

	CArray<CIoViewTitleButton*, CIoViewTitleButton*>	m_apPublicBtns;				// 所有公共按钮
	CArray<CIoViewTitleButton*, CIoViewTitleButton*>	m_apPrivateBtns;			// 所有私有按钮
	CArray<CIoViewTitleButton*, CIoViewTitleButton*>	m_apFuctionBtns;			// 所有功能按钮
protected:
	//{{AFX_MSG(CIoViewTitle)
	afx_msg void OnPaint();															// 绘制
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);							// 左键按下
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);							// 右键按下
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);							// 鼠标移动
	afx_msg LRESULT OnMouseLeave(WPARAM wParam,LPARAM lParam);						// 鼠标离开
	afx_msg void OnTimer(UINT nIDEvent);											// 定时器
	// virtual BOOL PreTranslateMessage(MSG* pMsg);					
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
// 标题栏按钮
 
class CIoViewTitleButton
{
friend class CIoViewTitle;

public:
	CIoViewTitleButton();
	
	CIoViewTitleButton(const HWND hOwner, CIoViewBase* pParent, const bool32 bLeft);																															// 左右箭头
	CIoViewTitleButton(const HWND hOwner, CIoViewBase* pParent, const UINT uID, const bool32 bPublic, const CString& StrText, const CString& StrTips, UINT uiBmpNormal, UINT uiBmpFocus, UINT uiBmpSelf);		// 普通按钮
	
	virtual		~CIoViewTitleButton();

public:
	void		FillWithBitmap(CDC* pDC, UINT uiBitmapID, const CRect& rt);		// 用位图填充
	void		DrawButton(CDC* pDC, COLORREF clrBk, bool32 bMax);					// 绘制

	CRect		GetShowRect();														// 得到显示区域
	void		SetShowRect(const CRect& RectButton);								// 设置显示区域														

	void		OnLButtonDown();													// 左键单击
	void		OnRButtonDown();
	
	UINT		GetButtonID();														// 得到ID号

	bool32		GetSelfFlag();														// 得到标识
	void		SetSelfFlag(bool32 bSelf);											// 设置标识
	
	bool32		BeFocusButton();													// 是否是高亮
	void		SetFocusButton(bool32 bFocus);										// 设置是否高亮	

	void		SetText(const CString& StrText);									// 设置标题
	void		SetTips(const CString& StrTip);										// 设置Tips	
	
	CIoViewBase* TestGetParentIoView();
private:		
	HWND		m_hOwner;															// 接受消息的
	CIoViewBase*m_pOwner;
	UINT		m_uID;																// 从右向左(1,2,3,4,5,6,7..)
	bool32 		m_bPublic;															// 是否公共按钮
	CRect		m_RectButton;														// 显示区域
	CString		m_StrText;															// 标题
	CString		m_StrTips;															// Tips

	bool32		m_bRight;															// 向右箭头按钮
	bool32		m_bLeft;															// 向左箭头按钮
	bool32		m_bSelf;															// 自己这个按钮
	bool32		m_bFocus;															// 鼠标移动到这个.高亮
	
	UINT		m_uiBmpNormal;														// 正常的位图ID
	UINT		m_uiBmpFocus;														// 高亮时候的位图ID	
	UINT		m_uiBmpSelf;														// 显示自己时候的位图ID

	Image*		m_pImageNormal;														// 图片资源 (正常)
	Image*		m_pImageFocus;														// 图片资源 (高亮)
	Image*		m_pImageSelf;														// 图片资源 (自己)
};
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_IOVIEWTITLE_H__)

