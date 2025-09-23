#ifndef _IO_VIEW_SUGGEST_POSITION_H_
#define _IO_VIEW_SUGGEST_POSITION_H_

#include "XScrollBar.h"
#include "IoViewBase.h"
#include "GridCtrlSys.h"
#include "CStaticSD.h"
#include "Meter.h"

//////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CIoViewCapitalFlow

class CIoViewSuggestPosition : public CIoViewBase
{
// Construction
public:
	CIoViewSuggestPosition();
	virtual ~CIoViewSuggestPosition();

	DECLARE_DYNCREATE(CIoViewSuggestPosition)

	
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

	void			OnTickExResp(const CMmiRespTickEx *pResp);		// �ֱ�ͳ�ƻذ�
	void			OnTickExHistoryResp(const CMmiRespHistoryTickEx *pResp);
private:
	void			SetSize();


private:
	CMeter			m_Meter;
	CRect			m_RectWindows;
	int32			m_iDataServiceType;

	CRect			m_RectTitle;		// ����
	CRect			m_RectStrategyTitle;// ���Ա���
	CRect			m_RectStrategy;		// �Ƽ�����
	CRect			m_RectImage;		// �Ƽ�����



	Image*			m_pDrawImage;				// ���ջ���ͼƬ
	Image*			m_pImagePercentEight;		// 80
	Image*			m_pImagePercentFive;		// 50
	Image*			m_pImagePercentThree;		// 30

	CRect			m_RectButton;		// һ�� ����
	CRect			m_RectChartRate;
	CRect			m_RectChartVol;
	CRect			m_RectChartAmount;
	CFont			m_Font14;			// 14�߶�����




	
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
