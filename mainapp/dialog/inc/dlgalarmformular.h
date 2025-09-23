#if !defined(_DLGALARMFORMULAR_H_)
#define _DLGALARMFORMULAR_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgalarmformular.h : header file
//


#include "DialogEx.h"
#include "CFormularContent.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgAlarmFormular dialog

class CDlgAlarmFormular : public CDialogEx
{
public:
	// 条件选股公式组合框
    void			FillComBoxFormular();

	// 设置参数
	void			SetRadioParam(int32 iIndex);

	// 改变参数后, 修改指标公式
	void			ModifyFormular(int32 iIndexParam);	

	// 得到最后的结果
	CFormularContent* GetFormular() const { return m_pFomularNow; }

	// 设置初始化时候的公式
	void			SetInitialFormular(CFormularContent* pContent) { m_pFomularNow = pContent; }

private:
	// 当前的公式:
	CFormularContent*		m_pFomularNow;

	// Construction
public:
	CDlgAlarmFormular(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgAlarmFormular)
	enum { IDD = IDD_DIALOG_ALARM_FORMULAR };
	CComboBox	m_ComboFormular;
	CString	m_StrNameP1;
	CString	m_StrNameP2;
	CString	m_StrNameP3;
	CString	m_StrNameP4;
	float	m_fP1;
	float	m_fP2;
	float	m_fP3;
	float	m_fP4;
	CString	m_StrRangeP1;
	CString	m_StrRangeP2;
	CString	m_StrRangeP3;
	CString	m_StrRangeP4;
	int		m_iRadio;

	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAlarmFormular)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgAlarmFormular)
	afx_msg void OnRadioP1();
	afx_msg void OnRadioP2();
	afx_msg void OnRadioP3();
	afx_msg void OnRadioP4();
	afx_msg void OnSelchangeCombo1();
	virtual void OnOK();
	afx_msg void OnChangeEditParam();
	afx_msg void OnChangeEditParam2();
	afx_msg void OnChangeEditParam4();
	afx_msg void OnChangeEditParam3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_DLGALARMFORMULAR_H_)
