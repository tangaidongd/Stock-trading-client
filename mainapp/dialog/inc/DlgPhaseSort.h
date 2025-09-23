#ifndef _DLGPHASESORT_H_
#define _DLGPHASESORT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgnotepad.h : header file
//

#include "DialogEx.h"

#include "IoViewPhaseSort.h"
#include "BlockConfig.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgPhaseSort dialog

// 阶段排行参数设置对话框

class CDlgPhaseSort : public CDialogEx
{
public:
	
	static bool32 PhaseSortSetting(OUT T_PhaseOpenBlockParam &BlockParam, IN const T_PhaseOpenBlockParam *pInputBlockParam = NULL); // 弹出对话框，让用户选择

	static void  GetPhaseSortSetting(OUT T_PhaseOpenBlockParam &BlockParam);
	
private:
	
	// Construction
protected:
	CDlgPhaseSort(CWnd* pParent = NULL);   // standard constructor
	
	void		PromptErrorInput(const CString &StrPrompt, CWnd *pWndFocus = NULL);

	void		SetPhaseBlockParam(const T_PhaseOpenBlockParam *pInputBlockParam);
	void		FillCBBlockList(bool32 bOften);

	bool32		m_bUseStaticParam;	//  是否使用了静态的参数
	T_PhaseOpenBlockParam m_PhaseBlockParam;
	
	static   void SavePhaseBlockParamStatic();
	static   void LoadPhaseBlockParamStatic(bool32 bOnlyFirst);
	static T_PhaseOpenBlockParam s_PhaseBlockParam;		// 最后一次选择的参数, 如果没有指定参数，则会使用该参数初始化对话框
	
	// Dialog Data
	//{{AFX_DATA(CDlgPhaseSort)
	enum { IDD = IDD_DIALOG_PHASESORT };
	CDateTimeCtrl	m_DTCStart;		// 开始时间
	CDateTimeCtrl   m_DTCEnd;		// 结束时间
	CButton			m_BtnPreWeight;	// 精确复权
	CComboBox		m_CBBlockList;	// 板块选择列表
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPhaseSort)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgPhaseSort)
	afx_msg void OnBtnOftenClicked();
	afx_msg void OnBtnNormalClicked();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_DLGPHASESORT_H_