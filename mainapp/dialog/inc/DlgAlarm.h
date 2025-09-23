#if !defined(AFX_DLGALARM_H__)
#define AFX_DLGALARM_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAlarm.h : header file
//
#include "GridCtrlSys.h"
#include "CStaticSD.h"
#include "AlarmCenter.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgAlarm dialog

class CDlgAlarm : public CDialogEx, public CAlarmNotify
{

	// from CAlarmNotify
public:

	// Ԥ�������ı�
	virtual void OnAlarmsChanged(void* pData, CAlarmNotify::E_AlarmNotify eNotify);

	// ������
	virtual void Alarm(CMerch* pMerch, const CString& StrPrompt);

	// ������
	virtual void AlarmArbitrage(CArbitrage* pArbitrage, const CString& StrPrompt);

public:

	// ����
	bool32			ConstructGrid();

	void            SetColWidthAccordingFont();

	// ȡʱ���ַ���
	CString			GetTimeString(/*const CGmtTime& stTime*/);

	// ����
	void			ActAlarm();

	// ���ñ�־
	void			SetCheckFlag();

private:
	
	// �ж��������Ʒ��������
	int32			BeArbitrageData(int32 iRow);

private:	
	
	// Ԥ������
	CAlarmCenter*	m_pAlarmCenter;
	
	// ViewData ָ��
	CAbsCenterManager*		m_pAbsCenterManager;
	
	
	CGridCtrlSys    m_GridCtrl;
// 	CImageList		m_ImageList;
	CXScrollBar		m_XSBVert;
	CXScrollBar		m_XSBHorz;
	
	// Construction
public:
	CDlgAlarm(CWnd* pParent = NULL);   // standard constructor
	~CDlgAlarm();
	
	// Dialog Data
	//{{AFX_DATA(CDlgAlarm)
	enum { IDD = IDD_DIALOG_ALARM };
	BOOL	m_bCheck;
	BOOL	m_bPromptCheck;
	CString m_StrShow;
	//CStaticSD m_StaticShow;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAlarm)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
	virtual BOOL OnInitDialog();		
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgAlarm)
	afx_msg void OnClose();		
	afx_msg void OnButtonSet();
	afx_msg void OnCheckOnoff();
	afx_msg void OnCheckPrompt();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnButtonClear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGALARM_H__)
