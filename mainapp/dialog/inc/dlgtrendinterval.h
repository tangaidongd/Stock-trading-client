#if !defined(_DLGTRENDINTERVAL_H_)
#define _DLGTRENDINTERVAL_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgtrendinterval.h : header file
//


#include "DialogEx.h"
#include "ViewDataListener.h"
#include "PieChartCtrl.h"
#include <map>

class CMerch;
class CKLine;
class CTick;
class CIoViewTrend;

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CDlgTrendInterval dialog

// �����õ�
typedef struct ListCmpData
{
	int32	m_iFlag;	// �������
	float	m_fPrice;	// �ɽ���
	float	m_fVolume;	// �ɽ���
	long	m_lTime;	// ����
	
}ListCmpData;

// ����ҳ��ÿһ�е�������
typedef struct T_TrendIntervalBS
{
	T_TrendIntervalBS()
	{
		m_fBuyVolumn = 0.;
		m_fSellVolumn= 0.;
	}

	//
	float		m_fBuyVolumn;
	float		m_fSellVolumn;

}T_TrendIntervalBS;

typedef struct T_TrendInterval
{
public:
	T_TrendInterval()
	{
		Clear();
	}

	void Clear()
	{
		m_fNeipan			= 0.;						
		m_fWaipan			= 0.;						
		
		m_iCountBig			= 0.;					
		m_fVolumnBig		= 0.;					
		
		m_iCountBigBuy      = 0.;					
		m_fVolumnBigBuy     = 0.;				
		
		m_iCountBigSell     = 0.;				
		m_fVolumnBigSell    = 0.;				
		
		m_iCountSmall       = 0.;					
		m_fVolumnSmall      = 0.;					
		
		m_iCountSmallBuy    = 0.;				
		m_fVolumnSmallBuy   = 0.;				
		
		m_iCountSmallSell   = 0.;				
		m_fVolumnSmallSell  = 0.;				
		
		m_iCountUnKnown     = 0.;				
		m_fVolumnUnKnown    = 0.;				
		
		m_fRateBigBuy       = 0.;					
		m_fRateSmallBuy     = 0.;				
		m_fRateBigSell      = 0.;					
		m_fRateSmallSell    = 0.;				
		m_fRateUnKnown      = 0.;					
		
		m_fMinPrice			= 0.;					
		m_fMaxPrice			= 0.;					
		
		m_fMaxBuyVolumn     = 0.;				
		m_fMaxSellVolumn    = 0.;				
		
		m_mapBuySell.clear(); 
	}

	// �����ֶ�
	float   m_fNeipan;						// ����
	float   m_fWaipan;						// ����
		
	int32	m_iCountBig;					// ����
	float	m_fVolumnBig;					// ����

	int32	m_iCountBigBuy;					// �����
	float   m_fVolumnBigBuy;				// �����
	
	int32	m_iCountBigSell;				// ������
	float   m_fVolumnBigSell;				// ������

	int32	m_iCountSmall;					// С����
	float	m_fVolumnSmall;					// С����
	
	int32	m_iCountSmallBuy;				// ��С����
	float   m_fVolumnSmallBuy;				// ��С����
	
	int32	m_iCountSmallSell;				// ��С����
	float   m_fVolumnSmallSell;				// ��С����

	int32	m_iCountUnKnown;				// δ֪�ɽ���
	float	m_fVolumnUnKnown;				// δ֪�ɽ���

	// ���� (��һҳ��״ͼ)
	float	m_fRateBigBuy;					// ����󵥱���
	float	m_fRateSmallBuy;				// ����С������
	float	m_fRateBigSell;					// �����󵥱���
	float	m_fRateSmallSell;				// ����С������
	float	m_fRateUnKnown;					// δ֪�ɽ�����

	// ����ҳ��ʾ������
	float   m_fMinPrice;					// ��ͼ�
	float	m_fMaxPrice;					// ��߼�

	float	m_fMaxBuyVolumn;				// �������
	float	m_fMaxSellVolumn;				// �������

	map<float, T_TrendIntervalBS>	m_mapBuySell; // ÿһ���۸��Ӧ��������

}T_TrendInterval;

class CDlgTrendInterval : public CDialogEx , public CViewDataListner
{
	// From CViewDataListner
public: 
	// �ֱ�
	void OnDataRespMerchTimeSales(int iMmiReqId, IN const CMmiRespMerchTimeSales *pMmiRespMerchTimeSales);

public:

	// ���ó�ʼ���Ĳ���
	void SetKLineData(CIoViewTrend* pIoViewTrend, CMerch* pMerch, float fPreColse, const CArray<CKLine, CKLine>& aKLineSrc) 
	{
		if ( NULL == pMerch || NULL == pIoViewTrend )
		{
			return;
		}
	
		m_pIoViewTrend = pIoViewTrend;
		m_pMerch = pMerch;
		m_fPricePreClose = fPreColse;
		m_aKLineSrc.Copy(aKLineSrc);
	}

	// ������ʾ
	void	UpdateShow(bool32 bForceUpdate = false);
		
	// ��һҳ
	void	ShowPage1();

 	// �ڶ�ҳ
	void	ShowPage2(bool32 bForceUpdate = false);
	
	// ����ҳ
	void	ShowPage3();

	// �����ݹ�����, ����������
	void	RequestData();

	// �Ƿ��ǵ��ڿ���ʱ��
	bool32	BeSpecialBegin(IN const CGmtTime& Time, OUT CGmtTime& TimeBegin);

	// �Ƿ��������ʱ��
	bool32	BeSpecialEnd(IN const CGmtTime& Time, OUT CGmtTime& TimeEnd);

	// ȡTICK ����
	void	UpdateLoaclData();

	// ͳ������
	void	CalcStatisticData();

	bool32	IsRectInRgn(const CRgn &rgn, const CRect &rc);

	void	RedrawAsync();

	// ��ȡ����ֵ
	void	FromXml();

	// �������ֵ
	void	ToXml();

	// 
	bool32	CreateDefaultXml();

	// ����
	void	Sort(int32 iCol, bool32 bAscend);

	// ����ص�����
	static int	CALLBACK CompareFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamData);

private:
	
	// Tick �����Ƿ��㹻
	bool32		m_bTickEnough;

	// ������ͼ
	CIoViewTrend* m_pIoViewTrend;

	// ��Ʒ
	CMerch*		m_pMerch;

	// ��һ��K �ߵ����ռ�
	float		m_fPricePreClose;

	// ����� K ��
	CArray<CKLine, CKLine> m_aKLineSrc;

	// �����㹻��ʱ��, �ͱ��汾��, �Ժ�ļ�����ʾ���ñ��ص�, ����Ͳ�ͣ�Ĵ���Ʒ������ȡ, ���ܷ�ȡ�����¸��������, ���ص�ʼ��Ϊ��
	CArray<CTick, CTick>   m_aTicks;

	// ������
	int32		m_iLimit;

	// ͳ����Ϣ
	T_TrendInterval m_stResult;

	// �յ���tick����ʱ���
	CTick		m_TickStart;
	CTick		m_TickEnd;
	bool32		m_bTryReqTick;	// ��������tick���ݵı�־

	// ���
	CPieChartCtrl m_Pie;

	// �������
	int32		m_iSortColumn;
	bool32		m_bAscend; 

// Construction
public:
	CDlgTrendInterval(CWnd* pParent = NULL);   // standard constructor
	~CDlgTrendInterval();
// Dialog Data
	//{{AFX_DATA(CDlgTrendInterval)
	enum { IDD = IDD_DIALOG_INTERVAL_TREDN };
	CListCtrl	m_ListTick;
	CString	m_StrV1;
	CString	m_StrV2;
	CString	m_StrV3;
	CString	m_StrV4;
	CString	m_StrV5;
	CString	m_StrAvgVolumn;
	CString	m_StrBuy;
	float	m_fClose;
	int		m_iCountMin;
	int		m_iCountTick;
	float	m_fHigh;
	float	m_fLow;
	CString	m_StrNei;
	float	m_fOpen;
	CString	m_StrRiseRate;
	CString	m_StrSell;
	CString m_StrBigAll;
	CString	m_StrShakeRate;
	CString	m_StrTimeBegin;
	CString	m_StrTimeEnd;
	CString	m_StrVolumn;
	CString	m_StrWai;
	int		m_iEditLimit;
	float	m_fAvgPrice;
	int		m_iRadio;
	CImageList m_ImageList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTrendInterval)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTrendInterval)
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnRadio3();
	afx_msg void OnLimitChange();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnColumnclickListTick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(_DLGTRENDINTERVAL_H_)
