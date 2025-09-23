#ifndef _IO_VIEW_DK_MONEY_H_
#define _IO_VIEW_DK_MONEY_H_

#include "XScrollBar.h"
#include "IoViewBase.h"
#include "GridCtrlSys.h"
#include "CStaticSD.h"

//////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CIoViewCapitalFlow

class CIoViewDKMoney : public CIoViewBase
{
// Construction
public:
	CIoViewDKMoney();
	virtual ~CIoViewDKMoney();

	DECLARE_DYNCREATE(CIoViewDKMoney)

	
	// from CControlBase
public:
	//virtual BOOL	TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	// from CIoViewBase
public:
	virtual void	RequestViewData();
	virtual void	RequestData(){};
	virtual void	SetChildFrameTitle();
	virtual bool32	FromXml(TiXmlElement *pElement);
	virtual CString	ToXml();
	virtual	CString GetDefaultXML();
	
	virtual void	OnIoViewActive();
	virtual void	OnIoViewDeactive();

	virtual void	LockRedraw();
	virtual void	UnLockRedraw();
	// from CIoViewBase
public:
	
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual void	OnVDataMerchKLineUpdate(IN CMerch *pMerch);
	virtual void	OnVDataMerchTimeSalesUpdate(IN CMerch *pMerch);
	virtual void	OnVDataForceUpdate();

	virtual bool32	IsAttendData(IN CMerch *pMerch, E_DataServiceType eDataServiceType);
	

	virtual void	OnIoViewColorChanged();			
	virtual void	OnIoViewFontChanged();


	virtual	bool32	IsPluginDataView() { return true; }
	virtual void    OnVDataPluginResp(const CMmiCommBase *pResp);

	void			OnDKMoneyResp( const CMmiRespDKMoney *pResp );
	void			OnTickExHistoryResp(const CMmiRespHistoryTickEx *pResp);
private:


private:
	void			DrawPillar(CDC &dc, const CRect &rcItem, float fValue, float fPercentage, CRect rcPercentPos,COLORREF clrStart, COLORREF clrEnd);					//������
	void			DrawBottomText(CDC &dc, const CRect &rcItem, float fRise, float fFall, COLORREF clrRise, COLORREF clrFall);	// ���ײ�����

	void			MemsetDKMoneyData();

	CRect			m_RectWindows;
	int32			m_iDataServiceType;

	CRect			m_RectTitle;		// ����

	CRect			m_RectText;			// �²���������ʾ
	CRect			m_RectRedPillar;	// ������
	CRect			m_RectGreenPillar;	// ������
	CRect			m_rcPercentPosRed;	// �����Ӱٷֱ�
	CRect			m_rcPercentPosGreen;	// �����Ӱٷֱ�
	CRect			m_RectImage;		// �Ƽ�����

	
	CRect			m_RectButton;		// һ�� ����
	CRect			m_RectChartRate;
	CRect			m_RectChartVol;
	CRect			m_RectChartAmount;

	CMmiRespDKMoney  m_RespDKMoney;


	CFont			m_Font14;			// 14�߶�����
	CFont			m_Font12;			// 12�߶�����




	
////////////////////////////////////////////////////////////
//
protected:
		

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewCapitalFlow)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewCapitalFlow)
	afx_msg void OnPaint();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_CAPITALFLOW_H_
