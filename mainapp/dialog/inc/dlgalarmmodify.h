#if !defined(_DLGALARMMODIFY_H_)
#define _DLGALARMMODIFY_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgalarmmodify.h : header file
//


#include "DialogEx.h"
#include "AlarmCenter.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgAlarmModify dialog

typedef enum _AlarmType
{
	EAT_MERCH,
	EAT_ARBITRAGE,
}E_AlarmType;

class CDlgAlarmModify : public CDialogEx
{
public:
	// 初始化时候设置
	void	SetAlarmConditions(const CAlarmConditions& stAlarmConditions);

	// 得到结果
	void	GetAlarmConditions(CAlarmConditions& stAlarmConditions);
	
private:
	
	//
	bool32	ValidParams(OUT CString& StrErr);

private:
	CAlarmConditions	m_stAlarmConditions;

	E_AlarmType         m_eAlarmType;

// Construction
public:
	CDlgAlarmModify(CWnd* pParent = NULL, E_AlarmType tType = EAT_MERCH);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgAlarmModify)
	enum { IDD = IDD_DIALOG_ALARM_MODIFY };
	BOOL	m_bTrend;
	float	m_fPriceDown;
	float	m_fPriceUp;	
	float   m_fRiseRate;
	float   m_fChangeRate;
	//}}AFX_DATA
 

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAlarmModify)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgAlarmModify)	
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickedAllSetting();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_DLGALARMMODIFY_H_)
