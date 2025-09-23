#if !defined(AFX_DLGMANUAL_H__9CB21D62_4A60_4D37_A70B_2030CC35CCD8__INCLUDED_)
#define AFX_DLGMANUAL_H__9CB21D62_4A60_4D37_A70B_2030CC35CCD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgmanual.h : header file
//

#include "DialogEx.h"
#include "XScrollBar.h"
#include "GridCtrl.h"
#include "OptimizeServer.h"

class CCommunication;

/////////////////////////////////////////////////////////////////////////////
// CDlgManual dialog


class CDlgManual : public CDialogEx, public COptimizeServerNotify
{
	enum{
		ON_SORT_SERVER,			// 协助线程同步
		ON_ALL_PING,
		ON_PING_SERVER,
	};
	// From COptimizeServerNotify
public:
	virtual void	OnFinishSortServer();				// 接口处理需要线程同步
	virtual void	OnFinishAllPing();					
	virtual void	OnFinishPingServer(CServerState stServer);

public:
	void			SetInitialParams(IN const arrServer& aServers);
	void			ChangeControlState();
 
private:
	void			OnSizeChanged();
	void			ConstructGrid();
	
	void			UpDateGrid();
	void			UpDateOneRow(const CServerState& stServer);
	void			InsertOneRow(const CServerState& stServer, int32 iRow = -1);
	
	void			ForcePing();
	void			GetFinalResult(int32 iRowDbClick = -1);

private:
	CImageList		m_ImageList;	
	CGridCtrl		m_Grid;

	bool32			m_bForcePinging;
	arrServer		m_aServers;

	// Construction
public:
	CDlgManual(CWnd* pParent = NULL);   // standard constructor
    ~CDlgManual();

// Dialog Data
	//{{AFX_DATA(CDlgManual)
	enum { IDD = IDD_DIALOG_MANUAL };	
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgManual)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgManual)
	afx_msg void OnButtonRecheck();
	afx_msg void OnButtonConnect();
	afx_msg void OnButtonUp();
	afx_msg void OnButtonDown();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnMsgOptimizeServerNotify(WPARAM w, LPARAM l);  // w为具体notify类型, l为各消息对应的参数 // 异步线程转Dlg消息处理线程 - UpdateData需要线程同步
	//}}AFX_MSG
	afx_msg void OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMANUAL_H__9CB21D62_4A60_4D37_A70B_2030CC35CCD8__INCLUDED_)
