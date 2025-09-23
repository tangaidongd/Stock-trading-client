#ifndef _IOVIEWDUOGUTONGLIE_H_
#define _IOVIEWDUOGUTONGLIE_H_

// 多股同列视图
// 多股同列下分为多股同列K线与分时, 在激活为k线下，调用的多股同列为K线，在激活为其它情况下，调用的激活为分时
// 多股下的小视图的group全部为-1, 双击进入对应的k、分时页面, 屏蔽右键菜单
// 多股K:
//		统一调换指标，PageUp/Down统一商品切换，按键精灵切换当前聚焦的子视图商品
// 多股分时:
//		PageUp/Down统一商品切换，按键精灵切换当前聚焦的子视图商品

#include "IoViewBase.h"

class CIoViewKLine;
class CIoViewTrend;
/////////////////////////////////////////////////////////////////////////////
// CIoViewDuoGuTongLie

class CIoViewDuoGuTongLie : public CIoViewBase
{

public:
	// 多股同列的三种表现形式
	enum E_DuoGuTongLieType
	{
		EDGLT_KLINE,                    // K线多股同列
		EDGLT_TREND,					// 分时多股同列
		EDGLT_KLINE_TREND,				// 分时K线多股同列
	};

	// Construction
public:
	CIoViewDuoGuTongLie();
	virtual ~CIoViewDuoGuTongLie();

	DECLARE_DYNCREATE(CIoViewDuoGuTongLie)

	// from CIoViewBase
public:
	virtual void	RequestViewData(){};
	virtual void	SetChildFrameTitle();				// 需要能给IoViewManager识别，所以要实现xml
	virtual bool32	FromXml(TiXmlElement *pElement);
	virtual CString	ToXml();
	virtual	CString GetDefaultXML();

	virtual bool32	GetStdMenuEnable(MSG* pMsg) { return true; };

	virtual void	OnIoViewActive();					// 转接给激活的实际ioview
	virtual void	OnIoViewDeactive();

	virtual void	LockRedraw();						// 锁定激活的ioview
	virtual void	UnLockRedraw();

	virtual CMerch *GetMerchXml();						// 转到子ioview
	virtual E_IoViewType GetIoViewType() { return EIVT_MultiMerch; }

	virtual	CMerch *GetNextMerch(CMerch* pMerchNow, bool32 bPre);

	virtual void	OnEscBackFrameMerch(CMerch *pMerch);

	// from CIoViewBase
public:

	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual void	OnVDataMerchTimeSalesUpdate(IN CMerch *pMerch);
	virtual void	OnVDataForceUpdate();

	virtual void	OnIoViewColorChanged();			
	virtual void	OnIoViewFontChanged();

	virtual	bool32	IsIoViewContainer() { return true; }		// 是IoView容器视图，下面还存在子视图
	virtual	CIoViewBase    *GetActiveIoView();

public:
	void		ShowViews(int iPicId, bool32 bChangeMerchByOldShow = false);	// 显示k线或者分时视图的多股同列

	void		ShowViews(CIoViewBase *pIoViewSrc);		// 根据前ioview显示现在的多股同列视图
	void		ShowNextViews();		// 显示另外一种多股同列视图 k->分时 分时->k

	void		SetBaseMerch(CMerch *pMerch);	// 设定基础的商品，确定 pageup/down，mousewheel滚动范围

	void		OnShowSubIoView();	// 单独显示子视图, 跳转到图表窗口

	void		RefreshView(CMerch* pMerch);

	CMerch*     GetFirstMerch() { return m_pFirstMerch; }

	void		KLineCycleChange(UINT nID);

	void        ChangeDuoGuTongLieType(E_DuoGuTongLieType eType, int iVieRowCount, int iViewColCount);      // 切换多股同列的表现类型，并设置要显示的视图个数
private:

	void			RecalcLayout();

	void			ChangeColorFont();

	void			SetActiveIoView(int32 iIndex);
	void			SetActiveIoView(CIoViewBase *pIoViewActive);

	void			GetShowIoViews(OUT CArray<CIoViewBase *, CIoViewBase *> &aIoViews);

	void			DeleteSubIoViews();

	void			AddKlineIndex(const CString &StrIndexName);

	void			OnPageUpDown(int32 iPage);	// 正数上滚，负数下滚

	// 计算纯数字方向
	int32			GetUpRow(int32 iOldRow);
	int32			GetDownRow(int32 iOldRow);
	int32			GetLeftColumn(int32 iOldCol);
	int32			GetRightColumn(int32 iOldCol);

private:
	void AddTopButton(LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption);
	int TTopButtonHitTest(CPoint point);
	void DrawTopButton();
	void InitialTopBtns();

	void  CreateSubViews();
	void ResetView(int32 iBtnID);
	void ChangeKLineCycle(int32 iBtnID);
	int32 FindKLineCycle( int32 eTimeInterval );
	void RefreshIndex();

	
	CWnd*	FromPoint(CPoint pt);	//	查询鼠标在的图表视图
	void	DrawViewBorder(CDC* pDC, const CRect& rt, const COLORREF& clr);		//	绘制边框

private:

	CArray<CIoViewKLine *, CIoViewKLine *>	m_aIoViewKlines;		// 所有的k线视图
	CArray<CIoViewTrend *, CIoViewTrend *>	m_aIoViewTrends;		// 所有的分时视图

	int32			m_iActiveIoView;		// 当前激活的子视图索引
	UINT			m_nShowPicId;			// 当前显示哪种子视图

	CMerch			*m_pMerchBase;			// 基础商品

	E_DuoGuTongLieType  m_DGTLType;         // 多股同列的类型
	int32           m_iSubViewCount;        // 一屏显示的子视图个数
	int32           m_iSubViewRowCount;     // 一屏子视图行数
	int32           m_iSubViewColCount;     // 一屏子视图列数

	CRect			m_RectTopBar;           // 顶部用来显示分时、K线周期、显示的子视图个数
	std::map<int, CNCButton> m_mapTopBtn;			// 顶部工具栏左边公共按钮
	int32		    m_iTopBtnHovering;			// 标识鼠标进入按钮区域
	bool32			m_bTracking;	// 鼠标离开的标记
	UINT            m_uCurTimeInterval;        // 当前Ｋ线周期

	CMerch         *m_pFirstMerch;  

	CRect			m_rtSel;					//	图表选中边框位置
	COLORREF		m_clrSelBorder;				//	图表选中边框色
	////////////////////////////////////////////////////////////
	//
protected:

	/////////////////////////////////////////////////////
	/////////////////////////////////////////////////////
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewDuoGuTongLie)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	void	OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewDuoGuTongLie)
	afx_msg void OnPaint();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg	LRESULT OnMouseLeave(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnHotKeyMsg(WPARAM w, LPARAM l);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg	LRESULT	OnIdleUpdateCmdUI(WPARAM w, LPARAM l);

	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif //_IOVIEWDUOGUTONGLIE_H_