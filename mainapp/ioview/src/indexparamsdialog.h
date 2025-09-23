#if !defined(AFX_INDEXPARAMSDIALOG_H__31EF6C4A_F158_478F_8CB5_A05F660BF1CE__INCLUDED_)
#define AFX_INDEXPARAMSDIALOG_H__31EF6C4A_F158_478F_8CB5_A05F660BF1CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// indexparamsdialog.h : header file
//
#include "FloatEdit.h"
#include "CFormularContent.h"
#include "DialogEx.h"
/////////////////////////////////////////////////////////////////////////////
// CIndexParamsDialog dialog

class CIndexParamsDialog : public CDialogEx
{
// Construction
public:
	// CIndexParamsDialog(CFormularContent* p,CWnd* pParent = NULL);   // standard constructor

	// fangz0422 如果仅仅传递这个指标的指针进来,那么修改得不到保存,要得到CFormulaLib::instance() 里的指针
	
	// pRet2 是为了设为缺省时用的. pRet1 仅对这个指标临时修改
	CIndexParamsDialog(CFormularContent * pRet2 , CFormularContent* pRet1,CWnd* pParent = NULL);   // standard constructor

	~CIndexParamsDialog();
// Dialog Data
	//{{AFX_DATA(CIndexParamsDialog)
	enum { IDD = IDD_DIALOG_INDEX_PARAM };
		CFloatEdit	m_floatEdit11;
		CFloatEdit	m_floatEdit12;
		CFloatEdit	m_floatEdit13;
		CFloatEdit	m_floatEdit14;
		CFloatEdit	m_floatEdit15;
		CFloatEdit	m_floatEdit21;
		CFloatEdit	m_floatEdit22;
		CFloatEdit	m_floatEdit23;
		CFloatEdit	m_floatEdit24;
		CFloatEdit	m_floatEdit25;
		CFloatEdit	m_floatEdit31;
		CFloatEdit	m_floatEdit32;
		CFloatEdit	m_floatEdit33;
		CFloatEdit	m_floatEdit34;
		CFloatEdit	m_floatEdit35;
		CFloatEdit	m_floatEdit41;
		CFloatEdit	m_floatEdit42;
		CFloatEdit	m_floatEdit43;
		CFloatEdit	m_floatEdit44;
		CFloatEdit	m_floatEdit45;
		CTabCtrl	m_tabParam;

		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

public:
	int32	GetRet();
	CFormularContent*  m_pContentRet1;
	CFormularContent*  m_pContentRet2;
private:
	int32	m_nEID[4][5];
	CIndexParamData* pIndexParam;

	int32	m_iLastSel;
	bool32	m_bInit;
	int32	m_iRet;

private:
	void	GetStringParam(int x, int y, CString& s);
	void	SetStringParam(int x, int y, CString sIn);
	void	ShowData(CString &str, float fdata);
	float	StringToFloat(CString str);
	void	UpdateSel();

	void	UpdateParam();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIndexParamsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIndexParamsDialog)
		// NOTE: the ClassWizard will add member functions here
	afx_msg void OnSaveasDefault();
	afx_msg void OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INDEXPARAMSDIALOG_H__31EF6C4A_F158_478F_8CB5_A05F660BF1CE__INCLUDED_)
