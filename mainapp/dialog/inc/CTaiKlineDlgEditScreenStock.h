#if !defined(AFX_TECHDLGEDITSCREENSTOCK_H__10379B43_6173_11D4_970B_0080C8D6450E__INCLUDED_)
#define AFX_TECHDLGEDITSCREENSTOCK_H__10379B43_6173_11D4_970B_0080C8D6450E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CTaiKlineDlgEditScreenStock.h : header file
// 模块编号：005

// 文件名：CTaiKlineDlgEditScreenStock.h

// 创建者：

// 创建时间：2000.8.3

// 内容描述：

//		编辑选股指标公式、k线组合指标公式。
//

/////////////////////////////////////////////////////////////////////////////
// CTaiKlineDlgEditScreenStock dialog
#include "FloatEdit.h"
#include "AutoRichEditCtrl.h"
#include "RichEditFormular.h"
#include "DialogEx.h"
//#include "TabCtrlEx.h"

class CFormulaLib;
class CTaiKlineDlgEditScreenStock : public CDialogEx
{
// Construction

	CFormulaLib* m_pLib;
	CFormularContent* pClone;
public:
	BOOL m_bFormulaDataChanged;
	BOOL m_bParamChanged;
public:
	CTaiKlineDlgEditScreenStock(CFormulaLib* pLib,CFormularContent* p,CWnd* pParent = NULL);   // standard constructor
	virtual ~CTaiKlineDlgEditScreenStock();

	BOOL	FindIndicator(CString sName);
	void	SetStringParam(int x,int y,CString sIn);
	CString	GetStringParam(int x,int y);
	void	ParamIntoOther(int nFlag);
	BOOL	CheckParmNameInput(CString parname,CString & errmessage);
	BOOL	CheckParmInput(CString parminput);
	float	StringToFloat(CString str);
	void	ShowData(CString &str,float fdata);
	void	DoChangeInput();
	void	TestFormula( BOOL bAlert);

	CFormularContent &GetResultContent() { return *m_pResultContent; }

	int		m_nTabPre;
	BOOL	m_bInit;
	int		m_nCID[4][5];
	int		m_nLine[7];
	int     m_nExtraY[4];

	int     errorposition;//错误的位置
	CString errormessage;//错误信息
	CString functionstr;//插入的函数串

	CFormularContent*  m_pResultContent;
	CString help;
	int numPara;                          //参数个数
	CString	m_editParam[PARAM_NUM][5];//参数信息					// huhe* 加到16个参数设置

	CString	m_StrLastSetColorFormula;

// Dialog Data
	//{{AFX_DATA(CTaiKlineDlgEditScreenStock)
	enum { IDD = IDD_EDIT_TJXG_K };
	CButton	m_ok;
	CButton	m_cancel;
	CButton	m_dyhs;
	CButton	m_csgs;
	CButton	m_canused;	
	CFloatEdit	m_floatEditLine1;
	CFloatEdit	m_floatEditLine2;
	CFloatEdit	m_floatEditLine3;
	CFloatEdit	m_floatEditLine4;
	CFloatEdit	m_floatEditLine5;
	CFloatEdit	m_floatEditLine6;
	CFloatEdit	m_floatEditLine7;
	CFloatEdit	m_floatEdit12;
	CFloatEdit	m_floatEdit13;
	CFloatEdit	m_floatEdit14;
	CEdit	m_floatEdit15;
	CFloatEdit	m_floatEdit22;
	CFloatEdit	m_floatEdit23;
	CFloatEdit	m_floatEdit24;
	CEdit	m_floatEdit25;
	CFloatEdit	m_floatEdit32;
	CFloatEdit	m_floatEdit33;
	CFloatEdit	m_floatEdit34;
	CEdit	m_floatEdit35;
	CFloatEdit	m_floatEdit42;
	CFloatEdit	m_floatEdit43;
	CFloatEdit	m_floatEdit44;
	CEdit	m_floatEdit45;
	CTabCtrl	m_tabParam;
	CEdit	m_namecor;
	CEdit	m_passwordcor;
	CRichEditFormular	m_input_content;
	BOOL	m_checkPassword;
	BOOL	m_checkOften;
	CString	m_explainbrief;
	CString	m_help;
	CString	m_name;
	CString	m_password;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTaiKlineDlgEditScreenStock)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTaiKlineDlgEditScreenStock)
	afx_msg void OnTestFormula();		//测试公式
	afx_msg void OnAddFunc();			//插入函数
	afx_msg void OnRestore();//恢复缺省
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnCheckPassword();
	afx_msg void OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonCanUsed();//可用周期
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnChangeInput();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnZt();
	afx_msg void OnFt();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TECHDLGEDITSCREENSTOCK_H__10379B43_6173_11D4_970B_0080C8D6450E__INCLUDED_)
