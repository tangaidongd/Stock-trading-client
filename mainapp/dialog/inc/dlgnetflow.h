#if !defined(AFX_DLGNETFLOW_H__9173003E_B571_4089_B6C7_A969CF7EE31E__INCLUDED_)
#define AFX_DLGNETFLOW_H__9173003E_B571_4089_B6C7_A969CF7EE31E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgnetflow.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgNetFlow dialog
#include "DialogEx.h"
#include <map>

typedef struct T_ServerKey
{
	bool32 operator < (const T_ServerKey& stOther) const
	{
		if ( !m_bQuoteServer && stOther.m_bQuoteServer )
		{
			return true;
		}
		else if ( m_bQuoteServer && !stOther.m_bQuoteServer )
		{
			return false;
		}

		int32 iCmp = m_StrServerAddr.CompareNoCase(stOther.m_StrServerAddr);

		if ( iCmp != 0 )
		{
			return (iCmp < 0);
		}

		if ( m_uiPort != stOther.m_uiPort )
		{
			return (m_uiPort < stOther.m_uiPort);
		}

		if ( m_iCommunicationID != stOther.m_iCommunicationID )
		{
			return (m_iCommunicationID < stOther.m_iCommunicationID);
		}

		iCmp = m_StrServerName.CompareNoCase(stOther.m_StrServerName);
		if ( iCmp != 0 )
		{
			return (iCmp < 0);
		}
				
		return false;
	}

	bool32 operator == (const T_ServerKey& stOther) const
	{
		if ( m_bQuoteServer == stOther.m_bQuoteServer
		  && 0 == m_StrServerName.CompareNoCase(stOther.m_StrServerName)
		  && 0 == m_StrServerAddr.CompareNoCase(stOther.m_StrServerAddr)
		  && m_uiPort == stOther.m_uiPort
		  && m_iCommunicationID == stOther.m_iCommunicationID )
		{
			return true;
		}

		return false;
	}

	bool32		m_bQuoteServer;			// �Ƿ����������
	CString		m_StrServerName;		// ����
	CString		m_StrServerAddr;		// Ip ��ַ
	UINT		m_uiPort;				// Port
	int32		m_iCommunicationID;		// ID
	
}T_ServerKey;

typedef struct T_ServerValue
{
	T_ServerValue()
	{
		m_uiSendBytes = 0;
		m_uiRecvBytes = 0;
	}

	//
	uint32		m_uiSendBytes;			// ��������
	uint32		m_uiRecvBytes;			// ��������
	uint32		m_uiKeepSeconds;		// ����ʱ��
	uint32		m_uiTimeStarted;		// ��ʼʱ��

}T_ServerValue;

typedef map<T_ServerKey, T_ServerValue> mapServerNetFlow;

//
class CDlgNetFlow : public CDialogEx
{
public:
	// ����������
	void	OnServerReConnected(int32 iCommunicationID);

	//
	void	OnNewsServerReConnected(int32 iCommunicationID);

private:
	void	Test();
	// ��ʼ���ؼ�
	void	InitialControls();

	// ���¿ؼ�
	void	UpdateControls();

	// �õ�ʱ���ַ���
	CString	GetTimeString(time_t timeSource);

	// ���òο�ֵ
	void	SetReferenceValue();

	// ���赱ǰֵ
	void	SetCurrentValue();

	// �����Զ�������, ȡ���б���к�
	int32	GetItemByUserData(const CListCtrl* pList, const T_ServerKey* pData);

private:

	// �Ƿ���ͣ
	bool32	m_bPaused;

	// ��������յ�ֵ(ͳ��ʱ������ֵ�����ֵ����)
	mapServerNetFlow	m_mapReference;
	
	// ��ǰ����ֵ
	mapServerNetFlow	m_mapCurrent;

// Construction
public:
	CDlgNetFlow(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgNetFlow)
	enum { IDD = IDD_DIALOG_NET_FLOW };
	CListCtrl	m_ListNow;
	CListCtrl	m_ListHistory;
	CString	m_StrBegin;
	CString	m_StrEnd;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgNetFlow)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgNetFlow)
	afx_msg void OnButtonPause();
	afx_msg void OnButtonReset();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();	
	afx_msg void OnHide();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGNETFLOW_H__9173003E_B571_4089_B6C7_A969CF7EE31E__INCLUDED_)
