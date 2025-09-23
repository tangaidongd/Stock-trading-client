#pragma once
#include "afxcmn.h"
#include "afxwin.h"




// CDlgTimeWait 对话框

class CDlgTimeWait : public CDialog
{
	DECLARE_DYNAMIC(CDlgTimeWait)

public:
	CDlgTimeWait(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgTimeWait(); 

// 对话框数据
	enum { IDD = IDD_DIALOG_TIME_WAIT };

	int		ShowTimeWaitDlg(bool bShowTimer, int iTimeElapse, bool bUseProgress, LPCTSTR lpszTitle, LPCTSTR lpszTip);
	void	CancelDlg(int iResult);
	void	SetProgress(int iPos, int iRange);
	void	SetTipText(const CString &StrTip);
	void	AppendToShowTitle(const CString &StrAppend);
	void	SetTitle(const CString &StrTitle);

private:
	void	ShowTimer();

	int		m_iTimeElapse;	// 0则到时cancel此对话框
	bool	m_bShowTimer;	// 在cancel按钮上显示倒计时
	bool	m_bUseProgress;	// 如果不需要用，则隐藏
	CString	m_StrTitle;		
	CString	m_StrTip;

	CString	m_StrCancel;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl	m_ctrlProgress;
	CStatic			m_ctrlTip;

	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
