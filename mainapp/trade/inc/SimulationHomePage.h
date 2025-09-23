#if !defined(AFX_SIMULATIONHOMEPAGE_H__104414B0_D41A_40A9_A3A2_9508C249481B__INCLUDED_)
#define AFX_SIMULATIONHOMEPAGE_H__104414B0_D41A_40A9_A3A2_9508C249481B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SimulationHomePage.h : header file
//

#include "BeautifulStatic.h"
#include "tradenginestruct.h"

/////////////////////////////////////////////////////////////////////////////
// CSimulationHomePage dialog

struct T_CommUserInfoList 
{
	CString pName1;
	CString pValue1;
	CString pName2;
	CString pValue2;
	CString pName3;
	CString pValue3;
	
	T_CommUserInfoList()
	{
		pName1 = "";
		pValue1 = "";
		pName2 = "";
		pValue2 = "";
		pName3 = "";
		pValue3 = "";
	}
};

class CSimulationHomePage : public CDialog
{
// Construction
public:
	CSimulationHomePage(CWnd* pParent = NULL);   // standard constructor
	inline int Create(CWnd *pParent) { return CDialog::Create(IDD, pParent); }
	void AdjustAllStaticSize();		// 调整所有静态文本框的大小
	void SetClientRespUserInfo(T_CommUserInfoList *pList);	// 设置信息

	T_TradeLoginInfo *m_pTradeLoginInfo;
private:
	void StaticCenterVer(CBeautifulStatic *pStatic);	// 使静态文本框垂直居中显示
// Dialog Data
	//{{AFX_DATA(CSimulationHomePage)
	enum { IDD = IDD_DIALOG_SIMULATION_HOMEPAGE };

	CBeautifulStatic	m_staticInitialAsset;
	CBeautifulStatic	m_staticInitialAssetValue;
	CBeautifulStatic	m_staticTotalAssets;
	CBeautifulStatic	m_staticTotalAssetsValue;
	CBeautifulStatic	m_staticProfitLossRate;
	CBeautifulStatic	m_staticProfitLossRateValue;

	CBeautifulStatic	m_staticHazardRate;
	CBeautifulStatic	m_staticHazardRateValue;
	CBeautifulStatic	m_staticAvailableDeposit;
	CBeautifulStatic	m_staticAvailableDepositVlaue;
	CBeautifulStatic	m_staticFreezeDeposit;
	CBeautifulStatic	m_staticFreezeDepositValue;

	CBeautifulStatic	m_staticYesterdayEarningRate;
	CBeautifulStatic	m_staticYesterdayEarningRateValue;
	CBeautifulStatic	m_staticTotalProfitLoss;
	CBeautifulStatic	m_staticTotalProfitLossValue;
	CBeautifulStatic	m_staticContinueProfitDays;
	CBeautifulStatic	m_staticContinueProfitDaysValue;

	CBeautifulStatic	m_staticThisWeekProfitRate;
	CBeautifulStatic	m_staticThisWeekProfitRateValue;
	CBeautifulStatic	m_staticOccupyDeposit;
	CBeautifulStatic	m_staticOccupyDepositValue;
	CBeautifulStatic	m_staticProfitBeyondTenDays;
	CBeautifulStatic	m_staticProfitBeyondTenDaysValue;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimulationHomePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSimulationHomePage)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMULATIONHOMEPAGE_H__104414B0_D41A_40A9_A3A2_9508C249481B__INCLUDED_)
