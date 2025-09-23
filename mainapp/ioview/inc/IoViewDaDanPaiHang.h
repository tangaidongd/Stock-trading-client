#ifndef _IOVIEW_DADANPAIHANG_H_
#define _IOVIEW_DADANPAIHANG_H_


#include "IoViewTimeSaleRank.h"
#include "IoViewTimeSaleStatistic.h"

#include "XScrollBar.h"
#include "GuiTabWnd.h"
// 大单排行ioview容器

/////////////////////////////////////////////////////////////////////////////
// CIoViewDaDanPaiHang

class CIoViewDaDanPaiHang : public CIoViewBase
{
// Construction
public:
	CIoViewDaDanPaiHang();
	virtual ~CIoViewDaDanPaiHang();

	DECLARE_DYNCREATE(CIoViewDaDanPaiHang)
	
	// from CControlBase
public:
	//virtual BOOL	TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	// from CIoViewBase
public:
	virtual void	RequestViewData(){};
	virtual void	SetChildFrameTitle();				// 需要能给IoViewManager识别，所以要实现xml
	virtual bool32	FromXml(TiXmlElement *pElement);
	virtual CString	ToXml();
	virtual	CString GetDefaultXML();

	virtual bool32	GetStdMenuEnable(MSG* pMsg) { return false; };
	
	virtual void	OnIoViewActive();					// 转接给激活的实际ioview
	virtual void	OnIoViewDeactive();

	virtual void	LockRedraw();						// 锁定激活的ioview
	virtual void	UnLockRedraw();

	virtual CMerch *GetMerchXml();						// 转到子ioview
	virtual E_IoViewType GetIoViewType() { return EIVT_MultiMerch; }

	virtual	CMerch *GetNextMerch(CMerch* pMerchNow, bool32 bPre);

	virtual void	OnEscBackFrameMerch(CMerch *pMerch);
	
public:
	// from CIoViewBase
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual void	OnVDataMerchTimeSalesUpdate(IN CMerch *pMerch);
	virtual void	OnVDataForceUpdate();
	
	virtual void	OnIoViewColorChanged();			
	virtual void	OnIoViewFontChanged();

	virtual	bool32	IsIoViewContainer() { return true; }		// 是IoView容器视图，下面还存在子视图
	virtual	CIoViewBase    *GetActiveIoView() { return GetCurTabIoView(); };

private:
	
	void			AddNCButton(LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption);
	int				TNCButtonHitTest(CPoint point);

	void			RecalcLayout();

	void			ChangeColorFont();

	void			SetCurTab(int32 iTab);
	void			TabChange();				// 切换响应

	CIoViewBase    *GetCurTabIoView();

	void			UapdateBtnRect();								// 更新按钮位置

	void			CreateBtnList();								// 可选周期按钮

	void			RedrawCycleBtn(CPaintDC *pPainDC);				// 重绘周期按钮

private:
	CXScrollBar		m_xsbHorzStatistic;		// 大单统计所需要的水平滚动条
	CXScrollBar		m_xsbHorzRank;			// 排行所需要的水平滚动条
	CGuiTabWnd		m_wndGuiTab;			// 大单统计，大单排行切换


	CNCButton						btnPrompt;
	std::map<int, CNCButton>	    m_mapBtnCycleList;					// 周期按钮列表	
	int32                           m_iCurrentPeriod;                   // 当前的周期，取值可以为1,3,5,10
	
	Image		*m_pImgBtn;
	
	CIoViewTimeSaleStatistic	m_wndStatistic;		// 大单统计
	CIoViewTimeSaleRank			m_wndRank;			// 大单排行
	
////////////////////////////////////////////////////////////
//
protected:
			
		

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewDaDanPaiHang)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewDaDanPaiHang)
	afx_msg void OnPaint();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSelChange(NMHDR *pHdr, LRESULT *pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

#endif //_IOVIEW_DADANPAIHANG_H_