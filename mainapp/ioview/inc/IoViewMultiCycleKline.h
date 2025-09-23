#ifndef _IOVIEW_MULTI_CYCLE_KLINE_H_
#define _IOVIEW_MULTI_CYCLE_KLINE_H_

//设置显示多个IoViewReport，右侧边缘添加可缩放至全屏的按钮

#include "IoViewBase.h"


class CIoViewKLine;

class CIoViewMultiCycleKline : public CIoViewBase
{

    //Construction
public:
    CIoViewMultiCycleKline();
    virtual ~CIoViewMultiCycleKline();

    DECLARE_DYNCREATE(CIoViewMultiCycleKline)

public:
    virtual void  OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
    virtual void  SetChildFrameTitle() {}                     // 需要能给IoViewManager识别，所以要实现xml
    virtual bool32  FromXml(TiXmlElement *pElement);
    virtual CString  ToXml();
    virtual	CString  GetDefaultXML() { return L""; }
    virtual bool32	GetStdMenuEnable(MSG* pMsg);
    virtual CMerch *GetMerchXml();						// 转到子ioview
	virtual void	OnIoViewActive();					// 转接给激活的实际ioview
	virtual void	OnIoViewDeactive();

	CWnd*	FromPoint(CPoint pt);	//	查询鼠标在的图表视图
	void	DrawViewBorder(CDC* pDC, const CRect& rt, const COLORREF& clr);		//	绘制边框

private:

	CArray<CIoViewKLine *, CIoViewKLine *> 	m_aSubIoViews;

	CRect	    m_RectTopBar;

	std::map<int, CNCButton> m_mapTopBtn;			// 顶部工具栏左边公共按钮
	//int32		m_iTopBtnHovering;					// 标识鼠标进入按钮区域
	bool32	    m_bTracking;						// 鼠标离开的标记

	// 目前需求上子视图为四个或者六个，都按两行来排列
	int32       m_iSubRowCount;					    // 子视图行数
	int32       m_iSubColumnCount;					// 子视图列数

	int32		m_iActiveIoView;		// 当前激活的子视图索引

	CRect			m_rtSel;					//	图表选中边框位置
	COLORREF		m_clrSelBorder;				//	图表选中边框色

private:
    void RecalcLayout();
    void CreateSubView();
	void DeleteSubIoViews();
	void AddTopButton(LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption);
	int TTopButtonHitTest(CPoint point);
	void DrawTopButton();
	void InitialTopBtns();
	void SetActiveIoView(int32 iIndex);
	void RefreshButtonStatus();
	CIoViewBase * GetActiveIoView();

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
    afx_msg void OnPaint();
    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);	// 菜单
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg	LRESULT OnMouseLeave(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnHotKeyMsg(WPARAM w, LPARAM l);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT nIDEvent);
    DECLARE_MESSAGE_MAP()
};

#endif