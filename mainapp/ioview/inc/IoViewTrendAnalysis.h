#ifndef _IO_VIEW_TREND_ANALYSIS_H_
#define _IO_VIEW_TREND_ANALYSIS_H_

#include "XScrollBar.h"
#include "IoViewBase.h"
#include "GridCtrlSys.h"
#include "CStaticSD.h"
#include "Meter.h"

//////////////////////////////////////////////////////////////////////////



enum E_ANALYSISI_TYPE				
{
	ANALYSISI_RED  = 0,	
	ANALYSISI_GREEN,
	ANALYSISI_YELLOEW,
	ANALYSISI_YELLOEW_TWO,
	ANALYSISI_COUNT,
};	
	


// �����г�����
typedef struct ST_SpecialMarket
{
	ST_SpecialMarket(const int _iMarketid = 0, 
		const CString _strMerchChName = _T(""),
		const CString _strMercCode = _T(""))
	{
		iMarketId		= _iMarketid;
		strMerchChName  = _strMerchChName;
		strMercCode		= _strMercCode;
	}

	int iMarketId;
	CString strMerchChName;
	CString strMercCode;

	bool operator==(const ST_SpecialMarket& st1) const
	{
		return (st1.iMarketId == iMarketId) && (strMercCode.CompareNoCase(st1.strMercCode) == 0) ;
	}
} ST_SpecialMarket;




/////////////////////////////////////////////////////////////////////////////
// CIoViewCapitalFlow

class CIoViewTrendAnalysis : public CIoViewBase
{
// Construction
public:
	CIoViewTrendAnalysis();
	virtual ~CIoViewTrendAnalysis();

	DECLARE_DYNCREATE(CIoViewTrendAnalysis)

	
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

	int				m_iTrendMarketId;
	CString			m_strTrendMerchCode;

	vector<ST_SpecialMarket> m_vSpecialMarket;


	Image*			m_pDrawImage;		// ���ջ���ͼƬ
// 	CString			m_strDisStrategy;	// ������ʾ�Ĳ���


	Image*			m_pImageXiongshi;
	Image*			m_pImageNiushi;
	Image*			m_pImageZhengDangshi;


	
	CRect			m_RectButton;		// һ�� ����
	CRect			m_RectChartRate;
	CRect			m_RectChartVol;
	CRect			m_RectChartAmount;

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
