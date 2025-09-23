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

// 筹码设置对话框，全局筹码视图响应设置

class CDlgChouMaSetting : public CDialogEx
{
public:
	
	static bool32 ChouMaSetting(); // 弹出对话框，让用户选择筹码设置
	
private:
	
	// Construction
protected:
	CDlgChouMaSetting(CWnd* pParent = NULL);   // standard constructor
	
	void		PromptErrorInput(const CString &StrPrompt, CWnd *pWndFocus = NULL);

	void		FillCurList();
	ChengBenFenBuCycleArray &GetCurCycles(OUT CString *pStrFmt = NULL);

	bool32		CheckCycle(OUT int32 &iCycle, bool32 bPrompt = true);	// 检查周期的值
	bool32		CheckAttenuation(OUT float &fAttenuation, bool32 bPrompt = true); // 检查衰减系数的值
	bool32		CheckDefaultTradeRate(OUT float &fDefaultTradeRate, bool32 bPrompt = true); // 检查默认换手率的值
	
	CIoViewChouMa::T_CBFBParam  m_param;	// 筹码设置参数
	
	// Dialog Data
	//{{AFX_DATA(CDlgChouMaSetting)
	enum { IDD = IDD_DIALOG_CHOUMA };
	CEdit		  m_EditAttenuation;	// 衰减系数
	CEdit		  m_EditDefaultTradeRate; // 默认换手率
	CEdit		  m_EditCycle;			// 周期
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