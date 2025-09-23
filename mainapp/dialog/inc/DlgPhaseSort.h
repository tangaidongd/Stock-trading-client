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

// �׶����в������öԻ���

class CDlgPhaseSort : public CDialogEx
{
public:
	
	static bool32 PhaseSortSetting(OUT T_PhaseOpenBlockParam &BlockParam, IN const T_PhaseOpenBlockParam *pInputBlockParam = NULL); // �����Ի������û�ѡ��

	static void  GetPhaseSortSetting(OUT T_PhaseOpenBlockParam &BlockParam);
	
private:
	
	// Construction
protected:
	CDlgPhaseSort(CWnd* pParent = NULL);   // standard constructor
	
	void		PromptErrorInput(const CString &StrPrompt, CWnd *pWndFocus = NULL);

	void		SetPhaseBlockParam(const T_PhaseOpenBlockParam *pInputBlockParam);
	void		FillCBBlockList(bool32 bOften);

	bool32		m_bUseStaticParam;	//  �Ƿ�ʹ���˾�̬�Ĳ���
	T_PhaseOpenBlockParam m_PhaseBlockParam;
	
	static   void SavePhaseBlockParamStatic();
	static   void LoadPhaseBlockParamStatic(bool32 bOnlyFirst);
	static T_PhaseOpenBlockParam s_PhaseBlockParam;		// ���һ��ѡ��Ĳ���, ���û��ָ�����������ʹ�øò�����ʼ���Ի���
	
	// Dialog Data
	//{{AFX_DATA(CDlgPhaseSort)
	enum { IDD = IDD_DIALOG_PHASESORT };
	CDateTimeCtrl	m_DTCStart;		// ��ʼʱ��
	CDateTimeCtrl   m_DTCEnd;		// ����ʱ��
	CButton			m_BtnPreWeight;	// ��ȷ��Ȩ
	CComboBox		m_CBBlockList;	// ���ѡ���б�
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