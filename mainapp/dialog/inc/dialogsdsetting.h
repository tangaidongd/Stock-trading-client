#if !defined(AFX_DIALOGSDSETTING_H__79961DA9_BDFE_4471_BF17_50ED3D08393C__INCLUDED_)
#define AFX_DIALOGSDSETTING_H__79961DA9_BDFE_4471_BF17_50ED3D08393C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dialogsdsetting.h : header file
//
#include "DialogEx.h"
/////////////////////////////////////////////////////////////////////////////
// CDialogSDSetting dialog
class CNodeSequence;
class CDialogSDSetting : public CDialogEx
{
// Construction
public:
	CDialogSDSetting(CSelfDrawNode*& pNode,T_MerchNodeUserData* pData,double fYMin,double fYMax,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogSDSetting)
	enum { IDD = IDD_DIALOG_SD_SETTING };
	CComboBox	m_list3;
	CComboBox	m_list2;
	CComboBox	m_list1;
	double	m_val1;
	double	m_val2;
	double	m_val3;
	//}}AFX_DATA

	int32 m_iSel1;
	int32 m_iSel2;
	int32 m_iSel3;

	bool32 m_bStart;
	bool32 m_bEnd;
	bool32 m_bAdd;

	CSelfDrawNode* m_pNode;
	T_MerchNodeUserData* m_pData;
	double m_fYMin;
	double m_fYMax;

	int32 FillList (CComboBox* pCombo,int32 iSelVal );
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogSDSetting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogSDSetting)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCombo1();
	afx_msg void OnSelchangeCombo3();
	afx_msg void OnSelchangeCombo2();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGSDSETTING_H__79961DA9_BDFE_4471_BF17_50ED3D08393C__INCLUDED_)
