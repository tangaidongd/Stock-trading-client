#if !defined(AFX_DLGNCHOUMASETTING_H_)
#define AFX_DLGNCHOUMASETTING_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgnotepad.h : header file
//

#include "DialogEx.h"

#include "IoViewChouMa.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgChouMaSetting dialog

// �������öԻ���ȫ�ֳ�����ͼ��Ӧ����

class CDlgChouMaSetting : public CDialogEx
{
public:
	
	static bool32 ChouMaSetting(); // �����Ի������û�ѡ���������
	
private:
	
	// Construction
protected:
	CDlgChouMaSetting(CWnd* pParent = NULL);   // standard constructor
	
	void		PromptErrorInput(const CString &StrPrompt, CWnd *pWndFocus = NULL);

	void		FillCurList();
	ChengBenFenBuCycleArray &GetCurCycles(OUT CString *pStrFmt = NULL);

	bool32		CheckCycle(OUT int32 &iCycle, bool32 bPrompt = true);	// ������ڵ�ֵ
	bool32		CheckAttenuation(OUT float &fAttenuation, bool32 bPrompt = true); // ���˥��ϵ����ֵ
	bool32		CheckDefaultTradeRate(OUT float &fDefaultTradeRate, bool32 bPrompt = true); // ���Ĭ�ϻ����ʵ�ֵ
	
	CIoViewChouMa::T_CBFBParam  m_param;	// �������ò���
	
	// Dialog Data
	//{{AFX_DATA(CDlgChouMaSetting)
	enum { IDD = IDD_DIALOG_CHOUMA };
	CEdit		  m_EditAttenuation;	// ˥��ϵ��
	CEdit		  m_EditDefaultTradeRate; // Ĭ�ϻ�����
	CEdit		  m_EditCycle;			// ����
	CTabCtrl	  m_Tab;
	CListBox		  m_List;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgChouMaSetting)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgChouMaSetting)
	afx_msg void OnBtnAdd();
	afx_msg void OnBtnDel();
	afx_msg void OnTabChange(NMHDR *pHdr, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGNCHOUMASETTING_H_)