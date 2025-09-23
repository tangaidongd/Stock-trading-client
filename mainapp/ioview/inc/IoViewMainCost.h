#ifndef _IO_VIEW_MAIN_COST_H_
#define _IO_VIEW_MAIN_COST_H_

#include "XScrollBar.h"
#include "IoViewBase.h"
#include "GridCtrlSys.h"
#include "CStaticSD.h"

//////////////////////////////////////////////////////////////////////////

#define  EQUIVALENT_DATA_NUM	8.0	// �Ȳ����ݸ���
typedef struct tagMainCostData
{
	float  fPriceNow;			// ��Ʊ�ּ�
	float  fUlShortTermCost;	// �����ڳɱ�
	float  fShortTermCost;		// ���ڳɱ�
	float  fMediumermCost;		// ���ڳɱ�
	float  fLongTermCost;		// ���ڳɱ�

	tagMainCostData()
	{
		fPriceNow		 = 0;
		fUlShortTermCost = 0;
		fShortTermCost   = 0;
		fMediumermCost   = 0;
		fLongTermCost    = 0;
	};
} ST_MAIN_COST_DATA;

enum E_UserBlock_SoftType			// �ĸ���Բ��λ��
{
	BIG_ULSHORT_TERM = 0,			// ������
	BIG_SHORT_TERM,					// ����
	BIG_MEDIUMERM_TERM,				// ����
	BIG_LONG_TERM,					// ����
	BIG_TERM_COUNT
};

/////////////////////////////////////////////////////////////////////////////
// CIoViewCapitalFlow

class CIoViewMainCost : public CIoViewBase
{
// Construction
public:
	CIoViewMainCost();
	virtual ~CIoViewMainCost();

	DECLARE_DYNCREATE(CIoViewMainCost)

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
	void			GetMaxGapAndPriceLow(const ST_MAIN_COST_DATA& stMainCostData, float &fMaxGap, float &fPriceLow);
	float			GetHeightPercent(float fPriceLow, float fCalPrice, float fMaxGap);
	void			DrawBigRound(CDC &dc,COLORREF clrHighValue, COLORREF clrLowValue);
	void			DrawSmallRound(CDC &dc,COLORREF clrHighValue, COLORREF clrLowValue);
	void			DrawLineAndPriceNow(CDC &dc,COLORREF clrHighValue, COLORREF clrLowValue);
	void			DrawMainText(CDC &dc,COLORREF clrHighValue, COLORREF clrLowValue);

	void			SetSize();

	int32			m_iDataServiceType;

	ST_MAIN_COST_DATA  m_stMainCostData;

	CRect			m_RectTitle;		// ����
	CRect			m_RectPriceNow;			   // ��Ʊ�ּ�
	CRect			m_RectLine;				   // ����
	CRect			m_RectMainText;			   // ��ʾ�۸�����
	CRect			m_RectMainRound;		   // ��Բ������

	CPoint			m_PointUlShortTermCost;	   // �����ڳɱ�
	CPoint			m_PointShortTermCost;	   // ���ڳɱ�
	CPoint			m_PointMediumermCost;	   // ���ڳɱ�
	CPoint			m_PointLongTermCost;		   // ���ڳɱ�
	CPoint			m_aPointTerm[BIG_TERM_COUNT];
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
