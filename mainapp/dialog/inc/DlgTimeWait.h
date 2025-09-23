#pragma once
#include "afxcmn.h"
#include "afxwin.h"




// CDlgTimeWait �Ի���

class CDlgTimeWait : public CDialog
{
	DECLARE_DYNAMIC(CDlgTimeWait)

public:
	CDlgTimeWait(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgTimeWait(); 

// �Ի�������
	enum { IDD = IDD_DIALOG_TIME_WAIT };

	int		ShowTimeWaitDlg(bool bShowTimer, int iTimeElapse, bool bUseProgress, LPCTSTR lpszTitle, LPCTSTR lpszTip);
	void	CancelDlg(int iResult);
	void	SetProgress(int iPos, int iRange);
	void	SetTipText(const CString &StrTip);
	void	AppendToShowTitle(const CString &StrAppend);
	void	SetTitle(const CString &StrTitle);

private:
	void	ShowTimer();

	int		m_iTimeElapse;	// 0��ʱcancel�˶Ի���
	bool	m_bShowTimer;	// ��cancel��ť����ʾ����ʱ
	bool	m_bUseProgress;	// �������Ҫ�ã�������
	CString	m_StrTitle;		
	CString	m_StrTip;

	CString	m_StrCancel;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl	m_ctrlProgress;
	CStatic			m_ctrlTip;

	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
