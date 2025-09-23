#ifndef _IO_VIEW_TIMESALE_H_
#define _IO_VIEW_TIMESALE_H_

#include "XScrollBar.h"
#include "IoViewBase.h"
#include "GridCtrlSys.h"
#include "GridCtrlNormal.h"
#include "MerchManager.h"
#include "ReportScheme.h"


class CShowTick : public CTick
{
public:
	CShowTick();
	~CShowTick()	{}

public:
	const CShowTick& operator=(const CTick &Tick);
	void			Calculate();

public:
	CMsTime			m_TimePrev;					// ��һ�ʵ�ʱ��
	float			m_fHoldTotalPrev;			// ��һ�ʳֲ�
	float			m_fPricePre;				// ��һ�ʵļ۸�
	CString			m_StrProperty;				// ����
};

/////////////////////////////////////////////////////////////////////////////
// CIoViewTimeSale

class CIoViewTimeSale : public CIoViewBase
{
public:
	enum E_TimeSaleHeadType
	{
		ETHTSimple = 0,		// ��
		ETHTFuture,			// �ڻ�
		// 
		ETHTCount
	};

public:
	CIoViewTimeSale();
	virtual ~CIoViewTimeSale();

	DECLARE_DYNCREATE(CIoViewTimeSale)
	
	// from CControlBase
public:
	virtual BOOL	TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	// from CIoViewBase
public:
	virtual void	SetChildFrameTitle();
	virtual bool32	FromXml(TiXmlElement *pElement);
	virtual CString	ToXml();
	virtual	CString GetDefaultXML();

	virtual void	OnIoViewActive();
	virtual void	OnIoViewDeactive();
	virtual void	OnIoViewFontChanged();

	virtual void	LockRedraw();
	virtual void	UnLockRedraw();

	virtual void    DoShowStdPopupMenu();
	// from CIoViewBase
public:
	virtual void	RequestViewData();

	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual void	OnVDataMerchTimeSalesUpdate(IN CMerch *pMerch);
	virtual void	OnVDataMerchKLineUpdate(IN CMerch *pMerch);
	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerch);
	virtual void	OnVDataForceUpdate();

public:
	static E_TimeSaleHeadType GetMerchTimeSaleHeadType(E_ReportType eReportType);

	void			SetHistoryFlag(bool32 bHistory) { m_bHistoryTimeSale = bHistory; }
	void			SetHistoryTime(const CGmtTime &TimeAttend);

	bool32			GetCurrentPerfectWidth(OUT int32 &iWidth);

private:
	void			DeleteTableContent();
	void			SetTableData();
	void			SetRowValue(const CShowTick &Tick, int32 iRowIndex, int32 iColUintIndex, COLORREF clrRise, COLORREF clrFall, COLORREF clrKeep, COLORREF clrVolume);
	void			SetRowHeightAccordingFont();
	void			SetTimeGridHead(E_TimeSaleHeadType eTimeSaleHeadType);
	void			SetActiveCell();

	//	
	void			SetShowType(bool32 bShowFull);						// �ı���ʾ��ģʽ,������ʾ���������ʾ
	void			UpDateFullGridNewestData();							// �������ʾ��ʱ��,�������¼�
	void			CalcFullGridParams();								// ���������ʾʱ�����ز���
	bool32			BeValidFullGridIndex();								// �ж������Ƿ�Ϸ�

	void			RequestTimeSaleData(CMmiReqMerchTimeSales &req);
	void			OnPageScroll(int32 iScrollPage);					// ����ҳ�棬������ʾ�������ʵ��������� iScrollPage = INT_MAX ������ҳβ, INT_MIN ҳ�ף�+1 ��һҳ, -1 ��һҳ
	int32			CalcMaxVisibleTimeSaleCount();						// ���������ӵķֱʳɽ�������

	bool32			NoShowNewvol();	// �Ƿ���ʾ�����֡�
///////////////////////////////////////////////////////////////////////
//
protected:
	CXScrollBar				m_XSBVert;
	CXScrollBar				m_XSBHorz;
	CGridCtrlNormalSys		m_GridCtrl;
	CImageList				m_ImageList;	

private:
	int32					m_iSaveDec;
	CTrendTradingDayInfo	m_TrendTradingDayInfo;	// 

	// ��ʷ�ֱ���ʾ
	bool32					m_bHistoryTimeSale;
	CGmtTime				m_TimeHistory;

	CArray<CTick, CTick>	m_aTicksShow;
	E_TimeSaleHeadType		m_eTimeSaleHeadType;	

	// 
	bool32					m_bShowFull;			// �Ƿ�����󻯹�����ʾ (ֻҪ��ͼ��ȴ��� m_iPerfectWidth ����������ô��ʾ)
	bool32					m_bShowNewestFull;		// ��󻯵�ʱ��,�Ƿ���ʾ��������
	bool32					m_bRequestedTodayFirstData;	  // �Ѿ�������������µ����ݣ���UpdateMainTimeSale�и�ֵ
	CGridCtrlNormalSys		m_GridCtrlFull;			// ��󻯵ı��
	
	int32					m_iPerfectWidth;		// ���ʱ,һ��������ʾ���п�	
	int32					m_iGridFullRow;			// ���ʱ��ʾ������
	int32					m_iGridFullCol;			// ���ʱ��ʾ������
	int32					m_iGridFullColUints;	// ���ʱ��ʾ���ٸ���Ԫ��
	int32					m_iDataFullNums;		// ���ʱÿһҳ�����ݸ���
	
	int32					m_iFullBeginIndex;		// ���ʱ�����Ͻ�(��һ��)�����ݵ�����ֵ.
	int32					m_iFullEndIndex;		// ���ʱ�����Ͻ�(��һ��)�����ݵ�����ֵ.

	bool32					m_bSetTimer;			// �Ƿ������˶�ʱ��
	bool32					m_bMerchChange;			// ��Ʒ�л�
	CGmtTime				m_TimePre;

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewTimeSale)
	//}}AFX_VIRTUAL
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewTimeSale)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnExportData();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_CHART_H_