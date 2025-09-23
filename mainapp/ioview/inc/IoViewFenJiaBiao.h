#ifndef _IOVIEW_FENJIABIAO_H_
#define _IOVIEW_FENJIABIAO_H_

#include "XScrollBar.h"
#include "IoViewBase.h"
#include "GridCtrlNormal.h"
#include "MerchManager.h"

/////////////////////////////////////////////////////////////////////////////
// CIoViewFenJiaBiao
class CIoViewFenJiaBiao : public CIoViewBase
{
public:
	CIoViewFenJiaBiao();
	virtual ~CIoViewFenJiaBiao();

	DECLARE_DYNCREATE(CIoViewFenJiaBiao)
	
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
	virtual void	OnIoViewColorChanged();
	virtual void	OnIoViewFontChanged();

	virtual void	LockRedraw();
	virtual void	UnLockRedraw();
	// from CIoViewBase
public:
	virtual void	RequestViewData();

	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual void	OnVDataMerchTimeSalesUpdate(IN CMerch *pMerch);
	virtual void	OnVDataMerchKLineUpdate(IN CMerch *pMerch);
	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerch);
	virtual void	OnVDataForceUpdate();

public:
	void			SetHistoryFlag(bool32 bHistory) { m_bHistoryTimeSale = bHistory; }
	void			SetHistoryTime(const CGmtTime &TimeAttend);

	bool32			GetCurrentPerfectWidth(OUT int32 &iWidth);

private:
	void			RecalcLayout();

	int32			RecalcTableRowColumn();	// �ؼ���������, ���ض��������(���4������)

	void			DeleteTableContent(bool32 bDelRealData = false);			// �������
	void			ClearShowData();	// �������ʾ����
	void			CalcTableData();				// �ؼ������е�����
	
	void			UpdateTableTickData(const CTick &tick);	// ���¸�tick����

	bool32			IsInMultiColumnMode();		// ��ǰ�Ƿ��ڶ���ģʽ

	//	
	void			RequestTimeSaleData(CMmiReqMerchTimeSales &req);

	bool32			DoShowFullSreen();
///////////////////////////////////////////////////////////////////////
//
protected:
	CXScrollBar				m_XSBVert;
	CGridCtrlNormalSys		m_GridCtrl;	// ����

	CRect					m_RectTitle;	// ����״̬�»��Ʊ���

private:
	CTrendTradingDayInfo	m_TrendTradingDayInfo;	// ��Ҫ���ռ�������

	// ��ʷ�ּ۱���ʾ??
	bool32					m_bHistoryTimeSale;
	CGmtTime				m_TimeHistory;

	CArray<CTick, CTick>	m_aTicksShow;		// ԭʼ����

	struct T_TickStatistic{
		float m_fPrice;			// �۸�
		float m_fVolTotal;		// �ü۸��Ӧ �ɽ����� - ��Ʊ���ڻ���Ʒ
		float m_fAmountTotal;	// �ü۸��Ӧ �ɽ����� - ָ����ʾ
		float m_fVolBuy;		// �ɽ��� ����			- �������������ָ������ʾ
		float m_fVolSell;		// �ɽ��� ����
		float m_fAmountBuy;		// �����
		float m_fAmountSell;	// ������

		E_SysColor	m_eColor;	// ���ӵ���ɫ���ݣ�����ÿ���ж�
	};

	CArray<T_TickStatistic, const T_TickStatistic &>	m_aPricesSort;			// �۸�������ʾ����
	float					m_fVolTotal;			// �ܳɽ���
	float					m_fAmountTotal;			// �ܳɽ���
	float					m_fVolMax;				// �ɽ����ֵ
	float					m_fAmountMax;			// �ɽ������ֵ

	int32					m_iPreferWidth;			// һ��������(4��)����Ҫ�Ĵ�С
	int32					m_iDecSave;				// �����С����
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewFenJiaBiao)
	//}}AFX_VIRTUAL
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewFenJiaBiao)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGridGetDispInfo(NMHDR *pNotifyStruct, LRESULT *pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif //_IOVIEW_FENJIABIAO_H_