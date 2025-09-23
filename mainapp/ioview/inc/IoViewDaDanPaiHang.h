#ifndef _IOVIEW_DADANPAIHANG_H_
#define _IOVIEW_DADANPAIHANG_H_


#include "IoViewTimeSaleRank.h"
#include "IoViewTimeSaleStatistic.h"

#include "XScrollBar.h"
#include "GuiTabWnd.h"
// ������ioview����

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
	virtual void	SetChildFrameTitle();				// ��Ҫ�ܸ�IoViewManagerʶ������Ҫʵ��xml
	virtual bool32	FromXml(TiXmlElement *pElement);
	virtual CString	ToXml();
	virtual	CString GetDefaultXML();

	virtual bool32	GetStdMenuEnable(MSG* pMsg) { return false; };
	
	virtual void	OnIoViewActive();					// ת�Ӹ������ʵ��ioview
	virtual void	OnIoViewDeactive();

	virtual void	LockRedraw();						// ���������ioview
	virtual void	UnLockRedraw();

	virtual CMerch *GetMerchXml();						// ת����ioview
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

	virtual	bool32	IsIoViewContainer() { return true; }		// ��IoView������ͼ�����滹��������ͼ
	virtual	CIoViewBase    *GetActiveIoView() { return GetCurTabIoView(); };

private:
	
	void			AddNCButton(LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption);
	int				TNCButtonHitTest(CPoint point);

	void			RecalcLayout();

	void			ChangeColorFont();

	void			SetCurTab(int32 iTab);
	void			TabChange();				// �л���Ӧ

	CIoViewBase    *GetCurTabIoView();

	void			UapdateBtnRect();								// ���°�ťλ��

	void			CreateBtnList();								// ��ѡ���ڰ�ť

	void			RedrawCycleBtn(CPaintDC *pPainDC);				// �ػ����ڰ�ť

private:
	CXScrollBar		m_xsbHorzStatistic;		// ��ͳ������Ҫ��ˮƽ������
	CXScrollBar		m_xsbHorzRank;			// ��������Ҫ��ˮƽ������
	CGuiTabWnd		m_wndGuiTab;			// ��ͳ�ƣ��������л�


	CNCButton						btnPrompt;
	std::map<int, CNCButton>	    m_mapBtnCycleList;					// ���ڰ�ť�б�	
	int32                           m_iCurrentPeriod;                   // ��ǰ�����ڣ�ȡֵ����Ϊ1,3,5,10
	
	Image		*m_pImgBtn;
	
	CIoViewTimeSaleStatistic	m_wndStatistic;		// ��ͳ��
	CIoViewTimeSaleRank			m_wndRank;			// ������
	
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