#if !defined(AFX_DLGARBITRAGE_H__)
#define AFX_DLGARBITRAGE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAlarm.h : header file
//

#include "ArbitrageManage.h"
#include "DialogEx.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgArbitrage dialog

class CDlgArbitrage : public CDialogEx
{
	typedef set<CMerch *> MerchSet;
	typedef map<int32, CArbitrage::ArbMerchArray > Index2MerchArrayMap;
public:
	// 设置初始化参数
	void		SetInitParams(bool32 bAdd, CArbitrage* pArbitrage);

private:

	// 设置控件
	void		InitialCtrls();

	// 显示控件
	void		ShowCtrlMerchC(bool32 bShow);

	// 获取商品
	bool32		GetMerch(int32 iIndex);

	// 获取计算公式
	bool32		GetFormular(int32 iIndex);

	// 生成当前的套利模型
	bool32		GeneralArbitrage();

private:
	
	// 是增加还是修改
	bool32		m_bAdd;

	// 当前的套利模型 
	CArbitrage	m_stArbitrage;

	// 备份的
	CArbitrage	m_stArbitrageBk;

// Construction
public:
	CDlgArbitrage(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgArbitrage)
	enum { IDD = IDD_DIALOG_ARBITRAGE };
	CComboBox	m_ComboPrice;
	CComboBox	m_ComboArbitrage;
	CString	m_StrFormularA;
	CString	m_StrFormularB;
	CString	m_StrFormularC;
	CString m_StrMerchA;
	CString m_StrMerchB;
	CString m_StrMerchC;
	UINT	m_uiSaveDec;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgArbitrage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgArbitrage)
	afx_msg void OnButtonChooseA();
	afx_msg void OnButtonChooseB();
	afx_msg void OnButtonChooseC();
	afx_msg void OnButtonChangeA();
	afx_msg void OnButtonChangeB();
	afx_msg void OnButtonChangeC();
	virtual void OnOK();
	afx_msg void OnSelchangeComboArbitrageType();
	afx_msg void OnChangeEditSavedec();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // AFX_DLGARBITRAGE_H__
